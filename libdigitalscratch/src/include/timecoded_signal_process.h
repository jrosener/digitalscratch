/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*               libdigitalscratch: the Digital Scratch engine.               */
/*                                                                            */
/*                                                                            */
/*---------------------------------------------( timecoded_signal_process.h )-*/
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
/*         Compute a timecoded signal to find the dynamic vinyl disc          */
/*                   characteristics (speed and volume).                      */
/*                                                                            */
/*============================================================================*/

#pragma once

#include <string>
#include <QVector>

#include "timecoded_vinyl.h"
#include "final_scratch_vinyl.h"
#include "serato_vinyl.h"
#include "mixvibes_vinyl.h"
#include "iir_filter.h"
#include "inst_freq_extrator.h"

class Timecoded_signal_process
{
    private:
        Timecoded_vinyl *vinyl;
        float            speed;
        float            volume;

        // Frequency and amplitude analysis.
        IIR_filter          speed_IIR;
        Inst_freq_extractor freq_inst;
        double              filtered_freq_inst;

    public:
        Timecoded_signal_process(dscratch_vinyls_t coded_vinyl_type,
                                 unsigned int      sample_rate);

        virtual ~Timecoded_signal_process();


    public:
        bool run(const QVector<float> &input_samples_1,
                 const QVector<float> &input_samples_2);

        Timecoded_vinyl* get_coded_vinyl();
        bool change_coded_vinyl(dscratch_vinyls_t coded_vinyl_type);

        float get_speed();
        float get_volume();

    private:
        bool init(dscratch_vinyls_t coded_vinyl_type);
        void clean();
};
