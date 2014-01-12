/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*-----------------------------------------( audio_track_playback_process.h )-*/
/*                                                                            */
/*  Copyright (C) 2003-2012                                                   */
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

#ifndef AUDIO_TRACK_PLAYBACK_PROCESS_H_
#define AUDIO_TRACK_PLAYBACK_PROCESS_H_

#include <QObject>
#include <audio_track.h>
#include <playback_parameters.h>
#include <samplerate.h>
#include <application_const.h>

using namespace std;

#define SOUND_STRETCH_POND_MAX                 8192
#define SOUND_STRETCH_POND_MIN                 256
#define NB_CYCLE_WITHOUT_UPDATE_REMAINING_TIME 11
#define SOUND_STRETCH_MAX_BUFFER               32768

class Audio_track_playback_process : public QObject
{
    Q_OBJECT

 private:
    Audio_track          **ats;                     // Table of nb_decks audio track.
    Audio_track         ***at_samplers;             // Table of nb_decks samplers.
    Playback_parameters  **params;                  // Table of nb_decks playback parameters.
    unsigned int          *current_samples;         // Table of nb_decks playback current_sample.
    unsigned int         **cue_points;              // Table of nb_decks playback cue points.
    unsigned int          *remaining_times;         // Table of nb_decks remaining time (in msec).
    unsigned int          *sampler_current_samples; // Table of (nb_decks * nb_samplers) playback current_sample.
    unsigned int          *sampler_remaining_times; // Table of (nb_decks * nb_samplers) remaining time (in msec).
    bool                  *sampler_current_states;  // Table of (nb_decks * nb_samplers) states of sampler (true=play).
    unsigned short int     need_update_remaining_time;
    bool                  *stopped;                 // Table of state (stopped = true) of audio track playback.
    unsigned short int     nb_decks;
    unsigned short int     nb_samplers;
    SRC_STATE            **src_state;       // Libsamplerate internal state.
    SRC_DATA             **src_data;        // Libsamplerate internal structure.
    short signed int       src_int_input_data[SOUND_STRETCH_MAX_BUFFER];
    float                  src_float_input_data[SOUND_STRETCH_MAX_BUFFER];
    float                  src_float_output_data[SOUND_STRETCH_MAX_BUFFER];
    short signed int       src_int_output_data[SOUND_STRETCH_MAX_BUFFER];

 public:
    Audio_track_playback_process(Audio_track          *in_ats[],
                                 Audio_track         **in_at_samplers[],
                                 Playback_parameters  *in_params[],
                                 unsigned short int    in_nb_decks,
                                 unsigned short int    in_nb_samplers);

    virtual ~Audio_track_playback_process();

    bool run(unsigned short int  in_nb_samples,
             float              *out_samples_1,
             float              *out_samples_2,
             float              *out_samples_3,
             float              *out_samples_4);  // Prepare set of samples to be played in sound card.

    bool stop(unsigned short int in_deck_index);
    bool reset(unsigned short int in_deck_index);

    bool reset_sampler(unsigned short int in_index,
                       unsigned short int in_sampler_index);

    void del_sampler(unsigned short int in_deck_index,
                     unsigned short int in_sampler_index);

    bool jump_to_position(unsigned short int in_deck_index,
                          float              in_position);

    float    get_cue_point(unsigned short int in_deck_index,     unsigned short int in_cue_point_number);
    bool     read_cue_point(unsigned short int in_deck_index,    unsigned short int in_cue_point_number);
    bool     store_cue_point(unsigned short int in_deck_index,   unsigned short int in_cue_point_number);
    bool     jump_to_cue_point(unsigned short int in_deck_index, unsigned short int in_cue_point_number);
    bool     delete_cue_point(unsigned short int in_deck_index,  unsigned short int in_cue_point_number);
    QString  get_cue_point_str(unsigned short int in_deck_index, unsigned short int in_cue_point_number);

    float get_position(unsigned short int in_deck_index); // 0.0 < position < 1.0

    bool get_sampler_state(unsigned short int in_deck_index,
                           unsigned short int in_sampler_index);

    bool set_sampler_state(unsigned short int in_deck_index,
                           unsigned short int in_sampler_index,
                           bool               in_state);

    bool is_sampler_loaded(unsigned short int in_deck_index,
                           unsigned short int in_sampler_index);

 private:
    bool play_empty(unsigned short int   in_deck_index,
                    unsigned short int   in_nb_samples,
                    float              **out_samples);

    bool play_audio_track(unsigned short int   in_deck_index,
                          unsigned short int   in_nb_samples,
                          float              **out_samples);

    bool play_sampler(unsigned short int   in_deck_index,
                      unsigned short int   in_nb_samples,
                      float              **out_samples);

    bool update_remaining_time(unsigned short int in_deck_index);

    bool update_sampler_remaining_time(unsigned short int in_deck_index,
                                       unsigned short int in_sampler_index);

    bool play_data_with_playback_parameters(unsigned short int   in_deck_index,
                                            unsigned short int   in_nb_samples,
                                            float              **out_samples);

    bool change_volume(unsigned short int  in_deck_index,
                       float              *in_buffer,
                       unsigned short int  in_size);

    bool change_position(unsigned short int  in_deck_index,
                         float              *io_speed,
                         unsigned short int  in_nb_samples);

    bool speed_up(float              *io_speed,
                  int                 in_diff,
                  unsigned short int  in_nb_samples);

    bool speed_down(float              *io_speed,
                    unsigned int        in_vinyl_pos,
                    unsigned int        in_track_pos,
                    unsigned short int  in_nb_samples);

    float sample_index_to_float(unsigned short int in_deck_index,
                                unsigned int       in_sample_index);

    unsigned int sample_index_to_msec(unsigned short int in_deck_index,
                                      unsigned int       in_sample_index);

    unsigned int msec_to_sample_index(unsigned short int in_deck_index,
                                      unsigned int       in_position_msec);

 signals:
    void remaining_time_changed(unsigned int in_remaining_time, int in_index);
    void sampler_remaining_time_changed(unsigned int in_remaining_time, int in_deck_index, int in_sampler_index);
    void sampler_state_changed(int in_deck_index, int in_sampler_index, bool in_state);
};

#endif /* AUDIO_TRACK_PLAYBACK_PROCESS_H_ */
