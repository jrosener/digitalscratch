/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*--------------------------------------------------( playback_parameters.h )-*/
/*                                                                            */
/*  Copyright (C) 2003-2020                                                   */
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

#pragma once

#include <string>
#include <iostream>
#include <QObject>
#include <QString>

#include "app/application_const.h"

using namespace std;

class Playback_parameters
{
 private:
    float speed;        // Vinyl speed.
    float volume;       // Turntable sound volume.

 public:
    Playback_parameters();
    virtual ~Playback_parameters();

 public:
    bool  set_speed(const float &speed);
    float get_speed() const;
    bool  inc_speed(const float &speed);
    bool  set_volume(const float &volume);
    float get_volume() const;

 private:
    bool reset();
};
