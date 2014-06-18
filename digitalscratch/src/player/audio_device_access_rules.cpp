/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*--------------------------------------------( audio_device_access_rules.cpp )-*/
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
/*   Behavior class: access sound card device (open, close, list, ...)        */
/*                                                                            */
/*============================================================================*/

#include <QtDebug>

#include "sound_capture_and_playback_process.h"
#include "sound_driver_access_rules.h"
#include "audio_device_access_rules.h"
#include <singleton.h>
#include <application_settings.h>

Audio_device_access_rules::Audio_device_access_rules(unsigned short int in_nb_channels) : Sound_driver_access_rules(in_nb_channels)
{
    qDebug() << "Audio_device_access_rules::Audio_device_access_rules: create object...";

    qDebug() << "Audio_device_access_rules::Audio_device_access_rules: create object done.";

    return;
}

Audio_device_access_rules::~Audio_device_access_rules()
{
    qDebug() << "Audio_device_access_rules::~Audio_device_access_rules: delete object...";

    // Stop capture+playback.
    this->stop();

    qDebug() << "Audio_device_access_rules::~Audio_device_access_rules: delete object done.";

    return;
}

QList<QString> *Audio_device_access_rules::get_device_list()
{
    qDebug() << "Audio_device_access_rules::get_device_list...";

    // Get list of available devices.
    QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);

    // Fill a lis with device names (shoud be unique).
    QList<QString> *device_names = new QList<QString>();
    for(int i = 0; i < devices.size(); ++i)
    {
        device_names->append(devices.at(i).deviceName());
    }

    qDebug() << "Audio_device_access_rules::get_device_list: done.";

    return device_names;
}

bool
Audio_device_access_rules::start(void *in_callback_param)
{
    qDebug() << "Audio_device_access_rules::start...";

    // TODO

    // Everything is OK, keep callback parameters.
    this->callback_param = in_callback_param;
    this->running        = true;

    qDebug() << "Audio_device_access_rules::start: done.";

    return true;
}

bool
Audio_device_access_rules::restart()
{
    qDebug() << "Audio_device_access_rules::restart...";

    if (this->start(this->callback_param) == false)
    {
        this->running = false;
        qWarning() << "Audio_device_access_rules::restart: can not restart audio capture/playback.";
    }
    else
    {
        this->running = true;
    }

    qDebug() << "Audio_device_access_rules::restart done.";

    return true;
}

bool
Audio_device_access_rules::stop()
{
    qDebug() << "Audio_device_access_rules::stop...";

    // Stop the stream.
    // TODO
//    if ((this->running == true) && (jack_client_close(this->stream) != 0))
//    {
//        qWarning() << "Audio_device_access_rules::stop: can not close audio capture/playback.";
//        return false;
//    }
//    else
//    {
//        this->running = false;
//    }

    qDebug() << "Audio_device_access_rules::stop: done.";

    return true;
}

bool
Audio_device_access_rules::get_input_buffers(unsigned short int   in_nb_buffer_frames,
                                             float              **out_buffer_1,
                                             float              **out_buffer_2,
                                             float              **out_buffer_3,
                                             float              **out_buffer_4)
{
    bool result;

    qDebug() << "Audio_device_access_rules::get_input_buffers...";

    // TODO
    if (this->do_capture == true)
    {
        result = true;
    }
    else
    {
        result = false;
    }

    qDebug() << "Audio_device_access_rules::get_input_buffers: done.";

    return result;
}

bool
Audio_device_access_rules::get_output_buffers(unsigned short int   in_nb_buffer_frames,
                                              float              **out_buffer_1,
                                              float              **out_buffer_2,
                                              float              **out_buffer_3,
                                              float              **out_buffer_4)
{
    qDebug() << "Audio_device_access_rules::get_input_buffers...";

    // TODO

    qDebug() << "Audio_device_access_rules::get_input_buffers: done.";

    return true;
}
