/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------( speed.h )-*/
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

#pragma once

#include "dscratch_parameters.h"
#include "digital_scratch_api.h"

#define DEFAULT_MAX_NB_NO_NEW_SPEED_FOUND 1
#define DEFAULT_MAX_NB_CYCLE_BEFORE_STARTING  1

/**
 * Define a Speed class.\n
 * Define the playing music speed concept
 * @author Julien Rosener
 */
class Speed
{
    /* Attributes */
    private:
        /**
         * Speed value
         */
        float value;

        /**
         * This is the number of consecutive no new speed found.
         */
        short int nb_no_new_speed_found;

        /**
         * Number of cycle at speed equal to zero before starting playback.
         */
        short int wait_cycle_for_starting;

        /**
         * This is the maximum number of consecutive no new speed found.
         */
        int max_nb_no_new_speed_found;

        /**
         * Maximum number of cycle at speed equal to zero before starting
         * playback.
         */
        int max_nb_cycle_before_starting;

        /**
         * Old speed value.
         */
        float old_value;


    /* Constructor / Destructor */
    public:
        /**
         * Constructor.
         * @param turntable_name is the name of the virtual turntable for which
         *        we analyze playing parameters.
         */
        Speed();

        /**
         * Destructor
         */
        virtual ~Speed();


    /* Methods */
    public:
        /**
         * Get speed value
         * @return the playing speed value
         */
        float get_value();

        /**
         * Set a new speed value.
         * If the speed is NO_NEW_SPEED_FOUND for max_nb_no_new_speed_found, we
         * consider that speed = 0.0.
         * @param speed_value is the playing speed value.
         * @return TRUE if all is OK, otherwise false.
         */
        bool set_value(float speed_value);

        /**
         * Get maximum number of time we allow NO_NEW_SPEED_FOUND.
         * @return the max_nb_no_new_speed_found value.
         */
        int get_max_nb_no_new_speed_found();

        /**
         * Set maximum number of time we allow NO_NEW_SPEED_FOUND.
         * @param nb is the max_nb_no_new_speed_found value (must be > 0).
         * @return TRUE if all is OK, otherwise FALSE.
         */
        bool set_max_nb_no_new_speed_found(int nb);

        /**
         * Set number of time that we will wait for new speed before starting.
         * @return the nb_cycle_before_starting value.
         */
        int get_max_nb_cycle_before_starting();

        /**
         * Set number of time that we will wait for new speed before starting.
         * @param nb is the nb_cycle_before_starting value (must be > 0).
         * @return TRUE if all is OK, otherwise FALSE.
         */
        bool set_max_nb_cycle_before_starting(int nb);

};
