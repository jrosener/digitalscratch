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
#include "singleton.h"
#include "application_settings.h"
#include "application_logging.h"

Audio_device_access_rules::Audio_device_access_rules(unsigned short int in_nb_channels) : Sound_driver_access_rules(in_nb_channels)
{
    return;
}

Audio_device_access_rules::~Audio_device_access_rules()
{
    // Stop capture+playback.
    this->stop();

    return;
}

QList<QString> Audio_device_access_rules::get_device_list()
{
    // Get list of available devices.
    QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);

    // Fill a list with device names (should be unique).
    QList<QString> device_names;
    for(int i = 0; i < devices.size(); ++i)
    {
        device_names.append(devices.at(i).deviceName());
    }

    return device_names;
}

bool
Audio_device_access_rules::start(void *in_callback_param)
{
    // TODO

    // Everything is OK, keep callback parameters.
    this->callback_param = in_callback_param;
    this->running        = true;

    return true;
}

bool
Audio_device_access_rules::restart()
{
    if (this->start(this->callback_param) == false)
    {
        this->running = false;
        qCWarning(DS_SOUNDCARD) << "can not restart audio capture/playback";
    }
    else
    {
        this->running = true;
    }

    return true;
}

bool
Audio_device_access_rules::stop()
{
    // Stop the stream.
    // TODO
//    if ((this->running == true) && (jack_client_close(this->stream) != 0))
//    {
//        qCWarning(DS_SOUNDCARD) << "can not close audio capture/playback.";
//        return false;
//    }
//    else
//    {
//        this->running = false;
//    }

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

    // TODO
    Q_UNUSED(in_nb_buffer_frames);
    Q_UNUSED(out_buffer_1);
    Q_UNUSED(out_buffer_2);
    Q_UNUSED(out_buffer_3);
    Q_UNUSED(out_buffer_4);

    if (this->do_capture == true)
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}

bool
Audio_device_access_rules::get_output_buffers(unsigned short int   in_nb_buffer_frames,
                                              float              **out_buffer_1,
                                              float              **out_buffer_2,
                                              float              **out_buffer_3,
                                              float              **out_buffer_4)
{
    // TODO
    Q_UNUSED(in_nb_buffer_frames);
    Q_UNUSED(out_buffer_1);
    Q_UNUSED(out_buffer_2);
    Q_UNUSED(out_buffer_3);
    Q_UNUSED(out_buffer_4);

    return true;
}
