/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------( playing_parameter.h )-*/
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
/*           Playing_parameter class : define a playing parameter             */
/*                                                                            */
/*============================================================================*/

#pragma once

#include <iostream>
#include <string>
#include "dscratch_parameters.h"

/**
 * Define a Playing_parameter class.\n
 * It is a generic class (abstract) for all type of audio playing parameters
 * @author Julien Rosener
 */
class Playing_parameter
{
    /* Attributes */
    private:
        /**
         * The turntable name name corresponding to this playing parameter.
         */
        string turntable_name;


    /* Constructor / Destructor */
    public:
        /**
         * Constructor.
         */
        Playing_parameter(string turntable_name);

        /**
         * Destructor.
         */
        virtual ~Playing_parameter();

    /* Methods */
    public:
        /**
         * Get turntable name.
         * @return turntable name.
         */
        string get_turntable_name();

        /**
         * Set turntable name.
         * @param turntable_name is the new turntable name (can not be empty).
         * @return TRUE if all is OK, otherwise FALSE.
         */
        bool set_turntable_name(string turntable_name);
};
