/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*--------------------------------------------( sound_card_access_rules.cpp )-*/
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
/*        Behavior class: access sound card (open, close, list, ...)          */
/*                                                                            */
/*============================================================================*/

#include <QtDebug>

#include "sound_card_access_rules.h"

// Global pointer on object instance used for error callback.
Sound_card_access_rules* ptr;

Sound_card_access_rules::Sound_card_access_rules(unsigned short int in_nb_channels)
{
    qDebug() << "Sound_card_access_rules::Sound_card_access_rules: create object...";

    if ((in_nb_channels == 0) || (in_nb_channels > 4))
    {
        qFatal("Sound_card_access_rules::Sound_card_access_rules: digital-scratch can only handle 2 decks maximum.");
        return;
    }

    this->nb_channels = in_nb_channels;
    this->callback = NULL;
    this->callback_param = NULL;
    ptr = this;

    qDebug() << "Sound_card_access_rules::Sound_card_access_rules: create object done.";

    return;
}

Sound_card_access_rules::~Sound_card_access_rules()
{
    qDebug() << "Sound_card_access_rules::~Sound_card_access_rules: delete object...";

    // Stop capture+playback.
    this->stop();

    qDebug() << "Sound_card_access_rules::~Sound_card_access_rules: delete object done.";

    return;
}


bool
Sound_card_access_rules::start(AUDIO_CALLBACK_TYPE  in_callback,
                               void                *in_callback_param)
{
    qDebug() << "Sound_card_access_rules::start...";

    const char     **ports       = NULL;
    const char      *client_name = CLIENT_NAME;
    const char      *server_name = NULL;
    jack_options_t   options     = JackNullOption;
    jack_status_t    status;
    const char      *input_port_names[4]  = { INPUT_PORT_1, INPUT_PORT_2, INPUT_PORT_3, INPUT_PORT_4 };
    const char      *output_port_names[4] = { OUTPUT_PORT_1, OUTPUT_PORT_2, OUTPUT_PORT_3, OUTPUT_PORT_4 };

    // Check parameters.
    if (in_callback == NULL)
    {
        qFatal("Sound_card_access_rules::start: bad parameters.");
        return false;
    }

    // Open a client connection to the JACK server.
    this->stream = jack_client_open(client_name, options, &status, server_name);
    if (this->stream == NULL)
    {
        qWarning() << "Sound_card_access_rules::start: jack_client_open() failed.";
        if (status & JackServerFailed)
        {
            qWarning() << "Sound_card_access_rules::start: Unable to connect to JACK server.";
            emit error_msg(QString("Can not connect to JACK server, please configure/start Jack server properly."));
        }
        return false;
    }
    if (status & JackServerStarted)
    {
        qDebug() << "Sound_card_access_rules::start: JACK server started.";
    }
    if (status & JackNameNotUnique)
    {
        client_name = jack_get_client_name(this->stream);
        qDebug() << "Sound_card_access_rules::start: unique name " << client_name << " assigned";
    }

    // Tell the JACK server to call "in_callback" whenever there is work to be done.
    jack_set_process_callback(this->stream, in_callback, in_callback_param);

    // Display the current sample rate.
    qDebug() << "Sound_card_access_rules::start: engine sample rate = " << jack_get_sample_rate(this->stream);
    if (jack_get_sample_rate(this->stream) != 44100)
    {
        qWarning() << "Sound_card_access_rules::start: Digital-scratch only support sample rate of 44100Hz";
        emit error_msg(QString("Digital-scratch currently only support sample rate of 44100Hz, please configure/start Jack server properly."));
        return false;
    }

    // Create input ports.
    this->input_port = new jack_port_t*[this->nb_channels];
    for (int i = 0; i < this->nb_channels; i++)
    {
        this->input_port[i] = jack_port_register(this->stream,
                                                 input_port_names[i],
                                                 JACK_DEFAULT_AUDIO_TYPE,
                                                 JackPortIsInput,
                                                 0);
        if (this->input_port[i] == NULL)
        {
            qWarning() << "Sound_card_access_rules::start: no more JACK input ports available";
            emit error_msg(QString("No more Jack input ports available, please configure/start Jack server properly."));
            return false;
        }
    }

    // Create output ports.
    this->output_port = new jack_port_t*[this->nb_channels];
    for (int i = 0; i < this->nb_channels; i++)
    {
        this->output_port[i] = jack_port_register(this->stream,
                                                 output_port_names[i],
                                                 JACK_DEFAULT_AUDIO_TYPE,
                                                 JackPortIsOutput,
                                                 0);
        if (this->output_port[i] == NULL)
        {
            qWarning() << "Sound_card_access_rules::start: no more JACK output ports available";
            emit error_msg(QString("No more Jack output ports available, please configure/start Jack server properly."));
            return false;
        }
    }

    // Tell the JACK server that we are ready to roll. Our process() callback
    // will start running now.
    if (jack_activate(this->stream))
    {
        qWarning() << "Sound_card_access_rules::start: cannot activate client";
        emit error_msg(QString("Can not activate Jack client, please configure/start Jack server properly."));
        return false;
    }

    // Connect the ports.  You can't do this before the client is
    // activated, because we can't make connections to clients
    // that aren't running.  Note the confusing (but necessary)
    // orientation of the driver backend ports: playback ports are
    // "input" to the backend, and capture ports are "output" from
    // it.
    ports = jack_get_ports(this->stream, NULL, NULL, JackPortIsPhysical|JackPortIsOutput);
    if (ports == NULL)
    {
        qWarning() << "Sound_card_access_rules::start: no physical capture ports";
        emit error_msg(QString("No Jack physical capture ports available, please configure/start Jack server properly."));
        return false;
    }
    for (int i = 0; i < this->nb_channels; i++)
    {
        if (jack_connect(this->stream, ports[i], jack_port_name(this->input_port[i])))
        {
            qWarning() << "Sound_card_access_rules::start: cannot connect input ports";
            emit error_msg(QString("Can not connect Jack input ports, please configure/start Jack server properly."));
            return false;
        }
    }
    free(ports);

    ports = jack_get_ports(this->stream, NULL, NULL, JackPortIsPhysical|JackPortIsInput);
    if (ports == NULL)
    {
        qWarning() << "Sound_card_access_rules::start: no physical playback ports";
        emit error_msg(QString("No Jack physical playback ports available, please configure/start Jack server properly."));
        return false;
    }
    for (int i = 0; i < this->nb_channels; i++)
    {
        if (jack_connect(this->stream, jack_port_name (output_port[i]), ports[i]))
        {
            qWarning() << "Sound_card_access_rules::start: cannot connect output ports";
            emit error_msg(QString("Can not connect Jack output ports, please configure/start Jack server properly."));
            return false;
        }
    }
    free (ports);

    qDebug() << "Sound_card_access_rules::start: done.";

    // Everything is OK, keep callback parameters.
    this->callback       = in_callback;
    this->callback_param = in_callback_param;

    return true;
}

bool
Sound_card_access_rules::restart()
{
    qDebug() << "Sound_card_access_rules::restart...";

    if (this->callback != NULL)
    {
        if (this->start(this->callback, this->callback_param) == false)
        {
            qWarning() << "Sound_card_access_rules::restart: can not restart jack client.";
        }
    }

    qDebug() << "Sound_card_access_rules::restart done.";

    return true;
}

bool
Sound_card_access_rules::stop()
{
    qDebug() << "Sound_card_access_rules::stop...";

    // Stop the stream.
    if (jack_client_close(this->stream) != 0)
    {
        qWarning() << "Sound_card_access_rules::stop: can not close Jack client.";
        return false;
    }

    qDebug() << "Sound_card_access_rules::stop: done.";

    return true;
}

bool
Sound_card_access_rules::get_input_buffers(unsigned short int   in_nb_buffer_frames,
                                           float              **out_buffer_1,
                                           float              **out_buffer_2,
                                           float              **out_buffer_3,
                                           float              **out_buffer_4)
{
    qDebug() << "Sound_card_access_rules::get_input_buffers...";

    // Get buffers from jack ports.
    *out_buffer_1 = (float *)jack_port_get_buffer(this->input_port[0], in_nb_buffer_frames);
    *out_buffer_2 = (float *)jack_port_get_buffer(this->input_port[1], in_nb_buffer_frames);
    if (this->nb_channels >= 4)
    {
        *out_buffer_3 = (float *)jack_port_get_buffer(this->input_port[2], in_nb_buffer_frames);
        *out_buffer_4 = (float *)jack_port_get_buffer(this->input_port[3], in_nb_buffer_frames);
    }

    qDebug() << "Sound_card_access_rules::get_input_buffers: done.";

    return true;
}

bool
Sound_card_access_rules::get_output_buffers(unsigned short int   in_nb_buffer_frames,
                                            float              **out_buffer_1,
                                            float              **out_buffer_2,
                                            float              **out_buffer_3,
                                            float              **out_buffer_4)
{
    qDebug() << "Sound_card_access_rules::get_input_buffers...";

    // Get buffers from jack ports.
    *out_buffer_1 = (float *)jack_port_get_buffer(this->output_port[0], in_nb_buffer_frames);
    *out_buffer_2 = (float *)jack_port_get_buffer(this->output_port[1], in_nb_buffer_frames);
    if (this->nb_channels >= 4)
    {
        *out_buffer_3 = (float *)jack_port_get_buffer(this->output_port[2], in_nb_buffer_frames);
        *out_buffer_4 = (float *)jack_port_get_buffer(this->output_port[3], in_nb_buffer_frames);
    }

    qDebug() << "Sound_card_access_rules::get_input_buffers: done.";

    return true;
}
