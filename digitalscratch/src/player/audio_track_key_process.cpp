/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*---------------------------------------------( audio_file_key_process.cpp )-*/
/*                                                                            */
/*  Copyright (C) 2003-2013                                                   */
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

#include <QtDebug>

#include "audio_track_key_process.h"
#include "audio_track.h"

Audio_track_key_process::Audio_track_key_process(Audio_track *in_at)
{
    qDebug() << "Audio_track_key_process::Audio_track_key_process: create object...";

    if (in_at == NULL)
    {
        qCritical() << "Audio_track_key_process::Audio_track_key_process: audio track is NULL";
    }
    else
    {
        this->at = in_at;
    }

    qDebug() << "Audio_track_key_process::Audio_track_key_process: create object done.";

    return;
}

Audio_track_key_process::~Audio_track_key_process()
{
    qDebug() << "Audio_track_key_process::~Audio_track_key_process: delete object...";

    qDebug() << "Audio_track_key_process::~Audio_track_key_process: delete object done.";

    return;
}

bool
Audio_track_key_process::run()
{
    qDebug() << "Audio_track_key_process::run...";

    // Check if there are decoded audio data in audio track.
    if (this->at->get_end_of_samples() == 0)
    {
        return false;
    }

    // Compute the musical key.
    this->at->set_music_key(kfinder_get_key(at->get_samples(),
                                            at->get_end_of_samples(),
                                            SAMPLE_RATE, 2));

    qDebug() << "Audio_track_key_process::run: done.";

    return true;
}
