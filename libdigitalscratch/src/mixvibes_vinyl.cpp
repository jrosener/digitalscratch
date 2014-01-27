/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                                                                            */
/*                                                                            */
/*-----------------------------------------------------( mixvibes_vinyl.cpp )-*/
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
/*       Mixvibes_vinyl class : define a Mixvibes DVS timecoded vinyl         */
/*                                                                            */
/*============================================================================*/

#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

#include "include/dscratch_parameters.h"
#include "include/utils.h"
#include "include/coded_vinyl.h"
#include "include/mixvibes_vinyl.h"

Mixvibes_vinyl::Mixvibes_vinyl(unsigned int sample_rate) : Coded_vinyl(sample_rate)
{
    Utils::trace_object_life(TRACE_PREFIX_MVVINYL,
                             "+ Creating Mixvibes_vinyl object...");

    // Reverse direction detection.
    this->set_reverse_direction(true);

    Utils::trace_object_life(TRACE_PREFIX_MVVINYL,
                             "+ Mixvibes_vinyl object created");
}

Mixvibes_vinyl::~Mixvibes_vinyl()
{
    Utils::trace_object_life(TRACE_PREFIX_MVVINYL,
                             "- Deleting Mixvibes_vinyl object...");

    Utils::trace_object_life(TRACE_PREFIX_MVVINYL,
                             "- Mixvibes_vinyl object deleted");
}

float Mixvibes_vinyl::get_position()
{
    Utils::trace_position(TRACE_PREFIX_MVVINYL, "Searching new position...");

    // TODO: implement position detection.

    // no position found
    return NO_NEW_POSITION_FOUND;
}

int Mixvibes_vinyl::get_sinusoidal_frequency()
{
    if (this->get_rpm() == RPM_33)
        return MIXVIBES_SINUSOIDAL_FREQ;
    else
        return MIXVIBES_SINUSOIDAL_FREQ_45RPM;
}

float Mixvibes_vinyl::get_min_amplitude_for_normal_speed()
{
    // TODO: allow to customize this value by the user.
    return DEFAULT_MIXVIBES_MIN_AMPLITUDE_FOR_NORMAL_SPEED;
}
