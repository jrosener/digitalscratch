/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                                                                            */
/*                                                                            */
/*--------------------------------------------------( playing_parameter.cpp )-*/
/*                                                                            */
/*  Copyright (C) 2003-2014                                                   */
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
/*           Playing_parameter class : define a Playing_parameter             */
/*                                                                            */
/*============================================================================*/

#include <iostream>
#include <cstdio>
#include <string>

using namespace std;

#include "log.h"
#include "playing_parameter.h"

Playing_parameter::Playing_parameter(string turntable_name)
{
    this->set_turntable_name(turntable_name);
}

Playing_parameter::~Playing_parameter()
{
}

string Playing_parameter::get_turntable_name()
{
    return this->turntable_name;
}

bool Playing_parameter::set_turntable_name(string turntable_name)
{
    // Turntable name can not be empty.
    if (turntable_name == "")
    {
        qCCritical(DSLIB_CONTROLLER) << "Can not set turntable name";
        return false;
    }

    this->turntable_name = turntable_name;

    return true;
}
