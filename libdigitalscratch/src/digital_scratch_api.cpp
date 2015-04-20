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

using namespace std;

#include "log.h"
#include "digital_scratch_api.h"
#include "dscratch_parameters.h"
#include "digital_scratch.h"

#define XSTR(x) #x
#define STR(x) XSTR(x)

/****************************** Magic numbers *********************************/

#define INPUT_BUFFER_MIN_SIZE 512

/***************************** Global variables *******************************/

/**
 * List of turntables.
 */
vector<Digital_scratch*> tab_turntable;

/**
 * Input datas for left and right samples.
 */
vector<float> g_input_samples_1;
vector<float> g_input_samples_2;

/******************************** Internal functions *************************/

bool l_get_coded_vinyl(DSCRATCH_HANDLE   handle,
                       Coded_vinyl     **vinyl)
{
    Digital_scratch *dscratch = NULL;

    // Get Digital_scratch object (not safe, Digital_scratch object must
    // exists and must be not NULL).
    if (handle >= 0 && handle < (int)tab_turntable.size())
    {
        dscratch = tab_turntable[handle];
    }

    if (dscratch == NULL)
    {
        qCCritical(DSLIB_API) << "Cannot get Digital_scratch object.";
        return false;
    }

    // Get Final_scratch_vinyl object.
    *vinyl = dscratch->get_coded_vinyl();
    if (*vinyl == NULL)
    {
        qCCritical(DSLIB_API) << "Cannot get Coded_vinyl object.";
        return false;
    }

    return true;
}

/********************************* API functions ******************************/

DSCRATCH_STATUS dscratch_create_turntable(const char         *name,
                                          const char         *coded_vinyl_type,
                                          const unsigned int  sample_rate,
                                          DSCRATCH_HANDLE    *handle)
{
    // Error if no name is provided.
    if (name == NULL || QString::fromUtf8(name) == "")
    {
        qCCritical(DSLIB_API) << "No turntable name provided.";
        return DSCRATCH_ERROR;
    }

    // Error if no name is provided.
    if (coded_vinyl_type == NULL || QString::fromUtf8(coded_vinyl_type) == "")
    {
        qCCritical(DSLIB_API) << "No coded vinyl type provided.";
        return DSCRATCH_ERROR;
    }

    // Create Digital_scratch object.
    Digital_scratch *dscratch = new Digital_scratch(QString::fromUtf8(name).toStdString(),
                                                    QString::fromUtf8(coded_vinyl_type).toStdString(),
                                                    sample_rate);
    if (dscratch == NULL)
    {
        qCCritical(DSLIB_API) << "Digital_scratch object not created.";
        return DSCRATCH_ERROR;
    }

    // Go through the list of turntables and add this Digital_scratch object in
    // an empty place or at the end of the list.
    unsigned int i = 0;
    for (i = 0; i < tab_turntable.size(); i++)
    {
        if (tab_turntable[i] == NULL) // This is an empty place.
        {
            // Add this object in the empty place.
            tab_turntable[i] = dscratch;

            // Return index of Digital_scratch object in table of turntables.
            *handle = i;

            break;
        }
    }
    if (i == tab_turntable.size()) // No empty place found.
    {
        // Add this object to table of turntables.
        tab_turntable.push_back(dscratch);

        // Return index of Digital_scratch object in table of turntables.
        *handle = tab_turntable.size() - 1;
    }

    // Prepare global tables of samples to be able to handle at least 512 samples.
    g_input_samples_1.reserve(INPUT_BUFFER_MIN_SIZE);
    g_input_samples_2.reserve(INPUT_BUFFER_MIN_SIZE);

    return DSCRATCH_SUCCESS;
}

DSCRATCH_STATUS dscratch_delete_turntable(DSCRATCH_HANDLE handle)
{
    if (handle > ((int)tab_turntable.size() - 1))
    {
        qCCritical(DSLIB_API) << "Cannot remove turntable at index" << QString(handle);
        return DSCRATCH_ERROR;
    }

    // Delete Digital_scratch object from table of turntables.
    if (tab_turntable[handle] == NULL)
    {
        qCCritical(DSLIB_API) << "No turntable at index" << QString(handle);
        return DSCRATCH_ERROR;
    }
    delete tab_turntable[handle];
    tab_turntable[handle] = NULL;

    // All is OK.
    return DSCRATCH_SUCCESS;
}

DSCRATCH_STATUS dscratch_analyze_recorded_datas(DSCRATCH_HANDLE  handle,
                                                const float     *input_samples_1,
                                                const float     *input_samples_2,
                                                int              nb_frames)
{
    // Copy input samples in internal tables.
    g_input_samples_1.assign(input_samples_1, input_samples_1 + nb_frames);
    g_input_samples_2.assign(input_samples_2, input_samples_2 + nb_frames);

    // Amplify samples if needed.
    if (dscratch_get_input_amplify_coeff(handle) > 1)
    {
        std::transform(g_input_samples_1.begin(), g_input_samples_1.end(),
                       g_input_samples_1.begin(),
                       std::bind1st(std::multiplies<float>(), dscratch_get_input_amplify_coeff(handle)));
        std::transform(g_input_samples_2.begin(), g_input_samples_2.end(),
                       g_input_samples_2.begin(),
                       std::bind1st(std::multiplies<float>(), dscratch_get_input_amplify_coeff(handle)));
    }

    // Analyze new samples.
    if (tab_turntable[handle]->analyze_recording_data(g_input_samples_1,
                                                            g_input_samples_2) == false)
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

    // Clean internal tables of samples.
    g_input_samples_1.clear();
    g_input_samples_2.clear();

    // If internal tables of samples are not enough large, enlarge them.
    if (g_input_samples_1.capacity() < (unsigned int)nb_frames)
    {
        g_input_samples_1.reserve(nb_frames);
        g_input_samples_2.reserve(nb_frames);
    }

    // Uninterleaved datas, extract them in 2 tables.
    j = left_index;
    k = right_index;
    for (int i = 0; i < nb_frames; i++)
    {
        g_input_samples_1.push_back(input_samples_interleaved[j]);
        g_input_samples_2.push_back(input_samples_interleaved[k]);

        j = j + nb_channels;
        k = k + nb_channels;
    }

    // Analyze datas from uninterleaved tables.
    if (tab_turntable[handle]->analyze_recording_data(g_input_samples_1,
                                                      g_input_samples_2) == false)
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
    if (tab_turntable[handle]->get_playing_parameters(speed,
                                                            volume) == false)
    {
        // Playing parameters not found.
        return DSCRATCH_ERROR;
    }

    return DSCRATCH_SUCCESS;
}

DSCRATCH_STATUS dscratch_display_turntable(DSCRATCH_HANDLE handle)
{
    char *turntable_name = NULL;
    char *vinyl_name     = NULL;

    // Check turntable id.
    if (handle > ((int)tab_turntable.size() - 1))
    {
        qCCritical(DSLIB_API) << "Cannot access this turntable" << QString(handle);
        return DSCRATCH_ERROR;
    }

    // Check if turntable exists.
    if (tab_turntable[handle] == NULL)
    {
        qCCritical(DSLIB_API) << "No turntable at index" << QString(handle);
        return DSCRATCH_ERROR;
    }

    // General informations.
    dscratch_get_turntable_name(handle, &turntable_name);
    cout << "handle: " << handle << endl;
    cout << " turntable_name            : " << turntable_name << endl;

    // Specific stuff for FinalScratch vinyl.
    dscratch_get_vinyl_type(handle, &vinyl_name);

    if (strcmp(vinyl_name, FINAL_SCRATCH_VINYL) == 0)
    {
        cout << " vinyl_type: final_scratch" << endl;
    }
    else if (strcmp(vinyl_name, SERATO_VINYL) == 0)
    {
        cout << " vinyl_type: serato" << endl;
    }
    else if (strcmp(vinyl_name, MIXVIBES_VINYL) == 0)
    {
        cout << " vinyl_type: mixvibes" << endl;
    }

    // Cleanup.
    if (turntable_name != NULL)
    {
        free(turntable_name);
    }
    if (vinyl_name != NULL)
    {
        free(vinyl_name);
    }

    return DSCRATCH_SUCCESS;
}

int dscratch_get_number_of_turntables()
{
    return tab_turntable.size();
}

const char *dscratch_get_version()
{
    return STR(VERSION);
}

DSCRATCH_STATUS dscratch_get_turntable_name(DSCRATCH_HANDLE   handle,
                                            char            **turntable_name)
{
    char *name = NULL;
    int   size = 0;

    // Check turntable_name.
    if (turntable_name == NULL)
    {
        qCCritical(DSLIB_API) << "Cannot get turntable name.";
        return DSCRATCH_ERROR;
    }

    // Check turntable id.
    if (handle > ((int)tab_turntable.size() - 1))
    {
        qCCritical(DSLIB_API) << "Cannot access this turntable" << QString(handle);
        return DSCRATCH_ERROR;
    }

    // Check if turntable exists.
    if (tab_turntable[handle] == NULL)
    {
        qCCritical(DSLIB_API) << "No turntable at index " << QString(handle);
        return DSCRATCH_ERROR;
    }

    // Get name from Controller.
    string controller_name = tab_turntable[handle]->get_name();

    // Size of result.
    size = controller_name.length() + 1;

    // Allocate memory for resulting string.
    name = (char*)malloc(sizeof(char) * size);

    // Put turntable name in resulting string.
    #ifdef WIN32
        strncpy_s(name, size, controller_name.c_str(), size);
    #else
        strncpy(name, controller_name.c_str(), size);
    #endif

    // Return turntable name
    *turntable_name = name;

    return DSCRATCH_SUCCESS;
}

DSCRATCH_STATUS dscratch_get_vinyl_type(DSCRATCH_HANDLE   handle,
                                        char            **vinyl_type)
{
    char        *vinyl_name = NULL;
    int          size       = 0;
    Coded_vinyl *cv         = NULL;

    // Check vinyl name.
    if (vinyl_type == NULL)
    {
        qCCritical(DSLIB_API) << "Cannot get vinyl type.";
        return DSCRATCH_ERROR;
    }

    // Check turntable id.
    if (handle > ((int)tab_turntable.size() - 1))
    {
        qCCritical(DSLIB_API) << "Cannot access this turntable" << QString(handle);
        return DSCRATCH_ERROR;
    }

    // Check if turntable exists.
    if (tab_turntable[handle] == NULL)
    {
        qCCritical(DSLIB_API) << "No turntable at index" << QString(handle);
        return DSCRATCH_ERROR;
    }

    // Get the type of Coded_vinyl pointed by Digital_scratch.
    cv = tab_turntable[handle]->get_coded_vinyl();
    if (cv != NULL)
    {
        if (dynamic_cast<Final_scratch_vinyl*>(cv) != NULL)
        {
            // Size of result.
            size = strlen(FINAL_SCRATCH_VINYL)+1;

            // Allocate memory for resulting string.
            vinyl_name = (char*)malloc(sizeof(char) * size);

            // Put vinyl name in resulting string.           
            #ifdef WIN32
                strncpy_s(vinyl_name, size, FINAL_SCRATCH_VINYL, size);
            #else
                strncpy(vinyl_name, FINAL_SCRATCH_VINYL, size);
            #endif
        }
        else if (dynamic_cast<Serato_vinyl*>(cv) != NULL)
        {
            // Size of result.
            size = strlen(SERATO_VINYL)+1;

            // Allocate memory for resulting string.
            vinyl_name = (char*)malloc(sizeof(char) * size);

            // Put vinyl name in resulting string.
            #ifdef WIN32
                strncpy_s(vinyl_name, size, SERATO_VINYL, size);
            #else
                strncpy(vinyl_name, SERATO_VINYL, size);
            #endif
        }
        else if (dynamic_cast<Mixvibes_vinyl*>(cv) != NULL)
        {
            // Size of result.
            size = strlen(MIXVIBES_VINYL)+1;

            // Allocate memory for resulting string.
            vinyl_name = (char*)malloc(sizeof(char) * size);

            // Put vinyl name in resulting string.
            #ifdef WIN32
                strncpy_s(vinyl_name, size, MIXVIBES_VINYL, size);
            #else
                strncpy(vinyl_name, MIXVIBES_VINYL, size);
            #endif
        }
        else
        {
            qCCritical(DSLIB_API) << "Unknown timecoded vinyl type";
            return DSCRATCH_ERROR;
        }
    }
    else
    {
        qCCritical(DSLIB_API) << "Cannot access to coded vinyl.";
        return DSCRATCH_ERROR;
    }

    // Return vinyl type.
    *vinyl_type = vinyl_name;

    return DSCRATCH_SUCCESS;
}

DLLIMPORT const char* dscratch_get_default_vinyl_type()
{
    return SERATO_VINYL;
}


DLLIMPORT DSCRATCH_STATUS dscratch_change_vinyl_type(DSCRATCH_HANDLE  handle,
                                                     char            *vinyl_type)
{
    char *current_vinyl_type = NULL;

    // Check vinyl name.
    if (vinyl_type == NULL)
    {
        qCCritical(DSLIB_API) << "Cannot get vinyl type.";
        return DSCRATCH_ERROR;
    }

    // Check turntable id.
    if (handle > ((int)tab_turntable.size() - 1))
    {
        qCCritical(DSLIB_API) << "Cannot access this turntable" << QString(handle);
        return DSCRATCH_ERROR;
    }

    // Change vinyl if necessary.
    dscratch_get_vinyl_type(handle, &current_vinyl_type);
    if (current_vinyl_type == NULL)
    {
        return DSCRATCH_ERROR;
    }
    if (strcmp(current_vinyl_type, vinyl_type) != 0)
    {
        if (tab_turntable[handle]->change_coded_vinyl(QString::fromUtf8(vinyl_type).toStdString()) == false)
        {
            return DSCRATCH_ERROR;
        }
    }

    // Cleanup.
    if (current_vinyl_type != NULL)
    {
        free(current_vinyl_type);
    }

    return DSCRATCH_SUCCESS;
}

/**** API functions: General motion detection configuration parameters ********/
DLLIMPORT DSCRATCH_STATUS dscratch_set_input_amplify_coeff(DSCRATCH_HANDLE handle,
                                                           int             coeff)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = NULL;
    if (l_get_coded_vinyl(handle, &vinyl) == false) return DSCRATCH_ERROR;

    // Set input_amplify_coeff parameter to Coded_vinyl.
    if (vinyl->set_input_amplify_coeff(coeff) == false)
    {
        qCCritical(DSLIB_API) << "Cannot set input_amplify_coeff.";
        return DSCRATCH_ERROR;
    }

    return DSCRATCH_SUCCESS;
}

// FIXME: return an error code or the value, but not both !
DLLIMPORT int dscratch_get_input_amplify_coeff(DSCRATCH_HANDLE handle)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = NULL;
    if (l_get_coded_vinyl(handle, &vinyl) == false) return 1;

    // Get input_amplify_coeff parameter from Coded_vinyl.
    return vinyl->get_input_amplify_coeff();
}

DLLIMPORT int dscratch_get_default_input_amplify_coeff()
{
    return DEFAULT_INPUT_AMPLIFY_COEFF;
}

DLLIMPORT DSCRATCH_STATUS dscratch_set_rpm(DSCRATCH_HANDLE    handle,
                                           unsigned short int rpm)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = NULL;
    if (l_get_coded_vinyl(handle, &vinyl) == false) return DSCRATCH_ERROR;

    // Set turntable RPM.
    if (((rpm != RPM_33) && (rpm != RPM_45)) ||
       (vinyl->set_rpm(rpm) == false))
    {
        qCCritical(DSLIB_API) << "Cannot set RPM.";
        return DSCRATCH_ERROR;
    }

    return DSCRATCH_SUCCESS;
}

DLLIMPORT unsigned short int dscratch_get_rpm(DSCRATCH_HANDLE handle)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = NULL;
    if (l_get_coded_vinyl(handle, &vinyl) == false) return DSCRATCH_ERROR;

    // Get RPM parameter from Coded_vinyl.
    return vinyl->get_rpm();
}

DLLIMPORT unsigned short int dscratch_get_default_rpm()
{
    return DEFAULT_RPM;
}

DLLIMPORT DSCRATCH_STATUS dscratch_set_min_amplitude_for_normal_speed(DSCRATCH_HANDLE handle,
                                                                      float           amplitude)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = NULL;
    if (l_get_coded_vinyl(handle, &vinyl) == false) return DSCRATCH_ERROR;

    // Set amplitude.
    vinyl->set_min_amplitude_for_normal_speed(amplitude);
    return DSCRATCH_SUCCESS;
}

DLLIMPORT float dscratch_get_min_amplitude_for_normal_speed(DSCRATCH_HANDLE handle)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = NULL;
    if (l_get_coded_vinyl(handle, &vinyl) == false) return DSCRATCH_ERROR;

    // Get amplitude from Coded_vinyl.
    return vinyl->get_min_amplitude_for_normal_speed();
}

DLLIMPORT float dscratch_get_default_min_amplitude_for_normal_speed()
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = NULL;
    if (l_get_coded_vinyl(0, &vinyl) == false) return DSCRATCH_ERROR;

    // Get amplitude from Coded_vinyl.
    return vinyl->get_default_min_amplitude_for_normal_speed();
}

DLLIMPORT float dscratch_get_default_min_amplitude_for_normal_speed_from_vinyl_type(const char *coded_vinyl_type)
{
    float result = 0.0f;

    Coded_vinyl *vinyl = NULL;
    if (strcmp(coded_vinyl_type, SERATO_VINYL) == 0)
    {
        vinyl = new Serato_vinyl(44100);
    }
    else if (strcmp(coded_vinyl_type, FINAL_SCRATCH_VINYL) == 0)
    {
        vinyl = new Final_scratch_vinyl(44100);
    }
    else if (strcmp(coded_vinyl_type, MIXVIBES_VINYL) == 0)
    {
        vinyl = new Mixvibes_vinyl(44100);
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
    Coded_vinyl *vinyl = NULL;
    if (l_get_coded_vinyl(handle, &vinyl) == false) return DSCRATCH_ERROR;

    // Set amplitude.
    vinyl->set_min_amplitude(amplitude);
    return DSCRATCH_SUCCESS;
}

DLLIMPORT float dscratch_get_min_amplitude(DSCRATCH_HANDLE handle)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = NULL;
    if (l_get_coded_vinyl(handle, &vinyl) == false) return DSCRATCH_ERROR;

    // Get amplitude from Coded_vinyl.
    return vinyl->get_min_amplitude();
}

DLLIMPORT float dscratch_get_default_min_amplitude()
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = NULL;
    if (l_get_coded_vinyl(0, &vinyl) == false) return DSCRATCH_ERROR;

    // Get amplitude from Coded_vinyl.
    return vinyl->get_default_min_amplitude();
}

DLLIMPORT float dscratch_get_default_min_amplitude_from_vinyl_type(const char *coded_vinyl_type)
{
    float result = 0.0f;

    Coded_vinyl *vinyl = NULL;
    if (strcmp(coded_vinyl_type, SERATO_VINYL) == 0)
    {
        vinyl = new Serato_vinyl(44100);
    }
    else if (strcmp(coded_vinyl_type, FINAL_SCRATCH_VINYL) == 0)
    {
        vinyl = new Final_scratch_vinyl(44100);
    }
    else if (strcmp(coded_vinyl_type, MIXVIBES_VINYL) == 0)
    {
        vinyl = new Mixvibes_vinyl(44100);
    }

    if (vinyl != NULL)
    {
        result = vinyl->get_default_min_amplitude();
    }
    delete vinyl;

    return result;
}
