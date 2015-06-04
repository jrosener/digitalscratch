/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                                                                            */
/*                                                                            */
/*------------------------------------------------( digital_scratch_api.cpp )-*/
/*                                                                            */
/*  Copyright (C) 2003-2015                                                   */
/*                Julien Rosener <julien.rosener@digital-scratch.org>         */
/*                                                                            */
/*----------------------------------------------------------------( License )-*/
/*                                                                            */
/*  This program is free software: you can redistribute it and/or modify      */
/*  it under the terms of the GNU General Public License as published by      */ 
/*  the Free Software Foundation, either version 3 of the License, or         */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  This package is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU General Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with this program. If not, see <http://www.gnu.org/licenses/>.      */
/*                                                                            */
/*------------------------------------------------------------( Description )-*/
/*                                                                            */
/*              Implementation of Digital Scratch API functions               */
/*                                                                            */
/*============================================================================*/

#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <iostream>
#include <QVector>
#ifdef WIN32
#include <functional>
#endif

using namespace std;

#include "log.h"
#include "digital_scratch_api.h"
#include "dscratch_parameters.h"
#include "digital_scratch.h"

#define XSTR(x) #x
#define STR(x) XSTR(x)

static const char *dscratch_vinyl_names[NB_DSCRATCH_VINYLS] =
{ 
    "final scratch standard 2.0",
    "serato cv02",
    "mixvibes dvs"
};

#define INPUT_BUFFER_MIN_SIZE 512

typedef struct handle_struct
{
    QVector<float>   samples_1;
    QVector<float>   samples_2;
    Digital_scratch *dscratch;
} dscratch_handle_t_struct;



/******************************** Internal functions *************************/

bool l_get_typed_handle(dscratch_handle_t           handle,
                        dscratch_handle_t_struct  **out_typed_handle)
{
    if (handle == nullptr)
    {
        qCCritical(DSLIB_API) << "Handler is null.";
        return false;
    }
 
    *out_typed_handle = static_cast<dscratch_handle_t_struct*>(handle);
    return true;
}


/********************************* API functions ******************************/

dscratch_status_t dscratch_create_turntable(dscratch_vinyls_t   coded_vinyl_type,
                                            const unsigned int  sample_rate,
                                            dscratch_handle_t  *out_handle)
{
    // Check input pointer on handle.
    if (out_handle == nullptr)
    {
        qCCritical(DSLIB_API) << "out_handle is null";
        return DSCRATCH_ERROR;
    }

    // Create the handle.
    dscratch_handle_t_struct *hdl = new dscratch_handle_t_struct;

    // Create Digital_scratch object.
    Digital_scratch *dscratch = new Digital_scratch(coded_vinyl_type, sample_rate);
    if (dscratch == nullptr)
    {
        qCCritical(DSLIB_API) << "Digital_scratch object not created.";
        return DSCRATCH_ERROR;
    }
    hdl->dscratch = dscratch;

    // Prepare global tables of samples to be able to handle at least 512 samples.
    hdl->samples_1.reserve(INPUT_BUFFER_MIN_SIZE);
    hdl->samples_2.reserve(INPUT_BUFFER_MIN_SIZE);

    // Return a handle on the Digital_scratch instance.
    *out_handle = static_cast<dscratch_handle_t_struct*>(hdl);

    return DSCRATCH_SUCCESS;
}

dscratch_status_t dscratch_delete_turntable(dscratch_handle_t handle)
{
    // Get handle.
    dscratch_handle_t_struct *handle_typed;
    if (l_get_typed_handle(handle, &handle_typed) == false)
    {
        return DSCRATCH_ERROR;
    }

    // Delete Digital_scratch instance.
    delete handle_typed->dscratch;

    // Delete the full handle.
    delete handle_typed;

    return DSCRATCH_SUCCESS;
}

dscratch_status_t dscratch_analyze_recorded_datas(dscratch_handle_t  handle,
                                                  const float       *left_samples,
                                                  const float       *right_samples,
                                                  int                samples_table_size)
{
    // Get handle.
    dscratch_handle_t_struct *handle_typed;
    if (l_get_typed_handle(handle, &handle_typed) == false)
    {
        return DSCRATCH_ERROR;
    }

    // Copy input samples in internal tables.
    handle_typed->samples_1.clear();
    std::copy(left_samples, left_samples + samples_table_size, std::back_inserter(handle_typed->samples_1));
    handle_typed->samples_2.clear();
    std::copy(right_samples, right_samples + samples_table_size, std::back_inserter(handle_typed->samples_2));

    // Analyze new samples.
    if (handle_typed->dscratch->analyze_recording_data(handle_typed->samples_1, handle_typed->samples_2) == false)
    {
        qCCritical(DSLIB_API) << "Cannot analyze recorded datas.";
        return DSCRATCH_ERROR;
    }

    return DSCRATCH_SUCCESS;
}

dscratch_status_t dscratch_get_playing_parameters(dscratch_handle_t  handle,
                                                  float             *speed,
                                                  float             *volume) // FIXME : volume should not be returned by libdigitalscratch but calculated by the player
{
    // Get handle.
    dscratch_handle_t_struct *handle_typed;
    if (l_get_typed_handle(handle, &handle_typed) == false)
    {
        return DSCRATCH_ERROR;
    }

    // Get current speed and volume.
    if (handle_typed->dscratch->get_playing_parameters(speed, volume) == false)
    {
        return DSCRATCH_ERROR;
    }

    return DSCRATCH_SUCCESS;
}

dscratch_status_t dscratch_display_turntable(dscratch_handle_t handle)
{
    dscratch_vinyls_t vinyl;

    // Show handle (pointer).
    cout << "handle: " << handle << endl;

    // Display timecoded vinyl.
    if (dscratch_get_turntable_vinyl_type(handle, &vinyl) == DSCRATCH_ERROR)
    {
        return DSCRATCH_ERROR;
    }
    switch(vinyl)
    {
        case FINAL_SCRATCH :
            cout << " vinyl_type: final_scratch" << endl;
            break;
 
        case SERATO :
            cout << " vinyl_type: serato" << endl;
            break;
 
        case MIXVIBES :
            cout << " vinyl_type: mixvibes" << endl;
            break;
 
        default :
            cout << " vinyl_type: error, not found" << endl;
            return DSCRATCH_ERROR;
    }

    return DSCRATCH_SUCCESS;
}

const char *dscratch_get_version()
{
    return STR(VERSION);
}

dscratch_status_t dscratch_get_turntable_vinyl_type(dscratch_handle_t  handle,
                                                    dscratch_vinyls_t *vinyl_type)
{
    // Get handle.
    dscratch_handle_t_struct *handle_typed;
    if (l_get_typed_handle(handle, &handle_typed) == false)
    {
        return DSCRATCH_ERROR;
    }

    // Check type of vinyl.
    Coded_vinyl *vinyl = handle_typed->dscratch->get_coded_vinyl();
    if (vinyl != nullptr)
    {    
        if (dynamic_cast<Final_scratch_vinyl*>(vinyl) != nullptr)
        {
            *vinyl_type = FINAL_SCRATCH;
        }
        else if (dynamic_cast<Serato_vinyl*>(vinyl) != nullptr)
        {
            *vinyl_type = SERATO;
        }
        else if (dynamic_cast<Mixvibes_vinyl*>(vinyl) != nullptr)
        {
            *vinyl_type = MIXVIBES;
        }
        else
        {
            qCCritical(DSLIB_API) << "Unknown timecoded vinyl type";
            return DSCRATCH_ERROR;
        }
    }
    else
    {
        return DSCRATCH_ERROR;
    }

    return DSCRATCH_SUCCESS;
}

DLLIMPORT const char* dscratch_get_vinyl_name_from_type(dscratch_vinyls_t vinyl_type)
{
    return dscratch_vinyl_names[vinyl_type];
}

DLLIMPORT dscratch_vinyls_t dscratch_get_default_vinyl_type()
{
    return SERATO;
}

DLLIMPORT dscratch_status_t dscratch_change_vinyl_type(dscratch_handle_t handle,
                                                       dscratch_vinyls_t vinyl_type)
{
    // Get handle.
    dscratch_handle_t_struct *handle_typed;
    if (l_get_typed_handle(handle, &handle_typed) == false)
    {
        return DSCRATCH_ERROR;
    }

    // Change vinyl if necessary.
    dscratch_vinyls_t current_vinyl_type;
    dscratch_get_turntable_vinyl_type(handle, &current_vinyl_type);
    if (current_vinyl_type != vinyl_type)
    {
        if (handle_typed->dscratch->change_coded_vinyl(vinyl_type) == false)
        {
            return DSCRATCH_ERROR;
        }
    }

    return DSCRATCH_SUCCESS;
}

DLLIMPORT dscratch_status_t dscratch_set_rpm(dscratch_handle_t    handle,
                                             dscratch_vinyl_rpm_t rpm)
{
    // Get handle.
    dscratch_handle_t_struct *handle_typed;
    if (l_get_typed_handle(handle, &handle_typed) == false)
    {
        return DSCRATCH_ERROR;
    }

    // Set turntable RPM.
    if (((rpm != RPM_33) && (rpm != RPM_45)) ||
       (handle_typed->dscratch->get_coded_vinyl()->set_rpm(rpm) == false))
    {
        qCCritical(DSLIB_API) << "Cannot set RPM.";
        return DSCRATCH_ERROR;
    }

    return DSCRATCH_SUCCESS;
}

DLLIMPORT dscratch_status_t dscratch_get_rpm(dscratch_handle_t     handle,
                                             dscratch_vinyl_rpm_t *out_rpm)
{
    // Get handle.
    dscratch_handle_t_struct *handle_typed;
    if (l_get_typed_handle(handle, &handle_typed) == false)
    {
        return DSCRATCH_ERROR;
    }

    // Get RPM parameter from Coded_vinyl.
    if (out_rpm == nullptr)
    {
        qCCritical(DSLIB_API) << "out_rpm is null.";
        return DSCRATCH_ERROR;
    }
    *out_rpm = handle_typed->dscratch->get_coded_vinyl()->get_rpm();

    return DSCRATCH_SUCCESS;
}

DLLIMPORT dscratch_vinyl_rpm_t dscratch_get_default_rpm()
{
    return DEFAULT_RPM;
}

DLLIMPORT dscratch_status_t dscratch_set_min_amplitude(dscratch_handle_t handle,
                                                       float             amplitude)
{
    // Get handle.
    dscratch_handle_t_struct *handle_typed;
    if (l_get_typed_handle(handle, &handle_typed) == false)
    {
        return DSCRATCH_ERROR;
    }

    // Set amplitude.
    handle_typed->dscratch->get_coded_vinyl()->set_min_amplitude(amplitude);

    return DSCRATCH_SUCCESS;
}

DLLIMPORT dscratch_status_t dscratch_get_min_amplitude(dscratch_handle_t  handle,
                                                       float             *out_ampl)
{
    // Get handle.
    dscratch_handle_t_struct *handle_typed;
    if (l_get_typed_handle(handle, &handle_typed) == false)
    {
        return DSCRATCH_ERROR;
    }

    // Get amplitude from Coded_vinyl.
    if (out_ampl == nullptr)
    {
        qCCritical(DSLIB_API) << "out_ampl is null.";
        return DSCRATCH_ERROR;
    }
    *out_ampl = handle_typed->dscratch->get_coded_vinyl()->get_min_amplitude();

    return DSCRATCH_SUCCESS;
}

DLLIMPORT dscratch_status_t dscratch_get_current_amplitude(dscratch_handle_t  handle,
                                                           float             *out_ampl)
{
    // Get handle.
    dscratch_handle_t_struct *handle_typed;
    if (l_get_typed_handle(handle, &handle_typed) == false)
    {
        return DSCRATCH_ERROR;
    }

    // Get current detected amplitude from Coded_vinyl.
    if (out_ampl == nullptr)
    {
        qCCritical(DSLIB_API) << "out_ampl is null.";
        return DSCRATCH_ERROR;
    }
    *out_ampl = handle_typed->dscratch->get_coded_vinyl()->get_current_amplitude();

    return DSCRATCH_SUCCESS;
}

DLLIMPORT dscratch_status_t dscratch_get_default_min_amplitude(dscratch_handle_t  handle,
                                                               float             *out_ampl)
{
    // Get handle.
    dscratch_handle_t_struct *handle_typed;
    if (l_get_typed_handle(handle, &handle_typed) == false)
    {
        return DSCRATCH_ERROR;
    }

    // Get amplitude from Coded_vinyl.
    if (out_ampl == nullptr)
    {
        qCCritical(DSLIB_API) << "out_ampl is null.";
        return DSCRATCH_ERROR;
    }
    *out_ampl = handle_typed->dscratch->get_coded_vinyl()->get_default_min_amplitude();

    return DSCRATCH_SUCCESS;
}

DLLIMPORT float dscratch_get_default_min_amplitude_from_vinyl_type(dscratch_vinyls_t vinyl_type)
{
    float result = 0.0f;

    Coded_vinyl *vinyl = NULL;
    switch(vinyl_type)
    {
        case FINAL_SCRATCH :
            vinyl = new Final_scratch_vinyl(44100);
            break;
 
        case SERATO :
            vinyl = new Serato_vinyl(44100);
            break;
 
        case MIXVIBES :
            vinyl = new Mixvibes_vinyl(44100);
            break;

        default:
            break;
    }

    if (vinyl != NULL)
    {
        result = vinyl->get_default_min_amplitude();
    }
    delete vinyl;

    return result;
}
