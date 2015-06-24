/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                                                                            */
/*                                                                            */
/*--------------------------------------------------------( coded_vinyl.cpp )-*/
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
/*                Coded_vinyl class : define a coded vinyl disk               */
/*                                                                            */
/*============================================================================*/

#include <iostream>
#include <cstdio>
#include <string>
#include <cmath>
#include <iterator>
#include <QtGlobal>

using namespace std;

#include "log.h"
#include "dscratch_parameters.h"
#include "coded_vinyl.h"
#include <qmath.h>

#define FIR_COEFF (sample_rate/1000.0) / (3.1416 * 2.0)

Coded_vinyl::Coded_vinyl(unsigned int sample_rate) : sample_rate(sample_rate),
                                                     rpm(DEFAULT_RPM),
                                                     speed_IIR({1.0, -0.998}, {0.001, 0.001}),
                                                     freq_inst(sample_rate),
                                                     filtered_freq_inst(0.0),
                                                     current_amplitude(0.0)
{
}

Coded_vinyl::~Coded_vinyl()
{
}

void Coded_vinyl::run_recording_data_analysis(const QVector<float> &input_samples_1,
                                              const QVector<float> &input_samples_2)
{
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

    return;
}

float Coded_vinyl::get_signal_freq()
{
    return this->filtered_freq_inst;
}

bool Coded_vinyl::set_sample_rate(unsigned int sample_rate)
{
    if (sample_rate <= 0)
    {
        qCCritical(DSLIB_ANALYZEVINYL) << "sample_rate cannot be <= 0";

        return false;
    }

    this->sample_rate = sample_rate;

    return true;
}

unsigned int Coded_vinyl::get_sample_rate()
{
    return this->sample_rate;
}

bool Coded_vinyl::set_rpm(dscratch_vinyl_rpm_t rpm)
{
    this->rpm = rpm;
    return true;
}

dscratch_vinyl_rpm_t Coded_vinyl::get_rpm()
{
    return this->rpm;
}

void Coded_vinyl::set_min_amplitude(float amplitude)
{
    this->min_amplitude = amplitude;
    return;
}

float Coded_vinyl::get_min_amplitude()
{
    return this->min_amplitude;
}

float Coded_vinyl::get_current_amplitude()
{
    return this->current_amplitude;
}
