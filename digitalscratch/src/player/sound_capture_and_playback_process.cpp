/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*---------------------------------( sound_capture_and_playback_process.cpp )-*/
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
/* Behavior class: process called each time there are new captured data and   */
/*                 playable data are ready.                                   */
/*                                                                            */
/*============================================================================*/

#include <QtDebug>

#include "sound_capture_and_playback_process.h"
#include "application_logging.h"


Sound_capture_and_playback_process::Sound_capture_and_playback_process(QList<QSharedPointer<Timecode_control_process>>     &in_tcode_controls,
                                                                       QList<QSharedPointer<Audio_track_playback_process>> &in_playbacks,
                                                                       QSharedPointer<Sound_driver_access_rules>           &in_sound_card,
                                                                       unsigned short int                                   in_nb_decks)
{
    if (in_tcode_controls.count() == 0 ||
        in_playbacks.count()      == 0 ||
        in_sound_card.data()      == NULL)
    {
        qCWarning(DS_PLAYBACK) << "bad input parameters";
        return;
    }
    else
    {
        this->tcode_controls = in_tcode_controls;
        this->playbacks      = in_playbacks;
        this->sound_card     = in_sound_card;
        this->nb_decks       = in_nb_decks;
    }

    return;
}

Sound_capture_and_playback_process::~Sound_capture_and_playback_process()
{
    return;
}

bool
Sound_capture_and_playback_process::run(unsigned short int in_nb_buffer_frames)
{
    QList<float *> input_buffers;
    QList<float *> output_buffers;

    // Get sound card buffers. // TODO : a ne faire que si mode = timecode
    if(this->sound_card->get_input_buffers(in_nb_buffer_frames, input_buffers) == false)
    {
        qCWarning(DS_SOUNDCARD) << "can not get input buffers";
        return false;
    }
    if(this->sound_card->get_output_buffers(in_nb_buffer_frames, output_buffers) == false)
    {
        qCWarning(DS_SOUNDCARD) << "can not get output buffers";
        return false;
    }

    // Analyze captured data. // TODO : a ne faire que si mode = timecode
    for (unsigned short int i = 0; i < this->tcode_controls.size(); i++)
    {
        if (this->tcode_controls[i]->run(in_nb_buffer_frames,
                                         input_buffers[i*2],
                                         input_buffers[i*2 + 1]) == false)
        {
            qCWarning(DS_PLAYBACK) << "timecode analysis failed for deck " << i + 1;
            return false;
        }
    }

    // TODO: Faire un cas particulier pour mode = manual.

    // Play data.
    for (unsigned short int i = 0; i < this->playbacks.size(); i++)
    {
        if (this->playbacks[i]->run(in_nb_buffer_frames,
                                    output_buffers[i*2],
                                    output_buffers[i*2 + 1]) == false)
        {
            qCWarning(DS_PLAYBACK) << "playback process failed for deck 1";
            return false;
        }
    }

    return true;
}
