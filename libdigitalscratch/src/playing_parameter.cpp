/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                                                                            */
/*                                                                            */
/*--------------------------------------------------( playing_parameter.cpp )-*/
/*                                                                            */
/*  Copyright (C) 2003-2012                                                   */
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

#include "include/playing_parameter.h"
#include "include/utils.h"

Playing_parameter::Playing_parameter(string turntable_name)
{
    Utils::trace_object_life(TRACE_PREFIX_PLAYINGPARAMETER, "+ Creating Playing_parameter object...");

    if (this->set_turntable_name(turntable_name) == false)
    {
        Utils::trace_error(TRACE_PREFIX_PLAYINGPARAMETER, "Can not set turntable name");
    }

    Utils::trace_object_life(TRACE_PREFIX_PLAYINGPARAMETER, "+ Playing_parameter object created");
}

Playing_parameter::~Playing_parameter()
{
    Utils::trace_object_life(TRACE_PREFIX_PLAYINGPARAMETER, "- Deleting Playing_parameter object...");

    Utils::trace_object_life(TRACE_PREFIX_PLAYINGPARAMETER, "- Playing_parameter object deleted");
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
        return false;
    }

    this->turntable_name = turntable_name;

    return true;
}
