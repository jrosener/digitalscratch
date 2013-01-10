/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                               Scenario tests                               */
/*                                                                            */
/*                                                                            */
/*--------------------------------------------------------[ scenario_test.c ]-*/
/*                                                                            */
/*  Copyright (C) 2003-2007                                                   */
/*                Julien Rosener <julien.rosener@digital-scratch.org>         */
/*                                                                            */
/*----------------------------------------------------------------( License )-*/
/*                                                                            */
/*  This program is free software; you can redistribute it and/or modify      */
/*  it under the terms of the GNU General Public License as published by      */
/*  the Free Software Foundation; either version 2 of the License, or         */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              */
/*  GNU General Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with this program; if not, write to the Free Software               */
/*  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA */
/*                                                                            */
/*------------------------------------------------------------( Description )-*/
/*                                                                            */
/*                               Scenario tests                               */
/*                                                                            */
/*============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <digital_scratch_api.h>

#define CONFIG_FILE_PATH          "./config_file/.config_file_ok"
#define TURNTABLE_NAME_1          "left_turntable"
#define TURNTABLE_NAME_2          "right_turntable"
#define FREQUENCY                 "44100"
#define BUFFER_SIZE               "225"
#define MAX_NB_NO_NEW_SPEED_FOUND "1"
#define NB_CYCLE_BEFORE_STARTING  "1"
#define VINYL_TYPE                "final_scratch"
#define VINYL_PARAM_1             "0.01"
#define VINYL_PARAM_2             "3"
#define VINYL_PARAM_3             "1"
#define VINYL_PARAM_4             "0.90"
#define VINYL_PARAM_5             "0.70"
#define VINYL_PARAM_6             "0.50"
#define VINYL_PARAM_7             "0.40"
#define VINYL_PARAM_8             "1000.0"
#define VINYL_PARAM_9             "0.30"


void callback_change_playback_properties(double speed,
                                         double volume,
                                         double position,
                                         int    *is_position_enabled,
                                         double *track_length,
                                         double *current_position,
                                         void   *arg)
{
    // Change speed.
    if (speed != NO_NEW_SPEED_FOUND)
    {
        printf("speed: %.2f\t", speed);
    }

    // Change volume.
    if (volume != NO_NEW_VOLUME_FOUND)
    {
        printf("volume: %.2f\t", volume);
    }

    // Change position.
    if (position != NO_NEW_POSITION_FOUND)
    {
        printf("position: %.2f", position);
    }

    printf("\r");

    // Send position system status.
    *is_position_enabled = 1;

    // Send track length to digital-scratch.
    *track_length = 245; // seconds

    // Send current position to digital-scratch.
    *current_position = 123; // seconds
}

int scenario_test_01()
{
    // Change to 1 if at least one test fail.
    int test_error = 0;

    // Variables.
    dscratch_turntable_t* turntable = NULL;
    void*                 dummy_arg; // Used for callback.

    // Print current test name.
    printf("Scenario_test_01()...\n");

    // Call dscratch_create_turntable_from_config_file() with correct
    // parameters.
    if ((dscratch_create_turntable_from_config_file(TURNTABLE_NAME_1,
                                                   &turntable,
                                                   CONFIG_FILE_PATH) == 1)
        || (turntable == NULL)
        || (dscratch_get_turntable_number() == 0))
    {
        printf("TEST ERROR: dscratch_create_turntable_from_config_file() should works because it is called with correct parameters.\n");
        test_error++;
    }

    // Register callback used each time digital-scratch will provide new
    // playback properties.
    if (dscratch_register_change_playback_properties_callback_function(turntable,
                                                                       callback_change_playback_properties,
                                                                       dummy_arg) == 1)
    {
        printf("TEST ERROR: dscratch_register_change_playback_properties_callback_function() should works because it is called with correct parameters.\n");
        test_error++;
    }

    // Start motion detection.
    if (dscratch_start(turntable) == 1)
    {
        printf("TEST ERROR: dscratch_start() should works because it is called with correct parameters.\n");
        test_error++;
    }

// TODO: create a thread, in the routine: sleep(5) + dscratch_stop().
pthread_exit(NULL);


    // Stop motion detection.
    if (dscratch_stop(turntable) == 1)
    {
        printf("TEST ERROR: dscratch_stop() should works because it is called with correct parameters.\n");
        test_error++;
    }

    // Cleanup.
    if (dscratch_delete_turntable(&turntable) != 0)
    {
        printf("TEST ERROR: dscratch_delete_turntable() should work because turntable should be OK.\n");
        test_error++;
    }

    // Return code.
    if (test_error != 0)
    {
        return test_error;
    }
    else
    {
        return 0;
    }
}

int scenario_test_02()
{
    // Change to 1 if at least one test fail.
    int test_error = 0;

    // Variables.
    dscratch_turntable_t* turntable = NULL;
    void *dummy_arg; // Used for change paramters callback.
    void (*dscratch_analyze_datas_callback) (void*, double*, double*); // Send capture datas callback address.
    void *dscratch_callback_handle = NULL; // Used for send capture datas callback.

    // Print current test name.
    printf("Scenario_test_02()...\n");

    // Call dscratch_create_turntable_from_config_file() with correct
    // parameters.
    if ((dscratch_create_turntable_with_external_config(TURNTABLE_NAME_1,
                                                        FREQUENCY,
                                                        BUFFER_SIZE,
                                                        MAX_NB_NO_NEW_SPEED_FOUND,
                                                        NB_CYCLE_BEFORE_STARTING,
                                                        VINYL_TYPE,
                                                        VINYL_PARAM_1,
                                                        VINYL_PARAM_2,
                                                        VINYL_PARAM_3,
                                                        VINYL_PARAM_4,
                                                        VINYL_PARAM_5,
                                                        VINYL_PARAM_6,
                                                        VINYL_PARAM_7,
                                                        VINYL_PARAM_8,
                                                        VINYL_PARAM_9,
                                                        &turntable) == 1)
        || (turntable == NULL)
        || (dscratch_get_turntable_number() == 0))
    {
        printf("TEST ERROR: dscratch_create_turntable_with_external_config() should works because it is called with correct parameters.\n");
        test_error++;
    }

    // Register callback used each time digital-scratch will provide new
    // playback properties.
    if (dscratch_register_change_playback_properties_callback_function(turntable,
                                                                       callback_change_playback_properties,
                                                                       dummy_arg) == 1)
    {
        printf("TEST ERROR: dscratch_register_change_playback_properties_callback_function() should works because it is called with correct parameters.\n");
        test_error++;
    }

    // Start motion detection.
    if (dscratch_start(turntable) == 1)
    {
        printf("TEST ERROR: dscratch_start() should works because it is called with correct parameters.\n");
        test_error++;
    }

    // Get callback used to send captured datas to digitalscratch.
    if (dscratch_get_analyze_datas_callback(turntable,
                                            dscratch_analyze_datas_callback,
                                            &dscratch_callback_handle) != 0)
    {
        printf("TEST ERROR: dscratch_get_analyze_datas_callback() should works because it is called with correct parameters.\n");
        test_error++;
    }



// TODO: create a thread, in the routine: sleep(5) + dscratch_stop().
pthread_exit(NULL);


    // Stop motion detection.
    if (dscratch_stop(turntable) == 1)
    {
        printf("TEST ERROR: dscratch_stop() should works because it is called with correct parameters.\n");
        test_error++;
    }

    // Cleanup.
    if (dscratch_delete_turntable(&turntable) != 0)
    {
        printf("TEST ERROR: dscratch_delete_turntable() should work because turntable should be OK.\n");
        test_error++;
    }

    // Return code.
    if (test_error != 0)
    {
        return test_error;
    }
    else
    {
        return 0;
    }
}
