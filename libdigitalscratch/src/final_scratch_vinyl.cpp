/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                                                                            */
/*                                                                            */
/*------------------------------------------------( final_scratch_vinyl.cpp )-*/
/*                                                                            */
/*  Copyright (C) 2003-2014                                                   */
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
/*  Final_scratch_vinyl class : define a Stanton FinalScratch timecoded vinyl */
/*                                                                            */
/*============================================================================*/

#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

#include "log.h"
#include "dscratch_parameters.h"
#include "coded_vinyl.h"
#include "final_scratch_vinyl.h"

Final_scratch_vinyl::Final_scratch_vinyl(unsigned int sample_rate) : Coded_vinyl(sample_rate)
{
}

Final_scratch_vinyl::~Final_scratch_vinyl()
{
}

int Final_scratch_vinyl::get_sinusoidal_frequency()
{
    if (this->get_rpm() == RPM_33)
        return FINAL_SCRATCH_SINUSOIDAL_FREQ;
    else
        return FINAL_SCRATCH_SINUSOIDAL_FREQ_45RPM;
}

float Final_scratch_vinyl::get_default_min_amplitude_for_normal_speed()
{
    return DEFAULT_FS_MIN_AMPLITUDE_FOR_NORMAL_SPEED;
}

float Final_scratch_vinyl::get_default_min_amplitude()
{
    return DEFAULT_FS_MIN_AMPLITUDE;
}
