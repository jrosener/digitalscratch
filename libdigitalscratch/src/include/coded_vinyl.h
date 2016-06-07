/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*               libdigitalscratch: the Digital Scratch engine.               */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------( coded_vinyl.h )-*/
/*                                                                            */
/*  Copyright (C) 2003-2016                                                   */
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
/*            Coded_vynil class : define a coded vinyl disk                   */
/*                                                                            */
/*============================================================================*/

#pragma once

#include <string>
#include <QVector>

#include "dscratch_parameters.h"
#include "digital_scratch_api.h"
#include "iir_filter.h"
#include "inst_freq_extrator.h"

#define DEFAULT_RPM RPM_33

/**
 * Define a Coded_vinyl class.\n
 * A coded vinyl is the definition of a vinyl disc with a timecoded signal.
 * @author Julien Rosener
 */
class Coded_vinyl
{
 protected:
    float min_amplitude;

 private:
    unsigned int         sample_rate;
    dscratch_vinyl_rpm_t rpm;

    // Frequency and amplitude analysis.
    IIR_filter          speed_IIR;
    Inst_freq_extractor freq_inst;
    double              filtered_freq_inst;

 public:
    Coded_vinyl(unsigned int sample_rate);
    virtual ~Coded_vinyl();

 public:
    void run_recording_data_analysis(const QVector<float> &input_samples_1,
                                     const QVector<float> &input_samples_2);

    bool set_sample_rate(unsigned int sample_rate);
    unsigned int get_sample_rate();

    bool set_rpm(dscratch_vinyl_rpm_t rpm);
    dscratch_vinyl_rpm_t get_rpm();

    virtual float get_speed() = 0;
    virtual float get_volume() = 0;

 protected:
    float get_signal_freq();
};
