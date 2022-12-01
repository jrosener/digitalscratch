/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*---------------------------------------------( audio_file_key_process.cpp )-*/
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
/*    Behavior class: process to compute a musical key for an audio track     */
/*                                                                            */
/*============================================================================*/

#include <QtDebug>

#include "tracks/audio_track_key_process.h"
#include "tracks/audio_track.h"
#include "app/application_logging.h"
#include "utils.h"

Audio_track_key_process::Audio_track_key_process(const QSharedPointer<Audio_track> &at)
{
    if (at.data() == nullptr)
    {
        qCWarning(DS_MUSICKEY) << "audio track is null";
    }
    else
    {
        this->at = at;
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
    QString key = this->kfinder_get_key(at->get_samples(),
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

QString
Audio_track_key_process::kfinder_get_key(short signed int   *samples,
                                         unsigned int        nb_samples,
                                         short unsigned int  frame_rate,
                                         short unsigned int  nb_channels)
{
    // Check input parameter.
    if ((samples == NULL) || (nb_samples == 0) || (frame_rate == 0) || (nb_channels == 0))
    {
        return "";
    }

    // Build the main computing object.
    KeyFinder::KeyFinder k;

    // Build an empty audio object
    KeyFinder::AudioData a;

    // Prepare the object for your audio stream
    a.setFrameRate(frame_rate);
    a.setChannels(nb_channels);
    a.addToSampleCount(nb_samples);

    // Copy your audio into the object (as float).
    for (unsigned int i = 0; i < nb_samples; i++)
    {
        a.setSample(i, (float)samples[i]);
    }

    // Run the analysis
    KeyFinder::key_t r;
    try
    {
        r =  k.keyOfAudio(a);
    }
    catch(const std::exception& e)
    {
        qCCritical(DS_MUSICKEY) << "libKeyFinder: exception:" << e.what();
        return "";
    }
    catch(...)
    {
        qCCritical(DS_MUSICKEY) << "libKeyFinder: unknown exception:";
        return "";
    }


    // And do something with the result!
    switch(r)
    {
        case KeyFinder::A_MAJOR:      return "AM";
        case KeyFinder::A_MINOR:      return "Am";
        case KeyFinder::B_FLAT_MAJOR: return "BbM";
        case KeyFinder::B_FLAT_MINOR: return "Bbm";
        case KeyFinder::B_MAJOR:      return "BM";
        case KeyFinder::B_MINOR:      return "Bm";
        case KeyFinder::C_MAJOR:      return "CM";
        case KeyFinder::C_MINOR:      return "Cm";
        case KeyFinder::D_FLAT_MAJOR: return "DbM";
        case KeyFinder::D_FLAT_MINOR: return "Dbm";
        case KeyFinder::D_MAJOR:      return "DM";
        case KeyFinder::D_MINOR:      return "Dm";
        case KeyFinder::E_FLAT_MAJOR: return "EbM";
        case KeyFinder::E_FLAT_MINOR: return "Ebm";
        case KeyFinder::E_MAJOR:      return "EM";
        case KeyFinder::E_MINOR:      return "Em";
        case KeyFinder::F_MAJOR:      return "FM";
        case KeyFinder::F_MINOR:      return "Fm";
        case KeyFinder::G_FLAT_MAJOR: return "GbM";
        case KeyFinder::G_FLAT_MINOR: return "Gbm";
        case KeyFinder::G_MAJOR:      return "GM";
        case KeyFinder::G_MINOR:      return "Gm";
        case KeyFinder::A_FLAT_MAJOR: return "AbM";
        case KeyFinder::A_FLAT_MINOR: return "Abm";
        case KeyFinder::SILENCE:      return "";
        default:                      return "";
    }
}
