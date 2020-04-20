/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*--------------------------------------------------------( config_dialog.h )-*/
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
    explicit ShortcutQLabel(QWidget *parent=0);
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
    QLineEdit            *tracklist_path;
    QLineEdit            *playlist_path;
    QLineEdit            *extern_prog;
    QComboBox            *gui_style_select;
    QComboBox            *gui_lang_select;
    QComboBox            *nb_decks_select;
    QComboBox            *sample_rate_select;
    QCheckBox            *device_jack_check;
    QCheckBox            *auto_jack_connections_check;
    QCheckBox            *device_internal_check;
    QComboBox            *device_internal_select;

    QList<QComboBox*>     vinyl_type_select;
    QList<QComboBox*>     rpm_select;
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
    ShortcutQLabel       *kb_reset_signal_level_on_deck;
    ShortcutQLabel       *kb_fullscreen;
    ShortcutQLabel       *kb_load_track_on_sampler1;
    ShortcutQLabel       *kb_load_track_on_sampler2;
    ShortcutQLabel       *kb_load_track_on_sampler3;
    ShortcutQLabel       *kb_load_track_on_sampler4;
    ShortcutQLabel       *kb_help;
    ShortcutQLabel       *kb_file_search;
    ShortcutQLabel       *kb_pl_track_move_up;
    ShortcutQLabel       *kb_pl_track_move_down;
    Application_settings *settings;

 public:
    explicit Config_dialog(QWidget *parent);
    virtual ~Config_dialog();
    int show();

 private:
    void     accept();
    void     reject();
    QWidget *init_tab_player();
    QWidget *init_tab_sound_card();
    QWidget *init_tab_motion_detect(const unsigned short int &deck_index);
    QWidget *init_tab_shortcuts();
    void     fill_tab_player();
    void     fill_tab_sound_card();
    void     fill_tab_motion_detect(const unsigned short &deck_index);
    void     fill_tab_shortcuts();
    bool     is_duplicate_shortcut(const QString &value);

 private slots:
    bool show_browse_window_for_base_dir_path();
    bool show_browse_window_for_tracklist_path();
    bool show_browse_window_for_playlist_path();
    bool show_browse_extern_prog_window();
    void reset_shortcuts();
    void reset_motion_detection_params(const unsigned short int & deck_index);
    void validate_and_set_shortcut(const QString &value, ShortcutQLabel *label);
};
