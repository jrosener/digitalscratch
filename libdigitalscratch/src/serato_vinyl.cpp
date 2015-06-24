/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                                                                            */
/*                                                                            */
/*-------------------------------------------------------( serato_vinyl.cpp )-*/
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
/*  Final_scratch_vinyl class : define a Serato Scratch Live timecoded vinyl  */
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
#include "serato_vinyl.h"

Serato_vinyl::Serato_vinyl(unsigned int sample_rate) : Coded_vinyl(sample_rate)
{
    this->min_amplitude = DEFAULT_SERATO_MIN_AMPLITUDE;
}

Serato_vinyl::~Serato_vinyl()
{
}

float Serato_vinyl::get_default_min_amplitude()
{
    return DEFAULT_SERATO_MIN_AMPLITUDE;
}

float Serato_vinyl::get_speed()
{
    float speed = 0.0;
    if (this->get_rpm() == RPM_33)
    {
        speed = this->get_signal_freq() / SERATO_VINYL_SINUSOIDAL_FREQ;
    }
    else
    {
        speed = this->get_signal_freq() / SERATO_VINYL_SINUSOIDAL_FREQ_45RPM;
    }

//cout << "speed = " << speed << endl;
    return speed;
}

float Serato_vinyl::get_volume()
{
    // The volume is proportionnal to the speed.
    float volume = 0.0;
    if (this->get_rpm() == RPM_33)
    {
        volume = qMin(qAbs(this->get_signal_freq()) / SERATO_VINYL_SINUSOIDAL_FREQ, 1.0f);
    }
    else
    {
        volume = qMin(qAbs(this->get_signal_freq()) / SERATO_VINYL_SINUSOIDAL_FREQ_45RPM, 1.0f);
    }

//cout << "volume = " << volume << endl;
    return volume;
}
