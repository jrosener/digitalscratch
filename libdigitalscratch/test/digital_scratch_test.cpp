/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                                    Tests                                   */
/*                                                                            */
/*                                                                            */
/*-----------------------------------------------[ digital_scratch_test.cpp ]-*/
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
/*                        Digital-scratch Tests                               */
/*               Unit tests for Digital_scratch class                         */
/*                                                                            */
/*============================================================================*/

#include <string>
#include <vector>
#include <iostream>
using namespace std;

#include "test_utils.h"

// Include class to test.
#include <digital_scratch.h>

// Use Boost.Test
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>


/**
 * Test Digital_scratch::Digital_scratch().
 *
 * Test Description:
 *      - Create a Digital_scratch object with valid parameters.
 *      - Check initialization of all public parameters.
 */
BOOST_AUTO_TEST_CASE (test_digital_scratch_constructor)
{
    // Create Digital_scratch.
    Digital_scratch *dscratch = new Digital_scratch("left_turntable",
                                                    FINAL_SCRATCH_VINYL);

    // Check initilization of all public parameters.
    BOOST_CHECK_CLOSE(dscratch->get_max_speed_diff(), (float)DEFAULT_MAX_SPEED_DIFF, 0.1);
    BOOST_CHECK_CLOSE(dscratch->get_max_slow_speed(), (float)DEFAULT_MAX_SLOW_SPEED, 0.1);
    BOOST_CHECK_EQUAL(dscratch->get_max_nb_buffer(), DEFAULT_MAX_NB_BUFFER);
    BOOST_CHECK_EQUAL(dscratch->get_max_nb_speed_for_stability(), DEFAULT_MAX_NB_SPEED_FOR_STABILITY);

    // Cleanup.
    delete dscratch;
}

/**
 * Test Digital_scratch::set_max_nb_buffer().
 *
 * Test Description:
 *      - Create a Digital_scratch object with valid parameters.
 *      - Check if set_max_nb_buffer(0) return false and if the value is not
 *        set.
 *      - Check if set_max_nb_buffer(-99) return false and if the value is not
 *        set.
 *      - Check if set_max_nb_buffer(10) return true and if the value is set.
 */
BOOST_AUTO_TEST_CASE (test_digital_scratch_set_max_nb_buffer)
{
    // Create Digital_scratch.
    Digital_scratch *dscratch = new Digital_scratch("left_turntable",
                                                    FINAL_SCRATCH_VINYL);

    // Check set_max_nb_buffer().
    BOOST_CHECK_EQUAL(dscratch->set_max_nb_buffer(0), false);
    BOOST_CHECK_EQUAL(dscratch->get_max_nb_buffer(), DEFAULT_MAX_NB_BUFFER);
    BOOST_CHECK_EQUAL(dscratch->set_max_nb_buffer(-99), false);
    BOOST_CHECK_EQUAL(dscratch->get_max_nb_buffer(), DEFAULT_MAX_NB_BUFFER);
    BOOST_CHECK_EQUAL(dscratch->set_max_nb_buffer(10), true);
    BOOST_CHECK_EQUAL(dscratch->get_max_nb_buffer(), 10);

    // Cleanup.
    delete dscratch;
}

/**
 * Test Digital_scratch::set_max_nb_speed_for_stability().
 *
 * Test Description:
 *      - Create a Digital_scratch object with valid parameters.
 *      - Check if set_max_nb_speed_for_stability(0) return false and if the
 *        value is not set.
 *      - Check if set_max_nb_speed_for_stability(-99) return false and if the
 *        value is not set.
 *      - Check if set_max_nb_speed_for_stability(10) return true and if the
 *        value is set.
 *      - Check again if set_max_nb_speed_for_stability(8) return true and if
 *        the value is set (check if memory free/allocation are working well).
 */
BOOST_AUTO_TEST_CASE (test_digital_scratch_set_max_nb_speed_for_stability)
{
    // Create Digital_scratch.
    Digital_scratch *dscratch = new Digital_scratch("left_turntable",
                                                    FINAL_SCRATCH_VINYL);

    // Check set_max_nb_speed_for_stability().
    BOOST_CHECK_EQUAL(dscratch->set_max_nb_speed_for_stability(0), false);
    BOOST_CHECK_EQUAL(dscratch->get_max_nb_speed_for_stability(), DEFAULT_MAX_NB_SPEED_FOR_STABILITY);
    BOOST_CHECK_EQUAL(dscratch->set_max_nb_speed_for_stability(-99), false);
    BOOST_CHECK_EQUAL(dscratch->get_max_nb_speed_for_stability(), DEFAULT_MAX_NB_SPEED_FOR_STABILITY);
    BOOST_CHECK_EQUAL(dscratch->set_max_nb_speed_for_stability(10), true);
    BOOST_CHECK_EQUAL(dscratch->get_max_nb_speed_for_stability(), 10);
    BOOST_CHECK_EQUAL(dscratch->set_max_nb_speed_for_stability(8), true);
    BOOST_CHECK_EQUAL(dscratch->get_max_nb_speed_for_stability(), 8);

    // Cleanup.
    delete dscratch;
}

/**
 * Test Digital_scratch::set_max_slow_speed().
 *
 * Test Description:
 *      - Create a Digital_scratch object with valid parameters.
 *      - Check if set_max_slow_speed(0.0) return false and if the value is not
 *        set.
 *      - Check if set_max_slow_speed(-99.0) return false and if the value is
 *        not set.
 *      - Check if set_max_slow_speed(10.2) return true and if the value is set.
 */
BOOST_AUTO_TEST_CASE (test_digital_scratch_set_max_slow_speed)
{
    // Create Digital_scratch.
    Digital_scratch *dscratch = new Digital_scratch("left_turntable",
                                                    FINAL_SCRATCH_VINYL);

    // Check set_max_slow_speed()
    BOOST_CHECK_EQUAL(dscratch->set_max_slow_speed(0), false);
    BOOST_CHECK_EQUAL(dscratch->get_max_slow_speed(), DEFAULT_MAX_SLOW_SPEED);
    BOOST_CHECK_EQUAL(dscratch->set_max_slow_speed(-99), false);
    BOOST_CHECK_EQUAL(dscratch->get_max_slow_speed(), DEFAULT_MAX_SLOW_SPEED);
    BOOST_CHECK_EQUAL(dscratch->set_max_slow_speed(10.2), true);
    BOOST_CHECK_CLOSE(dscratch->get_max_slow_speed(), (float)10.2, 0.1);

    // Cleanup.
    delete dscratch;
}

/**
 * Test Digital_scratch::set_max_speed_diff().
 *
 * Test Description:
 *      - Create a Digital_scratch object with valid parameters.
 *      - Check if set_max_speed_diff(0.0) return false and if the value is not
 *        set.
 *      - Check if set_max_speed_diff(-99.0) return false and if the value is
 *        not set.
 *      - Check if set_max_speed_diff(10.2) return true and if the value is set.
 */
BOOST_AUTO_TEST_CASE (test_digital_scratch_set_max_speed_diff)
{
    // Create Digital_scratch.
    Digital_scratch *dscratch = new Digital_scratch("left_turntable",
                                                    FINAL_SCRATCH_VINYL);

    // Check set_max_speed_diff()
    BOOST_CHECK_EQUAL(dscratch->set_max_speed_diff(0), false);
    BOOST_CHECK_CLOSE(dscratch->get_max_speed_diff(), (float)DEFAULT_MAX_SPEED_DIFF, 0.1);
    BOOST_CHECK_EQUAL(dscratch->set_max_speed_diff(-99), false);
    BOOST_CHECK_CLOSE(dscratch->get_max_speed_diff(), (float)DEFAULT_MAX_SPEED_DIFF, 0.1);
    BOOST_CHECK_EQUAL(dscratch->set_max_speed_diff(10.2), true);
    BOOST_CHECK_CLOSE(dscratch->get_max_speed_diff(), (float)10.2, 0.1);

    // Cleanup.
    delete dscratch;
}

/**
 * Test Digital_scratch::analyze_recording_data().
 * This test is not dedicated to test the quality of playing parameters values
 * but only the behavior of analyze_recording_datas() against input paramaters.
 *
 * Test Description:
 *      - Create a Digital_scratch object with valid parameters.
 *      - Check if analyze_recording_datas() with empty size return false.
 *      - Check if analyze_recording_datas() with tabs with different sizes
          return false.
 *      - Check if analyze_recording_datas(tab_1, tab_2, 5) return true (tab_1
 *        and tab_2 well defined and nb_frames=nb_elem(tab_1 or tab_2).
 */
BOOST_AUTO_TEST_CASE (test_digital_scratch_analyze_recording_data_1)
{
    // Create Digital_scratch.
    Digital_scratch *dscratch = new Digital_scratch("left_turntable",
                                                    FINAL_SCRATCH_VINYL);

    // Check analyze_recording_data()
    vector<float> tab_1;
    vector<float> tab_2;
    BOOST_CHECK_EQUAL(dscratch->analyze_recording_data(tab_1, tab_2), false);

    tab_1.push_back(10); tab_1.push_back(20);
    tab_2.push_back(10);
    BOOST_CHECK_EQUAL(dscratch->analyze_recording_data(tab_1, tab_2), false);

    tab_1.clear();
    tab_2.clear();
    l_create_default_input_samples(tab_1, tab_2);
    BOOST_CHECK(tab_1.size() > 0);
    BOOST_CHECK(tab_2.size() > 0);
    BOOST_CHECK_EQUAL(dscratch->analyze_recording_data(tab_1, tab_2), true);

    // Cleanup.
    delete dscratch;
}

/**
 * Test Digital_scratch::analyze_recording_data().
 * This test is not dedicated to test the quality of playing parameters values
 * but only the behavior of analyze_recording_datas() against input paramaters.
 *
 * Test Description:
 *      - Create a Digital_scratch object with valid parameters.
 *      - Get buffers of samples from a file.
 *      - Check if analyze_recording_datas() return true.
 *      - Check if get_playing_parameters() return true (it will mean that
 *        playing parameters are found).
 */
BOOST_AUTO_TEST_CASE (test_digital_scratch_analyze_recording_data_2)
{
    // Create Digital_scratch.
    Digital_scratch *dscratch = new Digital_scratch("left_turntable",
                                                    FINAL_SCRATCH_VINYL);

    // Prepare table of samples.
    vector<float> full_tab_1;
    vector<float> full_tab_2;
    vector<int>   tab_index;
    BOOST_CHECK_EQUAL(l_read_input_samples_from_file(TIMECODE_FILE_1,
                                                     full_tab_1,
                                                     full_tab_2,
                                                     tab_index), 0);
    BOOST_CHECK(full_tab_1.size() > 0);

    // Check analyze_recording_data()
    vector<float> tab_1;
    vector<float> tab_2;
    l_select_samples(full_tab_1,
                     full_tab_2,
                     0,
                     tab_index[0],
                     tab_1,
                     tab_2);
    BOOST_CHECK_EQUAL(dscratch->analyze_recording_data(tab_1, tab_2), true);

    // Check if digital-scratch was able to find playing parameters.
    float speed    = 0.0;
    float volume   = 0.0;
    float position = 0.0;
    BOOST_CHECK_EQUAL(dscratch->get_playing_parameters(&speed,
                                                       &volume,
                                                       &position), true);

//cout << "speed=" << speed << "\t" << "volume=" << volume << "\t" << "position=" << position << endl;

    // Cleanup.
    delete dscratch;
}

/**
 * Test Digital_scratch::analyze_recording_data().
 * This test is not dedicated to test the quality of playing parameters values
 * but only the behavior of analyze_recording_datas() against input paramaters.
 *
 * Test Description:
 *      - Create a Digital_scratch object with valid parameters.
 *      - Get buffers of samples from a file.
 *      - Check if analyze_recording_datas() return true.
 *      - Check if get_playing_parameters() return true (it will mean that
 *        playing parameters are found).
 *      - Do same calls again with next buffers of samples.
 */
BOOST_AUTO_TEST_CASE (test_digital_scratch_analyze_recording_data_3)
{
    // Create Digital_scratch.
    Digital_scratch *dscratch = new Digital_scratch("left_turntable",
                                                    FINAL_SCRATCH_VINYL);

    // Enable detection of position.
    BOOST_CHECK_EQUAL(dscratch->enable_position_detection(true), true);
    BOOST_CHECK_EQUAL(dscratch->get_position_detection_state(), true);

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
        // Check analyze_recording_data()
        l_select_samples(full_tab_1,
                         full_tab_2,
                         start_index,
                         tab_index[i] - 1,
                         tab_1,
                         tab_2);
        BOOST_CHECK_EQUAL(dscratch->analyze_recording_data(tab_1, tab_2), true);
        start_index = tab_index[i];

        // Check if digital-scratch was able to find playing parameters.
        are_params_found = dscratch->get_playing_parameters(&speed,
                                                            &volume,
                                                            &position);
        if (are_params_found == true)
        {
            /*cout << "\nspeed=" << speed
                 << "\t" << "volume=" << volume
                 << "\t" << "position=" << position
                 << endl;*/

            // Speed diff should not be more than 4%.
            BOOST_CHECK_CLOSE(speed, (float)1.0, 4.0);
        }
    }

    // Cleanup.
    delete dscratch;
}
