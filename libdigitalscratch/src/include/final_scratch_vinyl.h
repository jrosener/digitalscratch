/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*               libdigitalscratch: the Digital Scratch engine.               */
/*                                                                            */
/*                                                                            */
/*--------------------------------------------------( final_scratch_vinyl.h )-*/
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
/*  Final_scratch_vinyl class : define a Stanton Final Scratch timecode vinyl */
/*                                                                            */
/*============================================================================*/

#pragma once

#include <string>

#include "dscratch_parameters.h"
#include "timecoded_vinyl.h"
#include "digital_scratch.h"

// Stanton Final Scratch vinyl sinusoidal frequency (Hz) (@33 rpm)
#define FINAL_SCRATCH_SINUSOIDAL_FREQ 1200.0f

// Stanton Final Scratch vinyl sinusoidal frequency (Hz) (@45 rpm).
// Same value than 33rpm because there is a specific vinyl side for 45 rpm.
#define FINAL_SCRATCH_SINUSOIDAL_FREQ_45RPM 1200.0f

/**
 * Define a Stanton Final Scratch timecode vinyl class.\n
 * @author Julien Rosener
 */
class Final_scratch_vinyl : public Timecoded_vinyl
{
    public:
        Final_scratch_vinyl();
        virtual ~Final_scratch_vinyl();

    public:
        float get_speed_from_freq(const float freq);
        float get_volume_from_freq(const float freq);
};
