/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                                                                            */
/*                                                                            */
/*------------------------------------------------( digital_scratch_api.cpp )-*/
/*                                                                            */
/*  Copyright (C) 2003-2013                                                   */
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

#include "include/digital_scratch_api.h"
#include "include/dscratch_parameters.h"
#include "include/digital_scratch.h"
#include "include/utils.h"

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

bool l_get_coded_vinyl(int           turntable_id,
                       Coded_vinyl **vinyl)
{
    Digital_scratch *dscratch = NULL;

    // Get Digital_scratch object (not safe, Digital_scratch object must
    // exists and must be not NULL).
    if (turntable_id >= 0 && turntable_id < (int)tab_turntable.size())
    {
        dscratch = tab_turntable[turntable_id];
    }

    if (dscratch == NULL)
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                           "Cannot get Digital_scratch object.");

        return false;
    }

    // Get Final_scratch_vinyl object.
    *vinyl = dscratch->get_coded_vinyl();
    if (*vinyl == NULL)
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                           "Cannot get Coded_vinyl object.");

        return false;
    }

    return true;
}

/********************************* API functions ******************************/

int dscratch_create_turntable(const char   *name,
                              const char   *coded_vinyl_type,
                              unsigned int  sample_rate,
                              int          *turntable_id)
{
    // Error if no name is provided.
    if (name == NULL || Utils::to_string(name) == "")
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                           "No turntable name provided.");

        return 1;
    }

    // Error if no name is provided.
    if (coded_vinyl_type == NULL || Utils::to_string(coded_vinyl_type) == "")
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                           "No coded vinyl type provided.");

        return 1;
    }

    // Create Digital_scratch object.
    Digital_scratch *dscratch = new Digital_scratch(Utils::to_string(name),
                                                    Utils::to_string(coded_vinyl_type),
                                                    sample_rate);
    if (dscratch == NULL)
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                           "Digital_scratch object not created.");
        return 1;
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
            *turntable_id = i;

            break;
        }
    }
    if (i == tab_turntable.size()) // No empty place found.
    {
        // Add this object to table of turntables.
        tab_turntable.push_back(dscratch);

        // Return index of Digital_scratch object in table of turntables.
        *turntable_id = tab_turntable.size() - 1;
    }

    // Prepare global tables of samples to be able to handle at least 512 samples.
    g_input_samples_1.reserve(INPUT_BUFFER_MIN_SIZE);
    g_input_samples_2.reserve(INPUT_BUFFER_MIN_SIZE);

    return 0;
}

int dscratch_delete_turntable(int turntable_id)
{
    if (turntable_id > ((int)tab_turntable.size() - 1))
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                           "Cannot remove turntable at index "
                           + Utils::to_string(turntable_id));

        return 1;
    }

    // Delete Digital_scratch object from table of turntables.
    if (tab_turntable[turntable_id] == NULL)
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                           "No turntable at index "
                           + Utils::to_string(turntable_id));

        return 1;
    }
    delete tab_turntable[turntable_id];
    tab_turntable[turntable_id] = NULL;

    // All is OK.
    return 0;
}

int dscratch_analyze_recorded_datas(int     turntable_id,
                                    float  *input_samples_1,
                                    float  *input_samples_2,
                                    int     nb_frames)
{
    // If internal tables of samples are not enough large, enlarge them.
    if (g_input_samples_1.capacity() < (unsigned int)nb_frames)
    {
        g_input_samples_1.reserve(nb_frames);
        g_input_samples_2.reserve(nb_frames);
    }

    // Copy input samples in internal tables.
    g_input_samples_1.assign(input_samples_1, input_samples_1 + nb_frames);
    g_input_samples_2.assign(input_samples_2, input_samples_2 + nb_frames);

    // Amplify samples if needed.
    if (dscratch_get_input_amplify_coeff(turntable_id) > 1)
    {
        std::transform(g_input_samples_1.begin(), g_input_samples_1.end(),
                       g_input_samples_1.begin(),
                       std::bind1st(std::multiplies<float>(), dscratch_get_input_amplify_coeff(turntable_id)));
        std::transform(g_input_samples_2.begin(), g_input_samples_2.end(),
                       g_input_samples_2.begin(),
                       std::bind1st(std::multiplies<float>(), dscratch_get_input_amplify_coeff(turntable_id)));
    }

    // Analyze new samples.
    if (tab_turntable[turntable_id]->analyze_recording_data(g_input_samples_1,
                                                            g_input_samples_2) == false)
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                           "Cannot analyze recorded datas.");
        return 1;
    }

    return 0;
}

int dscratch_analyze_recorded_datas_interleaved(int    turntable_id,
                                                int    nb_channels,
                                                int    left_index,
                                                int    right_index,
                                                float *input_samples_interleaved,
                                                int    nb_frames)
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
    if (tab_turntable[turntable_id]->analyze_recording_data(g_input_samples_1,
                                                            g_input_samples_2) == false)
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                           "Cannot analyze interleaved recorded datas.");
        return 1;
    }

    return 0;
}


int dscratch_get_playing_parameters(int    turntable_id,
                                    float *speed,
                                    float *volume,
                                    float *position)
{
    if (tab_turntable[turntable_id]->get_playing_parameters(speed,
                                                            volume,
                                                            position) == false)
    {
        // Playing parameters not found.
        return 1;
    }

    return 0;
}

int dscratch_set_position_detection(int turntable_id,
                                    int enable_position)
{
    if (enable_position == 1)
    {
        if (tab_turntable[turntable_id]->enable_position_detection(true) == false)
        {
            Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                                       "Cannot enable position detection.");
            return 1;
        }
    }
    else
    {
        if (tab_turntable[turntable_id]->enable_position_detection(false) == false)
        {
            Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                                       "Cannot disable position detection.");
            return 1;
        }
    }

    return 0;
}

int dscratch_get_position_detection_state(int turntable_id)
{
    // Check turntable id.
    if ((turntable_id > ((int)tab_turntable.size() - 1))
         || (tab_turntable[turntable_id] == NULL))
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                           "Cannot access this turntable "
                           + Utils::to_string(turntable_id));

        return -1;
    }

    if (tab_turntable[turntable_id]->get_position_detection_state() == true)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void dscratch_print_trace_used()
{
    string error_trace_modules = "";

    #ifdef TRACE_ERROR
        error_trace_modules += "TRACE_ERROR ";
    #endif
    #ifdef TRACE_THREAD
        error_trace_modules += "TRACE_THREAD ";
    #endif
    #ifdef TRACE_OBJECT_LIFE
        error_trace_modules += "TRACE_OBJECT_LIFE ";
    #endif
    #ifdef TRACE_OBJECT_ATTRIBUTS_CHANGE
        error_trace_modules += "TRACE_OBJECT_ATTRIBUTS_CHANGE ";
    #endif
    #ifdef TRACE_MISC
        error_trace_modules += "TRACE_MISC ";
    #endif
    #ifdef TRACE_CAPTURE
        error_trace_modules += "TRACE_CAPTURE ";
    #endif
    #ifdef TRACE_CAPTURE_SAMPLE
        error_trace_modules += "TRACE_CAPTURE_SAMPLE ";
    #endif
    #ifdef TRACE_ANALYZE_VINYL
        error_trace_modules += "TRACE_ANALYZE_VINYL ";
    #endif
    #ifdef TRACE_ANALYZE_SWAVE
        error_trace_modules += "TRACE_ANALYZE_SWAVE ";
    #endif
    #ifdef TRACE_ANALYZE_EXTREME
        error_trace_modules += "TRACE_ANALYZE_EXTREME ";
    #endif
    #ifdef TRACE_POSITION
        error_trace_modules += "TRACE_POSITION ";
    #endif
    #ifdef TRACE_EXTREME_USED_FOR_DETECTING_SPEED
        error_trace_modules += "TRACE_EXTREME_USED_FOR_DETECTING_SPEED ";
    #endif

    Utils::trace_misc(TRACE_PREFIX_DIGITALSCRATCHAPI, "Tracing level : " + error_trace_modules);
}

int dscratch_display_turntable(int turntable_id)
{
    char *turntable_name = NULL;
    char *vinyl_name     = NULL;

    // Check turntable id.
    if (turntable_id > ((int)tab_turntable.size() - 1))
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                           "Cannot access this turntable "
                           + Utils::to_string(turntable_id));

        return 1;
    }

    // Check if turntable exists.
    if (tab_turntable[turntable_id] == NULL)
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                           "No turntable at index "
                           + Utils::to_string(turntable_id));

        return 1;
    }

    // General informations.
    dscratch_get_turntable_name(turntable_id, &turntable_name);
    cout << "turntable_id: " << turntable_id << endl;
    cout << " turntable_name            : " << turntable_name << endl;
    cout << " max_speed_diff            : " << dscratch_get_max_speed_diff(turntable_id) << endl;
    cout << " min_speed                 : " << dscratch_get_slow_speed_algo_usage(turntable_id) << endl;
    cout << " max_nb_buffer             : " << dscratch_get_max_nb_buffer(turntable_id) << endl;
    cout << " max_nb_speed_for_stability: " << dscratch_get_max_nb_speed_for_stability(turntable_id) << endl;

    // Specific stuff for FinalScratch vinyl.
    dscratch_get_vinyl_type(turntable_id, &vinyl_name);

    if (strcmp(vinyl_name, FINAL_SCRATCH_VINYL) == 0)
    {
        cout << " vinyl_type: final_scratch" << endl;
        cout << "  extreme_min                       : " << dscratch_get_extreme_min(turntable_id) << endl;
        cout << "  max_buffer_coeff                  : " << dscratch_get_max_buffer_coeff(turntable_id) << endl;
        cout << "  nb_cycle_before_changing_direction: " << dscratch_get_nb_cycle_before_changing_direction(turntable_id) << endl;
        cout << "  coeff_right_dist_min_bit1_to_bit1 : " << dscratch_get_coeff_right_dist_min_bit1_to_bit1(turntable_id) << endl;
        cout << "  coeff_left_dist_min_bit1_to_bit1  : " << dscratch_get_coeff_left_dist_min_bit1_to_bit1(turntable_id) << endl;
        cout << "  coeff_right_dist_max_bit0_to_bit0 : " << dscratch_get_coeff_right_dist_max_bit0_to_bit0(turntable_id) << endl;
        cout << "  coeff_left_dist_max_bit0_to_bit0  : " << dscratch_get_coeff_left_dist_max_bit0_to_bit0(turntable_id) << endl;
        cout << "  progressive_volume_coeff          : " << dscratch_get_progressive_volume_coeff(turntable_id) << endl;
        cout << "  full_volume_amplitude             : " << dscratch_get_full_volume_amplitude(turntable_id) << endl;
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

    return 0;
}

int dscratch_get_number_of_turntables()
{
    return tab_turntable.size();
}

const char *dscratch_get_version()
{
    return STR(VERSION);
}

int dscratch_get_turntable_name(int    turntable_id,
                                char **turntable_name)
{
    char *name = NULL;
    int   size = 0;

    // Check turntable_name.
    if (turntable_name == NULL)
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                            "Cannot get turntable name.");
        return 1;
    }

    // Check turntable id.
    if (turntable_id > ((int)tab_turntable.size() - 1))
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                           "Cannot access this turntable "
                           + Utils::to_string(turntable_id));

        return 1;
    }

    // Check if turntable exists.
    if (tab_turntable[turntable_id] == NULL)
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                           "No turntable at index "
                           + Utils::to_string(turntable_id));

        return 1;
    }

    // Get name from Controller.
    string controller_name = tab_turntable[turntable_id]->get_name();

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

    return 0;
}

int dscratch_get_vinyl_type(int    turntable_id,
                            char **vinyl_type)
{
    char        *vinyl_name = NULL;
    int          size       = 0;
    Coded_vinyl *cv         = NULL;

    // Check vinyl name.
    if (vinyl_type == NULL)
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                            "Cannot get vinyl type.");
        return 1;
    }

    // Check turntable id.
    if (turntable_id > ((int)tab_turntable.size() - 1))
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                           "Cannot access this turntable "
                           + Utils::to_string(turntable_id));

        return 1;
    }

    // Check if turntable exists.
    if (tab_turntable[turntable_id] == NULL)
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                           "No turntable at index "
                           + Utils::to_string(turntable_id));

        return 1;
    }

    // Get the type of Coded_vinyl pointed by Digital_scratch.
    cv = tab_turntable[turntable_id]->get_coded_vinyl();
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
            Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                            "Unknown timecoded vinyl type");

            return 1;
        }
    }
    else
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                            "Cannot access to coded vinyl.");

        return 1;
    }

    // Return vinyl type.
    *vinyl_type = vinyl_name;

    return 0;
}

DLLIMPORT const char* dscratch_get_default_vinyl_type()
{
    return SERATO_VINYL;
}


DLLIMPORT int dscratch_change_vinyl_type(int   turntable_id,
                                         char *vinyl_type)
{
    char *current_vinyl_type = NULL;

    // Check vinyl name.
    if (vinyl_type == NULL)
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                            "Cannot get vinyl type.");
        return 1;
    }

    // Check turntable id.
    if (turntable_id > ((int)tab_turntable.size() - 1))
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                           "Cannot access this turntable "
                           + Utils::to_string(turntable_id));

        return 1;
    }

    // Change vinyl if necessary.
    dscratch_get_vinyl_type(turntable_id, &current_vinyl_type);
    if (current_vinyl_type == NULL)
    {
        return 1;
    }
    if (strcmp(current_vinyl_type, vinyl_type) != 0)
    {
        if (tab_turntable[turntable_id]->change_coded_vinyl(Utils::to_string(vinyl_type)) == false)
        {
            return 1;
        }
    }

    // Cleanup.
    if (current_vinyl_type != NULL)
    {
        free(current_vinyl_type);
    }

    return 0;
}

/**** API functions: General motion detection configuration parameters ********/

DLLIMPORT int dscratch_set_max_speed_diff(int   turntable_id,
                                          float diff)
{
    // Check turntable id.
    if ((turntable_id > ((int)tab_turntable.size() - 1))
         || (tab_turntable[turntable_id] == NULL))
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                           "Cannot access this turntable "
                           + Utils::to_string(turntable_id));

        return 1;
    }

    // Set max_speed_diff parameter to Digital_scratch.
    if (tab_turntable[turntable_id]->set_max_speed_diff(diff) == false)
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                            "Cannot set max_speed_diff.");
        return 1;
    }

    return 0;
}

DLLIMPORT float dscratch_get_max_speed_diff(int turntable_id)
{
    // Check turntable id.
    if ((turntable_id > ((int)tab_turntable.size() - 1))
         || (tab_turntable[turntable_id] == NULL))
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                           "Cannot access this turntable "
                           + Utils::to_string(turntable_id));

        return 1;
    }

    return tab_turntable[turntable_id]->get_max_speed_diff();
}

DLLIMPORT float dscratch_get_default_max_speed_diff()
{
    return (float)DEFAULT_MAX_SPEED_DIFF;
}

DLLIMPORT int dscratch_set_slow_speed_algo_usage(int   turntable_id,
                                                 float speed)
{
    // Check turntable id.
    if ((turntable_id > ((int)tab_turntable.size() - 1))
         || (tab_turntable[turntable_id] == NULL))
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                           "Cannot access this turntable "
                           + Utils::to_string(turntable_id));

        return 1;
    }

    // Set max_slow_speed parameter to Digital_scratch.
    if (tab_turntable[turntable_id]->set_max_slow_speed(speed) == false)
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                            "Cannot set max_slow_speed.");
        return 1;
    }

    return 0;
}

DLLIMPORT float dscratch_get_slow_speed_algo_usage(int turntable_id)
{
    // Check turntable id.
    if ((turntable_id > ((int)tab_turntable.size() - 1))
         || (tab_turntable[turntable_id] == NULL))
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                           "Cannot access this turntable "
                           + Utils::to_string(turntable_id));

        return 1;
    }

    return tab_turntable[turntable_id]->get_max_slow_speed();
}

DLLIMPORT float dscratch_get_default_slow_speed_algo_usage()
{
    return DEFAULT_MAX_SLOW_SPEED;
}

DLLIMPORT int dscratch_set_max_nb_buffer(int turntable_id,
                                         int nb_buffer)
{
    // Check turntable id.
    if ((turntable_id > ((int)tab_turntable.size() - 1))
         || (tab_turntable[turntable_id] == NULL))
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                           "Cannot access this turntable "
                           + Utils::to_string(turntable_id));

        return 1;
    }

    // Set max_nb_buffer parameter to Digital_scratch.
    if (tab_turntable[turntable_id]->set_max_nb_buffer(nb_buffer) == false)
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                            "Cannot set max_nb_buffer.");
        return 1;
    }

    return 0;
}

DLLIMPORT int dscratch_get_max_nb_buffer(int turntable_id)
{
    // Check turntable id.
    if ((turntable_id > ((int)tab_turntable.size() - 1))
         || (tab_turntable[turntable_id] == NULL))
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                           "Cannot access this turntable "
                           + Utils::to_string(turntable_id));

        return 1;
    }

    return tab_turntable[turntable_id]->get_max_nb_buffer();
}

DLLIMPORT int dscratch_get_default_max_nb_buffer()
{
    return DEFAULT_MAX_NB_BUFFER;
}

DLLIMPORT int dscratch_set_max_nb_speed_for_stability(int turntable_id,
                                                      int nb_speed)
{
    // Check turntable id.
    if ((turntable_id > ((int)tab_turntable.size() - 1))
         || (tab_turntable[turntable_id] == NULL))
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                           "Cannot access this turntable "
                           + Utils::to_string(turntable_id));

        return 1;
    }

    // Set max_nb_speed_for_stability parameter to Digital_scratch.
    if (tab_turntable[turntable_id]->set_max_nb_speed_for_stability(nb_speed) == false)
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                            "Cannot set max_nb_speed_for_stability.");
        return 1;
    }

    return 0;
}

DLLIMPORT int dscratch_get_max_nb_speed_for_stability(int turntable_id)
{
    // Check turntable id.
    if ((turntable_id > ((int)tab_turntable.size() - 1))
         || (tab_turntable[turntable_id] == NULL))
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                           "Cannot access this turntable "
                           + Utils::to_string(turntable_id));

        return 1;
    }

    return tab_turntable[turntable_id]->get_max_nb_speed_for_stability();
}

DLLIMPORT int dscratch_get_default_max_nb_speed_for_stability()
{
    return DEFAULT_MAX_NB_SPEED_FOR_STABILITY;
}

DLLIMPORT int dscratch_set_extreme_min(int   turntable_id,
                                       float extreme_min)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = NULL;
    if (l_get_coded_vinyl(turntable_id, &vinyl) == false) return 1;

    // Set extreme_min parameter to Coded_vinyl.
    if (vinyl->set_extreme_min(extreme_min) == false)
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                            "Cannot set extreme_min.");
        return 1;
    }

    return 0;
}

DLLIMPORT float dscratch_get_extreme_min(int turntable_id)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = NULL;
    if (l_get_coded_vinyl(turntable_id, &vinyl) == false) return 0.0;

    // Get extreme_min parameter from Coded_vinyl.
    return vinyl->get_extreme_min();
}

DLLIMPORT float dscratch_get_default_extreme_min()
{
    return (float)DEFAULT_EXTREME_MIN;
}

DLLIMPORT int dscratch_set_max_buffer_coeff(int turntable_id,
                                  int max_buffer_coeff)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = NULL;
    if (l_get_coded_vinyl(turntable_id, &vinyl) == false) return 1;

    // Set max_buffer_coeff parameter to Coded_vinyl.
    if (vinyl->set_max_buffer_coeff(max_buffer_coeff) == false)
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                            "Cannot set max_buffer_coeff.");
        return 1;
    }

    return 0;
}

DLLIMPORT int dscratch_get_max_buffer_coeff(int turntable_id)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = NULL;
    if (l_get_coded_vinyl(turntable_id, &vinyl) == false) return 1;

    // Get max_buffer_coeff parameter from Coded_vinyl.
    return vinyl->get_max_buffer_coeff();
}

DLLIMPORT int dscratch_get_default_max_buffer_coeff()
{
    return DEFAULT_MAX_BUFFER_COEFF;
}

DLLIMPORT int dscratch_set_nb_cycle_before_changing_direction(int turntable_id,
                                                              int nb_cycle)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = NULL;
    if (l_get_coded_vinyl(turntable_id, &vinyl) == false) return 1;

    // Set nb_cycle_before_changing_direction parameter to Coded_vinyl.
    if (vinyl->set_nb_cycle_before_changing_direction(nb_cycle) == false)
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                            "Cannot set nb_cycle_before_changing_direction.");
        return 1;
    }

    return 0;
}

DLLIMPORT int dscratch_get_nb_cycle_before_changing_direction(int turntable_id)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = NULL;
    if (l_get_coded_vinyl(turntable_id, &vinyl) == false) return 1;

    // Get nb_cycle_before_changing_direction parameter from Coded_vinyl.
    return vinyl->get_nb_cycle_before_changing_direction();
}

DLLIMPORT int dscratch_get_default_nb_cycle_before_changing_direction()
{
    return DEFAULT_NB_CYCLE_BEFORE_CHANGING_DIRECTION;
}

DLLIMPORT int dscratch_set_coeff_right_dist_min_bit1_to_bit1(int   turntable_id,
                                                             float coeff)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = NULL;
    if (l_get_coded_vinyl(turntable_id, &vinyl) == false) return 1;

    // Set coeff_right_dist_min_bit1_to_bit1 parameter to Coded_vinyl.
    if (vinyl->set_coeff_right_dist_min_bit1_to_bit1(coeff) == false)
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                            "Cannot set coeff_right_dist_min_bit1_to_bit1.");
        return 1;
    }

    return 0;
}

DLLIMPORT float dscratch_get_coeff_right_dist_min_bit1_to_bit1(int turntable_id)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = NULL;
    if (l_get_coded_vinyl(turntable_id, &vinyl) == false) return 0.0;

    // Get coeff_right_dist_min_bit1_to_bit1 parameter from Coded_vinyl.
    return vinyl->get_coeff_right_dist_min_bit1_to_bit1();
}

DLLIMPORT float dscratch_get_default_coeff_right_dist_min_bit1_to_bit1()
{
    return (float)DEFAULT_COEFF_RIGHT_DIST_MIN_BIT1_TO_BIT1;
}

DLLIMPORT int dscratch_set_coeff_left_dist_min_bit1_to_bit1(int   turntable_id,
                                                            float coeff)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = NULL;
    if (l_get_coded_vinyl(turntable_id, &vinyl) == false) return 1;

    // Set coeff_left_dist_min_bit1_to_bit1 parameter to Coded_vinyl.
    if (vinyl->set_coeff_left_dist_min_bit1_to_bit1(coeff) == false)
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                            "Cannot set coeff_left_dist_min_bit1_to_bit1.");
        return 1;
    }

    return 0;
}

DLLIMPORT float dscratch_get_coeff_left_dist_min_bit1_to_bit1(int turntable_id)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = NULL;
    if (l_get_coded_vinyl(turntable_id, &vinyl) == false) return 0.0;

    // Get coeff_left_dist_min_bit1_to_bit1 parameter from Coded_vinyl.
    return vinyl->get_coeff_left_dist_min_bit1_to_bit1();
}

DLLIMPORT float dscratch_get_default_coeff_left_dist_min_bit1_to_bit1()
{
    return (float)DEFAULT_COEFF_LEFT_DIST_MIN_BIT1_TO_BIT1;
}

DLLIMPORT int dscratch_set_coeff_right_dist_max_bit0_to_bit0(int   turntable_id,
                                                             float coeff)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = NULL;
    if (l_get_coded_vinyl(turntable_id, &vinyl) == false) return 1;

    // Set right_dist_max_bit0_to_bit0 parameter to Coded_vinyl.
    if (vinyl->set_coeff_right_dist_max_bit0_to_bit0(coeff) == false)
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                            "Cannot set coeff_right_dist_max_bit0_to_bit0.");
        return 1;
    }

    return 0;
}

DLLIMPORT float dscratch_get_coeff_right_dist_max_bit0_to_bit0(int turntable_id)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = NULL;
    if (l_get_coded_vinyl(turntable_id, &vinyl) == false) return 0.0;

    // Get coeff_right_dist_max_bit0_to_bit0 parameter from Coded_vinyl.
    return vinyl->get_coeff_right_dist_max_bit0_to_bit0();
}

DLLIMPORT float dscratch_get_default_coeff_right_dist_max_bit0_to_bit0()
{
    return DEFAULT_COEFF_RIGHT_DIST_MAX_BIT0_TO_BIT0;
}

DLLIMPORT int dscratch_set_coeff_left_dist_max_bit0_to_bit0(int   turntable_id,
                                                            float coeff)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = NULL;
    if (l_get_coded_vinyl(turntable_id, &vinyl) == false) return 1;

    // Set coeff_left_dist_max_bit0_to_bit0 parameter to Coded_vinyl.
    if (vinyl->set_coeff_left_dist_max_bit0_to_bit0(coeff) == false)
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                            "Cannot set coeff_left_dist_max_bit0_to_bit0.");
        return 1;
    }

    return 0;
}

DLLIMPORT float dscratch_get_coeff_left_dist_max_bit0_to_bit0(int turntable_id)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = NULL;
    if (l_get_coded_vinyl(turntable_id, &vinyl) == false) return 0.0;

    // Get coeff_left_dist_max_bit0_to_bit0 parameter from Coded_vinyl.
    return vinyl->get_coeff_left_dist_max_bit0_to_bit0();
}

DLLIMPORT float dscratch_get_default_coeff_left_dist_max_bit0_to_bit0()
{
    return (float)DEFAULT_COEFF_LEFT_DIST_MAX_BIT0_TO_BIT0;
}

DLLIMPORT int dscratch_set_progressive_volume_coeff(int   turntable_id,
                                                    float coeff)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = NULL;
    if (l_get_coded_vinyl(turntable_id, &vinyl) == false) return 1;

    // Set progressive_volume_coeff to Coded_vinyl.
    if (vinyl->set_progressive_volume_coeff(coeff) == false)
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                            "Cannot set progressive_volume_coeff.");
        return 1;
    }

    return 0;
}

DLLIMPORT float dscratch_get_progressive_volume_coeff(int turntable_id)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = NULL;
    if (l_get_coded_vinyl(turntable_id, &vinyl) == false) return 0.0;

    // Get progressive_volume_coeff parameter from Coded_vinyl.
    return vinyl->get_progressive_volume_coeff();
}

DLLIMPORT float dscratch_get_default_progressive_volume_coeff()
{
    return DEFAULT_PROGRESSIVE_VOLUME_COEFF;
}

DLLIMPORT int dscratch_set_full_volume_amplitude(int   turntable_id,
                                                 float volume)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = NULL;
    if (l_get_coded_vinyl(turntable_id, &vinyl) == false) return 1;

    // Set full_volume_amplitude to Coded_vinyl.
    if (vinyl->set_full_volume_amplitude(volume) == false)
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                            "Cannot set full_volume_amplitude.");
        return 1;
    }

    return 0;
}

DLLIMPORT float dscratch_get_full_volume_amplitude(int turntable_id)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = NULL;
    if (l_get_coded_vinyl(turntable_id, &vinyl) == false) return 0.0;

    // Get full_volume_amplitude parameter from Coded_vinyl.
    return vinyl->get_full_volume_amplitude();
}

DLLIMPORT float dscratch_get_default_full_volume_amplitude()
{
    return (float)DEFAULT_FULL_VOLUME_AMPLITUDE;
}

DLLIMPORT int dscratch_set_low_pass_filter_max_speed_usage(int   turntable_id,
                                                           float low_pass_filter_max_speed_usage)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = NULL;
    if (l_get_coded_vinyl(turntable_id, &vinyl) == false) return 1;

    // Set low_pass_filter_max_speed_usage parameter to Coded_vinyl.
    if (vinyl->set_low_pass_filter_max_speed_usage(low_pass_filter_max_speed_usage) == false)
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                            "Cannot set low_pass_filter_max_speed_usage.");
        return 1;
    }

    return 0;
}

DLLIMPORT float dscratch_get_low_pass_filter_max_speed_usage(int turntable_id)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = NULL;
    if (l_get_coded_vinyl(turntable_id, &vinyl) == false) return 0.0;

    // Get low_pass_filter_max_speed_usage parameter from Coded_vinyl.
    return vinyl->get_low_pass_filter_max_speed_usage();
}

DLLIMPORT float dscratch_get_default_low_pass_filter_max_speed_usage()
{
    return (float)DEFAULT_LOW_PASS_FILTER_MAX_SPEED_USAGE;
}

DLLIMPORT int dscratch_set_input_amplify_coeff(int turntable_id,
                                               int coeff)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = NULL;
    if (l_get_coded_vinyl(turntable_id, &vinyl) == false) return 1;

    // Set input_amplify_coeff parameter to Coded_vinyl.
    if (vinyl->set_input_amplify_coeff(coeff) == false)
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                            "Cannot set input_amplify_coeff.");
        return 1;
    }

    return 0;
}

DLLIMPORT int dscratch_get_input_amplify_coeff(int turntable_id)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = NULL;
    if (l_get_coded_vinyl(turntable_id, &vinyl) == false) return 1;

    // Get input_amplify_coeff parameter from Coded_vinyl.
    return vinyl->get_input_amplify_coeff();
}

DLLIMPORT int dscratch_get_default_input_amplify_coeff()
{
    return DEFAULT_INPUT_AMPLIFY_COEFF;
}

DLLIMPORT int dscratch_set_rpm(int turntable_id,
                               unsigned short int rpm)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = NULL;
    if (l_get_coded_vinyl(turntable_id, &vinyl) == false) return 1;

    // Set turntable RPM.
    if (((rpm != RPM_33) && (rpm != RPM_45)) ||
       (vinyl->set_rpm(rpm) == false))
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCHAPI,
                            "Cannot set RPM.");
        return 1;
    }

    return 0;
}

DLLIMPORT unsigned short int dscratch_get_rpm(int turntable_id)
{
    // Get Coded_vinyl object.
    Coded_vinyl *vinyl = NULL;
    if (l_get_coded_vinyl(turntable_id, &vinyl) == false) return 1;

    // Get RPM parameter from Coded_vinyl.
    return vinyl->get_rpm();
}

DLLIMPORT unsigned short int dscratch_get_default_rpm()
{
    return DEFAULT_RPM;
}
