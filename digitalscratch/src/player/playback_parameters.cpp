/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*------------------------------------------------( playback_parameters.cpp )-*/
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
/*                Class defining playback parameters of a track.              */
/*                                                                            */
/*============================================================================*/

#include <QtDebug>
#include <math.h>

#include "playback_parameters.h"

Playback_parameters::Playback_parameters()
{
    qDebug() << "Playback_parameters::Playback_parameters: create object...";

    this->reset();

    qDebug() << "Playback_parameters::Playback_parameters: create object done";

    return;
}

Playback_parameters::~Playback_parameters()
{
    qDebug() << "Playback_parameters::~Playback_parameters: delete object...";
    qDebug() << "Playback_parameters::~Playback_parameters: delete object done.";

    return;
}

bool
Playback_parameters::reset()
{
    qDebug() << "Playback_parameters::reset...";

    this->speed    = 0.0;
    this->position = 0.0;

    qDebug() << "Playback_parameters::reset done.";

    return true;
}

bool
Playback_parameters::set_speed(float in_speed)
{
    qDebug() << "Playback_parameters::set_speed...";

    if (in_speed != this->speed)
    {
        this->speed = in_speed;
        emit speed_changed((double)(floorf((this->speed * 100.0) * 10.0) / 10.0));
        //cout << in_speed << endl;
    }

    qDebug() << "Playback_parameters::set_speed done.";

    return true;
}

float
Playback_parameters::get_speed()
{
    qDebug() << "Playback_parameters::get_speed...";
    qDebug() << "Playback_parameters::get_speed done.";

    return this->speed;
}

bool
Playback_parameters::set_new_speed(bool in_new)
{
    qDebug() << "Playback_parameters::set_new_speed...";

    this->new_speed = in_new;

    qDebug() << "Playback_parameters::set_new_speed done.";

    return true;
}

bool
Playback_parameters::is_new_speed()
{
    qDebug() << "Playback_parameters::is_new_speed...";
    qDebug() << "Playback_parameters::is_new_speed done.";

    return this->new_speed;
}

bool
Playback_parameters::set_position(float in_position)
{
    qDebug() << "Playback_parameters::set_position...";

    if (in_position != this->position)
    {
        this->position = in_position;
        emit position_changed((double)this->position);
    }

    qDebug() << "Playback_parameters::set_position done.";

    return true;
}

float
Playback_parameters::get_position()
{
    qDebug() << "Playback_parameters::get_position...";
    qDebug() << "Playback_parameters::get_position done.";

    return this->position;
}

bool
Playback_parameters::set_new_position(bool in_new)
{
    qDebug() << "Playback_parameters::set_new_position...";

    this->new_position = in_new;

    qDebug() << "Playback_parameters::set_new_position done.";

    return true;
}

bool
Playback_parameters::is_new_position()
{
    qDebug() << "Playback_parameters::is_new_position...";
    qDebug() << "Playback_parameters::is_new_position done.";

    return this->new_position;
}

bool
Playback_parameters::set_volume(float in_volume)
{
    qDebug() << "Playback_parameters::set_volume...";

    if (in_volume != this->volume)
    {
        this->volume = in_volume;
        emit volume_changed((double)(floorf((this->volume * 100.0) * 10.0) / 10.0));
    }

    qDebug() << "Playback_parameters::set_volume done.";

    return true;
}

float
Playback_parameters::get_volume()
{
    qDebug() << "Playback_parameters::get_volume...";
    qDebug() << "Playback_parameters::get_volume done.";

    return this->volume;
}

bool
Playback_parameters::set_new_volume(bool in_new)
{
    qDebug() << "Playback_parameters::set_new_volume...";

    this->new_volume = in_new;

    qDebug() << "Playback_parameters::set_new_volume done.";

    return true;
}

bool
Playback_parameters::is_new_volume()
{
    qDebug() << "Playback_parameters::is_new_volume...";
    qDebug() << "Playback_parameters::is_new_volume done.";

    return this->new_volume;
}

bool
Playback_parameters::set_new_data(bool in_new_data)
{
    qDebug() << "Playback_parameters::new_data...";

    this->new_data = in_new_data;

    if (in_new_data == false)
    {
        this->new_speed    = false;
        this->new_position = false;
        this->new_volume   = false;
    }

    qDebug() << "Playback_parameters::new_data done.";

    return true;
}

bool
Playback_parameters::get_new_data()
{
    qDebug() << "Playback_parameters::get_new_data...";
    qDebug() << "Playback_parameters::get_new_data done.";

    return this->new_data;
}
