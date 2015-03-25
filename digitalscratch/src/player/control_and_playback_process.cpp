/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*---------------------------------( sound_capture_and_playback_process.cpp )-*/
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

#include <QtDebug>

#include "player/control_and_playback_process.h"
#include "app/application_logging.h"


Sound_capture_and_playback_process::Sound_capture_and_playback_process(const QList<QSharedPointer<Timecode_control_process>>     &tcode_controls,
                                                                       const QList<QSharedPointer<Manual_control_process>>       &manual_controls,
                                                                       const QList<QSharedPointer<Audio_track_playback_process>> &playbacks,
                                                                       const QSharedPointer<Sound_driver_access_rules>           &sound_card,
                                                                       const unsigned short int                                  &nb_decks)
{
    if (tcode_controls.count()  == 0 ||
        playbacks.count()       == 0 ||
        manual_controls.count() == 0 ||
        sound_card.data()       == nullptr)
    {
        qCWarning(DS_PLAYBACK) << "bad input parameters";
        return;
    }
    else
    {
        this->tcode_controls  = tcode_controls;
        this->manual_controls = manual_controls;
        this->playbacks       = playbacks;
        this->sound_card      = sound_card;
        this->nb_decks        = nb_decks;
        for (unsigned short int i = 0; i < nb_decks; i++)
        {
            this->modes << ProcessMode::TIMECODE;
        }
    }

    return;
}

Sound_capture_and_playback_process::~Sound_capture_and_playback_process()
{
    return;
}

bool
Sound_capture_and_playback_process::run(const unsigned short int &nb_buffer_frames)
{
    QList<float *> input_buffers;
    QList<float *> output_buffers;

    // Get sound card buffers.
    if(this->sound_card->get_input_buffers(nb_buffer_frames, input_buffers) == false)
    {
        qCWarning(DS_SOUNDCARD) << "can not get input buffers";
        return false;
    }
    if(this->sound_card->get_output_buffers(nb_buffer_frames, output_buffers) == false)
    {
        qCWarning(DS_SOUNDCARD) << "can not get output buffers";
        return false;
    }

    for (unsigned short int i = 0; i < this->tcode_controls.size(); i++)
    {
        switch(this->modes[i])
        {
            case ProcessMode::TIMECODE:
            {
                // Analyze captured data with libdigitalscratch.
                if (this->tcode_controls[i]->run(nb_buffer_frames,
                                                 input_buffers[i*2],
                                                 input_buffers[i*2 + 1]) == false)
                {
                    qCWarning(DS_PLAYBACK) << "timecode analysis failed for deck " << i + 1;
                    return false;
                }

                // Play data.
                if (this->playbacks[i]->run(output_buffers[i*2],
                                            output_buffers[i*2 + 1],
                                            nb_buffer_frames) == false)
                {
                    qCWarning(DS_PLAYBACK) << "playback process failed for deck " << i + 1;
                    return false;
                }

                break;
            }
            case ProcessMode::THRU:
            {
                // Copy data from input sound card buffers to output ones (bypass playback).
                memcpy(output_buffers[i*2],     input_buffers[i*2],     nb_buffer_frames * sizeof(float));
                memcpy(output_buffers[i*2 + 1], input_buffers[i*2 + 1], nb_buffer_frames * sizeof(float));
                break;
            }
            case ProcessMode::MANUAL:
            {
                // Get playback parameters (mainly speed) from gui buttons.
                if (this->manual_controls[i]->run() == false)
                {
                    qCWarning(DS_PLAYBACK) << "manual playback control failed for deck " << i + 1;
                    return false;
                }

                // Play data.
                if (this->playbacks[i]->run(output_buffers[i*2],
                                            output_buffers[i*2 + 1],
                                            nb_buffer_frames) == false)
                {
                    qCWarning(DS_PLAYBACK) << "playback process failed for deck " << i + 1;
                    return false;
                }
                break;
            }
        }
    }

    return true;
}

void
Sound_capture_and_playback_process::set_process_mode(const ProcessMode &mode, const unsigned short int &deck_index)
{
    this->modes[deck_index] = mode;
}

ProcessMode
Sound_capture_and_playback_process::get_process_mode(const unsigned short int &deck_index) const
{
    return this->modes[deck_index];
}
