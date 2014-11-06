/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*-------------------------------------------------( application_settings.h )-*/
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
/*                     Set and get application settings                       */
/*                                                                            */
/*============================================================================*/

#pragma once

#include <QObject>
#include <QSettings>

#include "audio_device_access_rules.h"
#include "application_const.h"

using namespace std;

// Application and main window.
#define APPLICATION_NAME          "digitalscratch"

#define MAIN_WIN_SIZE_CFG         "player/main_win_size"
#define MAIN_WIN_SIZE_CFG_DEFAULT "1024x640"

#define MAIN_WIN_POS_CFG          "player/main_win_position"
#define MAIN_WIN_POS_CFG_DEFAULT  "50,50"

#define BROWSER_SPLITTER_SIZE_CFG "player/browser_splitter_size"

#define BASE_DIR_PATH_CFG         "player/base_dir_path"

#define GUI_STYLE_CFG             "player/gui_style_select"
#define GUI_STYLE_NATIVE          "native"
#define GUI_STYLE_DARK            "dark"
#define GUI_STYLE_DARK_CSS        SKINS_PATH "dark.css"
#define GUI_STYLE_DEFAULT         "dark"
#define SAMPLERS_VISIBLE_CFG      "player/samplers_visible"
#define SAMPLERS_VISIBLE_DEFAULT  1
#define EXTERN_PROG_CFG           "player/extern_prog"
#define NB_DECKS_CFG              "player/nb_decks"
#define NB_DECKS_DEFAULT          2

// Sound caracteristics.
#define SAMPLE_RATE_CFG                     "sound_card/sample_rate"
#define SAMPLE_RATE_DEFAULT                 44100
#define AUTO_JACK_CONNECTIONS_CFG           "sound_card/auto_jack_connections"
#define AUTO_JACK_CONNECTIONS_DEFAULT       1
#define SOUND_DRIVER_CFG                    "sound_card/driver_select"
#define SOUND_DRIVER_JACK                   "jack"
#define SOUND_DRIVER_INTERNAL               "internal"
#define SOUND_DRIVER_DEFAULT                SOUND_DRIVER_JACK
#define SOUND_CARD_CFG                      "sound_card/sound_card_id"
#define SOUND_CARD_DEFAULT                  "0"

// Motion detection.
#define AUTOSTART_MOTION_DETECTION_CFG      "motion_detection/auto_start_motion_detection"
#define AUTOSTART_MOTION_DETECTION_DEFAULT  0
#define VINYL_TYPE_CFG                      "motion_detection/vinyl_type"
#define RPM_CFG                             "motion_detection/rpm"
#define EXTREME_MIN_CFG                     "motion_detection/extreme_min"
#define MAX_NB_BUFFER_CFG                   "motion_detection/max_nb_buffer"
#define MAX_BUFFER_COEFF_CFG                "motion_detection/max_buffer_coeff"
#define LOW_PASS_FILTER_MAX_SPEED_USAGE_CFG "motion_detection/low_pass_filter_max_speed_usage"
#define INPUT_AMPLIFY_COEFF                 "motion_detection/input_amplify_coeff"
#define MIN_AMPLITUDE_NORMAL_SPEED          "motion_detection/min_amplitude_normal_speed"
#define MIN_AMPLITUDE                       "motion_detection/min_amplitude"

// Playback parameters.
#define MAX_SPEED_DIFF_CFG                  "playback_parameters/max_speed_diff"
#define SLOW_SPEED_ALGO_USAGE_CFG           "playback_parameters/slow_speed_algo_usage"
#define MAX_NB_SPEED_STABIL_CFG             "playback_parameters/min_nb_speed_for_stability"
#define NB_CYCLE_CHANGING_DIR_CFG           "playback_parameters/nb_cycle_before_changing_direction"

// Keyboard shortcuts.
#define KB_SWITCH_PLAYBACK                  "keyboard_shortcuts/kb_switch_playback"
#define KB_SWITCH_PLAYBACK_DEFAULT          "SPACE"
#define KB_LOAD_TRACK_ON_DECK               "keyboard_shortcuts/kb_load_track_on_deck"
#define KB_LOAD_TRACK_ON_DECK_DEFAULT       "F1"
#define KB_PLAY_BEGIN_TRACK_ON_DECK         "keyboard_shortcuts/kb_play_begin_track_on_deck"
#define KB_PLAY_BEGIN_TRACK_ON_DECK_DEFAULT "F2"

#define KB_SET_CUE_POINT1_ON_DECK           "keyboard_shortcuts/kb_set_cue_point1_on_deck"
#define KB_SET_CUE_POINT1_ON_DECK_DEFAULT   "F5"
#define KB_PLAY_CUE_POINT1_ON_DECK          "keyboard_shortcuts/kb_play_cue_point1_on_deck"
#define KB_PLAY_CUE_POINT1_ON_DECK_DEFAULT  "F6"
#define KB_SET_CUE_POINT2_ON_DECK           "keyboard_shortcuts/kb_set_cue_point2_on_deck"
#define KB_SET_CUE_POINT2_ON_DECK_DEFAULT   "F7"
#define KB_PLAY_CUE_POINT2_ON_DECK          "keyboard_shortcuts/kb_play_cue_point2_on_deck"
#define KB_PLAY_CUE_POINT2_ON_DECK_DEFAULT  "F8"
#define KB_SET_CUE_POINT3_ON_DECK           "keyboard_shortcuts/kb_set_cue_point3_on_deck"
#define KB_SET_CUE_POINT3_ON_DECK_DEFAULT   "F9"
#define KB_PLAY_CUE_POINT3_ON_DECK          "keyboard_shortcuts/kb_play_cue_point3_on_deck"
#define KB_PLAY_CUE_POINT3_ON_DECK_DEFAULT  "F10"
#define KB_SET_CUE_POINT4_ON_DECK           "keyboard_shortcuts/kb_set_cue_point4_on_deck"
#define KB_SET_CUE_POINT4_ON_DECK_DEFAULT   "F11"
#define KB_PLAY_CUE_POINT4_ON_DECK          "keyboard_shortcuts/kb_play_cue_point4_on_deck"
#define KB_PLAY_CUE_POINT4_ON_DECK_DEFAULT  "F12"
#define KB_COLLAPSE_BROWSER                 "keyboard_shortcuts/kb_collapse_browse"
#define KB_COLLAPSE_BROWSER_DEFAULT         "Ctrl+Up"
#define KB_LOAD_TRACK_ON_SAMPLER1           "keyboard_shortcuts/kb_load_track_on_sampler1"
#define KB_LOAD_TRACK_ON_SAMPLER1_DEFAULT   "&"
#define KB_LOAD_TRACK_ON_SAMPLER2           "keyboard_shortcuts/kb_load_track_on_sampler2"
#define KB_LOAD_TRACK_ON_SAMPLER2_DEFAULT   "A"
#define KB_LOAD_TRACK_ON_SAMPLER3           "keyboard_shortcuts/kb_load_track_on_sampler3"
#define KB_LOAD_TRACK_ON_SAMPLER3_DEFAULT   "Q"
#define KB_LOAD_TRACK_ON_SAMPLER4           "keyboard_shortcuts/kb_load_track_on_sampler4"
#define KB_LOAD_TRACK_ON_SAMPLER4_DEFAULT   "W"
#define KB_SHOW_NEXT_KEYS                   "keyboard_shortcuts/kb_show_next_keys"
#define KB_SHOW_NEXT_KEYS_DEFAULT           "K"
#define KB_FULLSCREEN                       "keyboard_shortcuts/kb_fullscreen"
#define KB_FULLSCREEN_DEFAULT               "F"
#define KB_HELP                             "keyboard_shortcuts/kb_help"
#define KB_HELP_DEFAULT                     "H"
#define KB_FILE_SEARCH                      "keyboard_shortcuts/kb_file_search"
#define KB_FILE_SEARCH_DEFAULT              "Ctrl+F"

const QString KB_SET_CUE_POINTS_ON_DECK[]          = { KB_SET_CUE_POINT1_ON_DECK,
                                                       KB_SET_CUE_POINT2_ON_DECK,
                                                       KB_SET_CUE_POINT3_ON_DECK,
                                                       KB_SET_CUE_POINT4_ON_DECK };
const QString KB_SET_CUE_POINTS_ON_DECK_DEFAULT[]  = { KB_SET_CUE_POINT1_ON_DECK_DEFAULT,
                                                       KB_SET_CUE_POINT2_ON_DECK_DEFAULT,
                                                       KB_SET_CUE_POINT3_ON_DECK_DEFAULT,
                                                       KB_SET_CUE_POINT4_ON_DECK_DEFAULT };
const QString KB_PLAY_CUE_POINTS_ON_DECK[]         = { KB_PLAY_CUE_POINT1_ON_DECK,
                                                       KB_PLAY_CUE_POINT2_ON_DECK,
                                                       KB_PLAY_CUE_POINT3_ON_DECK,
                                                       KB_PLAY_CUE_POINT4_ON_DECK };
const QString KB_PLAY_CUE_POINTS_ON_DECK_DEFAULT[] = { KB_PLAY_CUE_POINT1_ON_DECK_DEFAULT,
                                                       KB_PLAY_CUE_POINT2_ON_DECK_DEFAULT,
                                                       KB_PLAY_CUE_POINT3_ON_DECK_DEFAULT,
                                                       KB_PLAY_CUE_POINT4_ON_DECK_DEFAULT };

class Application_settings : public QObject
{
    Q_OBJECT

 private:
    QSettings                 *settings;
    QList<QString>             available_gui_styles;
    QList<QString>             available_vinyl_types;
    QList<unsigned short int>  available_rpms;
    QList<unsigned int>        available_sample_rates;
    QList<unsigned int>        available_nb_decks;
    QList<QString>             available_sound_cards;
    bool                       audio_collection_full_refresh;

 public:
    Application_settings();
    virtual ~Application_settings();

 public:
    void    set_main_window_size(QSize in_size);
    QSize   get_main_window_size();
    QString get_main_window_size_default();

    void       set_browser_splitter_size(QByteArray in_state);
    QByteArray get_browser_splitter_size();

    void    set_main_window_position(QPoint in_pos);
    QPoint  get_main_window_position();
    QString get_main_window_position_default();

    void    set_tracks_base_dir_path(QString in_tracks_base_dir_path);
    QString get_tracks_base_dir_path();
    QString get_tracks_base_dir_path_default();

    void    set_extern_prog(QString in_extern_prog_path);
    QString get_extern_prog();
    QString get_extern_prog_default();

    void            set_gui_style(QString in_gui_style);
    QString         get_gui_style();
    QString         get_gui_style_default();
    QList<QString>  get_available_gui_styles();

    void                 set_nb_decks(signed short int in_nb_decks);
    signed short int     get_nb_decks();
    signed short int     get_nb_decks_default();
    QList<unsigned int>  get_available_nb_decks();

    void                 set_sample_rate(unsigned int in_sample_rate);
    unsigned int         get_sample_rate();
    unsigned int         get_sample_rate_default();
    QList<unsigned int>  get_available_sample_rates();

    void            set_sound_driver(QString in_driver);
    QString         get_sound_driver();
    QString         get_sound_driver_default();
    void            set_internal_sound_card(QString in_card);
    QString         get_internal_sound_card();
    QString         get_internal_sound_card_default();
    QList<QString>  get_available_internal_sound_cards();

    void            set_auto_jack_connections(bool in_autoconnect);
    bool            get_auto_jack_connections();
    bool            get_auto_jack_connections_default();

    void            set_autostart_motion_detection(bool in_autostart);
    bool            get_autostart_motion_detection();
    bool            get_autostart_motion_detection_default();

    void            set_vinyl_type(QString in_vinyl_type);
    QString         get_vinyl_type();
    QString         get_vinyl_type_default();
    QList<QString>  get_available_vinyl_types();

    void                   set_rpm(unsigned short int in_rpm);
    unsigned short int     get_rpm();
    unsigned short int     get_rpm_default();
    QList<unsigned short>  get_available_rpms();

    void  set_input_amplify_coeff(int in_coeff);
    int   get_input_amplify_coeff();
    int   get_input_amplify_coeff_default();

    void  set_min_amplitude_for_normal_speed(float in_amplitude);
    float get_min_amplitude_for_normal_speed();
    float get_min_amplitude_for_normal_speed_default();
    float get_min_amplitude_for_normal_speed_default_from_vinyl_type(QString vinyl_type);

    void  set_min_amplitude(float in_amplitude);
    float get_min_amplitude();
    float get_min_amplitude_default();
    float get_min_amplitude_default_from_vinyl_type(QString vinyl_type);

    void    set_keyboard_shortcut(QString in_kb_shortcut_path, QString in_value);
    QString get_keyboard_shortcut(QString in_kb_shortcut_path);

    void set_samplers_visible(bool is_visible);
    bool get_samplers_visible();
    bool get_samplers_visible_default();

    // Runtime settings (not stored in preference file).
    void set_audio_collection_full_refresh(bool in_full_refresh);
    bool get_audio_collection_full_refresh();

 private:
    void init_settings();
};
