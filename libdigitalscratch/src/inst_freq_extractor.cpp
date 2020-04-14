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

#include <inst_freq_extrator.h>
#include <qmath.h>
#include <math.h>

using namespace std;

Inst_freq_extractor::Inst_freq_extractor(double samplingFreq)
{
    this->x1 = 0.0f;
    this->x2 = 0.0f;
    this->y1 = 0.0f;
    this->y2 = 0.0f;
    this->currentInstFreq = 0.0f;
    this->currentInstModuleSquared = 0.0f;
    this->scalingFactor = 0.25 * samplingFreq / M_PI;
}

Inst_freq_extractor::~Inst_freq_extractor()
{
    return;
}

void Inst_freq_extractor::compute(double x0, double y0)
{
    this->currentInstModuleSquared = this->x1 * this->x1
                                   + this->y1 * this->y1
                                   + qPow(2, -20); // Avoid dividing by 0

    this->currentInstFreq = this->scalingFactor
                          * (this->x1 * (y0 - this->y2) - this->y1 * (x0 - this->x2))
                          / this->currentInstModuleSquared;

    this->x2 = this->x1;
    this->x1 = x0;
    this->y2 = this->y1;
    this->y1 = y0;
}

double Inst_freq_extractor::getCurrentInstModule()
{
    return qSqrt(this->currentInstModuleSquared);
}

double Inst_freq_extractor::getCurrentInstFreq()
{
    return this->currentInstFreq;
}
