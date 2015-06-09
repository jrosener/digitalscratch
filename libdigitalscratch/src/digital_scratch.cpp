/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------( digital_scratch.cpp )-*/
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
/*        Digital_scratch class : define a Digital_scratch controller         */
/*                                                                            */
/*============================================================================*/

#include <iostream>
#include <cstdio>
#include <string>
#include <iterator>

using namespace std;

#include "log.h"
#include "dscratch_parameters.h"
#include "digital_scratch_api.h"
#include "digital_scratch.h"

Digital_scratch::Digital_scratch(dscratch_vinyls_t coded_vinyl_type,
                                 unsigned int    sample_rate) : Controller()
{
    // Init.
    this->sample_rate = sample_rate;
    this->init(coded_vinyl_type);
}

bool Digital_scratch::init(dscratch_vinyls_t coded_vinyl_type)
{
    this->vinyl = nullptr;
    switch(coded_vinyl_type)
    {
        case FINAL_SCRATCH :
            this->vinyl = new Final_scratch_vinyl(sample_rate);
            break;
 
        case SERATO :
            this->vinyl = new Serato_vinyl(sample_rate);
            break;
 
        case MIXVIBES :
            this->vinyl = new Mixvibes_vinyl(sample_rate);
            break;
 
        default :
            qCCritical(DSLIB_CONTROLLER) << "Cannot create Digital_scratch object with NULL vinyl.";
            return false;
    }

    return true;
}

Digital_scratch::~Digital_scratch()
{
    // Cleanup.
    this->clean();
}

void Digital_scratch::clean()
{
    if (this->vinyl != NULL)
    {
        delete this->vinyl;
    }
}

bool Digital_scratch::analyze_captured_timecoded_signal(const QVector<float> &input_samples_1,
                                                        const QVector<float> &input_samples_2)
{
    if ((input_samples_1.size() == 0)
       || (input_samples_1.size() != input_samples_2.size()))
    {
        qCCritical(DSLIB_CONTROLLER) << "Wrong input samples table sizes";
        return false;
    }

    // The goal of this method is to analyze input datas and calculate speed and volume.
    this->vinyl->run_recording_data_analysis(input_samples_1, input_samples_2);
    this->speed  = this->vinyl->get_speed();
    this->volume = this->vinyl->get_volume();

    return true;
}

Coded_vinyl* Digital_scratch::get_coded_vinyl()
{
    return this->vinyl;
}

bool Digital_scratch::change_coded_vinyl(dscratch_vinyls_t coded_vinyl_type)
{
    // First clean all in Digital_scratch object.
    this->clean();

    // Then create a new coded vinyl.
    return this->init(coded_vinyl_type);
}

float Digital_scratch::get_speed()
{
    return this->speed;
}

float Digital_scratch::get_volume()
{
    return this->volume;
}
