/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*---------------------------------------------( timecode_control_process.h )-*/
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
/* Behavior class: analyze captured timecode and determine playback           */
/* parameters.                                                                */
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

class Timecode_control_process : public Control_process
{
    Q_OBJECT

 private:
    int dscratch_id;
    unsigned short int waitfor_emit_speed_changed; // Do not update speed (in gui) every time.

 public:
    Timecode_control_process(const QSharedPointer<Playback_parameters> &param,
                             const QString                             &vinyl_type,
                             const unsigned int                        &sample_rate);
    virtual ~Timecode_control_process();

    bool run(const unsigned short int &nb_samples,
             const float              *samples_1,
             const float              *samples_2);

    int get_dscratch_id();
};
