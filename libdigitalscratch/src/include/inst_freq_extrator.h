/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*               libdigitalscratch: the Digital Scratch engine.               */
/*                                                                            */
/*                                                                            */
/*--------------------------------------------------( inst_freq_extractor.h )-*/
/*                                                                            */
/*  Copyright (C) 2003-2020                                                   */
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
/*    Inst_Freq_Extractor class : Get the instantanous frequency of a         */
/*                                buffer of doubles.                          */
/*                                                                            */
/*============================================================================*/

#pragma once

#include <QVector>

class Inst_freq_extractor
{
 private:
        double x1, x2;
        double y1, y2;
        double currentInstFreq;
        double currentInstModuleSquared;
        double scalingFactor;

 public:
        Inst_freq_extractor(double samplingFreq);
        virtual ~Inst_freq_extractor();

 public:
    void compute(double x0, double y0);
    double getCurrentInstModule();
    double getCurrentInstFreq();
};
