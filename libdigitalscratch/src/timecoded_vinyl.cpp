/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*               libdigitalscratch: the Digital Scratch engine.               */
/*                                                                            */
/*                                                                            */
/*--------------------------------------------------------( coded_vinyl.cpp )-*/
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
#include "timecoded_vinyl.h"
#include <qmath.h>

Timecoded_vinyl::Timecoded_vinyl() : rpm(DEFAULT_RPM)
{
}

Timecoded_vinyl::~Timecoded_vinyl()
{
}

bool Timecoded_vinyl::set_rpm(dscratch_vinyl_rpm_t rpm)
{
    this->rpm = rpm;
    return true;
}

dscratch_vinyl_rpm_t Timecoded_vinyl::get_rpm()
{
    return this->rpm;
}
