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


Coded_vinyl::Coded_vinyl(unsigned int sample_rate) : sample_rate(sample_rate),
                                                     rpm(DEFAULT_RPM),
                                                     diff_FIR({-(sample_rate/1000.0)/(3.1416*2), (sample_rate/1000.0)/(3.1416*2)}),
                                                     unwraper(),
                                                     speed_IIR({1.0, -0.998}, {0.001, 0.001}),
                                                     amplitude_IIR({1.0, -0.998}, {0.001, 0.001}),
                                                     filtered_freq_inst(0.0),
                                                     filtered_amplitude_inst(0.0)
{
    this->set_reverse_direction(false);
}

Coded_vinyl::~Coded_vinyl()
{
}

void Coded_vinyl::run_recording_data_analysis(const QVector<float> &input_samples_1,
                                              const QVector<float> &input_samples_2)
{
    qCDebug(DSLIB_ANALYZEVINYL) << "Finding frenquency and amplitude of recorded samples...";

    // Processing loop: One sample per iteration.
    for (int i = 0; i < input_samples_1.size(); i++)
    {
        // Read one Right/Left sample.
        double left_sample  = input_samples_1[i];
        double right_sample = input_samples_2[i];

        // Extract instantaneous phase from the complex sample formed by left/right channels.
        double phase     = qAtan2(right_sample, left_sample);
        double amplitude = qSqrt(right_sample*right_sample + left_sample*left_sample);

        // Unwrap the phase to avoid discontinuity.
        double phase_unwrap = this->unwraper.compute(phase);

        // Diff the phase to get instantaneous frequency.
        double freq_inst = this->diff_FIR.compute(phase_unwrap);

        // Filter the instantaneous frequency.
        this->filtered_freq_inst = this->speed_IIR.compute(freq_inst);

        // Filter the amplitude.
        this->filtered_amplitude_inst = this->amplitude_IIR.compute(amplitude);
    }

    return;
}

float Coded_vinyl::get_speed()
{
    qCDebug(DSLIB_ANALYZEVINYL) << "Searching new speed...";

    float speed = 0.0;
    if (filtered_amplitude_inst > 0.00005) // for left turntable // FIXME: use get_min_amplitude => should be defined per turntable and not per vinyl type. => speed_min_amplitude
    //if (this->filteredAmplitudeInst > 0.00030) // for right turntable
    {
        speed = this->filtered_freq_inst;
        if (this->is_reverse_direction == true)
        {
            speed *= -1.0;
        }
    }

    return speed;
}

float Coded_vinyl::get_volume()
{
    qCDebug(DSLIB_ANALYZEVINYL) << "Searching new volume...";

    float volume = 0.0;
    //if (filteredAmplitudeInst > 0.00005) // for left turntable
    if (this->filtered_amplitude_inst > 0.00030) // for right turntable    => volume_min_apmlitude
    {
        // Turntable is running.
        volume = this->filtered_amplitude_inst * 150.0; // FIXME: get this value from app settings
        if (volume > 1.0)
        {
            volume = 1.0;
        }
    }

    return volume;
}

bool Coded_vinyl::set_reverse_direction(bool is_reverse_direction)
{
    this->is_reverse_direction = is_reverse_direction;

    return true;
}

bool Coded_vinyl::get_reverse_direction()
{
    return this->is_reverse_direction;
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
