/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                                                                            */
/*                                                                            */
/*--------------------------------------------------------------( speed.cpp )-*/
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
/*          Speed class : define the playing music speed concept              */
/*                                                                            */
/*============================================================================*/

#include <iostream>
#include <cstdio>
#include <string>

using namespace std;

#include "log.h"
#include "dscratch_parameters.h"
#include "speed.h"

Speed::Speed()
{
    this->value = 0.0;
}

Speed::~Speed()
{
}

float Speed::get_value()
{
    return this->value;
}

bool Speed::set_value(float speed_value)
{
    this->value = speed_value;
    qCDebug(DSLIB_SPEED) << "Changing speed value to " << QString::number(this->value);

    return true;
}

