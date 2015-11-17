/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*-----------------------------------------------( manual_control_process.h )-*/
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
/* Behavior class: determine playback parametrs based on keyboard and gui     */
/*                 buttons.                                                   */
/*                                                                            */
/*============================================================================*/

#pragma once

#include <iostream>
#include <QSharedPointer>
#include <QObject>

#include "player/playback_parameters.h"
#include "app/application_const.h"
#include "control/control_process.h"

using namespace std;

class Manual_control_process : public Control_process
{
    Q_OBJECT

 private:
    float speed;
    bool  do_temp_inc_speed;                      // True if we are in a temporary speed acceleration phase.
    float previous_speed;                         // Store speed before starting a temporary acceleration phase.
    unsigned short int nb_temp_speed_inc_cycles;  // Nb cycles used for the temporary speed acceleration.

 public:
    explicit Manual_control_process(const QSharedPointer<Playback_parameters> &param);
    virtual ~Manual_control_process();

    bool run();
    void inc_speed(const float &speed_inc);
    void reset_speed_to_100p();
    void inc_temporary_speed(const float &temp_speed_inc, const unsigned short int &nb_cycles);

 private:
    void set_new_speed(const float &speed);
};
