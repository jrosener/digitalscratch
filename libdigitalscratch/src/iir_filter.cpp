/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*               libdigitalscratch: the Digital Scratch engine.               */
/*                                                                            */
/*                                                                            */
/*-----------------------------------------------------------( iir_filter.h )-*/
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
/*    IIR_filter class : Ininite Impulse Response filter implementation       */
/*                                                                            */
/*============================================================================*/

#include <iir_filter.h>
#include <algorithm>

using namespace std;

IIR_filter::IIR_filter(QVector<double> a, QVector<double> b)
{
    if (a.length() == 0)
    {
        this->a << 0;
    }
    this->a = a; // Copy filter kernel
    this->b = b; // Copy filter kernel
    for (int i = 0; i < b.length(); ++i)
    {
        this->x << 0;
    }
    for (int i = 0; i < a.length(); ++i)
    {
        this->y << 0;
    }
}

IIR_filter::~IIR_filter()
{
    return;
}

double IIR_filter::compute(const double &sample)
{
    double y = 0.0f;

    // Shift delay line
//    for (int i = this->x.length() - 1; i > 0; i--)
//    {
//        this->x[i] = this->x[i-1];
//    }
    std::rotate(this->x.begin(), this->x.end()-1, this->x.end()); // This is equivalent to the previous "for" loop.
                                                                  // TODO: check if calculation speed is better when using QList instead of QVector.
    this->x[0] = sample;

//    for (int i = this->y.length() - 1; i > 0; i--)
//    {
//        this->y[i] = this->y[i-1];
//    }
    std::rotate(this->y.begin(), this->y.end()-1, this->y.end()); // This is equivalent to the previous "for" loop.

    // Compute filter output
    for (int i = 0; i < this->x.length(); ++i)
    {
        y += (this->x[i] * this->b[i]);
    }
    for (int i = 1; i < this->y.length(); ++i)
    {
        y -= (this->y[i] * this->a[i]);
    }
    y /= this->a[0];
    this->y[0] = y;

    return y;
}
