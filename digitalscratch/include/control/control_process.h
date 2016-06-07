/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*------------------------------------------------------( control_process.h )-*/
/*                                                                            */
/*  Copyright (C) 2003-2016                                                   */
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
/* Behavior class: mother class for controlling playback.                     */
/* Derived for timecode control, keyboard/mouse/gui control,...               */
/*                                                                            */
/*============================================================================*/

#pragma once

#include <QObject>
#include <QSharedPointer>
#include "player/playback_parameters.h"

using namespace std;

class Control_process : public QObject // FIXME : necessary ?
{
    Q_OBJECT

 protected:
    QSharedPointer<Playback_parameters> params;

 public:
    explicit Control_process(const QSharedPointer<Playback_parameters> &param);
    virtual ~Control_process();

 signals:
    void speed_changed(const float &speed);
};
