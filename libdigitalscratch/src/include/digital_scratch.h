/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                                                                            */
/*                                                                            */
/*------------------------------------------------------( digital_scratch.h )-*/
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
/*        Digital_scratch class : define a Digital_scratch controller         */
/*                                                                            */
/*============================================================================*/

#pragma once

#include <string>

#include "dscratch_parameters.h"
#include "controller.h"
#include "coded_vinyl.h"
#include "final_scratch_vinyl.h"
#include "serato_vinyl.h"
#include "mixvibes_vinyl.h"

// Speed states
#define UNSTABLE_SPEED 0
#define STABLE_SPEED   1
#define SLOW_SPEED     2

// Default values
#define DEFAULT_MAX_SPEED_DIFF             0.05f
#define DEFAULT_MAX_SLOW_SPEED             0.5f
#define DEFAULT_MAX_NB_BUFFER              5
#define DEFAULT_MAX_NB_SPEED_FOR_STABILITY 3

/**
 * Define a Digital_scratch class.\n
 * Base class : Controller\n
 * It implement a Controller class.
 * @author Julien Rosener
 */
class Digital_scratch : public Controller
{
    /* Attributes */
    private:
        /**
         * Coded vinyl object pointer.
         */
        Coded_vinyl *vinyl;

        unsigned int sample_rate;

    /* Constructor / Destructor */
    public:
        /**
         * Constructor.
         * @param controller_name is the name of this DigitalScratch controller.
         * @param timecoded_vinyl is the Coded_vinyl object used with
         *        Digital_scratch (e.g. Final_scratch_vinyl)
         * @param sample rate is the rate of the timecoded input signal.
         */
        Digital_scratch(string       controller_name,
                        string       coded_vinyl_type,
                        unsigned int sample_rate);

        /**
         * Destructor
         */
        virtual ~Digital_scratch();


    /* Methods */
    public:
        /**
         * Analyze recording datas and update playing parameters.\n
         * Define the pure virtual method in base class (Controller).
         * @param input_samples_1 are the samples of channel 1.
         * @param input_samples_2 are the samples of channel 2.
         * @return TRUE if all is OK, otherwise FALSE.
         *
         * @note input_samples_1 and input_samples_2 must have the same number
         *       of elements.
         */
        bool analyze_recording_data(vector<float> &input_samples_1,
                                    vector<float> &input_samples_2);

        /**
         * Get Coded_vinyl object.
         */
        Coded_vinyl* get_coded_vinyl();

        /**
         * Set Coded_vinyl object.
         */
        bool change_coded_vinyl(string coded_vinyl_type);

    private:
        bool init(string coded_vinyl_type);
        void clean();

        void  calculate_speed();
        float get_speed();

        void  calculate_volume();
        float get_volume();
};
