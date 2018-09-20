/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*               libdigitalscratch: the Digital Scratch engine.               */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------( digital_scratch.cpp )-*/
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
/*        Digital_scratch class : define a Digital_scratch controller         */
/*                                                                            */
/*============================================================================*/

#include <iostream>
#include <cstdio>
#include <string>
#include <iterator>

using namespace std;

#include "log.h"
#include "digital_scratch.h"
#include "timecoded_signal_process.h"

Timecoded_signal_process::Timecoded_signal_process(dscratch_vinyls_t coded_vinyl_type,
                                                   unsigned int      sample_rate) : speed_IIR({1.0, -0.998}, {0.001, 0.001}),
                                                                                    freq_inst(sample_rate),
                                                                                    filtered_freq_inst(0.0)
{
    // Init.
    this->init(coded_vinyl_type);
}

bool Timecoded_signal_process::init(dscratch_vinyls_t coded_vinyl_type)
{
    this->vinyl = nullptr;
    switch(coded_vinyl_type)
    {
        case FINAL_SCRATCH :
            this->vinyl = new Final_scratch_vinyl();
            break;

        case SERATO :
            this->vinyl = new Serato_vinyl();
            break;

        case MIXVIBES :
            this->vinyl = new Mixvibes_vinyl();
            break;

        default :
            qCCritical(DSLIB_CONTROLLER) << "Cannot create Digital_scratch object with NULL vinyl.";
            return false;
    }

    return true;
}

Timecoded_signal_process::~Timecoded_signal_process()
{
    // Cleanup.
    this->clean();
}

void Timecoded_signal_process::clean()
{
    if (this->vinyl != NULL)
    {
        delete this->vinyl;
    }
}

bool Timecoded_signal_process::run(const QVector<float> &input_samples_1,
                                   const QVector<float> &input_samples_2)
{
    if ((input_samples_1.size() == 0)
       || (input_samples_1.size() != input_samples_2.size()))
    {
        qCCritical(DSLIB_CONTROLLER) << "Wrong input samples table sizes";
        return false;
    }

    // The goal of this method is to analyze input datas and calculate speed and volume.
    qCDebug(DSLIB_ANALYZEVINYL) << "Extracting frequency and amplitude from recorded samples...";

    // Processing loop: One sample per iteration.
    for (int i = 0; i < input_samples_1.size(); i++)
    {
        // Read one Right/Left sample.
        double left_sample  = input_samples_1[i];
        double right_sample = input_samples_2[i];

        // Extract instantaneous phase from the complex sample formed by left/right channels
        this->freq_inst.compute(right_sample, left_sample);

        // Filter the instantaneous frequency
        this->filtered_freq_inst = this->speed_IIR.compute(this->freq_inst.getCurrentInstFreq());
    }

    this->speed  = this->vinyl->get_speed_from_freq(filtered_freq_inst);
    this->volume = this->vinyl->get_volume_from_freq(filtered_freq_inst);

    return true;
}

Timecoded_vinyl* Timecoded_signal_process::get_coded_vinyl()
{
    return this->vinyl;
}

bool Timecoded_signal_process::change_coded_vinyl(dscratch_vinyls_t coded_vinyl_type)
{
    // First clean all in Digital_scratch object.
    this->clean();

    // Then create a new coded vinyl.
    return this->init(coded_vinyl_type);
}

float Timecoded_signal_process::get_speed()
{
    return this->speed;
}

float Timecoded_signal_process::get_volume()
{
    return this->volume;
}
