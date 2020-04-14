/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*-------------------------------------------( sound_card_control_rules.cpp )-*/
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
/*   Behavior class: access sound card device (open, close, list, ...)        */
/*                                                                            */
/*============================================================================*/

#include <QtDebug>

#include "player/control_and_playback_process.h"
#include "audiodev/audio_io_control_rules.h"
#include "audiodev/sound_card_control_rules.h"
#include "app/application_settings.h"
#include "app/application_logging.h"
#include "singleton.h"

QList<QString> Sound_card_control_rules::get_device_list()
{
    // Get list of available devices.
    QList<QString> device_names;
    QList<QAudioDeviceInfo> input_devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    foreach(const QAudioDeviceInfo &device, input_devices)
    {
       #if 1
        cout << "Input Device name: " << qPrintable(device.deviceName()) << endl;;
        cout << "  SupportedChannelCount: "; foreach (auto item, device.supportedChannelCounts()) cout << qPrintable(QString::number(item)) << "/"; cout << endl;
        cout << "  SupportedSampleRates: "; foreach (auto item, device.supportedSampleRates()) cout << qPrintable(QString::number(item)) << "/"; cout << endl;
        cout << "  SupportedSampleSizes: "; foreach (auto item, device.supportedSampleSizes()) cout << qPrintable(QString::number(item)) << "/"; cout << endl;
        cout << "  SupportedCodecs: "; foreach (auto item, device.supportedCodecs()) cout << qPrintable(item) << "/"; cout << endl;
       #endif
    }

    QList<QAudioDeviceInfo> output_devices = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    foreach(const QAudioDeviceInfo &device, output_devices)
    {
       #if 1
        cout << "Output Device name: " << qPrintable(device.deviceName()) << endl;;
        cout << "  SupportedChannelCount: "; foreach (auto item, device.supportedChannelCounts()) cout << qPrintable(QString::number(item)) << "/"; cout << endl;
        cout << "  SupportedSampleRates: "; foreach (auto item, device.supportedSampleRates()) cout << qPrintable(QString::number(item)) << "/"; cout << endl;
        cout << "  SupportedSampleSizes: "; foreach (auto item, device.supportedSampleSizes()) cout << qPrintable(QString::number(item)) << "/"; cout << endl;
        cout << "  SupportedCodecs: "; foreach (auto item, device.supportedCodecs()) cout << qPrintable(item) << "/"; cout << endl;
       #endif
    }

//    foreach(const QAudioDeviceInfo &in_dev, input_devices)
//    {
//        // Look if this input device is also an output device with at least the same numbers of output channels.
//        if (output_devices.contains(in_dev) == true)
//        {
//            // Get the corresponding output device.


//            // Sound device supported.
//            device_names.append(device.deviceName());
//        }
//    }

    return device_names;
}

Sound_card_control_rules::Sound_card_control_rules(const unsigned short int &nb_channels) : Audio_IO_control_rules(nb_channels)
{
    return;
}

Sound_card_control_rules::~Sound_card_control_rules()
{
    // Stop capture+playback.
    this->stop();

    return;
}

bool
Sound_card_control_rules::start(void *in_callback_param)
{
    // TODO

    // Everything is OK, keep callback parameters.
    this->callback_param = in_callback_param;
    this->running        = true;

    return true;
}

bool
Sound_card_control_rules::restart()
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
Sound_card_control_rules::stop()
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
Sound_card_control_rules::get_input_buffers(const unsigned short int &nb_buffer_frames, QList<float *> &out_buffers) const
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
Sound_card_control_rules::get_output_buffers(const unsigned short int &nb_buffer_frames, QList<float *> &out_buffers) const
{
    // TODO
    Q_UNUSED(nb_buffer_frames);
    Q_UNUSED(out_buffers);

    return true;
}
