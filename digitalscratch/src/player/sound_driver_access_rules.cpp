/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*--------------------------------------------( Sound_driver_access_rules.cpp )-*/
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
/*        Behavior class: access sound card (open, close, list, ...)          */
/*                                                                            */
/*============================================================================*/

#include <QtDebug>

#include "sound_driver_access_rules.h"
#include "singleton.h"
#include "application_logging.h"

#ifdef ENABLE_TEST_MODE
#include "audio_track.h"
#include "audio_file_decoding_process.h"
#endif

Sound_driver_access_rules::Sound_driver_access_rules(const unsigned short int &nb_channels)
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
#endif

    return;
}

Sound_driver_access_rules::~Sound_driver_access_rules()
{
    return;
}

bool
Sound_driver_access_rules::is_running()
{
    return this->running;
}

void
Sound_driver_access_rules::set_capture(bool in_do_capture)
{
    this->do_capture = in_do_capture;
}

#ifdef ENABLE_TEST_MODE
bool
Sound_driver_access_rules::use_timecode_from_file(const QString &path)
{
    bool result = true;

    // Decode timecode file.    
    this->timecode = QSharedPointer<Audio_track>(new Audio_track(15, 44100));
    Audio_file_decoding_process decoder(timecode, false);
    QFileInfo file_info = QFileInfo(path);
    result = decoder.run(file_info.absoluteFilePath());

    if (result == true)
    {
        // Now use fake timecode as captured input buffers.
        this->using_fake_timecode = true;
        this->timecode_current_sample = 0;
    }

    return result;
}

bool
Sound_driver_access_rules::fill_input_buf(unsigned short int in_nb_buffer_frames, QList<float*> &io_buffers)
{
    if (this->using_fake_timecode == true)
    {
        // Overwrite buffer with pre-recorded timecode buffer (circular buffer).
        float *buffer = nullptr;
        for (unsigned short int i = 0; i < io_buffers.size(); i++)
        {
            // Reset input buffer.
            buffer = io_buffers[i];
            std::fill(buffer, buffer + in_nb_buffer_frames, 0);

            // Fill it with prerecorded timecode.
            unsigned int tcode_index  = timecode_current_sample + (i % 2); // i%2 = 0 or 1.
            unsigned int buffer_index = 0;
            while ((tcode_index < this->timecode->get_end_of_samples()) &&
                   (buffer_index < in_nb_buffer_frames))
            {
                buffer[buffer_index] = this->timecode->get_samples()[tcode_index];
                buffer_index++;
                tcode_index += 2;
            }
        }

        // Move the index to the timecode data buffer for the next time.
        timecode_current_sample += in_nb_buffer_frames * 2;
        if (timecode_current_sample > this->timecode->get_end_of_samples())
        {
            timecode_current_sample = 0;
        }
    }

    return true;
}
#endif
