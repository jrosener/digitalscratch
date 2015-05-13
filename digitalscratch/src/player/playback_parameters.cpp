/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*------------------------------------------------( playback_parameters.cpp )-*/
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
/*                Class defining playback parameters of a track.              */
/*                                                                            */
/*============================================================================*/

#include <QtDebug>
#include <math.h>

#include "player/playback_parameters.h"

Playback_parameters::Playback_parameters() : new_speed {false},
                                             new_volume {false},
                                             new_data {false}
{
    this->reset();

    return;
}

Playback_parameters::~Playback_parameters()
{
    return;
}

bool
Playback_parameters::reset()
{
    this->speed  = 0.0;
    this->volume = 0.0;

    return true;
}

bool
Playback_parameters::set_speed(const float &speed)
{
    this->speed = speed;
    return true;
}

float
Playback_parameters::get_speed() const
{
    return this->speed;
}

bool
Playback_parameters::inc_speed(const float &speed)
{
    if (speed != 0.0)
    {
        this->set_speed(this->speed + speed);
    }

    return true;
}

bool
Playback_parameters::set_speed_state(const bool &is_new)
{
    this->new_speed = is_new;

    return true;
}

bool
Playback_parameters::is_new_speed() const
{
    return this->new_speed;
}

bool
Playback_parameters::set_volume(const float &volume)
{
    if (volume != this->volume)
    {
        this->volume = volume;
    }

    return true;
}

float
Playback_parameters::get_volume() const
{
    return this->volume;
}

bool
Playback_parameters::set_volume_state(const bool &is_new)
{
    this->new_volume = is_new;

    return true;
}

bool
Playback_parameters::is_new_volume() const
{
    return this->new_volume;
}

bool
Playback_parameters::set_data_state(const bool &are_new)
{
    this->new_data = are_new;

    if (are_new == false)
    {
        this->new_speed  = false;
        this->new_volume = false;
    }

    return true;
}

