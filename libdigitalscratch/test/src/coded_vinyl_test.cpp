/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                                    Tests                                   */
/*                                                                            */
/*                                                                            */
/*---------------------------------------------------[ coded_vinyl_test.cpp ]-*/
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
/*                        Digital-scratch Tests                               */
/*                   Unit tests for Coded_vinyl class                         */
/*                                                                            */
/*============================================================================*/

#include <iostream>
using namespace std;

// Include class to test.
#include <speed.h>

// Use Boost.Test
#include <boost/test/auto_unit_test.hpp>

/**
 * Test Speed:Speed().
 *
 * Test Description:
 *      - Create a Speed object using an harcoded turntable name.
 *      - Check if turntable name was correctly stored.
 *      - Check if value is correctly initialized.
 */
BOOST_AUTO_TEST_CASE (test_speed_constructor)
{
    Speed *speed = new Speed("turntable_name");

    BOOST_CHECK_EQUAL(speed->get_turntable_name(), "turntable_name");
    BOOST_CHECK_EQUAL(speed->get_value(), 0.0);
    BOOST_CHECK_EQUAL(speed->get_max_nb_no_new_speed_found(),
                      DEFAULT_MAX_NB_NO_NEW_SPEED_FOUND);
    BOOST_CHECK_EQUAL(speed->get_max_nb_cycle_before_starting(),
                      DEFAULT_MAX_NB_CYCLE_BEFORE_STARTING);

    // Cleanup
    delete speed;
}

/**
 * Test Speed::bool set_max_nb_no_new_speed_found().
 *
 * Test Description:
 *      - Create a Speed object using an harcoded turntable name.
 *      - Check if set_max_nb_no_new_speed_found(-1) return false and if the
 *        value is not set.
 *      - Check if set_max_nb_no_new_speed_found(0) return false and if the
 *        value is not set.
 *      - Check if set_max_nb_no_new_speed_found(2) return true and if the
 *        value is not set.
 */
BOOST_AUTO_TEST_CASE (test_speed_set_max_nb_no_new_speed_found)
{
    Speed *speed = new Speed("turntable_name");

    BOOST_CHECK_EQUAL(speed->set_max_nb_no_new_speed_found(-1), false);
    BOOST_CHECK_EQUAL(speed->get_max_nb_no_new_speed_found(),
                      DEFAULT_MAX_NB_NO_NEW_SPEED_FOUND);
    BOOST_CHECK_EQUAL(speed->set_max_nb_no_new_speed_found(0), false);
    BOOST_CHECK_EQUAL(speed->get_max_nb_no_new_speed_found(),
                      DEFAULT_MAX_NB_NO_NEW_SPEED_FOUND);
    BOOST_CHECK_EQUAL(speed->set_max_nb_no_new_speed_found(2), true);
    BOOST_CHECK_EQUAL(speed->get_max_nb_no_new_speed_found(), 2);

    // Cleanup
    delete speed;
}

/**
 * Test Speed::set_max_nb_cycle_before_starting().
 *
 * Test Description:
 *      - Create a Speed object using an harcoded turntable name.
 *      - Check if set_max_nb_cycle_before_starting(-1) return false and if the
 *        value is not set.
 *      - Check if set_max_nb_cycle_before_starting(0) return false and if the
 *        value is not set.
 *      - Check if set_max_nb_cycle_before_starting(2) return true and if the
 *        value is not set.
 */
BOOST_AUTO_TEST_CASE (test_speed_set_max_nb_cycle_before_starting)
{
    Speed *speed = new Speed("turntable_name");

    BOOST_CHECK_EQUAL(speed->set_max_nb_cycle_before_starting(-1), false);
    BOOST_CHECK_EQUAL(speed->get_max_nb_cycle_before_starting(),
                      DEFAULT_MAX_NB_CYCLE_BEFORE_STARTING);
    BOOST_CHECK_EQUAL(speed->set_max_nb_cycle_before_starting(0), false);
    BOOST_CHECK_EQUAL(speed->get_max_nb_cycle_before_starting(),
                      DEFAULT_MAX_NB_CYCLE_BEFORE_STARTING);
    BOOST_CHECK_EQUAL(speed->set_max_nb_cycle_before_starting(2), true);
    BOOST_CHECK_EQUAL(speed->get_max_nb_cycle_before_starting(), 2);

    // Cleanup
    delete speed;
}

/**
 * Test Speed::set_value().
 *
 * Test Description:
 *      - Create a Speed object using an harcoded turntable name.
 *      - Prepare max_nb_no_new_speed_found = 3.
 *      - Check if set_value(2.5) return true and if the value is set.
 *      - Check for 3 times that if we set_value(NO_NEW_SPEED_FOUND) then
 *        NO_NEW_SPEED_FOUND is set.
 *      - Check that if we set_value(NO_NEW_SPEED_FOUND) again, then speed = 0.
 *      - Same check as before, but only 2 times and then a correct value, so
 *        speed should be correctly set.
 *      - Same check as previous one to check if internal counters are
 *        correctly initialized.
 */
BOOST_AUTO_TEST_CASE (test_speed_set_value)
{
    Speed *speed = new Speed("turntable_name");
    BOOST_CHECK_EQUAL(speed->set_max_nb_no_new_speed_found(3), true);

    BOOST_CHECK_EQUAL(speed->set_value(2.5), true);
    BOOST_CHECK_EQUAL(speed->get_value(), 2.5);

    for (int i = 0; i < speed->get_max_nb_no_new_speed_found(); i++)
    {
        BOOST_CHECK_EQUAL(speed->set_value(NO_NEW_SPEED_FOUND), true);
        BOOST_CHECK_EQUAL(speed->get_value(), NO_NEW_SPEED_FOUND);
    }
    BOOST_CHECK_EQUAL(speed->set_value(NO_NEW_SPEED_FOUND), true);
    BOOST_CHECK_EQUAL(speed->get_value(), 0.0);

    for (int i = 0; i < speed->get_max_nb_no_new_speed_found()-1; i++)
    {
        BOOST_CHECK_EQUAL(speed->set_value(NO_NEW_SPEED_FOUND), true);
        BOOST_CHECK_EQUAL(speed->get_value(), NO_NEW_SPEED_FOUND);
    }
    BOOST_CHECK_EQUAL(speed->set_value(1.08), true);
    BOOST_CHECK_EQUAL(speed->get_value(), 1.08);

    for (int i = 0; i < speed->get_max_nb_no_new_speed_found()-1; i++)
    {
        BOOST_CHECK_EQUAL(speed->set_value(NO_NEW_SPEED_FOUND), true);
        BOOST_CHECK_EQUAL(speed->get_value(), NO_NEW_SPEED_FOUND);
    }
    BOOST_CHECK_EQUAL(speed->set_value(5.04), true);
    BOOST_CHECK_EQUAL(speed->get_value(), 5.04);

    // Cleanup
    delete speed;
}
