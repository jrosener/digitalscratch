/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                                                                            */
/*                                                                            */
/*-------------------------------------------------------( serato_vinyl.cpp )-*/
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
/*  Final_scratch_vinyl class : define a Serato Scratch Live timecoded vinyl  */
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
#include "include/serato_vinyl.h"

Serato_vinyl::Serato_vinyl(unsigned int sample_rate) : Coded_vinyl(sample_rate)
{
    Utils::trace_object_life(TRACE_PREFIX_SERVINYL,
                             "+ Creating Serato_vinyl object...");

    Utils::trace_object_life(TRACE_PREFIX_SERVINYL,
                             "+ Serato_vinyl object created");
}

Serato_vinyl::~Serato_vinyl()
{
    Utils::trace_object_life(TRACE_PREFIX_SERVINYL,
                             "- Deleting Serato_vinyl object...");

    Utils::trace_object_life(TRACE_PREFIX_SERVINYL,
                             "- Serato_vinyl object deleted");
}

float Serato_vinyl::get_position()
{
    Utils::trace_position(TRACE_PREFIX_SERVINYL, "Searching new position...");

    // TODO: to be implemented

    return NO_NEW_POSITION_FOUND;
}

int Serato_vinyl::get_sinusoidal_frequency()
{
    if (this->get_rpm() == RPM_33)
        return SERATO_VINYL_SINUSOIDAL_FREQ;
    else
        return SERATO_VINYL_SINUSOIDAL_FREQ_45RPM;
}
