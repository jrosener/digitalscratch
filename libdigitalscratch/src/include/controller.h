/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                                                                            */
/*                                                                            */
/*-----------------------------------------------------------( controller.h )-*/
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
/*             Controller class : define a controller of a player             */
/*                                                                            */
/*============================================================================*/

#pragma once

#include <string>
#include "dscratch_parameters.h"
#include "speed.h"
#include "volume.h"

#ifdef DISPLAY_CODE_LINE
    #define TRACE_PREFIX_CONTROLLER "[Controller:" \
                + Utils::to_string(__LINE__) + "]\t\t\t" + this->get_name() + " : "
#else
    #define TRACE_PREFIX_CONTROLLER "[Controller]\t\t\t" + this->get_name() + " : "
#endif

/**
 * Define a Controller class.\n
 * It is a generic class (abstract) for all types of audio player controller.
 * It can set playback properties such as speed, direction and volume.
 * @author Julien Rosener
 */

class Controller
{
    /* Attributes */
    public:

    protected:
        /**
         * Name of the Controller.
         */
        string name;

        /**
         * Speed object pointer.
         */
        Speed *speed;

        /**
         * Volume object pointer.
         */
        Volume *volume;

        /**
         * Set to true by analyze_recording_datas() when playing parameters can
         * be used.
         */
        bool playing_parameters_ready;

    /* Constructor / Destructor */
    public:
        /**
            * Constructor.
            * @param name is the name of this controller.
            */
        Controller(string name);

        /**
         * Destructor.
         */
        virtual ~Controller();


    /* Methods */
    public:
        /**
         * Get controller name.
         * @return controller name.
         */
        string get_name();

        /**
         * Set controller name.
         * @param name is the controller name (can not be empty).
         * @return TRUE if all is OK, otherwise FALSE.
         */
        bool set_name(string name);

        /**
         * Get last detected speed, direction and volume.
         * @return TRUE if playing parameters are available, otherwise FALSE.
         */
        bool get_playing_parameters(float *speed,
                                    float *volume);

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

    protected:
        /**
         * Set flag to true if Controller is able to give playin parameters.
         * @return true is all is OK, otherwise false.
         */
        bool set_playing_parameters_ready(bool flag);
};
