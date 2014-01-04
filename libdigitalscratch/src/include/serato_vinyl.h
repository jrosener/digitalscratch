/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                                                                            */
/*                                                                            */
/*---------------------------------------------------------( serato_vinyl.h )-*/
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
/*  Final_scratch_vinyl class : define a Serato Scratch Live timecode vinyl   */
/*                                                                            */
/*============================================================================*/

#ifndef _SERATO_VINYL_H_
#define _SERATO_VINYL_H_

#include <string>
#include <vector>
#include "dscratch_parameters.h"
#include "coded_vinyl.h"
#include "digital_scratch_api.h"

#ifdef DISPLAY_CODE_LINE
    #define TRACE_PREFIX_SERVINYL "[Serato_vinyl:" + Utils::to_string(__LINE__) + "]\t\t"
#else
    #define TRACE_PREFIX_SERVINYL "[Serato_vinyl]\t\t"
#endif

/**
 * Serato vinyl sinusoidal frequency (Hz) (@33 rpm)
 */
#define SERATO_VINYL_SINUSOIDAL_FREQ 1000

/**
 * Serato vinyl sinusoidal frequency (Hz)  (@45 rpm)
 */
#define SERATO_VINYL_SINUSOIDAL_FREQ_45RPM 1350

/**
 * Define a Serato Scratch Live timecode vinyl class.\n
 * @author Julien Rosener
 */
class Serato_vinyl : public Coded_vinyl
{
    /* Attributes */
    private:
        

    /* Constructor / Destructor */
    public:
        /**
         * Constructor.
         */
        Serato_vinyl(unsigned int sample_rate);

        /**
         * Destructor.
         */
        virtual ~Serato_vinyl();


    /* Methods */
    public:
        /**
         * Get the current position value of needle on vinyl.
         * Define the pure virtual method in base class (Coded_vinyl)
         * @return the position value or NO_NEW_POSITION_FOUND if nothing is found
         */
        float get_position();

        int get_sinusoidal_frequency();

    private:

};

#endif //_SERATO_VINYL_H_
