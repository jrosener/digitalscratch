/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                                    Tests                                   */
/*                                                                            */
/*                                                                            */
/*-------------------------------------------[ digital_scratch_api_test.cpp ]-*/
/*                                                                            */
/*  Copyright (C) 2003-2008                                                   */
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
/*                        Digital-scratch API Tests                           */
/*                Tests for Digital_scratch_api API functions                 */
/*                                                                            */
/*============================================================================*/

#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
using namespace std;

#include "include/test_utils.h"

// Include class to test.
#include <digital_scratch_api.h>

// Use Boost.Test
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

/**
 * Test #1 of dscratch_create_turntable().
 *
 * Test Description:
 *      - Call dscratch_create_turntable() with an empty name, it must fail.
 *      - Call dscratch_create_turntable() with name=NULL, it must fail.
 *      - Call dscratch_create_turntable() with an empty vinyl, it must fail.
 *      - Call dscratch_create_turntable() with vinyl=NULL, it must fail.
 *      - Call dscratch_create_turntable() with correct params, it must pass.
 *      - Check id, it must have changed.
 *      - Call dscratch_create_turntable() again, and check that id is different
 *        than first one.
 *      - Delete both turntables.
 */
BOOST_AUTO_TEST_CASE (test_digital_scratch_api_dscratch_create_turntable_1)
{
    int turntable_id_1 = -1;
    int turntable_id_2 = -1;

    // Try to create a turntable with wrong parameters.
    BOOST_CHECK_EQUAL(dscratch_create_turntable("",
                                                FINAL_SCRATCH_VINYL,
                                                &turntable_id_1), !0);
    BOOST_CHECK_EQUAL(dscratch_create_turntable(NULL,
                                                FINAL_SCRATCH_VINYL,
                                                &turntable_id_1), !0);
    BOOST_CHECK_EQUAL(dscratch_create_turntable("left_turntable",
                                                "",
                                                &turntable_id_1), !0);
    BOOST_CHECK_EQUAL(dscratch_create_turntable("left_turntable",
                                                NULL,
                                                &turntable_id_1), !0);

    // Create a turntable with correct parameters.
    BOOST_CHECK_EQUAL(dscratch_create_turntable("left_turntable",
                                                FINAL_SCRATCH_VINYL,
                                                &turntable_id_1), 0);

    // Check turntable_id_1.
    BOOST_CHECK(turntable_id_1 != -1);

    // Create again a turntable with correct parameters.
    BOOST_CHECK_EQUAL(dscratch_create_turntable("left_turntable",
                                                FINAL_SCRATCH_VINYL,
                                                &turntable_id_2), 0);

    // Check turntable_id_2.
    BOOST_CHECK(turntable_id_2 != -1);

    // Check turntable_id_2 against turntable_id_1.
    BOOST_CHECK(turntable_id_2 != turntable_id_1);

    // Cleanup.
    BOOST_CHECK_EQUAL(dscratch_delete_turntable(turntable_id_1), 0);
    BOOST_CHECK_EQUAL(dscratch_delete_turntable(turntable_id_2), 0);
}

/**
 * Test #2 of dscratch_create_turntable().
 *
 * Test Description:
 *      - Note: All calls of dscratch_create_turntable() are with correct params, so
 *        it must pass.
 *      - Call dscratch_create_turntable() and check if id0=0. (0)
 *      - Delete turntable id0.                                (x)
 *      - Call dscratch_create_turntable() and check if id0=0. (0)
 *      - Call dscratch_create_turntable() and check if id1=1. (0,1)
 *      - Call dscratch_create_turntable() and check if id2=2. (0,1,2)
 *      - Delete turntable id0.                                (x,1,2)
 *      - Delete turntable id1.                                (x,x,2)
 *      - Call dscratch_create_turntable() and check if id0=0. (0,x,2)
 *      - Delete turntable id0.                                (x,x,2)
 *      - Delete turntable id1, should fail.                   (x,x,2)
 *      - Delete turntable id2.                                (x,x,x)
 */
BOOST_AUTO_TEST_CASE (test_digital_scratch_api_dscratch_create_turntable_2)
{
    int id0 = -1;
    int id1 = -1;
    int id2 = -1;

    // Call dscratch_create_turntable() and check if id0=0. (0)
    BOOST_CHECK_EQUAL(dscratch_create_turntable("turntable0",
                                                FINAL_SCRATCH_VINYL,
                                                &id0), 0);
    BOOST_CHECK_EQUAL(id0, 0);

    // Delete turntable id0.                                (x)
    BOOST_CHECK_EQUAL(dscratch_delete_turntable(id0), 0);

    // Call dscratch_create_turntable() and check if id0=0. (0)
    BOOST_CHECK_EQUAL(dscratch_create_turntable("turntable0",
                                                FINAL_SCRATCH_VINYL,
                                                &id0), 0);
    BOOST_CHECK_EQUAL(id0, 0);

    // Call dscratch_create_turntable() and check if id1=1. (0,1)
    BOOST_CHECK_EQUAL(dscratch_create_turntable("turntable1",
                                                FINAL_SCRATCH_VINYL,
                                                &id1), 0);
    BOOST_CHECK_EQUAL(id1, 1);

    // Call dscratch_create_turntable() and check if id2=2. (0,1,2)
    BOOST_CHECK_EQUAL(dscratch_create_turntable("turntable2",
                                                FINAL_SCRATCH_VINYL,
                                                &id2), 0);
    BOOST_CHECK_EQUAL(id2, 2);

    // Delete turntable id0.                                (x,1,2)
    BOOST_CHECK_EQUAL(dscratch_delete_turntable(id0), 0);

    // Delete turntable id1.                                (x,x,2)
    BOOST_CHECK_EQUAL(dscratch_delete_turntable(id1), 0);

    // Call dscratch_create_turntable() and check if id0=0. (0,x,2)
    BOOST_CHECK_EQUAL(dscratch_create_turntable("turntable0",
                                                FINAL_SCRATCH_VINYL,
                                                &id0), 0);
    BOOST_CHECK_EQUAL(id0, 0);


    // Cleanup.

    // Delete turntable id0.                                (x,x,2)
    BOOST_CHECK_EQUAL(dscratch_delete_turntable(id0), 0);

    // Delete turntable id1, should fail.                   (x,x,2)
    BOOST_CHECK_EQUAL(dscratch_delete_turntable(id1), !0);

    // Delete turntable id2.                                (x,x,x)
    BOOST_CHECK_EQUAL(dscratch_delete_turntable(id2), 0);
}

/**
 * Test dscratch_analyze_recorded_datas() and dscratch_get_playing_parameters.
 *
 * Test Description:
 *      - Create a turntable with default parameters.
 *      - Enable position detection.
 *      - Call dscratch_analyze_recorded_datas() continously on next part of
 *        timecode. In the mean time check a little bit the quality of returned
 *        playing parameters.
 */
BOOST_AUTO_TEST_CASE (test_digital_scratch_api_dscratch_analyze_recorded_datas)
{
    int id = -1;

    // Create a turntable
    BOOST_CHECK_EQUAL(dscratch_create_turntable("turntable",
                                                FINAL_SCRATCH_VINYL,
                                                &id), 0);

    // Enable position detection.
    BOOST_CHECK_EQUAL(dscratch_set_position_detection(id, 1), 0);

    // Prepare table of samples.
    vector<float> full_tab_1;
    vector<float> full_tab_2;
    vector<int>   tab_index;
    BOOST_CHECK_EQUAL(l_read_input_samples_from_file(TIMECODE_FILE_1,
                                                     full_tab_1,
                                                     full_tab_2,
                                                     tab_index), 0);
    BOOST_CHECK(full_tab_1.size() > 0);

    // Provide several times next part of timecode to digital-scratch and get
    // playing parameters.
    vector<float> tab_1;
    vector<float> tab_2;
    int           start_index     = 0;
    float         speed           = 0.0;
    float         volume          = 0.0;
    float         position        = 0.0;
    bool          are_params_found = false;

    for (unsigned int i = 0; i < tab_index.size(); i++)
    {
        // Check dscratch_analyze_recorded_datas()
        l_select_samples(full_tab_1,
                         full_tab_2,
                         start_index,
                         tab_index[i] - 1,
                         tab_1,
                         tab_2);
        BOOST_CHECK_EQUAL(dscratch_analyze_recorded_datas(id,
                                                          &tab_1[0],
                                                          &tab_2[0],
                                                          (int)tab_1.size()), 0);
        start_index = tab_index[i];

        // Check if digital-scratch was able to find playing parameters.
        are_params_found = dscratch_get_playing_parameters(id,
                                                           &speed,
                                                           &volume,
                                                           &position);
        if (are_params_found == true)
        {
            /*cout << "\nspeed=" << speed
                 << "\t" << "volume=" << volume
                 << "\t" << "position=" << position
                 << endl;*/

            // Speed diff should not be more than 2%.
            BOOST_CHECK_CLOSE(speed, (float)1.0, 2.0);
        }
    }

    // Cleanup.
    BOOST_CHECK_EQUAL(dscratch_delete_turntable(id), 0);
}

/**
 * Test dscratch_analyze_recorded_datas_interleaved() and dscratch_get_playing_parameters.
 *
 * Test Description:
 *      - Same as test_digital_scratch_api_dscratch_analyze_recorded_datas but
 *        by providing interleaved input datas.
 */
BOOST_AUTO_TEST_CASE (test_digital_scratch_api_dscratch_analyze_recorded_datas_interleaved)
{
    int id = -1;

    // Create a turntable
    BOOST_CHECK_EQUAL(dscratch_create_turntable("turntable",
                                                FINAL_SCRATCH_VINYL,
                                                &id), 0);

    // Enable position detection.
    BOOST_CHECK_EQUAL(dscratch_set_position_detection(id, 1), 0);

    // Prepare table of samples.
    vector<float> full_tab_1;
    vector<float> full_tab_2;
    vector<int>   tab_index;
    BOOST_CHECK_EQUAL(l_read_input_samples_from_file(TIMECODE_FILE_1,
                                                     full_tab_1,
                                                     full_tab_2,
                                                     tab_index), 0);
    BOOST_CHECK(full_tab_1.size() > 0);

    // Provide several times next part of timecode to digital-scratch and get
    // playing parameters.
    vector<float> tab_1;
    vector<float> tab_2;
    vector<float> tab_interleaved;
    int           start_index     = 0;
    float         speed           = 0.0;
    float         volume          = 0.0;
    float         position        = 0.0;
    bool          are_params_found = false;

    for (unsigned int i = 0; i < tab_index.size(); i++)
    {
        // Prepare a set of samples.
        l_select_samples(full_tab_1,
                         full_tab_2,
                         start_index,
                         tab_index[i] - 1,
                         tab_1,
                         tab_2);
        for (unsigned int j = 0; j < tab_1.size(); j++)
        {
            tab_interleaved.push_back(tab_1[j]);
            tab_interleaved.push_back(tab_2[j]);
            tab_interleaved.push_back(0.0);
            tab_interleaved.push_back(0.0);
        }

        // Check dscratch_analyze_recorded_datas_interleaved()
        BOOST_CHECK_EQUAL(dscratch_analyze_recorded_datas_interleaved(id,
                                                                      4,
                                                                      0,
                                                                      1,
                                                                      &tab_interleaved[0],
                                                                      (int)tab_1.size()), 0);
        start_index = tab_index[i];

        // Check if digital-scratch was able to find playing parameters.
        are_params_found = dscratch_get_playing_parameters(id,
                                                           &speed,
                                                           &volume,
                                                           &position);
        if (are_params_found == true)
        {
            /*cout << "\nspeed=" << speed
                 << "\t" << "volume=" << volume
                 << "\t" << "position=" << position
                 << endl;*/

            // Speed diff should not be more than 2%.
            BOOST_CHECK_CLOSE(speed, (float)1.0, 2.0);
        }

        // Clean tables of input samples.
        tab_1.clear();
        tab_2.clear();
        tab_interleaved.clear();
    }

    // Cleanup.
    BOOST_CHECK_EQUAL(dscratch_delete_turntable(id), 0);
}

/**
 * Test final_scratch_get_extreme_min() and final_scratch_set_extreme_min().
 *
 * Test Description:
 *      - Create a turntable.
 *      - Get current value.
 *      - Try to set wrong value.
 *      - Set the same value+1.
 *      - Check if the result is value+1.
 */
BOOST_AUTO_TEST_CASE (test_digital_scratch_api_final_scratch_set_extreme_min)
{
    int   id  = -1;
    float val = 0.0;
    float inc = 1.0;

    // Create a turntable
    BOOST_CHECK_EQUAL(dscratch_create_turntable("turntable",
                                                FINAL_SCRATCH_VINYL,
                                                &id), 0);

    // Get default value.
    val = final_scratch_get_extreme_min(id);

    // Try to set a negative value.
    BOOST_CHECK_EQUAL(final_scratch_set_extreme_min(id, -val), 1);
    BOOST_CHECK_EQUAL(final_scratch_get_extreme_min(id), val);

    // Set value + 1
    BOOST_CHECK_EQUAL(final_scratch_set_extreme_min(id, val+inc), 0);
    BOOST_CHECK_CLOSE(final_scratch_get_extreme_min(id), val+inc, 0.1);

    // Cleanup.
    BOOST_CHECK_EQUAL(dscratch_delete_turntable(id), 0);
}

/**
 * Test dscratch_get_version().
 *
 * Test Description:
 *      - Ask version of digital-scratch and check it.
 */
BOOST_AUTO_TEST_CASE (test_digital_scratch_api_dscratch_get_version)
{
    char *version = NULL;

    BOOST_CHECK_EQUAL(dscratch_get_version(&version), 0);
    BOOST_CHECK_EQUAL(version, "2.0.0");

    // Cleanup.
    if (version != NULL)
    {
        free(version);
    }
}

/**
 * Test dscratch_display_turntable().
 *
 * Test Description:
 *      - Create a turntable with basic parameters.
 *      - Display information about the turntable (to be checked manually).
 */
BOOST_AUTO_TEST_CASE (test_digital_scratch_api_dscratch_display_turntable)
{
    int id = -1;

    // Create a turntable
    BOOST_CHECK_EQUAL(dscratch_create_turntable("turntable",
                                                FINAL_SCRATCH_VINYL,
                                                &id), 0);

#if 0 // Enable if you want to check result manually.
    // Display informations about the turntable.
    BOOST_CHECK_EQUAL(dscratch_display_turntable(id), 0);
#endif

    // Cleanup.
    BOOST_CHECK_EQUAL(dscratch_delete_turntable(id), 0);
}

/**
 * Test dscratch_get_turntable_name().
 *
 * Test Description:
 *      - Create a turntable with basic parameters.
 *      - Get turntable name and check it.
 */
BOOST_AUTO_TEST_CASE (test_digital_scratch_api_dscratch_get_turntable_name)
{
    char *name = NULL;
    int   id   = -1;

    // Create a turntable
    BOOST_CHECK_EQUAL(dscratch_create_turntable("turntable",
                                                FINAL_SCRATCH_VINYL,
                                                &id), 0);

    BOOST_CHECK_EQUAL(dscratch_get_turntable_name(id, &name), 0);
    BOOST_CHECK_EQUAL(name, "turntable");

    // Cleanup.
    if (name != NULL)
    {
        free(name);
    }
    BOOST_CHECK_EQUAL(dscratch_delete_turntable(id), 0);
}

/**
 * Test dscratch_get_vinyl_type().
 *
 * Test Description:
 *      - Create a turntable with basic parameters.
 *      - Check if vinyl type is final scratchh.
 */
BOOST_AUTO_TEST_CASE (test_digital_scratch_api_dscratch_get_vinyl_type)
{
    char *vinyl = NULL;
    int   id    = -1;

    // Create a turntable
    BOOST_CHECK_EQUAL(dscratch_create_turntable("turntable",
                                                FINAL_SCRATCH_VINYL,
                                                &id), 0);

    BOOST_CHECK_EQUAL(dscratch_get_vinyl_type(id, &vinyl), 0);
    BOOST_CHECK_EQUAL(vinyl, FINAL_SCRATCH_VINYL);

    // Cleanup.
    if (vinyl != NULL)
    {
        free(vinyl);
    }
    BOOST_CHECK_EQUAL(dscratch_delete_turntable(id), 0);
}

/**
 * Test dscratch_set_max_speed_diff() and dscratch_get_max_speed_diff().
 *
 * Test Description:
 *      - Create a turntable.
 *      - Get current value.
 *      - Try to set wrong value.
 *      - Set the same value+1.
 *      - Check if the result is value+1.
 */
BOOST_AUTO_TEST_CASE (test_digital_scratch_api_dscratch_set_max_speed_diff)
{
    int   id  = -1;
    float val = 0.0;
    float inc = 1.0;

    // Create a turntable
    BOOST_CHECK_EQUAL(dscratch_create_turntable("turntable",
                                                FINAL_SCRATCH_VINYL,
                                                &id), 0);

    // Get default value.
    val = dscratch_get_max_speed_diff(id);

    // Try to set a negative value.
    BOOST_CHECK_EQUAL(dscratch_set_max_speed_diff(id, -val), 1);
    BOOST_CHECK_EQUAL(dscratch_get_max_speed_diff(id), val);

    // Set value + 1
    BOOST_CHECK_EQUAL(dscratch_set_max_speed_diff(id, val+inc), 0);
    BOOST_CHECK_CLOSE(dscratch_get_max_speed_diff(id), val+inc, 0.1);

    // Cleanup.
    BOOST_CHECK_EQUAL(dscratch_delete_turntable(id), 0);
}
