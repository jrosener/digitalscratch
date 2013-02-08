/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*-------------------------------------------------( application_settings.h )-*/
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
/*                     Set and get application settings                       */
/*                                                                            */
/*============================================================================*/

#ifndef APPLICATION_SETTINGS_H
#define APPLICATION_SETTINGS_H

#include <QObject>
#include <QSettings>

using namespace std;

#define APPLICATION_NAME      "digitalscratch"

#define BASE_DIR_PATH_CFG     "player/base_dir_path"

#define GUI_STYLE_CFG         "player/gui_style_select"
#define GUI_STYLE_DEFAULT     "no style"
#define GUI_STYLE_NB1         "dark style"

#define VINYL_TYPE_CFG                      "motion_detection/vinyl_type"
#define EXTREME_MIN_CFG                     "motion_detection/extreme_min"
#define MAX_NB_BUFFER_CFG                   "motion_detection/max_nb_buffer"
#define MAX_BUFFER_COEFF_CFG                "motion_detection/max_buffer_coeff"
#define LOW_PASS_FILTER_MAX_SPEED_USAGE_CFG "motion_detection/low_pass_filter_max_speed_usage"

#define MAX_SPEED_DIFF_CFG                  "playback_parameters/max_speed_diff"
#define SLOW_SPEED_ALGO_USAGE_CFG           "playback_parameters/slow_speed_algo_usage"
#define MAX_NB_SPEED_STABIL_CFG             "playback_parameters/min_nb_speed_for_stability"
#define NB_CYCLE_CHANGING_DIR_CFG           "playback_parameters/nb_cycle_before_changing_direction"

#define KB_SWITCH_PLAYBACK                  "keyboard_shortcuts/kb_switch_playback"
#define KB_SWITCH_PLAYBACK_DEFAULT          "SPACE"
#define KB_LOAD_TRACK_ON_DECK               "keyboard_shortcuts/kb_load_track_on_deck"
#define KB_LOAD_TRACK_ON_DECK_DEFAULT       "F1"
#define KB_PLAY_BEGIN_TRACK_ON_DECK         "keyboard_shortcuts/kb_play_begin_track_on_deck"
#define KB_PLAY_BEGIN_TRACK_ON_DECK_DEFAULT "F2"
#define KB_SET_CUE_POINT_ON_DECK            "keyboard_shortcuts/kb_set_cue_point_on_deck"
#define KB_SET_CUE_POINT_ON_DECK_DEFAULT    "F3"
#define KB_PLAY_CUE_POINT_ON_DECK           "keyboard_shortcuts/kb_play_cue_point_on_deck"
#define KB_PLAY_CUE_POINT_ON_DECK_DEFAULT   "F4"
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
#define KB_FULLSCREEN                       "keyboard_shortcuts/kb_fullscreen"
#define KB_FULLSCREEN_DEFAULT               "F"
#define KB_HELP                             "keyboard_shortcuts/kb_help"
#define KB_HELP_DEFAULT                     "H"

class Application_settings : public QObject
{
    Q_OBJECT

 private:
    QSettings      *settings;
    QList<QString> *available_gui_styles;
    QList<QString> *available_vinyl_types;

 public:
    Application_settings();
    virtual ~Application_settings();

 public:
    void    set_tracks_base_dir_path(QString in_tracks_base_dir_path);
    QString get_tracks_base_dir_path();
    QString get_tracks_base_dir_path_default();

    void            set_gui_style(QString in_gui_style);
    QString         get_gui_style();
    QString         get_gui_style_default();
    QList<QString>* get_available_gui_styles();

    void            set_vinyl_type(QString in_vinyl_type);
    QString         get_vinyl_type();
    QString         get_vinyl_type_default();
    QList<QString>* get_available_vinyl_types();

    void  set_extreme_min(float in_extreme_min);
    float get_extreme_min();
    float get_extreme_min_default();

    void  set_max_nb_buffer(int in_nb_buffer);
    int   get_max_nb_buffer();
    int   get_max_nb_buffer_default();

    void  set_max_buffer_coeff(int in_buffer_coeff);
    int   get_max_buffer_coeff();
    int   get_max_buffer_coeff_default();

    void  set_max_speed_diff(float in_diff);
    float get_max_speed_diff();
    float get_max_speed_diff_default();

    void  set_slow_speed_algo_usage(float in_speed);
    float get_slow_speed_algo_usage();
    float get_slow_speed_algo_usage_default();

    void  set_max_nb_speed_for_stability(int in_nb_speed);
    int   get_max_nb_speed_for_stability();
    int   get_max_nb_speed_for_stability_default();

    void  set_nb_cycle_before_changing_direction(int in_nb_cycle);
    int   get_nb_cycle_before_changing_direction();
    int   get_nb_cycle_before_changing_direction_default();

    void  set_low_pass_filter_max_speed_usage(float in_low_pass_filter_max_speed_usage);
    float get_low_pass_filter_max_speed_usage();
    float get_low_pass_filter_max_speed_usage_default();

    void    set_keyboard_shortcut(QString in_kb_shortcut_path, QString in_value);
    QString get_keyboard_shortcut(QString in_kb_shortcut_path);

 private:
    void init_settings();
};

#endif // APPLICATION_SETTINGS_H
