/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*------------------------------------------------( deck_playback_process.h )-*/
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
/*  Behavior class: prepare samples of main track and samplers to be sent to  */
/*                  the sound card.                                           */
/*                                                                            */
/*============================================================================*/

#pragma once

#include <QObject>
#include <QSharedPointer>
#include <samplerate.h>

#include "tracks/audio_track.h"
#include "player/playback_parameters.h"
#include "app/application_const.h"

using namespace std;

#define SOUND_STRETCH_POND_MAX                 8192
#define SOUND_STRETCH_POND_MIN                 256
#define NB_CYCLE_WITHOUT_UPDATE_REMAINING_TIME 20
#define SOUND_STRETCH_MAX_BUFFER               SHRT_MAX

class Deck_playback_process : public QObject
{
    Q_OBJECT

 private:
    QSharedPointer<Audio_track>           at;
    QList<QSharedPointer<Audio_track>>    at_samplers;
    QSharedPointer<Playback_parameters>   param;
    unsigned int                          current_sample;
    QList<unsigned int>                   cue_points;
    unsigned int                          remaining_time;
    QList<unsigned int>                   sampler_current_samples;
    QList<unsigned int>                   sampler_remaining_times;
    QList<bool>                           sampler_current_states;         // States of sampler (true=play).
    unsigned short int                    need_update_remaining_time;
    unsigned short int                    need_update_samplers_remaining_time;
    bool                                  stopped;                        // State (stopped = true) of audio track playback.
    unsigned short int                    nb_samplers;
    SRC_STATE                            *src_state;                      // Libsamplerate internal state.
    SRC_DATA                             *src_data;                       // Libsamplerate internal structure.
    short signed int                      src_int_input_data[SOUND_STRETCH_MAX_BUFFER];
    float                                 src_float_input_data[SOUND_STRETCH_MAX_BUFFER];
    float                                 src_float_output_data[SOUND_STRETCH_MAX_BUFFER];
    short signed int                      src_int_output_data[SOUND_STRETCH_MAX_BUFFER];

 public:
    Deck_playback_process(const QSharedPointer<Audio_track>         &at,
                          const QList<QSharedPointer<Audio_track>>  &at_sampler,
                          const QSharedPointer<Playback_parameters> &param);

    virtual ~Deck_playback_process();

    bool run(float io_playback_buf_1[], float io_playback_buf_2[], const unsigned short int &buf_size);

    bool stop();
    bool reset();
    bool jump_to_position(const float &position);
    float get_position(); // 0.0 < position < 1.0
    bool is_track_loaded();

    bool is_cue_point_defined(const unsigned short int &cue_point_number);
    float get_cue_point(const unsigned short int &cue_point_number);
    bool read_cue_point(const unsigned short int &cue_point_number);
    bool store_cue_point(const unsigned short int &cue_point_number);
    bool jump_to_cue_point(const unsigned short int &cue_point_number);
    bool delete_cue_point(const unsigned short int &cue_point_number);
    QString get_cue_point_str(const unsigned short int &cue_point_number) const;

    bool reset_sampler(const unsigned short int &sampler_index);
    void del_sampler(const unsigned short int &sampler_index);
    bool get_sampler_state(const unsigned short int &sampler_index);
    bool set_sampler_state(const unsigned short int &sampler_index, const bool &state);
    bool is_sampler_loaded(const unsigned short int &sampler_index);

 private:
    bool play_silence(QVector<float*> &io_playback_bufs, const unsigned short int &buf_size);
    bool play_main_track(QVector<float*> &io_playback_bufs, const unsigned short int &buf_size);
    bool play_samplers(QVector<float*> &io_playback_bufs, const unsigned short int &buf_size);
    bool play_data_with_playback_parameters(QVector<float*> &io_playback_bufs, const unsigned short int &buf_size);
    bool change_volume(float io_samples[], const unsigned short int &size);

    bool update_remaining_time();
    bool update_samplers_remaining_time();

    float sample_index_to_float(const unsigned int &sample_index);
    unsigned int sample_index_to_msec(const unsigned int &sample_index);
    unsigned int msec_to_sample_index(const unsigned int &position_msec);

 signals:
    void remaining_time_changed(const unsigned int &remaining_time);
    void sampler_remaining_time_changed(const unsigned int &remaining_time, const int &sampler_index);
    void sampler_state_changed(const int &sampler_index, const bool &state);
};
