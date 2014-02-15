/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------( digital_scratch.cpp )-*/
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

#include <iostream>
#include <cstdio>
#include <string>
#include <iterator>

using namespace std;

#include "include/dscratch_parameters.h"
#include "include/utils.h"
#include "include/digital_scratch_api.h"
#include "include/digital_scratch.h"

Digital_scratch::Digital_scratch(string       controller_name,
                                 string       coded_vinyl_type,
                                 unsigned int sample_rate) : Controller(controller_name)
{
    Utils::trace_object_life(TRACE_PREFIX_DIGITALSCRATCH,
                             "+ Creating Digital_scratch object...");

    // Init.
    this->sample_rate = sample_rate;
    this->init(coded_vinyl_type);


    Utils::trace_object_life(TRACE_PREFIX_DIGITALSCRATCH,
                             "+ Digital_scratch object created");
}

bool Digital_scratch::init(string coded_vinyl_type)
{
    // Internal parameters.
    this->set_playing_parameters_ready(false);

    this->vinyl = NULL;
    if (coded_vinyl_type == FINAL_SCRATCH_VINYL)
    {
        this->vinyl = new Final_scratch_vinyl(sample_rate);
    }
    else if (coded_vinyl_type == SERATO_VINYL)
    {
       this->vinyl = new Serato_vinyl(sample_rate);
    }
    else if (coded_vinyl_type == MIXVIBES_VINYL)
    {
       this->vinyl = new Mixvibes_vinyl(sample_rate);
    }
    else
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCH, "Cannot create Digital_scratch object using NULL vinyl.");
        return false;
    }

    return true;
}

Digital_scratch::~Digital_scratch()
{
    Utils::trace_object_life(TRACE_PREFIX_DIGITALSCRATCH,
                             "- Deleting Digital_scratch object...");

    // Cleanup.
    this->clean();

    Utils::trace_object_life(TRACE_PREFIX_DIGITALSCRATCH,
                             "- Digital_scratch object deleted");
}

void Digital_scratch::clean()
{
    if (this->vinyl != NULL)
    {
        delete this->vinyl;
    }
}

bool Digital_scratch::analyze_recording_data(vector<float> &input_samples_1,
                                             vector<float> &input_samples_2)
{
    if ((input_samples_1.size() == 0)
       || (input_samples_1.size() != input_samples_2.size()))
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCH,
                    "Wrong input samples table sizes");
                return false;
    }

    // The goal of this method is to analyze input datas, calculate playing
    // parameters and then say if yes or no we are ready to use these
    // values to control the player.
    this->set_playing_parameters_ready(false);

    // Add timecoded data to vinyl internals.
    this->vinyl->add_sound_data(input_samples_1, input_samples_2);

    // Calculate speed (and direction) and volume.
    this->calculate_speed();
    this->calculate_volume();

    this->set_playing_parameters_ready(true);

    return true;
}

Coded_vinyl* Digital_scratch::get_coded_vinyl()
{
    return this->vinyl;
}

bool Digital_scratch::change_coded_vinyl(string coded_vinyl_type)
{
    // First clean all in Digital_scratch object.
    this->clean();

    // Then create a new coded vinyl.
    return this->init(coded_vinyl_type);
}

void Digital_scratch::calculate_speed()
{
    this->speed->set_value(this->vinyl->get_speed());
}

float Digital_scratch::get_speed()
{
    return this->speed->get_value();
}

void Digital_scratch::calculate_volume()
{
    this->volume->set_value(this->vinyl->get_volume());
}

float Digital_scratch::get_volume()
{
    return this->volume->get_value();
}
