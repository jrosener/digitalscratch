/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                                    Tests                                   */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------[ controller_test.cpp ]-*/
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
/*                   Unit tests for Controller class                          */
/*                                                                            */
/*============================================================================*/

#include <iostream>
using namespace std;

// Include class to test.
#include <controller.h>

// Use Boost.Test
#include <boost/test/auto_unit_test.hpp>

/**
 * Test Controller::Controller().
 *
 * Test Description:
 *      - Create a Controller object using a valid name.
 *      - Check if controller name was correctly stored.
 *      - Check if speed, position and volume are correctly initialized.
 */
BOOST_AUTO_TEST_CASE (test_controller_constructor)
{
    Controller *ctrl = new Controller("controller_name");
    float speed    = 0.0;
    float volume   = 0.0;
    float position = 0.0;

    BOOST_CHECK_EQUAL(ctrl->get_name(), "controller_name");
    BOOST_CHECK_EQUAL(ctrl->get_playing_parameters(&speed, &volume, &position), false);
    BOOST_CHECK_EQUAL(speed, 0.0);
    BOOST_CHECK_EQUAL(volume, 0.0);
    BOOST_CHECK_EQUAL(position, 0.0);
    BOOST_CHECK_EQUAL(ctrl->get_max_nb_no_new_speed_found(), 1);
    BOOST_CHECK_EQUAL(ctrl->get_max_nb_cycle_before_starting(), 1);

    // Cleanup
    delete ctrl;
}

/**
 * Test Controller::set_name().
 *
 * Test Description:
 *      - Create a Controller object using an valid name.
 *      - Check if the name was correctly stored.
 *      - Put an empty name using set_name(""), check if it return false and
 *        if the name is not set.
 */
BOOST_AUTO_TEST_CASE (test_controller_set_name)
{
    Controller *ctrl = new Controller("controller_name");

    BOOST_CHECK_EQUAL(ctrl->get_name(), "controller_name");

    BOOST_CHECK_EQUAL(ctrl->set_name(""), false);
    BOOST_CHECK_EQUAL(ctrl->get_name(), "controller_name");

    // Cleanup
    delete ctrl;
}
