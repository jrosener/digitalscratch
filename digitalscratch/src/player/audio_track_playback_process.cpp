/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*---------------------------------------( audio_track_playback_process.cpp )-*/
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

Audio_track_playback_process::Audio_track_playback_process(Audio_track          *in_ats[],
                                                           Audio_track         **in_at_samplers[],
                                                           Playback_parameters  *in_params[],
                                                           unsigned short int    in_nb_decks,
                                                           unsigned short int    in_nb_samplers)
{
    if (in_ats         == NULL ||
        in_at_samplers == NULL ||
        in_params      == NULL)
    {
        qCWarning(DS_PLAYBACK) << "parameter is null";
        return;
    }
    else
    {
        this->ats         = in_ats;
        this->at_samplers = in_at_samplers;
        this->params      = in_params;
        this->nb_decks    = in_nb_decks;
        this->nb_samplers = in_nb_samplers;
        this->src_state   = NULL;
        this->src_data    = NULL;
    }

    this->cue_points = new unsigned int* [in_nb_decks];
    for (unsigned short int i = 0; i < in_nb_decks; i++)
    {
        this->cue_points[i] = new unsigned int[MAX_NB_CUE_POINTS];
        std::fill(this->cue_points[i], this->cue_points[i] + MAX_NB_CUE_POINTS, 0);

    }
    this->current_samples            = new unsigned int[in_nb_decks];
    this->stopped                    = new bool        [in_nb_decks];
    this->remaining_times            = new unsigned int[in_nb_decks];
    this->src_state                  = new SRC_STATE*  [in_nb_decks];
    this->src_data                   = new SRC_DATA*   [in_nb_decks];
    this->sampler_current_samples    = new unsigned int[in_nb_decks * in_nb_samplers];
    this->sampler_remaining_times    = new unsigned int[in_nb_decks * in_nb_samplers];
    this->sampler_current_states     = new bool        [in_nb_decks * in_nb_samplers];

    std::fill(this->current_samples,         this->current_samples + in_nb_decks, 0);
    std::fill(this->remaining_times,         this->remaining_times + in_nb_decks, 0);
    std::fill(this->sampler_current_samples, this->sampler_current_samples + (in_nb_decks * in_nb_samplers), 0);
    std::fill(this->sampler_remaining_times, this->sampler_remaining_times + (in_nb_decks * in_nb_samplers), 0);
    std::fill(this->sampler_current_states,  this->sampler_current_states  + (in_nb_decks * in_nb_samplers), false);

    this->need_update_remaining_time = 0;

    for (unsigned int i = 0; i < in_nb_decks; i++)
    {
        // Init libsamplerate.
        int error;
        if ((this->src_state[i] = src_new(SRC_LINEAR, 2, &error)) == NULL)
        {
            qCWarning(DS_PLAYBACK) << "src_new() failed : " << src_strerror(error);
            return;
        }
        this->src_data[i] = new SRC_DATA;

        // Reset internal parameters.
        this->reset(i);
    }

    return;
}

Audio_track_playback_process::~Audio_track_playback_process()
{
    for (unsigned short int i = 0; i < this->nb_decks; i++)
    {
        delete [] this->cue_points[i];
    }
    delete [] this->cue_points;
    delete [] this->current_samples;
    delete [] this->stopped;
    delete [] this->remaining_times;
    delete [] this->sampler_current_samples;
    delete [] this->sampler_remaining_times;
    delete [] this->sampler_current_states;

    // Close libsamplerate.
    for (unsigned short int i = 0; i < this->nb_decks; i++)
    {
        if (this->src_state[i] != NULL)
        {
            delete this->src_data[i];
            src_delete(this->src_state[i]);
        }
    }
    delete [] this->src_data;
    delete [] this->src_state;

    return;
}

bool
Audio_track_playback_process::reset(unsigned short int in_deck_index)
{
    this->current_samples[in_deck_index] = 0;
    this->remaining_times[in_deck_index] = 0;
    this->stopped[in_deck_index]         = false;
    for (int i = 0; i < MAX_NB_CUE_POINTS; i++)
    {
        this->read_cue_point(in_deck_index, i);
    }

    // Reset libsamplerate.
    if (this->src_state[in_deck_index] != NULL)
    {
        src_reset(this->src_state[in_deck_index]);
    }

    return true;
}

bool
Audio_track_playback_process::stop(unsigned short int in_deck_index)
{
    this->stopped[in_deck_index] = true;

    return true;
}

bool
Audio_track_playback_process::reset_sampler(unsigned short int in_index,
                                            unsigned short int in_sampler_index)
{
    this->sampler_current_samples[in_sampler_index + (this->nb_samplers * in_index)] = 0;
    this->sampler_remaining_times[in_sampler_index + (this->nb_samplers * in_index)] = 0;

    return true;
}

void
Audio_track_playback_process::del_sampler(unsigned short int in_deck_index,
                                          unsigned short int in_sampler_index)
{
    this->reset_sampler(in_deck_index, in_sampler_index);
    this->set_sampler_state(in_deck_index, in_sampler_index, false);
    emit sampler_remaining_time_changed(0, in_deck_index, in_sampler_index);
    this->at_samplers[in_deck_index][in_sampler_index]->reset();

    return;
}

bool
Audio_track_playback_process::play_empty(unsigned short int   in_deck_index,
                                         unsigned short int   in_nb_samples,
                                         float              **out_samples)
{
    // For each output samples (only for the selected deck), play silence.
    int index = in_deck_index * 2;
    for (int i = 0; i < in_nb_samples; i++)
    {
        out_samples[index][i]   = 0;
        out_samples[index+1][i] = 0;
    }

    // Play samplers.
    this->play_sampler(in_deck_index, in_nb_samples, out_samples);

    return true;
}

bool
Audio_track_playback_process::play_audio_track(unsigned short int   in_deck_index,
                                               unsigned short int   in_nb_samples,
                                               float              **out_samples)
{
    // Prevent sample table overflow if going forward.
    if ((this->params[in_deck_index]->get_speed() >= 0.0) &&
       ((this->current_samples[in_deck_index] + 1) > (this->ats[in_deck_index]->get_end_of_samples() - in_nb_samples)))
    {
        qCWarning(DS_PLAYBACK) << "audio track sample table overflow";
        this->play_empty(in_deck_index, in_nb_samples, out_samples);

        // Update remaining time to 0.
        this->update_remaining_time(in_deck_index);

        return false;
    }

#if 0
    // Fake implementation (just play track), do not use playback parameters.
    short signed int *sample_pointer = &((this->ats[in_deck_index]->get_samples())[this->current_samples[in_deck_index]]);
    std::copy(sample_pointer, sample_pointer + (in_nb_samples * 2), this->src_int_input_data);
    src_short_to_float_array(this->src_int_input_data, this->src_float_input_data, in_nb_samples * 2);

    for (int i = (0 + (in_deck_index * 2));
         i < (in_nb_samples * this->nb_decks * 2);
         i = i + (this->nb_decks * 2))
    {
        out_samples[i]   = &this->src_float_input_data[i];
        out_samples[i+1] = &this->src_float_input_data[i+1];
    }
    this->current_samples[in_deck_index] += in_nb_samples*2;
#else
    if (this->play_data_with_playback_parameters(in_deck_index, in_nb_samples, out_samples) == false)
    {
        qCWarning(DS_PLAYBACK) << "can not prepare data using playback parameters";
        this->play_empty(in_deck_index, in_nb_samples, out_samples);
        return false;
    }

#endif

    return true;
}

bool
Audio_track_playback_process::get_sampler_state(unsigned short int in_deck_index,
                                                unsigned short int in_sampler_index)
{
    return this->sampler_current_states[in_sampler_index + (in_deck_index * this->nb_samplers)];
}

bool
Audio_track_playback_process::set_sampler_state(unsigned short int in_deck_index,
                                                unsigned short int in_sampler_index,
                                                bool               in_state)
{
    this->sampler_current_states[in_sampler_index + (in_deck_index * this->nb_samplers)] = in_state;

    // In case of stopping, go back to the beginning of the sample.
    if (in_state == false)
    {
        this->sampler_current_samples[in_sampler_index + (in_deck_index * this->nb_samplers)] = 0;
    }

    return true;
}

bool
Audio_track_playback_process::is_sampler_loaded(unsigned short int in_deck_index,
                                                unsigned short int in_sampler_index)
{
    bool result = false;

    if (this->at_samplers[in_deck_index][in_sampler_index]->get_end_of_samples() == 0)
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
Audio_track_playback_process::play_sampler(unsigned short int   in_deck_index,
                                           unsigned short int   in_nb_samples,
                                           float              **out_samples)
{
    short signed int *sample_pointer = NULL;
    float sample = 0.0;
    int   index  = 0;

    // For each sampler of this particular deck:
    //    - check if it is still possible to play in_nb_samples, if so, add 1 to nb_playable_sampler.
    //    - add in_nb_samples from audio track to existing table of samples
    //    - update current sample
    for (int i = 0; i < this->nb_samplers; i++)
    {
        // Check if sampler is not empty.
        if (this->at_samplers[in_deck_index][i]->get_end_of_samples() > 0)
        {
            // Check if sampler is stopped.
            if (this->get_sampler_state(in_deck_index, i) == true)
            {
                // Prevent sample table overflow.
                if ((this->sampler_current_samples[i + (in_deck_index * this->nb_samplers)] + 1) <= (this->at_samplers[in_deck_index][i]->get_end_of_samples() - (in_nb_samples * 2)))
                {
                    sample_pointer = &((this->at_samplers[in_deck_index][i]->get_samples())[this->sampler_current_samples[i + (in_deck_index * this->nb_samplers)]]);
                    index = in_deck_index * 2;
                    for (int j = 0; j < in_nb_samples; j++)
                    {
                        sample                  = (float)*sample_pointer / (float)SHRT_MAX;
                        out_samples[index][j]   = out_samples[index][j] + sample - (out_samples[index][j] * sample);
                        sample                  = (float)*(sample_pointer++) / (float)SHRT_MAX;
                        out_samples[index+1][j] = out_samples[index+1][j] + sample - (out_samples[index+1][j] * sample);
                        sample_pointer++;
                    }
                    this->sampler_current_samples[i + (in_deck_index * this->nb_samplers)] += in_nb_samples * 2;
                }
                else
                {
                    // Stop playback of this sample.
                    this->set_sampler_state(in_deck_index, i, false);
                    emit sampler_state_changed(in_deck_index, i, false);
                }
            }
        }
    }

    return true;
}

bool
Audio_track_playback_process::update_remaining_time(unsigned short int in_deck_index)
{
    // Check if we have to update remaining time.
    if (this->need_update_remaining_time > NB_CYCLE_WITHOUT_UPDATE_REMAINING_TIME)
    {
        // Yes it's time to do it.
        this->need_update_remaining_time = 0;

        // Prevent remaining time overflow.
        if (this->current_samples[in_deck_index] > this->ats[in_deck_index]->get_end_of_samples())
        {
            this->remaining_times[in_deck_index] = 0;
        }
        else
        {
            // Calculate remaining time in msec.
            this->remaining_times[in_deck_index] = (unsigned int)(1000.0 * ((float)(this->ats[in_deck_index]->get_end_of_samples() - this->current_samples[in_deck_index]) + 1.0)
                                                              / (2.0 * (float)this->ats[in_deck_index]->get_sample_rate()));
        }

        // Send remaining time.
        emit remaining_time_changed(this->remaining_times[in_deck_index], in_deck_index);
    }
    else
    {
        // No, let's see next time.
        this->need_update_remaining_time++;
    }

    return true;
}

bool
Audio_track_playback_process::update_sampler_remaining_time(unsigned short int in_deck_index,
                                                            unsigned short int in_sampler_index)
{
    // Calculate remaining time in msec.
    this->sampler_remaining_times[in_sampler_index + (in_deck_index * this->nb_samplers)] = (unsigned int)(1000.0 * ((float)(this->at_samplers[in_deck_index][in_sampler_index]->get_end_of_samples()
                                                                                            - this->sampler_current_samples[in_sampler_index + (in_deck_index * this->nb_samplers)]) + 1.0)
                                                                                            / (2.0 * (float)this->at_samplers[in_deck_index][in_sampler_index]->get_sample_rate()));

    if (this->sampler_remaining_times[in_sampler_index + (in_deck_index * this->nb_samplers)] > 0)
        this->sampler_remaining_times[in_sampler_index + (in_deck_index * this->nb_samplers)] -= 1;

    // Send remaining time.
    emit sampler_remaining_time_changed(this->sampler_remaining_times[in_sampler_index + (in_deck_index * this->nb_samplers)], in_deck_index, in_sampler_index);

    return true;
}

bool
Audio_track_playback_process::run(unsigned short int  in_nb_samples,
                                  float              *out_samples_1,
                                  float              *out_samples_2,
                                  float              *out_samples_3,
                                  float              *out_samples_4)
{
    float *samples[4] = { out_samples_1, out_samples_2, out_samples_3, out_samples_4 };

    // Iterate over decks and play track data.
    for (unsigned short int i = 0; i < this->nb_decks; i++)
    {
        // Update samplers remaining time
        if (this->need_update_remaining_time > NB_CYCLE_WITHOUT_UPDATE_REMAINING_TIME)
        {
            for (unsigned short int j = 0; j < this->nb_samplers; j++)
            {
                if (this->is_sampler_loaded(i, j) == true)
                {
                    this->update_sampler_remaining_time(i, j);
                }
            }
            this->need_update_remaining_time = 0;
        }
        else
        {
            this->need_update_remaining_time++;
        }

        // Track is not loaded, play empty sound.
        if ((this->ats[i]->get_end_of_samples() == 0) || (this->stopped[i] == true))
        {
            this->play_empty(i, in_nb_samples, samples);
        }
        else
        {
            // If track is loaded, play it.
            if (this->play_audio_track(i, in_nb_samples, samples) == false)
            {
                qCWarning(DS_PLAYBACK) << "waiting for file decoding process";
            }
            else
            {
                // Update remaining time.
                this->update_remaining_time(i);
            }
        }
    }

    return true;
}

bool
Audio_track_playback_process::play_data_with_playback_parameters(unsigned short int   in_deck_index,
                                                                 unsigned short int   in_nb_samples,
                                                                 float              **out_samples)
{
    float speed = this->params[in_deck_index]->get_speed();

    // If speed is null, play empty sound.
    if (speed == 0.0)
    {
        this->play_empty(in_deck_index, in_nb_samples, out_samples);
        return true;
    }

    // Determine the approximate number of input data we need to make time stretching.
    unsigned short int nb_input_data = (unsigned short int)((float)in_nb_samples * fabs(speed));
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
        remaining_samples = (this->ats[in_deck_index]->get_end_of_samples() - this->current_samples[in_deck_index]) / 2.0;
    }
    else
    {
        remaining_samples = this->current_samples[in_deck_index] / 2.0;
    }
    if (nb_input_data > remaining_samples)
    {
        nb_input_data = remaining_samples;
    }
    if (nb_input_data == 0) // No data to play.
    {
        this->play_empty(in_deck_index, in_nb_samples, out_samples);
        return true;
    }
    if ((nb_input_data * 2) > SOUND_STRETCH_MAX_BUFFER)
    {
        this->play_empty(in_deck_index, in_nb_samples, out_samples);
        qCWarning(DS_PLAYBACK) << "too many data to stretch";
        return false;
    }

    // Prepare samples to play.
    short signed int *start_sample = &this->ats[in_deck_index]->get_samples()[this->current_samples[in_deck_index]];
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
    std::fill(this->src_float_output_data, this->src_float_output_data + (in_nb_samples * 2), float(0.0));
    this->src_data[in_deck_index]->data_in           = this->src_float_input_data;
    this->src_data[in_deck_index]->data_out          = this->src_float_output_data;
    this->src_data[in_deck_index]->end_of_input      = 0;
    this->src_data[in_deck_index]->input_frames      = nb_input_data;
    this->src_data[in_deck_index]->output_frames     = in_nb_samples;
    this->src_data[in_deck_index]->src_ratio         = fabs(1.0 / speed);
    if ((err = src_process(this->src_state[in_deck_index], this->src_data[in_deck_index])) != 0)
    {
        qCWarning(DS_PLAYBACK) << "libsamplerate fails: " << src_strerror(err);
    }

    // Change current pointer on sample to play
    if (speed > 0.0)
    {
        this->current_samples[in_deck_index] += this->src_data[in_deck_index]->input_frames_used * 2;
    }
    else
    {
        this->current_samples[in_deck_index] -= this->src_data[in_deck_index]->input_frames_used * 2;
    }

    // Change volume.
    this->change_volume(in_deck_index, this->src_float_output_data, in_nb_samples * 2);

    // Put result in sound card interleaved output stream.
    float *ptr = this->src_float_output_data;
    int index = in_deck_index * 2;
    for (int i = 0; i < in_nb_samples; i++)
    {
        out_samples[index][i] = *ptr;
        ptr++;
        out_samples[index+1][i] = *ptr;
        ptr++;
    }

    // Play samplers.
    this->play_sampler(in_deck_index, in_nb_samples, out_samples);

    return true;
}

bool
Audio_track_playback_process::change_volume(unsigned short int  in_deck_index,
                                            float              *in_buffer,
                                            unsigned short int  in_size)
{
    // Get current volume.
    float volume = this->params[in_deck_index]->get_volume();

    // Change volume of table.
    if (volume != 1.0)
    {
        // Change samples.
        for (int i = 0; i < in_size; i++)
        {
            in_buffer[i] = in_buffer[i] * volume;
        }
    }

    return true;
}

bool
Audio_track_playback_process::jump_to_position(unsigned short int in_deck_index,
                                               float              in_position)
{
    // Calculate position to jump (0.0 < in_position < 1.0).
    unsigned int new_pos = (unsigned int)((float)in_position * (float)this->ats[in_deck_index]->get_max_nb_samples());
    if (new_pos % 2 != 0)
    {
        new_pos++;
    }

    // We jump.
    this->current_samples[in_deck_index] = new_pos;

    return true;
}

float
Audio_track_playback_process::get_cue_point(unsigned short int in_deck_index, unsigned short int in_cue_point_number)
{
    return this->sample_index_to_float(in_deck_index, this->cue_points[in_deck_index][in_cue_point_number]);
}

bool
Audio_track_playback_process::read_cue_point(unsigned short int in_deck_index, unsigned short int in_cue_point_number)
{
    // Get cue point from DB.
    Data_persistence *data_persist = &Singleton<Data_persistence>::get_instance();
    unsigned int position = 0;
    if (this->ats[in_deck_index]->get_hash() != "")
    {
        data_persist->get_cue_point(this->ats[in_deck_index], in_cue_point_number, position);
        this->cue_points[in_deck_index][in_cue_point_number] = this->msec_to_sample_index(in_deck_index, position);
    }

    return true;
}

bool
Audio_track_playback_process::store_cue_point(unsigned short int in_deck_index, unsigned short int in_cue_point_number)
{
    // Store cue point.
    this->cue_points[in_deck_index][in_cue_point_number] = this->current_samples[in_deck_index];

    // Store it also to DB.
    Data_persistence *data_persist = &Singleton<Data_persistence>::get_instance();

    return data_persist->store_cue_point(this->ats[in_deck_index],
                                         in_cue_point_number,
                                         this->sample_index_to_msec(in_deck_index, this->cue_points[in_deck_index][in_cue_point_number]));
}

bool
Audio_track_playback_process::jump_to_cue_point(unsigned short int in_deck_index, unsigned short int in_cue_point_number)
{
    // Jump
    this->current_samples[in_deck_index] = this->cue_points[in_deck_index][in_cue_point_number];

    return true;
}

bool
Audio_track_playback_process::delete_cue_point(unsigned short int in_deck_index, unsigned short int in_cue_point_number)
{
    // Delete cue point from playback process list.
    this->cue_points[in_deck_index][in_cue_point_number] = 0;

    // Delete cue point from database.
    Data_persistence *data_persist = &Singleton<Data_persistence>::get_instance();
    return data_persist->delete_cue_point(this->ats[in_deck_index], in_cue_point_number);
}

float
Audio_track_playback_process::get_position(unsigned short int in_deck_index)
{
    return this->sample_index_to_float(in_deck_index, this->current_samples[in_deck_index]);
}

QString
Audio_track_playback_process::get_cue_point_str(unsigned short int in_deck_index, unsigned short in_cue_point_number)
{
    return Utils::get_str_time_from_sample_index(this->cue_points[in_deck_index][in_cue_point_number],
                                                 this->ats[in_deck_index]->get_sample_rate(),
                                                 true);
}

float
Audio_track_playback_process::sample_index_to_float(unsigned short int in_deck_index,
                                                    unsigned int       in_sample_index)
{
    // Convert a sample index to a float position (from 0.0 to 1.0).
    return (float)((float)in_sample_index / (float)this->ats[in_deck_index]->get_max_nb_samples());
}

unsigned int
Audio_track_playback_process::sample_index_to_msec(unsigned short int in_deck_index,
                                                   unsigned int       in_sample_index)
{
    // Convert a sample index to milliseconds.
    return qRound((1000.0 * (float)in_sample_index) / (2.0 * (float)this->ats[in_deck_index]->get_sample_rate()));
}

unsigned int
Audio_track_playback_process::msec_to_sample_index(unsigned short int in_deck_index,
                                                   unsigned int       in_position_msec)
{
    // Convert a position from msec to sample index.
    return qRound(((float)in_position_msec * 2.0 * (float)this->ats[in_deck_index]->get_sample_rate()) / 1000.0);
}
