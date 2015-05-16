/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                                                                            */
/*                                                                            */
/*-----------------------------------------------------( mixvibes_vinyl.cpp )-*/
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
/*       Mixvibes_vinyl class : define a Mixvibes DVS timecoded vinyl         */
/*                                                                            */
/*============================================================================*/

#include <iostream>
#include <cstdio>
#include <string>
#include <cmath>

using namespace std;

#include "log.h"
#include "dscratch_parameters.h"
#include "coded_vinyl.h"
#include "mixvibes_vinyl.h"

Mixvibes_vinyl::Mixvibes_vinyl(unsigned int sample_rate) : Coded_vinyl(sample_rate)
{
    // Reverse direction detection.
    this->set_reverse_direction(true);
}

Mixvibes_vinyl::~Mixvibes_vinyl()
{
}

int Mixvibes_vinyl::get_sinusoidal_frequency()
{
    if (this->get_rpm() == RPM_33)
        return MIXVIBES_SINUSOIDAL_FREQ;
    else
        return MIXVIBES_SINUSOIDAL_FREQ_45RPM;
}

float Mixvibes_vinyl::get_default_min_amplitude()
{
    return DEFAULT_MIXVIBES_MIN_AMPLITUDE;
}
