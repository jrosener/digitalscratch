/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                                                                            */
/*                                                                            */
/*-------------------------------------------------------( mixvibes_vinyl.h )-*/
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
/*        Mixvibes_vinyl class : define a Mixvibes DVS timecode vinyl         */
/*                                                                            */
/*============================================================================*/

#pragma once

#include <string>
#include <vector>
#include "dscratch_parameters.h"
#include "coded_vinyl.h"
#include "digital_scratch_api.h"

#ifdef DISPLAY_CODE_LINE
    #define TRACE_PREFIX_MVVINYL "[Mixvibes_v:" + Utils::to_string(__LINE__) + "]\t\t"
#else
    #define TRACE_PREFIX_MVVINYL "[Mixvibes_v]\t\t"
#endif

/**
 * Mixvibes vinyl sinusoidal frequency (Hz) (@33 rpm)
 */
#define MIXVIBES_SINUSOIDAL_FREQ 1300

/**
 * Mixvibes vinyl sinusoidal frequency (Hz) (@45 rpm)
 */
#define MIXVIBES_SINUSOIDAL_FREQ_45RPM 1755

#define DEFAULT_MIXVIBES_MIN_AMPLITUDE_FOR_NORMAL_SPEED 0.10f
#define DEFAULT_MIXVIBES_MIN_AMPLITUDE                  0.005f

/**
 * Define a Mixvibes DVS timecode vinyl class.\n
 * @author Julien Rosener
 */
class Mixvibes_vinyl : public Coded_vinyl
{
    /* Constructor / Destructor */
    public:
        /**
         * Constructor.
         */
        Mixvibes_vinyl(unsigned int sample_rate);

        /**
         * Destructor.
         */
        virtual ~Mixvibes_vinyl();


    /* Methods */
    public:
        int get_sinusoidal_frequency();
        float get_default_min_amplitude_for_normal_speed();
        float get_default_min_amplitude();
};
