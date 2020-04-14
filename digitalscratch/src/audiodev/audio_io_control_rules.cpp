/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*---------------------------------------------( audio_io_control_rules.cpp )-*/
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
/*        Behavior class: access sound card (open, close, list, ...)          */
/*                                                                            */
/*============================================================================*/

#include <QtDebug>

#include "audiodev/audio_io_control_rules.h"
#include "app/application_logging.h"
#include "singleton.h"

#ifdef ENABLE_TEST_MODE
#include "tracks/audio_track.h"
#include "tracks/audio_file_decoding_process.h"
#endif

Audio_IO_control_rules::Audio_IO_control_rules(const unsigned short int &nb_channels)
{
    if (nb_channels < 2)
    {
        qCWarning(DS_SOUNDCARD) << "Number of channels must be at least 2.";
        return;
    }

    this->nb_channels    = nb_channels;
    this->callback_param = nullptr;
    this->do_capture     = true;
    this->running        = false;

#ifdef ENABLE_TEST_MODE
    this->using_fake_timecode = false;
    this->timecode_current_sample = 0;
#endif

    return;
}

Audio_IO_control_rules::~Audio_IO_control_rules()
{
    return;
}

bool
Audio_IO_control_rules::is_running()
{
    return this->running;
}

void
Audio_IO_control_rules::set_capture(const bool &do_capture)
{
    this->do_capture = do_capture;
}

#ifdef ENABLE_TEST_MODE
bool
Audio_IO_control_rules::use_timecode_from_file(const QString &path)
{
    // Decode timecode file.
    this->timecode = QSharedPointer<Audio_track>(new Audio_track(15, 44100));
    Audio_file_decoding_process decoder(timecode, false);
    QFileInfo file_info = QFileInfo(path);
    bool result = decoder.run(file_info.absoluteFilePath());

    if (result == true)
    {
        // Now use fake timecode as captured input buffers.
        this->using_fake_timecode = true;
        this->timecode_current_sample = 0;
    }

    return result;
}

bool
Audio_IO_control_rules::fill_input_buf(const unsigned short int &nb_buffer_frames, QList<float*> &io_buffers)
{
    if (this->using_fake_timecode == true)
    {
        // Overwrite buffer with pre-recorded timecode buffer (circular buffer).
        for (unsigned short int i = 0; i < io_buffers.size(); i++)
        {
            // Reset input buffer.
            float *buffer = io_buffers[i];
            std::fill(buffer, buffer + nb_buffer_frames, 0.0f);

            // Fill it with prerecorded timecode.
            unsigned int tcode_index  = timecode_current_sample + (i % 2); // i%2 = 0 or 1.
            unsigned int buffer_index = 0;
            while ((tcode_index < this->timecode->get_end_of_samples()) &&
                   (buffer_index < nb_buffer_frames))
            {
                buffer[buffer_index] = this->timecode->get_samples()[tcode_index];
                buffer_index++;
                tcode_index += 2;
            }
        }

        // Move the index to the timecode data buffer for the next time.
        timecode_current_sample += nb_buffer_frames * 2;
        if (timecode_current_sample > this->timecode->get_end_of_samples())
        {
            timecode_current_sample = 0;
        }
    }

    return true;
}
#endif
