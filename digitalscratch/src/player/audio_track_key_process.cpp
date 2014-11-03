/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*---------------------------------------------( audio_file_key_process.cpp )-*/
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
/*    Behavior class: process to compute a musical key for an audio track     */
/*                                                                            */
/*============================================================================*/

#include <QtDebug>

#include "audio_track_key_process.h"
#include "audio_track.h"
#include "utils.h"

Audio_track_key_process::Audio_track_key_process(QSharedPointer<Audio_track> &in_at)
{
    if (in_at.data() == NULL)
    {
        qCWarning(DS_MUSICKEY) << "audio track is NULL";
    }
    else
    {
        this->at = in_at;
    }

    return;
}

Audio_track_key_process::~Audio_track_key_process()
{
    return;
}

bool
Audio_track_key_process::run()
{
    // Check if there are decoded audio data in audio track.
    if (this->at->get_end_of_samples() == 0)
    {
        return false;
    }

    // Compute the musical key.
    QString key = kfinder_get_key(at->get_samples(),
                                  at->get_end_of_samples(),
                                  at->get_sample_rate(),
                                  2);
    if (key != "")
    {
        // Transform music key to a clock number.
        key = Utils::convert_music_key_to_clock_number(key);

        // Set music key to the audio track.
        this->at->set_music_key(key);
    }
    else
    {
        qCWarning(DS_MUSICKEY) << "no music key found" << this->at->get_path();
        return false;
    }

    return true;
}
