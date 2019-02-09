/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*--------------------------------------------( sound_card_access_rules.cpp )-*/
/*                                                                            */
/*  Copyright (C) 2003-2017                                                   */
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

#include <QtDebug>
#include <cmath>

#include "player/control_and_playback_process.h"
#include "audiodev/audio_io_control_rules.h"
#include "audiodev/jack_client_control_rules.h"
#include "app/application_settings.h"
#include "app/application_logging.h"
#include "singleton.h"

Jack_client_control_rules::Jack_client_control_rules(const unsigned short int &nb_channels) : Audio_IO_control_rules(nb_channels)
{
    this->stream = nullptr;
    return;
}

Jack_client_control_rules::~Jack_client_control_rules()
{
    // Stop capture+playback.
    this->stop();

    return;
}

int
Jack_client_control_rules::capture_and_playback_callback(AUDIO_CALLBACK_NB_FRAMES_TYPE  nb_buffer_frames,
                                                 void                          *data)
{
    // Call process for consuming captured data and preparing playback ones.
    Control_and_playback_process *control_and_playback = static_cast<Control_and_playback_process*>(data);

    if (control_and_playback->run((unsigned short int)nb_buffer_frames) == false)
    {
        qCWarning(DS_SOUNDCARD) << "can not run control and playback process";
    }

    return 0;
}

void
Jack_client_control_rules::error_callback(const char *msg)
{
    qCWarning(DS_SOUNDCARD) << "jack error: " << msg;
}

bool
Jack_client_control_rules::start(void *callback_param)
{
    if (this->running == false)
    {
        jack_status_t status;

        // Init.
        this->running = false;

        // Open a client connection to the JACK server.
        this->stream = jack_client_open(CLIENT_NAME, JackNullOption, &status);
        if (this->stream == nullptr)
        {
            qCWarning(DS_SOUNDCARD) << "jack_client_open() failed.";
            if (status & JackServerFailed)
            {
                qCWarning(DS_SOUNDCARD) << "unable to connect to JACK server.";
            }
            emit error_msg(QString("Can not connect to JACK server, please configure/start Jack server properly."));
            return false;
        }

        // Tell the JACK server to call "in_callback" whenever there is work to be done.
        jack_set_process_callback(this->stream, &capture_and_playback_callback, callback_param);
        jack_set_error_function(&error_callback);

        // Display the current sample rate.
        Application_settings *settings = &Singleton<Application_settings>::get_instance();
        if (jack_get_sample_rate(this->stream) != settings->get_sample_rate())
        {
            qCWarning(DS_SOUNDCARD) << "DigitalScratch is configured to support sample rate of " << qPrintable(QString::number(settings->get_sample_rate()));
            emit error_msg(QString("DigitalScratch is configured to support a sample rate of " + QString::number(settings->get_sample_rate()) + "Hz, you can change it in the settings dialog."));
            return false;
        }

        // Create input ports.
        if (this->do_capture == true)
        {
            for (int i = 0; i < this->nb_channels; i++)
            {
                this->input_port << jack_port_register(this->stream,
                                                       QString(QString("turntable_") + QString::number(trunc((i/2) + 0.5)) // ex: turntable_1_out_right
                                                               + QString("_out_") + QString((i%2) == 0 ? "left" : "right")).toStdString().c_str(),
                                                       JACK_DEFAULT_AUDIO_TYPE, // =  "32 bit float mono audio"
                                                       JackPortIsInput,
                                                       0);
                if (this->input_port[i] == nullptr)
                {
                    qCWarning(DS_SOUNDCARD) << "no more JACK input ports available";
                    emit error_msg(QString("No more Jack input ports available, please configure/start Jack server properly."));
                    return false;
                }
            }
        }

        // Create output ports.
        for (int i = 0; i < this->nb_channels; i++)
        {
            this->output_port << jack_port_register(this->stream,
                                                    QString(QString("mixer_") + QString::number(trunc((i/2) + 0.5)) // ex: mixer_1_in_right
                                                            + QString("_in_") + QString((i%2) == 0 ? "left" : "right")).toStdString().c_str(),
                                                    JACK_DEFAULT_AUDIO_TYPE,
                                                    JackPortIsOutput,
                                                    0);
            if (this->output_port[i] == nullptr)
            {
                qCWarning(DS_SOUNDCARD) << "no more JACK output ports available";
                emit error_msg(QString("No more Jack output ports available, please configure/start Jack server properly."));
                return false;
            }
        }

        // Tell the JACK server that we are ready to roll. Our process() callback
        // will start running now.
        if (jack_activate(this->stream))
        {
            qCWarning(DS_SOUNDCARD) << "cannot activate client";
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
            const char **ports = nullptr;

            ports = jack_get_ports(this->stream, nullptr, nullptr, JackPortIsPhysical|JackPortIsOutput);
            if (ports == nullptr)
            {
                qCWarning(DS_SOUNDCARD) << "no physical capture ports";
                emit error_msg(QString("No Jack physical capture ports available, please configure/start Jack server properly."));
                return false;
            }
            for (int i = 0; i < this->nb_channels; i++)
            {
                if (ports[i] == nullptr)
                {
                    qCWarning(DS_SOUNDCARD) << "no JACK input port available to connect with " << jack_port_name(this->input_port[i]);
                    emit error_msg(QString("Can not connect Jack input ports, \
                                            no JACK input port available to connect with ").append(jack_port_name(this->input_port[i])));
                    return false;
                }
                else if (jack_connect(this->stream, ports[i], jack_port_name(this->input_port[i])))
                {
                    qCWarning(DS_SOUNDCARD) << "cannot connect input ports";
                    emit error_msg(QString("Can not connect Jack input ports, please configure/start Jack server properly."));
                    return false;
                }
            }
            jack_free(ports);

            if (this->do_capture == true)
            {
                ports = jack_get_ports(this->stream, nullptr, nullptr, JackPortIsPhysical|JackPortIsInput);
                if (ports == nullptr)
                {
                    qCWarning(DS_SOUNDCARD) << "no physical playback ports";
                    emit error_msg(QString("No Jack physical playback ports available, please configure/start Jack server properly."));
                    return false;
                }
                for (int i = 0; i < this->nb_channels; i++)
                {
                    if (ports[i] == nullptr)
                    {
                        qCWarning(DS_SOUNDCARD) << "no JACK output port available to connect with " << jack_port_name(this->output_port[i]);
                        emit error_msg(QString("Can not connect Jack output ports, \
                                                no JACK output port available to connect with ").append(jack_port_name(this->output_port[i])));
                        return false;
                    }
                    else if (jack_connect(this->stream, jack_port_name (output_port[i]), ports[i]))
                    {
                        qCWarning(DS_SOUNDCARD) << "cannot connect output ports";
                        emit error_msg(QString("Can not connect Jack output ports, please configure/start Jack server properly."));
                        return false;
                    }
                }
                jack_free(ports);
            }
        }

        // Everything is OK, keep callback parameters.
        this->callback_param = callback_param;
        this->running        = true;

        return true;
    }
    else
    {
        return false;
    }
}

bool
Jack_client_control_rules::restart()
{
    if (this->start(this->callback_param) == false)
    {
        this->running = false;
        qCWarning(DS_SOUNDCARD) << "can not restart jack client.";
    }
    else
    {
        this->running = true;
    }

    return true;
}

bool
Jack_client_control_rules::stop()
{
    // Stop the stream.
    if ((this->running == true) && (jack_client_close(this->stream) != 0))
    {
        qCWarning(DS_SOUNDCARD) << "can not close Jack client.";
        return false;
    }

    this->running = false;
    return true;
}

bool
Jack_client_control_rules::get_input_buffers(const unsigned short int &nb_buffer_frames, QList<float*> &out_buffers)
{
    bool result;

    if (this->do_capture == true)
    {
        // Get buffers from jack ports.
        for (unsigned short int i = 0; i < this->nb_channels; i++)
        {
            out_buffers << (float *)jack_port_get_buffer(this->input_port[i], nb_buffer_frames);
        }

#ifdef ENABLE_TEST_MODE
        // Fill buffer with pre-recorded timecode buffer (circular buffer).
        result = this->fill_input_buf(nb_buffer_frames, out_buffers);
#else
        result = true;
#endif
    }
    else
    {
        result = false;
    }

    return result;
}

bool
Jack_client_control_rules::get_output_buffers(const unsigned short int &nb_buffer_frames, QList<float *> &out_buffers)
{
    // Get buffers from jack ports.
    for (unsigned short int i = 0; i < this->nb_channels; i++)
    {
        out_buffers <<  (float *)jack_port_get_buffer(this->output_port[i], nb_buffer_frames);
    }

    return true;
}
