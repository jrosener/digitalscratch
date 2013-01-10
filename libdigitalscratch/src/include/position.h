/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                                                                            */
/*                                                                            */
/*-------------------------------------------------------------( position.h )-*/
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

#ifndef _POSITION_H_
#define _POSITION_H_

#include "dscratch_parameters.h"
#include "playing_parameter.h"
#include "digital_scratch_api.h"

#ifdef DISPLAY_CODE_LINE
    #define TRACE_PREFIX_POSITION "[Position:" + Utils::to_string(__LINE__) + "]\t\t\t" + this->get_turntable_name() + " : "
#else
    #define TRACE_PREFIX_POSITION "[Position]\t\t\t\t" + this->get_turntable_name() + " : "
#endif

/**
 * Define a Position class (position of needle on vinyl disc).\n
 * Base class : Playing_parameter\n
 * Define the needle position concept.
 * @author Julien Rosener
 */
class Position : public Playing_parameter
{
    /* Attributes */
    public:
        /**
         * Position value
         */
        float value;

    private:
        /**
         * Number of new positions found before setting the real new position.
         */
        int nb_new_value_before_set;

        /**
         * The latest position value.
         */
        float old_value;

    /* Constructor / Destructor */
    public:
        /**
         * Constructor
         */
        Position(string turntable_name);

        /**
         * Destructor
         */
        virtual ~Position();


    /* Methods */
    public:
        /**
         * Get position value.
         * @return the playing position value
         */
        float get_value();

        /**
         * Set a new position value.
         * @param position_value is the playing position value. New value will
         *                       be stored only if it confirm the previous one
         *                       (for that
         *                       previous_value-1<new_value<previous_value+1).
         * @return TRUE if value is positive or NO_NEW_POSITION_FOUND,
         *         otherwise FALSE.
         */
        bool set_value(float position_value);
};

#endif //_POSITION_H_
