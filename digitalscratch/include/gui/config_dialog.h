/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*--------------------------------------------------------( config_dialog.h )-*/
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
/*                Creates configuration GUI for DigitalScratch player         */
/*                                                                            */
/*============================================================================*/

#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QString>
#include <QCloseEvent>
#include <QWidget>
#include <QLabel>
#include <QSlider>

#include "app/application_settings.h"
#include "app/application_const.h"

#define ICON ":/pixmaps/digitalscratch-icon.png"

using namespace std;

class ShortcutQLabel : public QLabel
{
    Q_OBJECT

 public:
    ShortcutQLabel(QWidget *parent=0);
    virtual ~ShortcutQLabel();
    void set_old_text();

 private:
    bool    capturing;
    QString old_text;

 private:
    void start_capture();
    void cancel_capture();
    void finish_capture(int in_key);

 signals:
    void clicked();
    void new_value(QString in_value);

 protected:
    void mousePressEvent(QMouseEvent *in_mouse_event);
    void keyPressEvent(QKeyEvent *in_key_event);
};

class Config_dialog : public QDialog
{
    Q_OBJECT

 public:

 private:
    QLineEdit            *base_dir_path;
    QLineEdit            *extern_prog;
    QComboBox            *gui_style_select;
    QComboBox            *nb_decks_select;
    QComboBox            *sample_rate_select;
    QCheckBox            *device_jack_check;
    QCheckBox            *auto_jack_connections_check;
    QCheckBox            *device_internal_check;
    QComboBox            *device_internal_select;

    QCheckBox            *autostart_detection_check;
    QComboBox            *vinyl_type_select;
    QComboBox            *rpm_select;
    QSlider              *amplify_coeff;
    QLabel               *amplify_coeff_value;
    QSlider              *min_amplitude_for_normal_speed;
    QLabel               *min_amplitude_for_normal_speed_value;
    QSlider              *min_amplitude;
    QLabel               *min_amplitude_value;
    ShortcutQLabel       *kb_switch_playback;
    ShortcutQLabel       *kb_load_track_on_deck;
    ShortcutQLabel       *kb_play_begin_track_on_deck;
    ShortcutQLabel       *kb_get_next_track_from_deck;
    ShortcutQLabel       *kb_set_cue_point1_on_deck;
    ShortcutQLabel       *kb_play_cue_point1_on_deck;
    ShortcutQLabel       *kb_set_cue_point2_on_deck;
    ShortcutQLabel       *kb_play_cue_point2_on_deck;
    ShortcutQLabel       *kb_set_cue_point3_on_deck;
    ShortcutQLabel       *kb_play_cue_point3_on_deck;
    ShortcutQLabel       *kb_set_cue_point4_on_deck;
    ShortcutQLabel       *kb_play_cue_point4_on_deck;
    ShortcutQLabel       *kb_fullscreen;
    ShortcutQLabel       *kb_collapse_browse;
    ShortcutQLabel       *kb_load_track_on_sampler1;
    ShortcutQLabel       *kb_load_track_on_sampler2;
    ShortcutQLabel       *kb_load_track_on_sampler3;
    ShortcutQLabel       *kb_load_track_on_sampler4;
    ShortcutQLabel       *kb_help;
    ShortcutQLabel       *kb_file_search;
    Application_settings *settings;

 public:
    Config_dialog(QWidget *parent);
    virtual ~Config_dialog();
    int show();

 private:
    void     accept();
    void     reject();
    QWidget *init_tab_player();
    QWidget *init_tab_sound_card();
    QWidget *init_tab_motion_detect();
    QWidget *init_tab_shortcuts();
    void     fill_tab_player();
    void     fill_tab_sound_card();
    void     fill_tab_motion_detect();
    void     fill_tab_shortcuts();
    void     set_amplify_coeff_slider(const int &value);
    int      get_amplify_coeff_slider();
    void     set_min_amplitude_for_normal_speed_slider(const float &value);
    float    get_min_amplitude_for_normal_speed_slider();
    void     set_min_amplitude_slider(const float &value);
    float    get_min_amplitude_slider();
    bool     is_duplicate_shortcut(const QString &value);

 private slots:
    bool show_browse_window();
    bool show_browse_extern_prog_window();
    void reset_shortcuts();
    void reset_motion_detection_params();
    void set_amplify_coeff_value(const int &value);
    void set_min_amplitude_for_normal_speed_value(const int &value);
    void set_min_amplitude_value(const int &value);
    void validate_and_set_shortcut(const QString &value, ShortcutQLabel *label);
};
