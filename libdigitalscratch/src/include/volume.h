/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                                                                            */
/*                                                                            */
/*---------------------------------------------------------------( volume.h )-*/
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
/*               Volume class : define the music volume concept.              */
/*                                                                            */
/*============================================================================*/

#pragma once

#include "dscratch_parameters.h"
#include "playing_parameter.h"

#ifdef DISPLAY_CODE_LINE
    #define TRACE_PREFIX_VOLUME "[Volume:" + Utils::to_string(__LINE__) + "]\t\t\t" + this->get_turntable_name() + " : "
#else
    #define TRACE_PREFIX_VOLUME "[Volume]\t\t\t\t" + this->get_turntable_name() + " : "
#endif

/**
 * Define a Volume class.\n
 * Base class : Playing_parameter\n
 * Define the music volume concept.
 * @author Julien Rosener
 */
class Volume : public Playing_parameter
{
    /* Attributes */
    private:
        /**
         * Volume value
         */
        float value;

    /* Constructor / Destructor */
    public:
        /**
         * Constructor
         * @param turntable_name is the turntable name
         */
        Volume(string turntable_name);

        /**
         * Destructor
         */
        virtual ~Volume();


    /* Methods */
    public:
        /**
         * Get volume value.
         * @return the volume value
         */
        float get_value();

        /**
         * Set a new volume value.
         * @param volume_value is the new volume value
         * @return TRUE if all is OK, otherwise FALSE
         */
        bool set_value(float volume_value);
};
