/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*------------------------------------------------( playback_parameters.cpp )-*/
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
/*                Class defining playback parameters of a track.              */
/*                                                                            */
/*============================================================================*/

#include <QtDebug>
#include <math.h>
#include "playback_parameters.h"

Playback_parameters::Playback_parameters()
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
    this->speed = 0.0;

    return true;
}

bool
Playback_parameters::set_speed(float in_speed)
{
    if (in_speed != this->speed)
    {
        this->speed = in_speed;
        emit speed_changed(this->speed);
    }

    return true;
}

float
Playback_parameters::get_speed()
{
    return this->speed;
}

bool
Playback_parameters::inc_speed(float in_speed)
{
    if (in_speed != 0.0)
    {
        this->set_speed(this->speed + in_speed);
    }

    return true;
}

bool
Playback_parameters::set_new_speed(bool in_new)
{
    this->new_speed = in_new;

    return true;
}

bool
Playback_parameters::is_new_speed()
{
    return this->new_speed;
}

bool
Playback_parameters::set_volume(float in_volume)
{
    if (in_volume != this->volume)
    {
        this->volume = in_volume;
        emit volume_changed((double)(floorf((this->volume * 100.0) * 10.0) / 10.0));
    }

    return true;
}

float
Playback_parameters::get_volume()
{
    return this->volume;
}

bool
Playback_parameters::set_new_volume(bool in_new)
{
    this->new_volume = in_new;

    return true;
}

bool
Playback_parameters::is_new_volume()
{
    return this->new_volume;
}

bool
Playback_parameters::set_new_data(bool in_new_data)
{
    this->new_data = in_new_data;

    if (in_new_data == false)
    {
        this->new_speed  = false;
        this->new_volume = false;
    }

    return true;
}

bool
Playback_parameters::get_new_data()
{
    return this->new_data;
}
