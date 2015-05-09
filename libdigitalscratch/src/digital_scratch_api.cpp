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

static const char *DSCRATCH_VINYLS_NAMES[NB_DSCRATCH_VINYLS] =
{ 
    "final scratch standard 2.0",
    "serato cv02",
    "mixvibes dvs"
};

#define INPUT_BUFFER_MIN_SIZE 512

typedef struct handle_struct
{
    vector<float>   *samples_1;
    vector<float>   *samples_2;
    Digital_scratch *dscratch;
} dscratch_handle_struct;



/******************************** Internal functions *************************/

bool l_get_dscratch_from_handle(DSCRATCH_HANDLE   handle,
                                Digital_scratch **dscratch)
{
    if (handle == nullptr)
    {
        qCCritical(DSLIB_API) << "Cannot get Digital_scratch object from handle.";
        return false;
    }
 
    *dscratch = static_cast<dscratch_handle_struct*>(handle)->dscratch;
    return true;
}

bool l_get_samples1_vector_from_handle(DSCRATCH_HANDLE   handle,
                                       vector<float>   **samples)
{
    if (handle == nullptr)
    {
        qCCritical(DSLIB_API) << "Cannot get samples1 vector from handle.";
        return false;
    }

    *samples = static_cast<dscratch_handle_struct*>(handle)->samples_1;
    return true;
}

bool l_get_samples2_vector_from_handle(DSCRATCH_HANDLE   handle,
                                       vector<float>   **samples)
{
    if (handle == nullptr)
    {
        qCCritical(DSLIB_API) << "Cannot get samples2 vector from handle.";
        return false;
    }

    *samples = static_cast<dscratch_handle_struct*>(handle)->samples_2;
    return true;
}

bool l_get_coded_vinyl_from_handle(DSCRATCH_HANDLE   handle,
                                   Coded_vinyl     **vinyl)
{
    // Get Digital_scratch instance from handle.
    Digital_scratch *dscratch = nullptr;
    if (l_get_dscratch_from_handle(handle, &dscratch) == false)
    {
        return false;
    }

    // Get the corresponding Coded_vinyl object.
    *vinyl = dscratch->get_coded_vinyl();
    if (*vinyl == nullptr)
    {
        qCCritical(DSLIB_API) << "Cannot get Coded_vinyl object.";
        return false;
    }

    return true;
}

/********************************* API functions ******************************/

DSCRATCH_STATUS dscratch_create_turntable(DSCRATCH_VINYLS     coded_vinyl_type,
                                          const unsigned int  sample_rate,
                                          DSCRATCH_HANDLE    *out_handle)
{
    // Check input pointer on handle.
    if (out_handle == nullptr)
    {
        qCCritical(DSLIB_API) << "out_handle is null";
        return DSCRATCH_ERROR;
    }

    // Create the handle.
    dscratch_handle_struct *hdl = new dscratch_handle_struct;

    // Create Digital_scratch object.
    Digital_scratch *dscratch = new Digital_scratch(coded_vinyl_type, sample_rate);
    if (dscratch == nullptr)
    {
        qCCritical(DSLIB_API) << "Digital_scratch object not created.";
        return DSCRATCH_ERROR;
    }
    hdl->dscratch = dscratch;

    // Prepare global tables of samples to be able to handle at least 512 samples.
    hdl->samples_1 = new vector<float>(INPUT_BUFFER_MIN_SIZE);
    hdl->samples_2 = new vector<float>(INPUT_BUFFER_MIN_SIZE);

    // Return a handle on the Digital_scratch instance.
    *out_handle = static_cast<dscratch_handle_struct*>(hdl);

    return DSCRATCH_SUCCESS;
}

DSCRATCH_STATUS dscratch_delete_turntable(DSCRATCH_HANDLE handle)
{
    // Get Digital_scratch instance from handle.
    Digital_scratch *dscratch = nullptr;
    if (l_get_dscratch_from_handle(handle, &dscratch) == false)
    {
        return DSCRATCH_ERROR;
    }

    // Delete Digital_scratch instance.
    delete dscratch;

    // Delete samples vectors.
    vector<float> *samples_1;
    vector<float> *samples_2;
    if (l_get_samples1_vector_from_handle(handle, &samples_1) == false)
    {
        return DSCRATCH_ERROR;
    }
    if (l_get_samples2_vector_from_handle(handle, &samples_2) == false)
    {
        return DSCRATCH_ERROR;
    }
    delete samples_1;
    delete samples_2;

    delete static_cast<dscratch_handle_struct*>(handle);

    return DSCRATCH_SUCCESS;
}

DSCRATCH_STATUS dscratch_analyze_recorded_datas(DSCRATCH_HANDLE  handle,
                                                const float     *input_samples_1,
                                                const float     *input_samples_2,
                                                int              nb_frames)
{
    // Get Digital_scratch instance from handle.
    Digital_scratch *dscratch = nullptr;
    if (l_get_dscratch_from_handle(handle, &dscratch) == false)
    {
        return DSCRATCH_ERROR;
    }

    // Copy input samples in internal tables.
    vector<float> *samples_1;
    vector<float> *samples_2;
    if (l_get_samples1_vector_from_handle(handle, &samples_1) == false)
    {
        return DSCRATCH_ERROR;
    }
    if (l_get_samples2_vector_from_handle(handle, &samples_2) == false)
    {
        return DSCRATCH_ERROR;
    }
    samples_1->assign(input_samples_1, input_samples_1 + nb_frames);
    samples_2->assign(input_samples_2, input_samples_2 + nb_frames);

    // Amplify samples if needed.
    int ampl_coeff = 1;
    if (dscratch_get_input_amplify_coeff(handle, &ampl_coeff) == DSCRATCH_ERROR)
    {
        return DSCRATCH_ERROR;
    }
    else if (ampl_coeff > 1)
    {
        std::transform(samples_1->begin(), samples_1->end(),
                       samples_1->begin(),
                       std::bind1st(std::multiplies<float>(), ampl_coeff));
        std::transform(samples_2->begin(), samples_2->end(),
                       samples_2->begin(),
                       std::bind1st(std::multiplies<float>(), ampl_coeff));
    }

    // Analyze new samples.
    if (dscratch->analyze_recording_data(*samples_1, *samples_2) == false)
    {
        qCCritical(DSLIB_API) << "Cannot analyze recorded datas.";
        return DSCRATCH_ERROR;
    }

    return DSCRATCH_SUCCESS;
}

DSCRATCH_STATUS dscratch_analyze_recorded_datas_interleaved(DSCRATCH_HANDLE  handle,
                                                            int              nb_channels,
                                                            int              left_index,
                                                            int              right_index,
                                                            float           *input_samples_interleaved,
                                                            int              nb_frames)
{

    int j;
    int k;

    // Get Digital_scratch instance from handle.
    Digital_scratch *dscratch = nullptr;
    if (l_get_dscratch_from_handle(handle, &dscratch) == false)
    {
        return DSCRATCH_ERROR;
    }

    // Clean internal tables of samples.
    vector<float> *samples_1;
    vector<float> *samples_2;
    if (l_get_samples1_vector_from_handle(handle, &samples_1) == false)
    {
        return DSCRATCH_ERROR;
    }
    if (l_get_samples2_vector_from_handle(handle, &samples_2) == false)
    {
        return DSCRATCH_ERROR;
    }
    samples_1->clear();
    samples_1->clear();

    // If internal tables of samples are not enough large, enlarge them.
    if (samples_1->capacity() < (unsigned int)nb_frames)
    {
        samples_1->reserve(nb_frames);
        samples_2->reserve(nb_frames);
    }

    // Uninterleaved datas, extract them in 2 tables.
    j = left_index;
    k = right_index;
    for (int i = 0; i < nb_frames; i++)
    {
        samples_1->push_back(input_samples_interleaved[j]);
        samples_2->push_back(input_samples_interleaved[k]);

        j = j + nb_channels;
        k = k + nb_channels;
    }

    // Analyze datas from uninterleaved tables.
    if (dscratch->analyze_recording_data(*samples_1, *samples_2) == false)
    {
        qCCritical(DSLIB_API) << "Cannot analyze interleaved recorded datas.";
        return DSCRATCH_ERROR;
    }

    return DSCRATCH_SUCCESS;
}


DSCRATCH_STATUS dscratch_get_playing_parameters(DSCRATCH_HANDLE  handle,
                                                float           *speed,
                                                float           *volume)
{
    // Get Digital_scratch instance from handle.
    Digital_scratch *dscratch = nullptr;
    if (l_get_dscratch_from_handle(handle, &dscratch) == false)
    {
        return DSCRATCH_ERROR;
    }

    // Get current speed and volume.
    if (dscratch->get_playing_parameters(speed, volume) == false)
    {
        return DSCRATCH_ERROR;
    }

    return DSCRATCH_SUCCESS;
}

DSCRATCH_STATUS dscratch_display_turntable(DSCRATCH_HANDLE handle)
{
    DSCRATCH_VINYLS vinyl;

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

DSCRATCH_STATUS dscratch_get_turntable_vinyl_type(DSCRATCH_HANDLE   handle,
                                                  DSCRATCH_VINYLS   *vinyl_type)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = nullptr;
    if (l_get_coded_vinyl_from_handle(handle, &vinyl) == false)
    {
        return DSCRATCH_ERROR;
    }

    // Check type of vinyl.
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

    return DSCRATCH_SUCCESS;
}

DLLIMPORT const char* dscratch_get_vinyl_name_from_type(DSCRATCH_VINYLS vinyl_type)
{
    return DSCRATCH_VINYLS_NAMES[vinyl_type];
}

DLLIMPORT DSCRATCH_VINYLS dscratch_get_default_vinyl_type()
{
    return SERATO;
}

DLLIMPORT DSCRATCH_STATUS dscratch_change_vinyl_type(DSCRATCH_HANDLE  handle,
                                                     DSCRATCH_VINYLS  vinyl_type)
{
    // Get Digital_scratch instance from handle.
    Digital_scratch *dscratch = nullptr;
    if (l_get_dscratch_from_handle(handle, &dscratch) == false)
    {
        return DSCRATCH_ERROR;
    }

    // Change vinyl if necessary.
    DSCRATCH_VINYLS current_vinyl_type;
    dscratch_get_turntable_vinyl_type(handle, &current_vinyl_type);
    if (current_vinyl_type != vinyl_type)
    {
        if (dscratch->change_coded_vinyl(vinyl_type) == false)
        {
            return DSCRATCH_ERROR;
        }
    }

    return DSCRATCH_SUCCESS;
}

/**** API functions: General motion detection configuration parameters ********/
DLLIMPORT DSCRATCH_STATUS dscratch_set_input_amplify_coeff(DSCRATCH_HANDLE handle,
                                                           int             coeff)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = nullptr;
    if (l_get_coded_vinyl_from_handle(handle, &vinyl) == false)
    {
        return DSCRATCH_ERROR;
    }

    // Set input_amplify_coeff parameter to Coded_vinyl.
    if (vinyl->set_input_amplify_coeff(coeff) == false)
    {
        qCCritical(DSLIB_API) << "Cannot set input_amplify_coeff.";
        return DSCRATCH_ERROR;
    }

    return DSCRATCH_SUCCESS;
}

DLLIMPORT DSCRATCH_STATUS dscratch_get_input_amplify_coeff(DSCRATCH_HANDLE  handle,
                                                           int             *out_coeff)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = nullptr;
    if (l_get_coded_vinyl_from_handle(handle, &vinyl) == false)
    {
        return DSCRATCH_ERROR;
    }

    // Get input_amplify_coeff parameter from Coded_vinyl.
    if (out_coeff == nullptr)
    {
        qCCritical(DSLIB_API) << "out_coeff is null.";
        return DSCRATCH_ERROR;
    }

    *out_coeff = vinyl->get_input_amplify_coeff();

    return DSCRATCH_SUCCESS;
}

DLLIMPORT int dscratch_get_default_input_amplify_coeff()
{
    return DEFAULT_INPUT_AMPLIFY_COEFF;
}

DLLIMPORT DSCRATCH_STATUS dscratch_set_rpm(DSCRATCH_HANDLE    handle,
                                           unsigned short int rpm)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = nullptr;
    if (l_get_coded_vinyl_from_handle(handle, &vinyl) == false)
    {
        return DSCRATCH_ERROR;
    }

    // Set turntable RPM.
    if (((rpm != RPM_33) && (rpm != RPM_45)) ||
       (vinyl->set_rpm(rpm) == false))
    {
        qCCritical(DSLIB_API) << "Cannot set RPM.";
        return DSCRATCH_ERROR;
    }

    return DSCRATCH_SUCCESS;
}

DLLIMPORT DSCRATCH_STATUS dscratch_get_rpm(DSCRATCH_HANDLE     handle,
                                           unsigned short int *out_rpm)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = nullptr;
    if (l_get_coded_vinyl_from_handle(handle, &vinyl) == false)
    {
        return DSCRATCH_ERROR;
    }

    // Get RPM parameter from Coded_vinyl.
    if (out_rpm == nullptr)
    {
        qCCritical(DSLIB_API) << "out_rpm is null.";
        return DSCRATCH_ERROR;
    }
    *out_rpm = vinyl->get_rpm();

    return DSCRATCH_SUCCESS;
}

DLLIMPORT unsigned short int dscratch_get_default_rpm()
{
    return DEFAULT_RPM;
}

DLLIMPORT DSCRATCH_STATUS dscratch_set_min_amplitude_for_normal_speed(DSCRATCH_HANDLE handle,
                                                                      float           amplitude)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = nullptr;
    if (l_get_coded_vinyl_from_handle(handle, &vinyl) == false)
    {
        return DSCRATCH_ERROR;
    }

    // Set amplitude.
    vinyl->set_min_amplitude_for_normal_speed(amplitude);

    return DSCRATCH_SUCCESS;
}

DLLIMPORT DSCRATCH_STATUS dscratch_get_min_amplitude_for_normal_speed(DSCRATCH_HANDLE  handle,
                                                                      float           *out_ampl)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = nullptr;
    if (l_get_coded_vinyl_from_handle(handle, &vinyl) == false)
    {
        return DSCRATCH_ERROR;
    }

    // Get amplitude from Coded_vinyl.
    if (out_ampl == nullptr)
    {
        qCCritical(DSLIB_API) << "out_ampl is null.";
        return DSCRATCH_ERROR;
    }
    *out_ampl = vinyl->get_min_amplitude_for_normal_speed();

    return DSCRATCH_SUCCESS;
}

DLLIMPORT DSCRATCH_STATUS dscratch_get_default_min_amplitude_for_normal_speed(DSCRATCH_HANDLE  handle,
                                                                              float           *out_ampl)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = nullptr;
    if (l_get_coded_vinyl_from_handle(handle, &vinyl) == false)
    {
        return DSCRATCH_ERROR;
    }

    // Get amplitude from Coded_vinyl.
    if (out_ampl == nullptr)
    {
        qCCritical(DSLIB_API) << "out_ampl is null.";
        return DSCRATCH_ERROR;
    }
    *out_ampl = vinyl->get_default_min_amplitude_for_normal_speed();

    return DSCRATCH_SUCCESS;
}

DLLIMPORT float dscratch_get_default_min_amplitude_for_normal_speed_from_vinyl_type(DSCRATCH_VINYLS vinyl_type)
{
    float result = 0.0f;

    Coded_vinyl *vinyl = nullptr;
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
        result = vinyl->get_default_min_amplitude_for_normal_speed();
    }
    delete vinyl;

    return result;
}

DLLIMPORT DSCRATCH_STATUS dscratch_set_min_amplitude(DSCRATCH_HANDLE handle,
                                                     float           amplitude)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = nullptr;
    if (l_get_coded_vinyl_from_handle(handle, &vinyl) == false)
    {
        return DSCRATCH_ERROR;
    }

    // Set amplitude.
    vinyl->set_min_amplitude(amplitude);

    return DSCRATCH_SUCCESS;
}

DLLIMPORT DSCRATCH_STATUS dscratch_get_min_amplitude(DSCRATCH_HANDLE  handle,
                                                     float           *out_ampl)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = NULL;
    if (l_get_coded_vinyl_from_handle(handle, &vinyl) == false)
    {
        return DSCRATCH_ERROR;
    }

    // Get amplitude from Coded_vinyl.
    if (out_ampl == nullptr)
    {
        qCCritical(DSLIB_API) << "out_ampl is null.";
        return DSCRATCH_ERROR;
    }
    *out_ampl = vinyl->get_min_amplitude();

    return DSCRATCH_SUCCESS;
}

DLLIMPORT DSCRATCH_STATUS dscratch_get_default_min_amplitude(DSCRATCH_HANDLE  handle,
                                                            float           *out_ampl)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = NULL;
    if (l_get_coded_vinyl_from_handle(handle, &vinyl) == false)
    {
        return DSCRATCH_ERROR;
    }

    // Get amplitude from Coded_vinyl.
    if (out_ampl == nullptr)
    {
        qCCritical(DSLIB_API) << "out_ampl is null.";
        return DSCRATCH_ERROR;
    }
    *out_ampl = vinyl->get_default_min_amplitude();

    return DSCRATCH_SUCCESS;
}

DLLIMPORT float dscratch_get_default_min_amplitude_from_vinyl_type(DSCRATCH_VINYLS vinyl_type)
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
