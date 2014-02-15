/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*--------------------------------------------( sound_card_access_rules.cpp )-*/
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
/*        Behavior class: access sound card (open, close, list, ...)          */
/*                                                                            */
/*============================================================================*/

#include <QtDebug>

#include "sound_capture_and_playback_process.h"
#include "sound_card_access_rules.h"
#include "jack_access_rules.h"
#include <singleton.h>
#include <application_settings.h>

// Pass-through function.
int capture_and_playback_callback(AUDIO_CALLBACK_NB_FRAMES_TYPE  in_nb_buffer_frames,
                                  void                          *in_data)
{
    qDebug() << "Jack_access_rules::capture_and_playback_callback...";

    // Call process for consuming captured data and preparing playback ones.
    Sound_capture_and_playback_process *capture_and_playback = static_cast<Sound_capture_and_playback_process*>(in_data);

    if (capture_and_playback->run((unsigned short int)in_nb_buffer_frames) == false)
    {
        qWarning() << "capture_and_playback_callback: can not run capture and playback process";
    }

    qDebug() << "Jack_access_rules::capture_and_playback_callback done.";

    return 0;
}

Jack_access_rules::Jack_access_rules(unsigned short int in_nb_channels) : Sound_card_access_rules(in_nb_channels)
{
    qDebug() << "Jack_access_rules::Jack_access_rules: create object...";

    qDebug() << "Jack_access_rules::Jack_access_rules: create object done.";

    return;
}

Jack_access_rules::~Jack_access_rules()
{
    qDebug() << "Jack_access_rules::~Jack_access_rules: delete object...";

    // Stop capture+playback.
    this->stop();

    qDebug() << "Jack_access_rules::~Jack_access_rules: delete object done.";

    return;
}

bool
Jack_access_rules::start(void *in_callback_param)
{
    qDebug() << "Jack_access_rules::start...";

    const char     **ports       = NULL;
    const char      *client_name = CLIENT_NAME;
    const char      *server_name = NULL;
    jack_options_t   options     = JackNullOption;
    jack_status_t    status;
    const char      *input_port_names[4]  = { INPUT_PORT_1, INPUT_PORT_2, INPUT_PORT_3, INPUT_PORT_4 };
    const char      *output_port_names[4] = { OUTPUT_PORT_1, OUTPUT_PORT_2, OUTPUT_PORT_3, OUTPUT_PORT_4 };

    // Init.
    this->running = false;

    // Open a client connection to the JACK server.
    this->stream = jack_client_open(client_name, options, &status, server_name);
    if (this->stream == NULL)
    {
        qWarning() << "Jack_access_rules::start: jack_client_open() failed.";
        if (status & JackServerFailed)
        {
            qWarning() << "Jack_access_rules::start: Unable to connect to JACK server.";
        }
        emit error_msg(QString("Can not connect to JACK server, please configure/start Jack server properly."));
        return false;
    }
    if (status & JackServerStarted)
    {
        qDebug() << "Jack_access_rules::start: JACK server started.";
    }
    if (status & JackNameNotUnique)
    {
        client_name = jack_get_client_name(this->stream);
        qDebug() << "Jack_access_rules::start: unique name " << client_name << " assigned";
    }

    // Tell the JACK server to call "in_callback" whenever there is work to be done.
    jack_set_process_callback(this->stream, &capture_and_playback_callback, in_callback_param);

    // Display the current sample rate.
    Application_settings *settings = &Singleton<Application_settings>::get_instance();
    qDebug() << "Jack_access_rules::start: engine sample rate = " << jack_get_sample_rate(this->stream);
    if (jack_get_sample_rate(this->stream) != settings->get_sample_rate())
    {
        qWarning() << "Jack_access_rules::start: DigitalScratch is configured to support sample rate of " << qPrintable(settings->get_sample_rate());
        emit error_msg(QString("DigitalScratch is configured to support a sample rate of " + QString::number(settings->get_sample_rate()) + "Hz, you can change it in the settings dialog."));
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
            qWarning() << "Jack_access_rules::start: no more JACK input ports available";
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
            qWarning() << "Jack_access_rules::start: no more JACK output ports available";
            emit error_msg(QString("No more Jack output ports available, please configure/start Jack server properly."));
            return false;
        }
    }

    // Tell the JACK server that we are ready to roll. Our process() callback
    // will start running now.
    if (jack_activate(this->stream))
    {
        qWarning() << "Jack_access_rules::start: cannot activate client";
        emit error_msg(QString("Can not activate Jack client, please configure/start Jack server properly."));
        return false;
    }

    // Connect the ports.  You can't do this before the client is
    // activated, because we can't make connections to clients
    // that aren't running.  Note the confusing (but necessary)
    // orientation of the driver backend ports: playback ports are
    // "input" to the backend, and capture ports are "output" from
    // it.
    if (settings->get_auto_jack_connections() == true)
    {
        ports = jack_get_ports(this->stream, NULL, NULL, JackPortIsPhysical|JackPortIsOutput);
        if (ports == NULL)
        {
            qWarning() << "Jack_access_rules::start: no physical capture ports";
            emit error_msg(QString("No Jack physical capture ports available, please configure/start Jack server properly."));
            return false;
        }
        for (int i = 0; i < this->nb_channels; i++)
        {
            if (jack_connect(this->stream, ports[i], jack_port_name(this->input_port[i])))
            {
                qWarning() << "Jack_access_rules::start: cannot connect input ports";
                emit error_msg(QString("Can not connect Jack input ports, please configure/start Jack server properly."));
                return false;
            }
        }
        jack_free(ports);

        ports = jack_get_ports(this->stream, NULL, NULL, JackPortIsPhysical|JackPortIsInput);
        if (ports == NULL)
        {
            qWarning() << "Jack_access_rules::start: no physical playback ports";
            emit error_msg(QString("No Jack physical playback ports available, please configure/start Jack server properly."));
            return false;
        }
        for (int i = 0; i < this->nb_channels; i++)
        {
            if (jack_connect(this->stream, jack_port_name (output_port[i]), ports[i]))
            {
                qWarning() << "Jack_access_rules::start: cannot connect output ports";
                emit error_msg(QString("Can not connect Jack output ports, please configure/start Jack server properly."));
                return false;
            }
        }
        jack_free(ports);
    }

    qDebug() << "Jack_access_rules::start: done.";

    // Everything is OK, keep callback parameters.
    this->callback_param = in_callback_param;
    this->running        = true;

    return true;
}

bool
Jack_access_rules::restart()
{
    qDebug() << "Jack_access_rules::restart...";

    if (this->start(this->callback_param) == false)
    {
        this->running = false;
        qWarning() << "Jack_access_rules::restart: can not restart jack client.";
    }
    else
    {
        this->running = true;
    }

    qDebug() << "Jack_access_rules::restart done.";

    return true;
}

bool
Jack_access_rules::stop()
{
    qDebug() << "Jack_access_rules::stop...";

    // Stop the stream.
    if ((this->running == true) && (jack_client_close(this->stream) != 0))
    {
        qWarning() << "Jack_access_rules::stop: can not close Jack client.";
        return false;
    }
    else
    {
        this->running = false;
    }

    qDebug() << "Jack_access_rules::stop: done.";

    return true;
}

bool
Jack_access_rules::get_input_buffers(unsigned short int   in_nb_buffer_frames,
                                     float              **out_buffer_1,
                                     float              **out_buffer_2,
                                     float              **out_buffer_3,
                                     float              **out_buffer_4)
{
    qDebug() << "Jack_access_rules::get_input_buffers...";

    // Get buffers from jack ports.
    *out_buffer_1 = (float *)jack_port_get_buffer(this->input_port[0], in_nb_buffer_frames);
    *out_buffer_2 = (float *)jack_port_get_buffer(this->input_port[1], in_nb_buffer_frames);
    if (this->nb_channels >= 4)
    {
        *out_buffer_3 = (float *)jack_port_get_buffer(this->input_port[2], in_nb_buffer_frames);
        *out_buffer_4 = (float *)jack_port_get_buffer(this->input_port[3], in_nb_buffer_frames);
    }

    qDebug() << "Jack_access_rules::get_input_buffers: done.";

    return true;
}

bool
Jack_access_rules::get_output_buffers(unsigned short int   in_nb_buffer_frames,
                                      float              **out_buffer_1,
                                      float              **out_buffer_2,
                                      float              **out_buffer_3,
                                      float              **out_buffer_4)
{
    qDebug() << "Jack_access_rules::get_input_buffers...";

    // Get buffers from jack ports.
    *out_buffer_1 = (float *)jack_port_get_buffer(this->output_port[0], in_nb_buffer_frames);
    *out_buffer_2 = (float *)jack_port_get_buffer(this->output_port[1], in_nb_buffer_frames);
    if (this->nb_channels >= 4)
    {
        *out_buffer_3 = (float *)jack_port_get_buffer(this->output_port[2], in_nb_buffer_frames);
        *out_buffer_4 = (float *)jack_port_get_buffer(this->output_port[3], in_nb_buffer_frames);
    }

    qDebug() << "Jack_access_rules::get_input_buffers: done.";

    return true;
}
