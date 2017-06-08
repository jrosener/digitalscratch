/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*               libdigitalscratch: the Digital Scratch engine.               */
/*                                                                            */
/*                                                                            */
/*-----------------------------------------------------( mixvibes_vinyl.cpp )-*/
/*                                                                            */
/*  Copyright (C) 2003-2017                                                   */
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
}

Mixvibes_vinyl::~Mixvibes_vinyl()
{
}

float Mixvibes_vinyl::get_speed()
{
    float speed = 0.0;
    if (this->get_rpm() == RPM_33)
    {
        speed = this->get_signal_freq() / MIXVIBES_SINUSOIDAL_FREQ;
    }
    else
    {
        speed = this->get_signal_freq() / MIXVIBES_SINUSOIDAL_FREQ_45RPM;
    }

    return speed * -1.0; // Mixvibes stereo signal temporal shift is reversed (than Serato, FinalScratch,...)
}

float Mixvibes_vinyl::get_volume()
{
    // The volume is proportionnal to the speed.
    float volume = 0.0;
    if (this->get_rpm() == RPM_33)
    {
        volume = qMin(qAbs(this->get_signal_freq()) / MIXVIBES_SINUSOIDAL_FREQ, 1.0f);
    }
    else
    {
        volume = qMin(qAbs(this->get_signal_freq()) / MIXVIBES_SINUSOIDAL_FREQ_45RPM, 1.0f);
    }

    return volume;
}
