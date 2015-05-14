/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                                                                            */
/*                                                                            */
/*-----------------------------------------------------------( controller.h )-*/
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
/*             Controller class : define a controller of a player             */
/*                                                                            */
/*============================================================================*/

#pragma once

#include <string>

#include "dscratch_parameters.h"
#include "speed.h"
#include "volume.h"

/**
 * Define a Controller class.\n
 * It is a generic class (abstract) for all types of audio player controller.
 * It can set playback properties such as speed, direction and volume.
 * @author Julien Rosener
 */

class Controller // FIXME: still necessary ?
{
    /* Attributes */
    protected:
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
        Controller();
        virtual ~Controller();


    /* Methods */
    public:
        /**
         * Get last detected speed, direction and volume.
         * @return TRUE if playing parameters are available, otherwise FALSE.
         */
        bool get_playing_parameters(float *speed,
                                    float *volume);

    protected:
        /**
         * Set flag to true if Controller is able to give playin parameters.
         * @return true is all is OK, otherwise false.
         */
        bool set_playing_parameters_ready(bool flag);
};
