/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                                    Tests                                   */
/*                                                                            */
/*                                                                            */
/*--------------------------------------------------------[ volume_test.cpp ]-*/
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
/*                     Unit tests for Volume class                            */
/*                                                                            */
/*============================================================================*/

#include <iostream>
using namespace std;

// Include class to test.
#include <volume.h>

// Use Boost.Test
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

/**
 * Test Volume::Volume().
 *
 * Test Description:
 *      - Create a Volume object using an harcoded turntable name.
 *      - Check if turntable name was correctly stored by Volume object.
 *      - Check if value is correctly initialized.
 */
BOOST_AUTO_TEST_CASE (test_volume_constructor)
{
    Volume *vol = new Volume("turntable_name");

    BOOST_CHECK_EQUAL(vol->get_turntable_name(), "turntable_name");
    BOOST_CHECK_EQUAL(vol->get_value(), 0.0);

    // Cleanup
    delete vol;
}

/**
 * Test Volume::set_value().
 *
 * Test Description:
 *      - Create a Volume object using an harcoded turntable name.
 *      - Check if set_value(-1.0) return false and if the value is not set.
 *      - Check if set_value(2.4) return true and if the value is set.
 */
BOOST_AUTO_TEST_CASE (test_volume_set_value)
{
    Volume *vol = new Volume("turntable_name");

    BOOST_CHECK_EQUAL(vol->get_turntable_name(), "turntable_name");
    BOOST_CHECK_EQUAL(vol->set_value(-1.0), false);
    BOOST_CHECK(vol->get_value() != -1.0);
    BOOST_CHECK_EQUAL(vol->set_value(2.4), true);
    BOOST_CHECK_CLOSE(vol->get_value(), (float)2.4, 0.1);

    // Cleanup
    delete vol;
}

/**
 * Test Volume::set_turntable_name().
 *
 * Test Description:
 *      - Create a Volume object using an harcoded turntable name.
 *      - Check if set_turntable_name("") return false and if the name is not
 *        set.
 *      - Check if set_turntable_name("left_turntable") return true and if the
 *        name is set.
 */
BOOST_AUTO_TEST_CASE (test_volume_set_turntable_name)
{
    Volume *vol = new Volume("turntable_name");

    BOOST_CHECK_EQUAL(vol->get_turntable_name(), "turntable_name");
    BOOST_CHECK_EQUAL(vol->set_turntable_name(""), false);
    BOOST_CHECK(vol->get_turntable_name() != "");
    BOOST_CHECK_EQUAL(vol->set_turntable_name("left_turntable"), true);
    BOOST_CHECK_EQUAL(vol->get_turntable_name(), "left_turntable");

    // Cleanup
    delete vol;
}
