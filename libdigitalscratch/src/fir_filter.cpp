/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                                                                            */
/*                                                                            */
/*---------------------------------------------------------( fir_filter.cpp )-*/
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
/*    FIR_filter class : Finite Impulse Response filter implementation        */
/*                                                                            */
/*============================================================================*/

#include <fir_filter.h>

using namespace std;

FIR_filter::FIR_filter(QVector<double> h)
{
    this->h = h; // Copy filter kernel
    for (int i = 0; i < h.length(); ++i) // Create delay line
    {
        this->x << 0;
    }
}

FIR_filter::~FIR_filter()
{
    return;
}

double FIR_filter::compute(const double &sample)
{
    double y = 0.0f;

    // Shift delay line
    for(int i = this->x.length() - 1; i > 0; i--)
    {
        this->x[i] = this->x[i-1];
    }
    this->x[0] = sample;

    // Compute filter output
    for(int i = 0; i < this->x.length(); ++i)
    {
        y += (this->x[i] * this->h[i]);
    }

    return y;
}
