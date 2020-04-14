/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------( sound_card_access_rules.h )-*/
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
/*   Behavior class: access sound card via Jack (open, close, list, ...)      */
/*                                                                            */
/*============================================================================*/

#pragma once

#include <iostream>
#include <QObject>
#include <QString>

#include "audiodev/audio_io_control_rules.h"
#include "jack/jack.h"
#include "app/application_const.h"

typedef jack_client_t *     AUDIO_STREAM_TYPE;
typedef JackProcessCallback AUDIO_CALLBACK_TYPE;
typedef jack_nframes_t      AUDIO_CALLBACK_NB_FRAMES_TYPE;

#define CLIENT_NAME "digitalscratch"

using namespace std;

class Jack_client_control_rules : public Audio_IO_control_rules
{
 private:
    AUDIO_STREAM_TYPE stream;
    QList<jack_port_t*> input_port;
    QList<jack_port_t*> output_port;

 public:
    explicit Jack_client_control_rules(const unsigned short int &nb_channels);
    virtual ~Jack_client_control_rules();

 private:
    static int capture_and_playback_callback(AUDIO_CALLBACK_NB_FRAMES_TYPE nb_buffer_frames, void *data);
    static void error_callback(const char *msg);

 public:
    bool start(void *callback_param);
    bool restart();
    bool stop();
    bool get_input_buffers(const unsigned short int &nb_buffer_frames, QList<float*> &out_buffers);
    bool get_output_buffers(const unsigned short int &nb_buffer_frames, QList<float*> &out_buffers);
};
