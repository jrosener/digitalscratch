/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                                                                            */
/*                                                                            */
/*--------------------------------------------------( final_scratch_vinyl.h )-*/
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
/*  Final_scratch_vinyl class : define a Stanton Final Scratch timecode vinyl */
/*                                                                            */
/*============================================================================*/

#ifndef _FINAL_SCRATCH_VINYL_H_
#define _FINAL_SCRATCH_VINYL_H_

#include <string>
#include <vector>
#include "dscratch_parameters.h"
#include "coded_vinyl.h"
#include "digital_scratch_api.h"

#ifdef DISPLAY_CODE_LINE
    #define TRACE_PREFIX_FSVINYL "[Final_scratch_v:" + Utils::to_string(__LINE__) + "]\t\t"
#else
    #define TRACE_PREFIX_FSVINYL "[Final_scratch_v]\t\t"
#endif

/**
 * Stanton Final Scratch vinyl sinusoidal frequency (Hz) (@33 rpm)
 */
#define FINAL_SCRATCH_SINUSOIDAL_FREQ 1200

/**
 * Stanton Final Scratch vinyl sinusoidal frequency (Hz) (@45 rpm).
 * Same value than 33rpm because there is a specific vinyl side for 45 rpm.
 */
#define FINAL_SCRATCH_SINUSOIDAL_FREQ_45RPM 1200

#define DEFAULT_FS_MIN_AMPLITUDE_FOR_NORMAL_SPEED 0.05f

/**
 * Bit value 1 for timecode start sequence
 */
#define START_SEQ_BIT1 1

/**
 * Bit value 2 for timecode start sequence
 */
#define START_SEQ_BIT2 1

/**
 * Bit value 3 for timecode start sequence
 */
#define START_SEQ_BIT3 1

/**
 * Bit value 4 for timecode start sequence
 */
#define START_SEQ_BIT4 0

/**
 * Define a Stanton Final Scratch timecode vinyl class.\n
 * @author Julien Rosener
 */
class Final_scratch_vinyl : public Coded_vinyl
{
    /* Attributes */
    private:
        

    /* Constructor / Destructor */
    public:
        /**
         * Constructor.
         */
        Final_scratch_vinyl(unsigned int sample_rate);

        /**
         * Destructor.
         */
        virtual ~Final_scratch_vinyl();


    /* Methods */
    public:
        /**
         * Get the current position value of needle on vinyl.
         * Define the pure virtual method in base class (Coded_vinyl)
         * @return the position value or NO_NEW_POSITION_FOUND if nothing is found
         */
        float get_position();

        int get_sinusoidal_frequency();
        float get_min_amplitude_for_normal_speed();

    private:
        /**
         * Search a start sequence in bit_list and if found, get the index
         * @return the index in bit_list of the beginning of the start sequence
         *         or -1 if no start sequence is found
         */
        int get_timecode_start_sequence_position();

        /**
         * Get timecode value from bit list (that start with a start sequence)
         * @return the timecode value
         */
        unsigned int get_timecode();

        /**
         * Get needle position in seconde from timecode value
         * @param timecode is the timecoded value detected by get_timecode()
         * @return a position (needle on disk) in seconde
         */
        float get_position_from_timecode_value(unsigned int timecode);
};

#endif //_FINAL_SCRATCH_VINYL_H_
