/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*--------------------------------------------------------( config_dialog.h )-*/
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
/*                Creates configuration GUI for DigitalScratch player         */
/*                                                                            */
/*============================================================================*/

#ifndef CONFIG_DIALOG_H_
#define CONFIG_DIALOG_H_

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QString>
#include <QCloseEvent>
#include <QWidget>
#include <QLabel>
#include <QSlider>
#include "application_settings.h"

#define ICON ":/pixmaps/digitalscratch-icon.png"

using namespace std;

class ShortcutQLabel : public QLabel
{
    Q_OBJECT

 public:
    ShortcutQLabel(QWidget *parent=0);
    virtual ~ShortcutQLabel();

 private:
    bool    capturing;
    QString old_text;

 private:
    void start_capture();
    void cancel_capture();
    void finish_capture(int in_key);

 signals:
    void clicked();

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
    QComboBox            *gui_style_select;
    QComboBox            *vinyl_type_select;
    QSlider              *extreme_min;
    QLabel               *extreme_min_value;
    QSlider              *max_speed_diff;
    QLabel               *max_speed_diff_value;
    QSlider              *slow_speed_algo_usage;
    QLabel               *slow_speed_algo_usage_value;
    QSlider              *max_nb_speed_for_stability;
    QLabel               *max_nb_speed_for_stability_value;
    QSlider              *nb_cycle_before_changing_direction;
    QLabel               *nb_cycle_before_changing_direction_value;
    QSlider              *low_pass_filter_max_speed_usage;
    QLabel               *low_pass_filter_max_speed_usage_value;
    ShortcutQLabel       *kb_switch_playback;
    ShortcutQLabel       *kb_load_track_on_deck;
    ShortcutQLabel       *kb_play_begin_track_on_deck;
    ShortcutQLabel       *kb_set_cue_point_on_deck;
    ShortcutQLabel       *kb_play_cue_point_on_deck;
    ShortcutQLabel       *kb_fullscreen;
    ShortcutQLabel       *kb_collapse_browse;
    ShortcutQLabel       *kb_load_track_on_sampler1;
    ShortcutQLabel       *kb_load_track_on_sampler2;
    ShortcutQLabel       *kb_load_track_on_sampler3;
    ShortcutQLabel       *kb_load_track_on_sampler4;
    ShortcutQLabel       *kb_help;
    Application_settings *settings;

 public:
    Config_dialog(QWidget              *parent,
                  Application_settings *in_settings);
    virtual ~Config_dialog();
    int show();

 private:
    void     accept();
    void     reject();
    QWidget *init_tab_player();
    QWidget *init_tab_motion_detect();
    QWidget *init_tab_shortcuts();
    void     fill_tab_player();
    void     fill_tab_motion_detect();
    void     fill_tab_shortcuts();
    void     set_extreme_min_slider(float in_value);
    float    get_extreme_min_slider();
    void     set_low_pass_filter_max_speed_usage_slider(float in_value);
    float    get_low_pass_filter_max_speed_usage_slider();
    void     set_max_speed_diff_slider(float in_value);
    float    get_max_speed_diff_slider();
    void     set_slow_speed_algo_usage_slider(float in_value);
    float    get_slow_speed_algo_usage_slider();
    void     set_max_nb_speed_for_stability_slider(int in_value);
    int      get_max_nb_speed_for_stability_slider();
    void     set_nb_cycle_before_changing_direction_slider(int in_value);
    int      get_nb_cycle_before_changing_direction_slider();

 private slots:
    bool show_browse_window();
    void reset_shortcuts();
    void reset_motion_detection_params();
    void set_extreme_min_value(int in_value);
    void set_low_pass_filter_max_speed_usage_value(int in_value);
    void set_max_speed_diff_value(int in_value);
    void set_slow_speed_algo_usage_value(int in_value);
    void set_max_nb_speed_for_stability_value(int in_value);
    void set_nb_cycle_before_changing_direction_value(int in_value);
};

#endif /* CONFIG_DIALOG_H_ */
