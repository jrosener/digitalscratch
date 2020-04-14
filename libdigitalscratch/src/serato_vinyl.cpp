/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*               libdigitalscratch: the Digital Scratch engine.               */
/*                                                                            */
/*                                                                            */
/*-------------------------------------------------------( serato_vinyl.cpp )-*/
/*                                                                            */
/*  Copyright (C) 2003-2020                                                   */
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
#include "timecoded_vinyl.h"
#include "serato_vinyl.h"

Serato_vinyl::Serato_vinyl() : Timecoded_vinyl()
{
}

Serato_vinyl::~Serato_vinyl()
{
}

float Serato_vinyl::get_speed_from_freq(const float freq)
{
    float speed = 0.0;
    if (this->get_rpm() == RPM_33)
    {
        speed = freq / SERATO_VINYL_SINUSOIDAL_FREQ;
    }
    else
    {
        speed = freq / SERATO_VINYL_SINUSOIDAL_FREQ_45RPM;
    }

//cout << "speed = " << speed << endl;
    return speed;
}

float Serato_vinyl::get_volume_from_freq(const float freq)
{
    // The volume is proportionnal to the speed.
    float volume = 0.0;
    if (this->get_rpm() == RPM_33)
    {
        volume = qMin(qAbs(freq) / SERATO_VINYL_SINUSOIDAL_FREQ, 1.0f);
    }
    else
    {
        volume = qMin(qAbs(freq) / SERATO_VINYL_SINUSOIDAL_FREQ_45RPM, 1.0f);
    }

//cout << "volume = " << volume << endl;
    return volume;
}
