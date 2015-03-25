/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*--------------------------------------------( audio_device_access_rules.cpp )-*/
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
/*   Behavior class: access sound card device (open, close, list, ...)        */
/*                                                                            */
/*============================================================================*/

#include <QtDebug>

#include "player/control_and_playback_process.h"
#include "audiodev/sound_driver_access_rules.h"
#include "audiodev/audio_device_access_rules.h"
#include "app/application_settings.h"
#include "app/application_logging.h"
#include "singleton.h"

QList<QString> Audio_device_access_rules::get_device_list()
{
    // Get list of available devices.
    QList<QString> device_names;
    foreach(const QAudioDeviceInfo &device, QAudioDeviceInfo::availableDevices(QAudio::AudioInput))
    {
       #if 0
        cout << "Device name: " << qPrintable(device.deviceName()) << endl;;
        cout << "  SupportedChannelCount: "; foreach (auto item, device.supportedChannelCounts()) cout << qPrintable(QString::number(item)) << "/"; cout << endl;
        cout << "  SupportedSampleRates: "; foreach (auto item, device.supportedSampleRates()) cout << qPrintable(QString::number(item)) << "/"; cout << endl;
        cout << "  SupportedSampleSizes: "; foreach (auto item, device.supportedSampleSizes()) cout << qPrintable(QString::number(item)) << "/"; cout << endl;
        cout << "  SupportedCodecs: "; foreach (auto item, device.supportedCodecs()) cout << qPrintable(item) << "/"; cout << endl;
       #endif
        device_names.append(device.deviceName());
    }

    return device_names;
}

Audio_device_access_rules::Audio_device_access_rules(const unsigned short int &nb_channels) : Sound_driver_access_rules(nb_channels)
{
    return;
}

Audio_device_access_rules::~Audio_device_access_rules()
{
    // Stop capture+playback.
    this->stop();

    return;
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
Audio_device_access_rules::get_input_buffers(const unsigned short int &nb_buffer_frames, QList<float *> &out_buffers) const
{
    bool result;

    // TODO
    Q_UNUSED(nb_buffer_frames);
    Q_UNUSED(out_buffers);

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
Audio_device_access_rules::get_output_buffers(const unsigned short int &nb_buffer_frames, QList<float *> &out_buffers) const
{
    // TODO
    Q_UNUSED(nb_buffer_frames);
    Q_UNUSED(out_buffers);

    return true;
}
