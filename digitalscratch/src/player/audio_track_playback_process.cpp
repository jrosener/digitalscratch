/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*---------------------------------------( audio_track_playback_process.cpp )-*/
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
/*  Behavior class: prepare samples of track to be sent to the sound card.    */
/*                                                                            */
/*============================================================================*/

#include <QtDebug>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <utils.h>
#include <data_persistence.h>
#include <singleton.h>

#include "audio_track_playback_process.h"

#define SPEED_MIN_TO_GO_DOWN 0.2

Audio_track_playback_process::Audio_track_playback_process(const QSharedPointer<Audio_track>         &at,
                                                           const QList<QSharedPointer<Audio_track>>  &at_sampler,
                                                           const QSharedPointer<Playback_parameters> &param)
{
    this->at          = at;
    this->at_samplers = at_sampler;
    this->param       = param;
    this->nb_samplers = at_sampler.count();
    this->src_state   = nullptr;
    this->src_data    = nullptr;

    for (unsigned short int i = 0; i < MAX_NB_CUE_POINTS; i++) this->cue_points << 0;
    this->current_sample             = 0;
    this->stopped                    = true;
    this->remaining_time             = 0;
    this->src_state                  = nullptr;
    this->src_data                   = nullptr;
    for (unsigned short int i = 0; i < this->nb_samplers; i++) this->sampler_current_samples << 0;
    for (unsigned short int i = 0; i < this->nb_samplers; i++) this->sampler_remaining_times << 0;
    for (unsigned short int i = 0; i < this->nb_samplers; i++) this->sampler_current_states  << false;
    this->need_update_remaining_time = 0;

    // Init libsamplerate.
    int error;
    if ((this->src_state = src_new(SRC_LINEAR, 2, &error)) == nullptr)
    {
        qCWarning(DS_PLAYBACK) << "src_new() failed : " << src_strerror(error);
        return;
    }
    this->src_data = new SRC_DATA;

    // Reset internal parameters.
    this->reset();

    return;
}

Audio_track_playback_process::~Audio_track_playback_process()
{
    // Close libsamplerate.
    if (this->src_state != nullptr)
    {
        delete this->src_data;
        src_delete(this->src_state);
    }

    return;
}

bool
Audio_track_playback_process::reset()
{
    this->current_sample = 0;
    this->remaining_time = 0;
    this->stopped        = false;
    for (int i = 0; i < MAX_NB_CUE_POINTS; i++)
    {
        this->read_cue_point(i);
    }

    // Reset libsamplerate.
    if (this->src_state != nullptr)
    {
        src_reset(this->src_state);
    }

    return true;
}

bool
Audio_track_playback_process::stop()
{
    this->stopped = true;

    return true;
}

bool
Audio_track_playback_process::reset_sampler(const unsigned short int &sampler_index)
{
    this->sampler_current_samples[sampler_index] = 0;
    this->sampler_remaining_times[sampler_index] = 0;

    return true;
}

void
Audio_track_playback_process::del_sampler(const unsigned short int &sampler_index)
{
    this->reset_sampler(sampler_index);
    this->set_sampler_state(sampler_index, false);
    emit sampler_remaining_time_changed(0, sampler_index);
    this->at_samplers[sampler_index]->reset();

    return;
}

bool
Audio_track_playback_process::play_empty(QVector<float*> &io_playback_bufs, const unsigned short int &buf_size)
{
    // For each output samples (only for the selected deck), play silence.
    std::fill(io_playback_bufs[0], io_playback_bufs[0] + buf_size, 0);
    std::fill(io_playback_bufs[1], io_playback_bufs[1] + buf_size, 0);

    // Play samplers.
    this->play_sampler(io_playback_bufs, buf_size);

    return true;
}

bool
Audio_track_playback_process::play_audio_track(QVector<float*> &io_playback_bufs, const unsigned short int &buf_size)
{
    // Prevent sample table overflow if going forward.
    if ((this->param->get_speed() >= 0.0) &&
       ((this->current_sample + 1) > (this->at->get_end_of_samples() - buf_size)))
    {
        qCDebug(DS_PLAYBACK) << "audio track sample table overflow";
        this->play_empty(io_playback_bufs, buf_size);

        // Update remaining time to 0.
        this->update_remaining_time();

        return false;
    }

#if 0
    // Fake implementation (just play track), do not use playback parameters.
    short signed int *sample_pointer = &this->at->get_samples()[this->current_sample];
    std::copy(sample_pointer, sample_pointer + (nb_samples * 2), this->src_int_input_data);
    src_short_to_float_array(this->src_int_input_data, this->src_float_input_data, nb_samples * 2);

    float *ptr = this->src_float_input_data;
    for (int i = 0; i < nb_samples; i++)
    {
        io_samples[0][i] = *ptr;
        ptr++;
        io_samples[1][i] = *ptr;
        ptr++;
    }

    this->current_sample += nb_samples*2;
#else
    if (this->play_data_with_playback_parameters(io_playback_bufs, buf_size) == false)
    {
        qCWarning(DS_PLAYBACK) << "can not prepare data using playback parameters";
        this->play_empty(io_playback_bufs, buf_size);
        return false;
    }
#endif

    return true;
}

bool
Audio_track_playback_process::get_sampler_state(const unsigned short int &sampler_index)
{
    return this->sampler_current_states[sampler_index];
}

bool
Audio_track_playback_process::set_sampler_state(const unsigned short int &sampler_index, const bool &state)
{
    this->sampler_current_states[sampler_index] = state;

    // In case of stopping, go back to the beginning of the sample.
    if (state == false)
    {
        this->sampler_current_samples[sampler_index] = 0;
    }

    return true;
}

bool
Audio_track_playback_process::is_sampler_loaded(const unsigned short int &sampler_index)
{
    bool result = false;

    if (this->at_samplers[sampler_index]->get_end_of_samples() == 0)
    {
        result = false;
    }
    else
    {
        result = true;
    }

    return result;
}

bool
Audio_track_playback_process::play_sampler(QVector<float*> &io_playback_bufs, const unsigned short int &buf_size)
{
    short signed int *sample_pointer = nullptr;
    float sample = 0.0;
    int   index  = 0;

    // For each sampler of this particular deck:
    //    - check if it is still possible to play nb_samples, if so, add 1 to nb_playable_sampler.
    //    - add nb_samples from audio track to existing table of samples
    //    - update current sample
    for (int i = 0; i < this->nb_samplers; i++)
    {
        // Check if sampler is not empty.
        if (this->at_samplers[i]->get_end_of_samples() > 0)
        {
            // Check if sampler is stopped.
            if (this->get_sampler_state(i) == true)
            {
                // Prevent sample table overflow.
                if ((this->sampler_current_samples[i] + 1) <= (this->at_samplers[i]->get_end_of_samples() - (buf_size * 2)))
                {
                    sample_pointer = &((this->at_samplers[i]->get_samples())[this->sampler_current_samples[i]]);
                    for (int j = 0; j < buf_size; j++)
                    {
                        sample                  = (float)*sample_pointer / (float)SHRT_MAX;
                        io_playback_bufs[index][j]   = io_playback_bufs[0][j] + sample - (io_playback_bufs[index][j] * sample);
                        sample                  = (float)*(sample_pointer++) / (float)SHRT_MAX;
                        io_playback_bufs[index+1][j] = io_playback_bufs[1][j] + sample - (io_playback_bufs[index+1][j] * sample);
                        sample_pointer++;
                    }
                    this->sampler_current_samples[i] += buf_size * 2;
                }
                else
                {
                    // Stop playback of this sample.
                    this->set_sampler_state(i, false);
                    emit sampler_state_changed(i, false);
                }
            }
        }
    }

    return true;
}

bool
Audio_track_playback_process::update_remaining_time()
{
    // Check if we have to update remaining time.
    if (this->need_update_remaining_time > NB_CYCLE_WITHOUT_UPDATE_REMAINING_TIME)
    {
        // Yes it's time to do it.
        this->need_update_remaining_time = 0;

        // Prevent remaining time overflow.
        if (this->current_sample > this->at->get_end_of_samples())
        {
            this->remaining_time = 0;
        }
        else
        {
            // Calculate remaining time in msec.
            this->remaining_time = (unsigned int)(1000.0 * ((float)(this->at->get_end_of_samples() - this->current_sample) + 1.0)
                                                 / (2.0 * (float)this->at->get_sample_rate()));
        }

        // Send remaining time.
        emit remaining_time_changed(this->remaining_time);
    }
    else
    {
        // No, let's see next time.
        this->need_update_remaining_time++;
    }

    return true;
}

bool
Audio_track_playback_process::update_sampler_remaining_time(const unsigned short int &sampler_index)
{
    // Calculate remaining time in msec.
    this->sampler_remaining_times[sampler_index] = (unsigned int)(1000.0 * ((float)(this->at_samplers[sampler_index]->get_end_of_samples()
                                                                                    - this->sampler_current_samples[sampler_index]) + 1.0)
                                                                                     / (2.0 * (float)this->at_samplers[sampler_index]->get_sample_rate()));

    if (this->sampler_remaining_times[sampler_index] > 0)
        this->sampler_remaining_times[sampler_index] -= 1;

    // Send remaining time.
    emit sampler_remaining_time_changed(this->sampler_remaining_times[sampler_index], sampler_index);

    return true;
}

bool
Audio_track_playback_process::run(float io_playback_buf_1[], float io_playback_buf_2[], const unsigned short int &buf_size)
{
    QVector<float*> playback_bufs = { io_playback_buf_1, io_playback_buf_2 };

    // Update samplers remaining time
// FIXME: refactor need_update_remaining_time into need_update_sampler_remaining_time
//    if (this->need_update_remaining_time > NB_CYCLE_WITHOUT_UPDATE_REMAINING_TIME)
//    {
//        for (unsigned short int i = 0; i < this->nb_samplers; i++)
//        {
//            if (this->is_sampler_loaded(i) == true)
//            {
//                this->update_sampler_remaining_time(i);
//            }
//        }
//        this->need_update_remaining_time = 0;
//    }
//    else
//    {
//        this->need_update_remaining_time++;
//    }

    // Track is not loaded, play empty sound.
    if ((this->at->get_end_of_samples() == 0) || (this->stopped == true))
    {
        this->play_empty(playback_bufs, buf_size);
    }
    else
    {
        // If track is loaded, play it.
        if (this->play_audio_track(playback_bufs, buf_size) == false)
        {
            qCDebug(DS_PLAYBACK) << "waiting for file decoding process";
        }
        else
        {
            // Update remaining time.
            this->update_remaining_time();
        }
    }

    return true;
}

bool
Audio_track_playback_process::play_data_with_playback_parameters(QVector<float*> &io_playback_bufs, const unsigned short int &buf_size)
{
    float speed = this->param->get_speed();

    // If speed is null, play empty sound.
    if (speed == 0.0)
    {
        this->play_empty(io_playback_bufs, buf_size);
        return true;
    }

    // Determine the approximate number of input data we need to make time stretching.
    unsigned short int nb_input_data = (unsigned short int)((float)buf_size * fabs(speed));
    if (fabs(speed) >= 1.0)
    {
        // Add a little bit of samples if speed is more than 1.0 (quality of stretched sound is better).
        nb_input_data += SOUND_STRETCH_POND_MIN;
    }
    else
    {
        // Add a lot of samples if speed is less than 1.0 (quality of stretched sound is better).
        nb_input_data += (SOUND_STRETCH_POND_MIN - SOUND_STRETCH_POND_MAX) * fabs(speed) + SOUND_STRETCH_POND_MAX;
    }

    // Do we have enough samples to play ?
    unsigned int remaining_samples = 0;
    if (speed > 0.0)
    {
        remaining_samples = (this->at->get_end_of_samples() - this->current_sample) / 2.0;
    }
    else
    {
        remaining_samples = this->current_sample / 2.0;
    }
    if (nb_input_data > remaining_samples)
    {
#ifdef ENABLE_TEST_MODE
        if (speed > 0.0)
        {
            this->current_sample = 0.0;
        }
        else
        {
            this->current_sample = this->at->get_end_of_samples();
        }
#else
        nb_input_data = remaining_samples;
#endif
    }
    if (nb_input_data == 0) // No data to play.
    {
        this->play_empty(io_playback_bufs, buf_size);
        return true;
    }
    if ((nb_input_data * 2) > SOUND_STRETCH_MAX_BUFFER)
    {
        this->play_empty(io_playback_bufs, buf_size);
        qCWarning(DS_PLAYBACK) << "too many data to stretch";
        return false;
    }

    // Prepare samples to play.
    short signed int *start_sample = &this->at->get_samples()[this->current_sample];
    std::fill(this->src_int_input_data, this->src_int_input_data + (nb_input_data * 2), 0);
    if (speed > 0.0)
    {
        std::copy(start_sample, start_sample + (nb_input_data * 2), this->src_int_input_data);
    }
    else
    {
        std::reverse_copy(start_sample - (nb_input_data * 2), start_sample, this->src_int_input_data);
    }

    // Since libsamplerate only use float, we need to convert set of input data.
    src_short_to_float_array(this->src_int_input_data, this->src_float_input_data, nb_input_data * 2);

    // Do time stretching.
    int err = 0;
    std::fill(this->src_float_output_data, this->src_float_output_data + (buf_size * 2), float(0.0));
    this->src_data->data_in           = this->src_float_input_data;
    this->src_data->data_out          = this->src_float_output_data;
    this->src_data->end_of_input      = 0;
    this->src_data->input_frames      = nb_input_data;
    this->src_data->output_frames     = buf_size;
    this->src_data->src_ratio         = fabs(1.0 / speed);
    if ((err = src_process(this->src_state, this->src_data)) != 0)
    {
        qCWarning(DS_PLAYBACK) << "libsamplerate fails: " << src_strerror(err);
    }
    else
    {
        // Change current pointer on sample to play
        if (speed > 0.0)
        {
            this->current_sample += this->src_data->input_frames_used * 2;
        }
        else
        {
            this->current_sample -= this->src_data->input_frames_used * 2;
        }

        // Change volume.
        this->change_volume(this->src_float_output_data, buf_size * 2);

        // Put result in sound card interleaved output stream.
        float *ptr = this->src_float_output_data;
        for (int i = 0; i < buf_size; i++)
        {
            io_playback_bufs[0][i] = *ptr;
            ptr++;
            io_playback_bufs[1][i] = *ptr;
            ptr++;
        }

        // Play samplers.
        this->play_sampler(io_playback_bufs, buf_size);
    }

    return true;
}

bool
Audio_track_playback_process::change_volume(float io_samples[], const unsigned short int &size)
{
    // Get current volume.
    float volume = this->param->get_volume();

    // Change volume of table.
    if (volume != 1.0)
    {
        // Change samples.
        for (int i = 0; i < size; i++)
        {
            io_samples[i] = io_samples[i] * volume;
        }
    }

    return true;
}

bool
Audio_track_playback_process::jump_to_position(const float &position)
{
    // Calculate position to jump (0.0 < position < 1.0).
    unsigned int new_pos = (unsigned int)((float)position * (float)this->at->get_max_nb_samples());
    if (new_pos % 2 != 0)
    {
        new_pos++;
    }

    // We jump.
    this->current_sample = new_pos;

    return true;
}

float
Audio_track_playback_process::get_cue_point(const unsigned short int &cue_point_number)
{
    return this->sample_index_to_float(this->cue_points[cue_point_number]);
}

bool
Audio_track_playback_process::read_cue_point(const unsigned short int &cue_point_number)
{
    // Get cue point from DB.
    Data_persistence *data_persist = &Singleton<Data_persistence>::get_instance();
    unsigned int position = 0;
    if (this->at->get_hash() != "")
    {
        data_persist->get_cue_point(this->at, cue_point_number, position);
        this->cue_points[cue_point_number] = this->msec_to_sample_index(position);
    }

    return true;
}

bool
Audio_track_playback_process::store_cue_point(const unsigned short int &cue_point_number)
{
    // Store cue point.
    this->cue_points[cue_point_number] = this->current_sample;

    // Store it also to DB.
    Data_persistence *data_persist = &Singleton<Data_persistence>::get_instance();

    return data_persist->store_cue_point(this->at,
                                         cue_point_number,
                                         this->sample_index_to_msec(this->cue_points[cue_point_number]));
}

bool
Audio_track_playback_process::jump_to_cue_point(const unsigned short int &cue_point_number)
{
    // Jump
    this->current_sample = this->cue_points[cue_point_number];

    return true;
}

bool
Audio_track_playback_process::delete_cue_point(const unsigned short int &cue_point_number)
{
    // Delete cue point from playback process list.
    this->cue_points[cue_point_number] = 0;

    // Delete cue point from database.
    Data_persistence *data_persist = &Singleton<Data_persistence>::get_instance();
    return data_persist->delete_cue_point(this->at, cue_point_number);
}

float
Audio_track_playback_process::get_position()
{
    return this->sample_index_to_float(this->current_sample);
}

QString
Audio_track_playback_process::get_cue_point_str(const unsigned short &cue_point_number) const
{
    return Utils::get_str_time_from_sample_index(this->cue_points[cue_point_number],
                                                 this->at->get_sample_rate(),
                                                 true);
}

float
Audio_track_playback_process::sample_index_to_float(const unsigned int &sample_index)
{
    // Convert a sample index to a float position (from 0.0 to 1.0).
    return (float)((float)sample_index / (float)this->at->get_max_nb_samples());
}

unsigned int
Audio_track_playback_process::sample_index_to_msec(const unsigned int &sample_index)
{
    // Convert a sample index to milliseconds.
    return qRound((1000.0 * (float)sample_index) / (2.0 * (float)this->at->get_sample_rate()));
}

unsigned int
Audio_track_playback_process::msec_to_sample_index(const unsigned int &position_msec)
{
    // Convert a position from msec to sample index.
    return qRound(((float)position_msec * 2.0 * (float)this->at->get_sample_rate()) / 1000.0);
}
