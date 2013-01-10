/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                                                                            */
/*                                                                            */
/*-----------------------------------------------------------( position.cpp )-*/
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
/*          Position class : define the needle position concept               */
/*                                                                            */
/*============================================================================*/

#include <iostream>
#include <cstdio>
#include <string>

using namespace std;

#include "include/dscratch_parameters.h"
#include "include/position.h"
#include "include/utils.h"

Position::Position(string turntable_name) : Playing_parameter(turntable_name)
{
    Utils::trace_object_life(TRACE_PREFIX_POSITION,
                             "+ Creating Position object...");

    this->value                   = 0.0;
    this->nb_new_value_before_set = 0;
    this->old_value               = 0.0;

    Utils::trace_object_life(TRACE_PREFIX_POSITION,
                             "+ Position object created");
}

Position::~Position()
{
    Utils::trace_object_life(TRACE_PREFIX_POSITION,
                             "- Deleting Position object...");

    Utils::trace_object_life(TRACE_PREFIX_POSITION,
                             "- Position object deleted");
}

float Position::get_value()
{
    return this->value;
}

bool Position::set_value(float position_value)
{
    if (position_value != NO_NEW_POSITION_FOUND)
    {
        if (position_value >= 0.0)
        {
            this->nb_new_value_before_set++;

            if (this->nb_new_value_before_set >= 2)
            {
                // This new value is close to the previous value,
                // so this value is OK.
                if ((position_value > (this->old_value-1.0))
                    && (position_value < (this->old_value+1.0)))
                {
                    this->value = position_value;
                    #ifdef TRACE_OBJECT_ATTRIBUTS_CHANGE
                        Utils::trace_object_attributs_change(TRACE_PREFIX_POSITION,
                            "Changing position value to " + Utils::to_string(this->value));
                    #endif

                    this->nb_new_value_before_set = 0;

                    return true;
                }
                else
                {
                    this->nb_new_value_before_set = 0;
                    return false;
                }
            }
            else
            {
                this->old_value = position_value;

                // This value is new but has not been confirmed by another
                // value.
                return true;
            }
        }
        else
        {
            // Position should be positive.
            return false;
        }
    }

    this->value = position_value;

    #ifdef TRACE_OBJECT_ATTRIBUTS_CHANGE
        Utils::trace_object_attributs_change(TRACE_PREFIX_POSITION,
            "Changing position value to " + Utils::to_string(this->value));
    #endif

    return true;
}
