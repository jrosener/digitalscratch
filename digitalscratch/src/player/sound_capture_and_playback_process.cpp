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

#include <sound_capture_and_playback_process.h>


Sound_capture_and_playback_process::Sound_capture_and_playback_process(Timecode_analyzis_process    *in_tcode_analyzis,
                                                                       Audio_track_playback_process *in_playback,
                                                                       Sound_card_access_rules      *in_sound_card)
{
    qDebug() << "Sound_capture_and_playback_process::Sound_capture_and_playback_process: create object...";

    if (in_tcode_analyzis == NULL ||
        in_playback       == NULL ||
        in_sound_card     == NULL)
    {
        qFatal("Sound_capture_and_playback_process::Sound_capture_and_playback_process: Null model objects.");
        return;
    }
    else
    {
        this->tcode_analyzis = in_tcode_analyzis;
        this->playback       = in_playback;
        this->sound_card     = in_sound_card;
    }

    qDebug() << "Sound_capture_and_playback_process::Sound_capture_and_playback_process: create object done.";

    return;
}

Sound_capture_and_playback_process::~Sound_capture_and_playback_process()
{
    qDebug() << "Sound_capture_and_playback_process::~Sound_capture_and_playback_process: delete object...";
    qDebug() << "Sound_capture_and_playback_process::~Sound_capture_and_playback_process: delete object done.";

    return;
}

bool
Sound_capture_and_playback_process::run(unsigned short int in_nb_buffer_frames)
{
    float *input_buffer_1  = NULL;
    float *input_buffer_2  = NULL;
    float *input_buffer_3  = NULL;
    float *input_buffer_4  = NULL;

    float *output_buffer_1  = NULL;
    float *output_buffer_2  = NULL;
    float *output_buffer_3  = NULL;
    float *output_buffer_4  = NULL;

    qDebug() << "Sound_capture_and_playback_process::run...";

    // Get sound card buffers.
    if(this->sound_card->get_input_buffers(in_nb_buffer_frames,
                                           &input_buffer_1,
                                           &input_buffer_2,
                                           &input_buffer_3,
                                           &input_buffer_4) == false)
    {
        qWarning() << "Sound_capture_and_playback_process::run: can not get input buffers.";
        return false;
    }
    if(this->sound_card->get_output_buffers(in_nb_buffer_frames,
                                            &output_buffer_1,
                                            &output_buffer_2,
                                            &output_buffer_3,
                                            &output_buffer_4) == false)
    {
        qWarning() << "Sound_capture_and_playback_process::run: can not get output buffers.";
        return false;
    }

    // Analyze captured data.
    if (this->tcode_analyzis->run(in_nb_buffer_frames,
                                  input_buffer_1,
                                  input_buffer_2,
                                  input_buffer_3,
                                  input_buffer_4) == false)
    {
        qWarning() << "Sound_capture_and_playback_process::run: timecode analysis failed.";
        return false;
    }

    // Play data.
    if (this->playback->run(in_nb_buffer_frames,
                            output_buffer_1,
                            output_buffer_2,
                            output_buffer_3,
                            output_buffer_4) == false)
    {
        qWarning() << "Sound_capture_and_playback_process::run: playback process failed.";
        return false;
    }

    qDebug() << "Sound_capture_and_playback_process::run done.";

    return true;
}
