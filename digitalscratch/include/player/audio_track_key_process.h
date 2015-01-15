/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------( audio_track_key_process.h )-*/
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
/*    Behavior class: process to compute a musical key for an audio track     */
/*                                                                            */
/*============================================================================*/

#pragma once

#include <iostream>
#include <QSharedPointer>

#include <audio_track.h>
#include <application_const.h>
#include <keyfinder_api.h>
#include <application_logging.h>

using namespace std;

class Audio_track_key_process
{
 private:
    QSharedPointer<Audio_track> at;

 public:
    Audio_track_key_process(QSharedPointer<Audio_track> &in_at);
    virtual ~Audio_track_key_process();

    bool run();         // Compute music key of the track and set it to the Audio_track object.
};
