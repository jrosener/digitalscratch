/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*--------------------------------------------------( playback_parameters.h )-*/
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

#ifndef PLAYBACK_PARAMETERS_H_
#define PLAYBACK_PARAMETERS_H_

#include <string>
#include <iostream>
#include <QObject>
#include <QString>
#include <application_const.h>

using namespace std;

class Playback_parameters : public QObject
{
    Q_OBJECT

 private:
    float speed;        // Vinyl speed.
    float volume;       // Turntable sound volume.
    bool  new_speed;    // If true: speed is updated.
    bool  new_volume;   // If true: volume is updated.
    bool  new_data;     // If true: data are updated.

 public:
    Playback_parameters();
    virtual ~Playback_parameters();

 public:
    bool  set_speed(float in_speed);
    float get_speed();
    bool  set_new_speed(bool in_new);
    bool  is_new_speed();

    bool  set_volume(float in_volume);
    float get_volume();
    bool  set_new_volume(bool in_new);
    bool  is_new_volume();

    bool  set_new_data(bool in_new_data);
    bool  get_new_data();

 private:
    bool reset();

 signals:
    void speed_changed(double in_speed);       // Speed.
    void volume_changed(double in_volume);     // Volume.
};

#endif /* PLAYBACK_PARAMETERS_H_ */
