/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*-----------------------------------( sound_capture_and_playback_process.h )-*/
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
/* Behavior class: process called each time there are new captured data and   */
/*                 playable data are ready.                                   */
/*                                                                            */
/*============================================================================*/

#pragma once

#include "timecode_control_process.h"
#include "manual_control_process.h"
#include "audio_track_playback_process.h"
#include "sound_driver_access_rules.h"
#include "application_const.h"

using namespace std;

enum class ProcessMode
{
    TIMECODE,
    MANUAL,
    THRU
};

class Sound_capture_and_playback_process
{
 private:
    QList<QSharedPointer<Manual_control_process>>       manual_controls;
    QList<QSharedPointer<Timecode_control_process>>     tcode_controls;
    QList<QSharedPointer<Audio_track_playback_process>> playbacks;
    QSharedPointer<Sound_driver_access_rules>           sound_card;
    unsigned short int                                  nb_decks;
    QList<ProcessMode>                                  modes;

 public:
    Sound_capture_and_playback_process(const QList<QSharedPointer<Timecode_control_process>>     &tcode_controls,
                                       const QList<QSharedPointer<Manual_control_process>>       &manual_controls,
                                       const QList<QSharedPointer<Audio_track_playback_process>> &playbacks,
                                       const QSharedPointer<Sound_driver_access_rules>           &sound_card,
                                       const unsigned short int                                  &nb_decks);
    virtual ~Sound_capture_and_playback_process();

    bool run(const unsigned short int &nb_buffer_frames);
    void set_process_mode(const ProcessMode &mode, const unsigned short &deck_index);
    ProcessMode get_process_mode(const unsigned short &deck_index) const;
};
