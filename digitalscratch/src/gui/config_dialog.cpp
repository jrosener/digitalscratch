/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*------------------------------------------------------( config_dialog.cpp )-*/
/*                                                                            */
/*  Copyright (C) 2003-2013                                                   */
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

#include <QIcon>
#include <QDialog>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QLabel>
#include <QGridLayout>
#include <QSettings>
#include <QPushButton>
#include <QtDebug>
#include <QMessageBox>
#include "config_dialog.h"
#include <digital_scratch_api.h>
#include <iostream>
#include <singleton.h>
#include <utils.h>

Config_dialog::Config_dialog(QWidget *parent) : QDialog(parent)
{
    this->settings = &Singleton<Application_settings>::get_instance();

    // Init player parameters widgets.
    this->base_dir_path    = new QLineEdit(this);
    this->gui_style_select = new QComboBox(this);
    QList<QString> *available_gui_styles = this->settings->get_available_gui_styles();
    for (int i = 0; i < available_gui_styles->size(); i++)
    {
        this->gui_style_select->addItem(available_gui_styles->at(i));
    }

    // Init sound card parameters widgets.
    this->sample_rate_select = new QComboBox(this);
    QList<unsigned int> *available_sample_rates = this->settings->get_available_sample_rates();
    for (int i = 0; i < available_sample_rates->size(); i++)
    {
        this->sample_rate_select->addItem(QString::number(available_sample_rates->at(i)));
    }

    // Init motion detection parameters widgets.
    this->extreme_min                              = new QSlider(Qt::Horizontal, this);
    this->extreme_min_value                        = new QLabel(this);
    this->amplify_coeff                            = new QSlider(Qt::Horizontal, this);
    this->amplify_coeff_value                      = new QLabel(this);
    this->max_speed_diff                           = new QSlider(Qt::Horizontal, this);
    this->max_speed_diff_value                     = new QLabel(this);
    this->slow_speed_algo_usage                    = new QSlider(Qt::Horizontal, this);
    this->slow_speed_algo_usage_value              = new QLabel(this);
    this->max_nb_speed_for_stability               = new QSlider(Qt::Horizontal, this);
    this->max_nb_speed_for_stability_value         = new QLabel(this);
    this->nb_cycle_before_changing_direction       = new QSlider(Qt::Horizontal, this);
    this->nb_cycle_before_changing_direction_value = new QLabel(this);
    this->low_pass_filter_max_speed_usage          = new QSlider(Qt::Horizontal, this);
    this->low_pass_filter_max_speed_usage_value    = new QLabel(this);
    this->vinyl_type_select                        = new QComboBox(this);
    QList<QString> *available_vinyl_types          = this->settings->get_available_vinyl_types();
    for (int i = 0; i < available_vinyl_types->size(); i++)
    {
        this->vinyl_type_select->addItem(available_vinyl_types->at(i));
    }
    this->autostart_detection_check                = new QCheckBox(this);
    this->auto_jack_connections_check              = new QCheckBox(this);
    this->autostart_detection_check->setTristate(false);
    this->auto_jack_connections_check->setTristate(false);

    // Init keyboard shortcuts widgets.
    this->kb_switch_playback           = new ShortcutQLabel(this);
    this->kb_load_track_on_deck        = new ShortcutQLabel(this);
    this->kb_play_begin_track_on_deck  = new ShortcutQLabel(this);
    this->kb_get_next_track_from_deck  = new ShortcutQLabel(this);
    this->kb_set_cue_point1_on_deck    = new ShortcutQLabel(this);
    this->kb_play_cue_point1_on_deck   = new ShortcutQLabel(this);
    this->kb_set_cue_point2_on_deck    = new ShortcutQLabel(this);
    this->kb_play_cue_point2_on_deck   = new ShortcutQLabel(this);
    this->kb_set_cue_point3_on_deck    = new ShortcutQLabel(this);
    this->kb_play_cue_point3_on_deck   = new ShortcutQLabel(this);
    this->kb_set_cue_point4_on_deck    = new ShortcutQLabel(this);
    this->kb_play_cue_point4_on_deck   = new ShortcutQLabel(this);
    this->kb_collapse_browse           = new ShortcutQLabel(this);
    this->kb_fullscreen                = new ShortcutQLabel(this);
    this->kb_help                      = new ShortcutQLabel(this);
    this->kb_load_track_on_sampler1    = new ShortcutQLabel(this);
    this->kb_load_track_on_sampler2    = new ShortcutQLabel(this);
    this->kb_load_track_on_sampler3    = new ShortcutQLabel(this);
    this->kb_load_track_on_sampler4    = new ShortcutQLabel(this);

    return;
}

Config_dialog::~Config_dialog()
{
    return;
}

int
Config_dialog::show()
{
    // Create the player tab.
    QWidget *player_tab = this->init_tab_player();
    this->fill_tab_player();

    // Create the sound card tab.
    QWidget *sound_card_tab = this->init_tab_sound_card();
    this->fill_tab_sound_card();

    // Create the motion detection tab: provide coded vinyl configuration parameters.
    QWidget *motion_detect_tab = this->init_tab_motion_detect();
    this->fill_tab_motion_detect();

    // Create the keyboard shortcuts tab.
    QWidget *shortcuts_tab = this->init_tab_shortcuts();
    this->fill_tab_shortcuts();

    //
    // Main window.
    //

    // Set window title.
    this->setWindowTitle(tr("Configuration"));

    // Set window icon
    this->setWindowIcon(QIcon(ICON));

    // Create 3 tabs: player, sound card and motion detection.
    QTabWidget *tabs = new QTabWidget(this);
    tabs->insertTab(0, player_tab,        tr("Player"));
    tabs->insertTab(1, sound_card_tab,    tr("Sound card"));
    tabs->insertTab(2, motion_detect_tab, tr("Motion detection"));
    tabs->insertTab(3, shortcuts_tab,     tr("Shortcuts"));

    // 2 buttons: OK and Cancel.
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |
                                                       QDialogButtonBox::Cancel,
                                                       Qt::Horizontal,
                                                       this);
    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    // Create main vertical layout.
    QVBoxLayout *main_layout = new QVBoxLayout(this);
    main_layout->addWidget(tabs);
    main_layout->addWidget(buttonBox);

    // Put main layout in window.
    this->setLayout(main_layout);

    return this->exec();
}

QWidget *Config_dialog::init_tab_player()
{
    // Player tab: select base music folder.
    QLabel *base_dir_label = new QLabel(tr("Base music directory: "), this);
    this->base_dir_path->setMinimumWidth(300);
    QPushButton *base_dir_button = new QPushButton(tr("Browse..."), this);
    QObject::connect(base_dir_button, SIGNAL(clicked()), this, SLOT(show_browse_window()));

    // Player tab: select GUI style.
    QLabel *gui_style_label = new QLabel(tr("GUI style: "), this);

    // Player tab: setup layout.
    QGridLayout *player_tab_layout = new QGridLayout(this);
    player_tab_layout->addWidget(base_dir_label,         0, 0);
    player_tab_layout->addWidget(this->base_dir_path,    0, 1);
    player_tab_layout->addWidget(base_dir_button,        0, 2);
    player_tab_layout->addWidget(gui_style_label,        1, 0);
    player_tab_layout->addWidget(this->gui_style_select, 1, 1);

    // Create tab.
    QWidget *player_tab = new QWidget(this);
    player_tab->setLayout(player_tab_layout);

    return player_tab;
}

void Config_dialog::fill_tab_player()
{
    this->base_dir_path->setText(this->settings->get_tracks_base_dir_path());
    this->gui_style_select->setCurrentIndex(this->gui_style_select->findText(this->settings->get_gui_style()));
}

QWidget *Config_dialog::init_tab_sound_card()
{
    // Sound card tab: select sample rate.
    QLabel *sample_rate_label = new QLabel(tr("Sample rate (need to restart): "), this);

    // Sound card tab: auto connect JACK.
    QLabel *auto_jack_connections_label = new QLabel(tr("Connect automatically JACK ports: "), this);

    // Sound card tab: setup layout.
    QGridLayout *soundcard_tab_layout = new QGridLayout(this);
    soundcard_tab_layout->addWidget(sample_rate_label,                 0, 0);
    soundcard_tab_layout->addWidget(this->sample_rate_select,          0, 1);
    soundcard_tab_layout->addWidget(auto_jack_connections_label,       1, 0);
    soundcard_tab_layout->addWidget(this->auto_jack_connections_check, 1, 1);

    // Create tab.
    QWidget *soundcard_tab = new QWidget(this);
    soundcard_tab->setLayout(soundcard_tab_layout);

    return soundcard_tab;
}

void Config_dialog::fill_tab_sound_card()
{
    this->sample_rate_select->setCurrentIndex(this->sample_rate_select->findText(QString::number(this->settings->get_sample_rate())));
    this->auto_jack_connections_check->setChecked(this->settings->get_auto_jack_connections());
}

QWidget *Config_dialog::init_tab_motion_detect()
{
    QGridLayout *motion_detect_layout = new QGridLayout(this);

    QLabel *autostart_detection_label = new QLabel(tr("Start motion detection at startup: "), this);
    motion_detect_layout->addWidget(autostart_detection_label,       0, 0);
    motion_detect_layout->addWidget(this->autostart_detection_check, 0, 1);

    QLabel *vinyl_type_label = new QLabel(tr("Vinyl type: "), this);
    motion_detect_layout->addWidget(vinyl_type_label,        1, 0);
    motion_detect_layout->addWidget(this->vinyl_type_select, 1, 1);

    QLabel *extreme_min_label = new QLabel(tr("Minimal signal:"), this);
    this->extreme_min->setMinimum(1);
    this->extreme_min->setMaximum(99);
    this->extreme_min->setSingleStep(1);
    motion_detect_layout->addWidget(extreme_min_label, 2, 0);
    motion_detect_layout->addWidget(this->extreme_min, 2, 1);
    motion_detect_layout->addWidget(this->extreme_min_value, 2, 2);
    QObject::connect(this->extreme_min, SIGNAL(valueChanged(int)), this, SLOT(set_extreme_min_value(int)));

    QLabel *amplify_coeff_label = new QLabel(tr("Amplification factor for input signal:"), this);
    this->amplify_coeff->setMinimum(1);
    this->amplify_coeff->setMaximum(20);
    this->amplify_coeff->setSingleStep(1);
    motion_detect_layout->addWidget(amplify_coeff_label, 3, 0);
    motion_detect_layout->addWidget(this->amplify_coeff, 3, 1);
    motion_detect_layout->addWidget(this->amplify_coeff_value, 3, 2);
    QObject::connect(this->amplify_coeff, SIGNAL(valueChanged(int)), this, SLOT(set_amplify_coeff_value(int)));

    QLabel *low_pass_filter_max_speed_usage_label = new QLabel(tr("Enable low pass filter under this speed:"), this);
    this->low_pass_filter_max_speed_usage->setMinimum(1);
    this->low_pass_filter_max_speed_usage->setMaximum(199);
    this->low_pass_filter_max_speed_usage->setSingleStep(1);
    motion_detect_layout->addWidget(low_pass_filter_max_speed_usage_label, 4, 0);
    motion_detect_layout->addWidget(this->low_pass_filter_max_speed_usage, 4, 1);
    motion_detect_layout->addWidget(this->low_pass_filter_max_speed_usage_value, 4, 2);
    QObject::connect(this->low_pass_filter_max_speed_usage, SIGNAL(valueChanged(int)), this, SLOT(set_low_pass_filter_max_speed_usage_value(int)));

    QLabel *max_speed_diff_label = new QLabel(tr("Maximum speed difference allowed beetween 2 speeds:"), this);
    this->max_speed_diff->setMinimum(1);
    this->max_speed_diff->setMaximum(99);
    this->max_speed_diff->setSingleStep(1);
    motion_detect_layout->addWidget(max_speed_diff_label, 5, 0);
    motion_detect_layout->addWidget(this->max_speed_diff, 5, 1);
    motion_detect_layout->addWidget(this->max_speed_diff_value, 5, 2);
    QObject::connect(this->max_speed_diff, SIGNAL(valueChanged(int)), this, SLOT(set_max_speed_diff_value(int)));

    QLabel *slow_speed_algo_usage_label = new QLabel(tr("Enable slow speed detection if speed is under this value:"), this);
    this->slow_speed_algo_usage->setMinimum(1);
    this->slow_speed_algo_usage->setMaximum(99);
    this->slow_speed_algo_usage->setSingleStep(1);
    motion_detect_layout->addWidget(slow_speed_algo_usage_label, 6, 0);
    motion_detect_layout->addWidget(this->slow_speed_algo_usage, 6, 1);
    motion_detect_layout->addWidget(this->slow_speed_algo_usage_value, 6, 2);
    QObject::connect(this->slow_speed_algo_usage, SIGNAL(valueChanged(int)), this, SLOT(set_slow_speed_algo_usage_value(int)));

    QLabel *max_nb_speed_for_stability_label = new QLabel(tr("Maximum number of speeds for stability system:"), this);
    this->max_nb_speed_for_stability->setMinimum(1);
    this->max_nb_speed_for_stability->setMaximum(99);
    this->max_nb_speed_for_stability->setSingleStep(1);
    motion_detect_layout->addWidget(max_nb_speed_for_stability_label, 7, 0);
    motion_detect_layout->addWidget(this->max_nb_speed_for_stability, 7, 1);
    motion_detect_layout->addWidget(this->max_nb_speed_for_stability_value, 7, 2);
    QObject::connect(this->max_nb_speed_for_stability, SIGNAL(valueChanged(int)), this, SLOT(set_max_nb_speed_for_stability_value(int)));

    QLabel *nb_cycle_before_changing_direction_label = new QLabel(tr("Number of cycles before switching to new direction:"), this);
    this->nb_cycle_before_changing_direction->setMinimum(1);
    this->nb_cycle_before_changing_direction->setMaximum(99);
    this->nb_cycle_before_changing_direction->setSingleStep(1);
    motion_detect_layout->addWidget(nb_cycle_before_changing_direction_label, 8, 0);
    motion_detect_layout->addWidget(this->nb_cycle_before_changing_direction, 8, 1);
    motion_detect_layout->addWidget(this->nb_cycle_before_changing_direction_value, 8, 2);
    QObject::connect(this->nb_cycle_before_changing_direction, SIGNAL(valueChanged(int)), this, SLOT(set_nb_cycle_before_changing_direction_value(int)));

    QPushButton *motion_params_reset_to_default = new QPushButton(this);
    motion_params_reset_to_default->setText(tr("Reset to default"));
    motion_detect_layout->addWidget(motion_params_reset_to_default, 9, 0, Qt::AlignLeft);
    QObject::connect(motion_params_reset_to_default, SIGNAL(clicked()), this, SLOT(reset_motion_detection_params()));

    motion_detect_layout->setColumnStretch(0, 0);
    motion_detect_layout->setColumnStretch(1, 30);
    motion_detect_layout->setColumnStretch(2, 5);

    // Create tab.
    QWidget *motion_detect_tab = new QWidget(this);
    motion_detect_tab->setLayout(motion_detect_layout);

    return motion_detect_tab;
}

void Config_dialog::fill_tab_motion_detect()
{
    this->autostart_detection_check->setChecked(this->settings->get_autostart_motion_detection());

    this->vinyl_type_select->setCurrentIndex(this->vinyl_type_select->findText(this->settings->get_vinyl_type()));

    this->set_extreme_min_slider(this->settings->get_extreme_min());
    this->set_extreme_min_value(this->extreme_min->value());

    this->set_amplify_coeff_slider(this->settings->get_input_amplify_coeff());
    this->set_amplify_coeff_value(this->amplify_coeff->value());

    this->set_low_pass_filter_max_speed_usage_slider(this->settings->get_low_pass_filter_max_speed_usage());
    this->set_low_pass_filter_max_speed_usage_value(this->low_pass_filter_max_speed_usage->value());

    this->set_max_speed_diff_slider(this->settings->get_max_speed_diff());
    this->set_max_speed_diff_value(this->max_speed_diff->value());

    this->set_slow_speed_algo_usage_slider(this->settings->get_slow_speed_algo_usage());
    this->set_slow_speed_algo_usage_value(this->slow_speed_algo_usage->value());

    this->set_max_nb_speed_for_stability_slider(this->settings->get_max_nb_speed_for_stability());
    this->set_max_nb_speed_for_stability_value(this->max_nb_speed_for_stability->value());

    this->set_nb_cycle_before_changing_direction_slider(this->settings->get_nb_cycle_before_changing_direction());
    this->set_nb_cycle_before_changing_direction_value(this->nb_cycle_before_changing_direction->value());
}

void Config_dialog::set_extreme_min_slider(float in_value)
{
    this->extreme_min->setValue(qRound(in_value * 100.0));
}

float Config_dialog::get_extreme_min_slider()
{
   return this->extreme_min->value() / 100.0;
}

void
Config_dialog::set_extreme_min_value(int)
{
    this->extreme_min_value->setText((new QString)->setNum(this->get_extreme_min_slider()));
}

void Config_dialog::set_amplify_coeff_slider(int in_value)
{
    this->amplify_coeff->setValue(in_value);
}

int Config_dialog::get_amplify_coeff_slider()
{
   return this->amplify_coeff->value();
}

void
Config_dialog::set_amplify_coeff_value(int)
{
    this->amplify_coeff_value->setText((new QString)->setNum(this->get_amplify_coeff_slider()));
}

void Config_dialog::set_low_pass_filter_max_speed_usage_slider(float in_value)
{
    this->low_pass_filter_max_speed_usage->setValue(qRound(in_value * 100.0));
}

float Config_dialog::get_low_pass_filter_max_speed_usage_slider()
{
   return this->low_pass_filter_max_speed_usage->value() / 100.0;
}

void
Config_dialog::set_low_pass_filter_max_speed_usage_value(int)
{
    this->low_pass_filter_max_speed_usage_value->setText((new QString)->setNum(this->get_low_pass_filter_max_speed_usage_slider()));
}

void Config_dialog::set_max_speed_diff_slider(float in_value)
{
    this->max_speed_diff->setValue(qRound(in_value * 100.0));
}

float Config_dialog::get_max_speed_diff_slider()
{
   return this->max_speed_diff->value() / 100.0;
}

void
Config_dialog::set_max_speed_diff_value(int)
{
    this->max_speed_diff_value->setText((new QString)->setNum(this->get_max_speed_diff_slider()));
}

void Config_dialog::set_slow_speed_algo_usage_slider(float in_value)
{
    this->slow_speed_algo_usage->setValue(qRound(in_value * 100.0));
}

float Config_dialog::get_slow_speed_algo_usage_slider()
{
   return this->slow_speed_algo_usage->value() / 100.0;
}

void
Config_dialog::set_slow_speed_algo_usage_value(int)
{
    this->slow_speed_algo_usage_value->setText((new QString)->setNum(this->get_slow_speed_algo_usage_slider()));
}

void Config_dialog::set_max_nb_speed_for_stability_slider(int in_value)
{
    this->max_nb_speed_for_stability->setValue(in_value);
}

int Config_dialog::get_max_nb_speed_for_stability_slider()
{
   return this->max_nb_speed_for_stability->value();
}

void
Config_dialog::set_max_nb_speed_for_stability_value(int)
{
    this->max_nb_speed_for_stability_value->setText((new QString)->setNum(this->get_max_nb_speed_for_stability_slider()));
}

void
Config_dialog::set_nb_cycle_before_changing_direction_value(int)
{
    this->nb_cycle_before_changing_direction_value->setText((new QString)->setNum(this->get_nb_cycle_before_changing_direction_slider()));
}

void Config_dialog::set_nb_cycle_before_changing_direction_slider(int in_value)
{
    this->nb_cycle_before_changing_direction->setValue(in_value);
}

int Config_dialog::get_nb_cycle_before_changing_direction_slider()
{
   return this->nb_cycle_before_changing_direction->value();
}

QWidget *Config_dialog::init_tab_shortcuts()
{
    QGridLayout *shortcuts_layout = new QGridLayout(this);

    QLabel *kb_load_track_on_deck_label = new QLabel(tr("Load track"), this);
    shortcuts_layout->addWidget(kb_load_track_on_deck_label, 1, 0);
    shortcuts_layout->addWidget(this->kb_load_track_on_deck, 1, 1, Qt::AlignVCenter);

    QLabel *kb_play_begin_track_on_deck_label = new QLabel(tr("Restart track"), this);
    shortcuts_layout->addWidget(kb_play_begin_track_on_deck_label, 2, 0);
    shortcuts_layout->addWidget(this->kb_play_begin_track_on_deck, 2, 1, Qt::AlignVCenter);

    QLabel *kb_get_next_track_from_deck_label = new QLabel(tr("Highlight next tracks"), this);
    shortcuts_layout->addWidget(kb_get_next_track_from_deck_label, 3, 0);
    shortcuts_layout->addWidget(this->kb_get_next_track_from_deck, 3, 1, Qt::AlignVCenter);

    QLabel *kb_set_cue_point1_on_deck_label = new QLabel(tr("Set cue point 1"), this);
    shortcuts_layout->addWidget(kb_set_cue_point1_on_deck_label, 4, 0);
    shortcuts_layout->addWidget(this->kb_set_cue_point1_on_deck, 4, 1, Qt::AlignVCenter);

    QLabel *kb_play_cue_point1_on_deck_label = new QLabel(tr("Play from cue point 1"), this);
    shortcuts_layout->addWidget(kb_play_cue_point1_on_deck_label, 5, 0);
    shortcuts_layout->addWidget(this->kb_play_cue_point1_on_deck, 5, 1, Qt::AlignVCenter);

    QLabel *kb_set_cue_point2_on_deck_label = new QLabel(tr("Set cue point 2"), this);
    shortcuts_layout->addWidget(kb_set_cue_point2_on_deck_label, 6, 0);
    shortcuts_layout->addWidget(this->kb_set_cue_point2_on_deck, 6, 1, Qt::AlignVCenter);

    QLabel *kb_play_cue_point2_on_deck_label = new QLabel(tr("Play from cue point 2"), this);
    shortcuts_layout->addWidget(kb_play_cue_point2_on_deck_label, 7, 0);
    shortcuts_layout->addWidget(this->kb_play_cue_point2_on_deck, 7, 1, Qt::AlignVCenter);

    QLabel *kb_set_cue_point3_on_deck_label = new QLabel(tr("Set cue point 3"), this);
    shortcuts_layout->addWidget(kb_set_cue_point3_on_deck_label, 8, 0);
    shortcuts_layout->addWidget(this->kb_set_cue_point3_on_deck, 8, 1, Qt::AlignVCenter);

    QLabel *kb_play_cue_point3_on_deck_label = new QLabel(tr("Play from cue point 3"), this);
    shortcuts_layout->addWidget(kb_play_cue_point3_on_deck_label, 9, 0);
    shortcuts_layout->addWidget(this->kb_play_cue_point3_on_deck, 9, 1, Qt::AlignVCenter);

    QLabel *kb_set_cue_point4_on_deck_label = new QLabel(tr("Set cue point 4"), this);
    shortcuts_layout->addWidget(kb_set_cue_point4_on_deck_label, 10, 0);
    shortcuts_layout->addWidget(this->kb_set_cue_point4_on_deck, 10, 1, Qt::AlignVCenter);

    QLabel *kb_play_cue_point4_on_deck_label = new QLabel(tr("Play from cue point 4"), this);
    shortcuts_layout->addWidget(kb_play_cue_point4_on_deck_label, 11, 0);
    shortcuts_layout->addWidget(this->kb_play_cue_point4_on_deck, 11, 1, Qt::AlignVCenter);

    QLabel *kb_load_track_on_sampler1_label = new QLabel(tr("Load track on sampler 1"), this);
    shortcuts_layout->addWidget(kb_load_track_on_sampler1_label, 1, 3);
    shortcuts_layout->addWidget(this->kb_load_track_on_sampler1, 1, 4, Qt::AlignVCenter);

    QLabel *kb_load_track_on_sampler2_label = new QLabel(tr("Load track on sampler 2"), this);
    shortcuts_layout->addWidget(kb_load_track_on_sampler2_label, 2, 3);
    shortcuts_layout->addWidget(this->kb_load_track_on_sampler2, 2, 4, Qt::AlignVCenter);

    QLabel *kb_load_track_on_sampler3_label = new QLabel(tr("Load track on sampler 3"), this);
    shortcuts_layout->addWidget(kb_load_track_on_sampler3_label, 3, 3);
    shortcuts_layout->addWidget(this->kb_load_track_on_sampler3, 3, 4, Qt::AlignVCenter);

    QLabel *kb_load_track_on_sampler4_label = new QLabel(tr("Load track on sampler 4"), this);
    shortcuts_layout->addWidget(kb_load_track_on_sampler4_label, 4, 3);
    shortcuts_layout->addWidget(this->kb_load_track_on_sampler4, 4, 4, Qt::AlignVCenter);

    QLabel *kb_switch_playback_label = new QLabel(tr("Switch playback"), this);
    shortcuts_layout->addWidget(kb_switch_playback_label, 6, 3);
    shortcuts_layout->addWidget(this->kb_switch_playback, 6, 4, Qt::AlignVCenter);

    QLabel *kb_fullscreen_label = new QLabel(tr("Full-screen"), this);
    shortcuts_layout->addWidget(kb_fullscreen_label, 7, 3);
    shortcuts_layout->addWidget(this->kb_fullscreen, 7, 4, Qt::AlignVCenter);

    QLabel *kb_collapse_browse_label = new QLabel(tr("Collapse file browser"), this);
    shortcuts_layout->addWidget(kb_collapse_browse_label, 8, 3);
    shortcuts_layout->addWidget(this->kb_collapse_browse, 8, 4, Qt::AlignVCenter);

    QLabel *kb_help_label = new QLabel(tr("Help"), this);
    shortcuts_layout->addWidget(kb_help_label, 9, 3);
    shortcuts_layout->addWidget(this->kb_help, 9, 4, Qt::AlignVCenter);

    QPushButton *shortcut_reset_to_default = new QPushButton(this);
    shortcut_reset_to_default->setText(tr("Reset to default"));
    shortcuts_layout->addWidget(shortcut_reset_to_default, 11, 4, Qt::AlignRight);
    QObject::connect(shortcut_reset_to_default, SIGNAL(clicked()), this, SLOT(reset_shortcuts()));

    // Force space between shortcuts columns.
    shortcuts_layout->setColumnMinimumWidth(1, 150);
    shortcuts_layout->setColumnMinimumWidth(2, 50);
    shortcuts_layout->setColumnMinimumWidth(4, 150);

    // Signal send ShortCutQLabels when a new value is there.
    QObject::connect(this->kb_switch_playback,           SIGNAL(new_value(QString)), this, SLOT(validate_and_set_shortcut(const QString&)));
    QObject::connect(this->kb_load_track_on_deck,        SIGNAL(new_value(QString)), this, SLOT(validate_and_set_shortcut(const QString&)));
    QObject::connect(this->kb_play_begin_track_on_deck,  SIGNAL(new_value(QString)), this, SLOT(validate_and_set_shortcut(const QString&)));
    QObject::connect(this->kb_get_next_track_from_deck,  SIGNAL(new_value(QString)), this, SLOT(validate_and_set_shortcut(const QString&)));
    QObject::connect(this->kb_set_cue_point1_on_deck,    SIGNAL(new_value(QString)), this, SLOT(validate_and_set_shortcut(const QString&)));
    QObject::connect(this->kb_play_cue_point1_on_deck,   SIGNAL(new_value(QString)), this, SLOT(validate_and_set_shortcut(const QString&)));
    QObject::connect(this->kb_set_cue_point2_on_deck,    SIGNAL(new_value(QString)), this, SLOT(validate_and_set_shortcut(const QString&)));
    QObject::connect(this->kb_play_cue_point2_on_deck,   SIGNAL(new_value(QString)), this, SLOT(validate_and_set_shortcut(const QString&)));
    QObject::connect(this->kb_set_cue_point3_on_deck,    SIGNAL(new_value(QString)), this, SLOT(validate_and_set_shortcut(const QString&)));
    QObject::connect(this->kb_play_cue_point3_on_deck,   SIGNAL(new_value(QString)), this, SLOT(validate_and_set_shortcut(const QString&)));
    QObject::connect(this->kb_set_cue_point4_on_deck,    SIGNAL(new_value(QString)), this, SLOT(validate_and_set_shortcut(const QString&)));
    QObject::connect(this->kb_play_cue_point4_on_deck,   SIGNAL(new_value(QString)), this, SLOT(validate_and_set_shortcut(const QString&)));
    QObject::connect(this->kb_fullscreen,                SIGNAL(new_value(QString)), this, SLOT(validate_and_set_shortcut(const QString&)));
    QObject::connect(this->kb_collapse_browse,           SIGNAL(new_value(QString)), this, SLOT(validate_and_set_shortcut(const QString&)));
    QObject::connect(this->kb_load_track_on_sampler1,    SIGNAL(new_value(QString)), this, SLOT(validate_and_set_shortcut(const QString&)));
    QObject::connect(this->kb_load_track_on_sampler2,    SIGNAL(new_value(QString)), this, SLOT(validate_and_set_shortcut(const QString&)));
    QObject::connect(this->kb_load_track_on_sampler3,    SIGNAL(new_value(QString)), this, SLOT(validate_and_set_shortcut(const QString&)));
    QObject::connect(this->kb_load_track_on_sampler4,    SIGNAL(new_value(QString)), this, SLOT(validate_and_set_shortcut(const QString&)));
    QObject::connect(this->kb_help,                      SIGNAL(new_value(QString)), this, SLOT(validate_and_set_shortcut(const QString&)));

    // Create tab.
    QWidget *shortcuts_tab = new QWidget(this);
    shortcuts_tab->setLayout(shortcuts_layout);

    return shortcuts_tab;
}

bool Config_dialog::is_duplicate_shortcut(const QString& in_value)
{
    // For each shortcut label, check if the provided shortcut already exists.
    if ((this->kb_switch_playback->text().compare(in_value,          Qt::CaseInsensitive) == 0) ||
        (this->kb_load_track_on_deck->text().compare(in_value,       Qt::CaseInsensitive) == 0) ||
        (this->kb_play_begin_track_on_deck->text().compare(in_value, Qt::CaseInsensitive) == 0) ||
        (this->kb_get_next_track_from_deck->text().compare(in_value, Qt::CaseInsensitive) == 0) ||
        (this->kb_set_cue_point1_on_deck->text().compare(in_value,   Qt::CaseInsensitive) == 0) ||
        (this->kb_play_cue_point1_on_deck->text().compare(in_value,  Qt::CaseInsensitive) == 0) ||
        (this->kb_set_cue_point2_on_deck->text().compare(in_value,   Qt::CaseInsensitive) == 0) ||
        (this->kb_play_cue_point2_on_deck->text().compare(in_value,  Qt::CaseInsensitive) == 0) ||
        (this->kb_set_cue_point3_on_deck->text().compare(in_value,   Qt::CaseInsensitive) == 0) ||
        (this->kb_play_cue_point3_on_deck->text().compare(in_value,  Qt::CaseInsensitive) == 0) ||
        (this->kb_set_cue_point4_on_deck->text().compare(in_value,   Qt::CaseInsensitive) == 0) ||
        (this->kb_play_cue_point4_on_deck->text().compare(in_value,  Qt::CaseInsensitive) == 0) ||
        (this->kb_fullscreen->text().compare(in_value,               Qt::CaseInsensitive) == 0) ||
        (this->kb_load_track_on_sampler1->text().compare(in_value,   Qt::CaseInsensitive) == 0) ||
        (this->kb_load_track_on_sampler2->text().compare(in_value,   Qt::CaseInsensitive) == 0) ||
        (this->kb_load_track_on_sampler3->text().compare(in_value,   Qt::CaseInsensitive) == 0) ||
        (this->kb_load_track_on_sampler4->text().compare(in_value,   Qt::CaseInsensitive) == 0) ||
        (this->kb_help->text().compare(in_value,                     Qt::CaseInsensitive) == 0))
    {
        return true;
    }

    return false;
}

void Config_dialog::validate_and_set_shortcut(const QString& in_value)
{
    // Get keyboard shortcut label to work on.
    QObject *sender_obj = sender();
    ShortcutQLabel *label = qobject_cast<ShortcutQLabel*>(sender_obj);
    if(label != 0)
    {
        // Check if new shortcut is duplicate.
        if (this->is_duplicate_shortcut(in_value) == true)
        {
            // Shortcut already exists, show an error and revert label.
            QMessageBox msg_box;
            msg_box.setWindowTitle("DigitalScratch");
            msg_box.setText("<h2>" + tr("Error") + "</h2>"
                            + "<br/>"
                            + "Keyboard shortcut [ " + in_value  + " ] already exists, please set another one.");
            msg_box.setStandardButtons(QMessageBox::Close);
            msg_box.setIcon(QMessageBox::Warning);
            msg_box.setStyleSheet(Utils::get_current_stylesheet_css());
            msg_box.setWindowIcon(QIcon(ICON_2));
            msg_box.exec();

            // Then put back the old shortcut.
            label->set_old_text();
        }
        else
        {
            // Shortcut is correct, set it in the label.
            label->setText(in_value);
        }
    }
}

void Config_dialog::fill_tab_shortcuts()
{
    this->kb_switch_playback->setText(this->settings->get_keyboard_shortcut(KB_SWITCH_PLAYBACK));
    this->kb_load_track_on_deck->setText(this->settings->get_keyboard_shortcut(KB_LOAD_TRACK_ON_DECK));
    this->kb_play_begin_track_on_deck->setText(this->settings->get_keyboard_shortcut(KB_PLAY_BEGIN_TRACK_ON_DECK));
    this->kb_get_next_track_from_deck->setText(this->settings->get_keyboard_shortcut(KB_GET_NEXT_TRACK_FROM_DECK));
    this->kb_set_cue_point1_on_deck->setText(this->settings->get_keyboard_shortcut(KB_SET_CUE_POINT1_ON_DECK));
    this->kb_play_cue_point1_on_deck->setText(this->settings->get_keyboard_shortcut(KB_PLAY_CUE_POINT1_ON_DECK));
    this->kb_set_cue_point2_on_deck->setText(this->settings->get_keyboard_shortcut(KB_SET_CUE_POINT2_ON_DECK));
    this->kb_play_cue_point2_on_deck->setText(this->settings->get_keyboard_shortcut(KB_PLAY_CUE_POINT2_ON_DECK));
    this->kb_set_cue_point3_on_deck->setText(this->settings->get_keyboard_shortcut(KB_SET_CUE_POINT3_ON_DECK));
    this->kb_play_cue_point3_on_deck->setText(this->settings->get_keyboard_shortcut(KB_PLAY_CUE_POINT3_ON_DECK));
    this->kb_set_cue_point4_on_deck->setText(this->settings->get_keyboard_shortcut(KB_SET_CUE_POINT4_ON_DECK));
    this->kb_play_cue_point4_on_deck->setText(this->settings->get_keyboard_shortcut(KB_PLAY_CUE_POINT4_ON_DECK));
    this->kb_fullscreen->setText(this->settings->get_keyboard_shortcut(KB_FULLSCREEN));
    this->kb_collapse_browse->setText(this->settings->get_keyboard_shortcut(KB_COLLAPSE_BROWSER));
    this->kb_load_track_on_sampler1->setText(this->settings->get_keyboard_shortcut(KB_LOAD_TRACK_ON_SAMPLER1));
    this->kb_load_track_on_sampler2->setText(this->settings->get_keyboard_shortcut(KB_LOAD_TRACK_ON_SAMPLER2));
    this->kb_load_track_on_sampler3->setText(this->settings->get_keyboard_shortcut(KB_LOAD_TRACK_ON_SAMPLER3));
    this->kb_load_track_on_sampler4->setText(this->settings->get_keyboard_shortcut(KB_LOAD_TRACK_ON_SAMPLER4));
    this->kb_help->setText(this->settings->get_keyboard_shortcut(KB_HELP));
}

bool
Config_dialog::show_browse_window()
{
    // Create browse window.
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    tr("Select music directory"),
                                                    this->settings->get_tracks_base_dir_path(),
                                                    QFileDialog::ShowDirsOnly);

    // Update base directory path.
    if (dir != NULL)
    {
        this->base_dir_path->setText(dir);
    }

    return true;
}

void Config_dialog::reset_motion_detection_params()
{
    // Reset all motion detection parameters to their default values.
    this->autostart_detection_check->setChecked(this->settings->get_autostart_motion_detection_default());
    this->vinyl_type_select->setCurrentIndex(this->vinyl_type_select->findText(this->settings->get_vinyl_type_default()));
    this->set_extreme_min_slider(this->settings->get_extreme_min_default());
    this->set_amplify_coeff_slider(this->settings->get_input_amplify_coeff_default());
    this->set_low_pass_filter_max_speed_usage_slider(this->settings->get_low_pass_filter_max_speed_usage_default());
    this->set_max_speed_diff_slider(this->settings->get_max_speed_diff_default());
    this->set_slow_speed_algo_usage_slider(this->settings->get_slow_speed_algo_usage_default());
    this->set_max_nb_speed_for_stability_slider(this->settings->get_max_nb_speed_for_stability_default());
    this->set_nb_cycle_before_changing_direction_slider(this->settings->get_nb_cycle_before_changing_direction_default());
}

void Config_dialog::reset_shortcuts()
{
    // Reset all keyboard shortcuts to their default values.
    this->kb_switch_playback->setText(KB_SWITCH_PLAYBACK_DEFAULT);
    this->kb_load_track_on_deck->setText(KB_LOAD_TRACK_ON_DECK_DEFAULT);
    this->kb_play_begin_track_on_deck->setText(KB_PLAY_BEGIN_TRACK_ON_DECK_DEFAULT);
    this->kb_get_next_track_from_deck->setText(KB_GET_NEXT_TRACK_FROM_DECK_DEFAULT);
    this->kb_set_cue_point1_on_deck->setText(KB_SET_CUE_POINT1_ON_DECK_DEFAULT);
    this->kb_play_cue_point1_on_deck->setText(KB_PLAY_CUE_POINT1_ON_DECK_DEFAULT);
    this->kb_set_cue_point2_on_deck->setText(KB_SET_CUE_POINT2_ON_DECK_DEFAULT);
    this->kb_play_cue_point2_on_deck->setText(KB_PLAY_CUE_POINT2_ON_DECK_DEFAULT);
    this->kb_set_cue_point3_on_deck->setText(KB_SET_CUE_POINT3_ON_DECK_DEFAULT);
    this->kb_play_cue_point3_on_deck->setText(KB_PLAY_CUE_POINT3_ON_DECK_DEFAULT);
    this->kb_set_cue_point4_on_deck->setText(KB_SET_CUE_POINT4_ON_DECK_DEFAULT);
    this->kb_play_cue_point4_on_deck->setText(KB_PLAY_CUE_POINT4_ON_DECK_DEFAULT);
    this->kb_fullscreen->setText(KB_FULLSCREEN_DEFAULT);
    this->kb_collapse_browse->setText(KB_COLLAPSE_BROWSER_DEFAULT);
    this->kb_load_track_on_sampler1->setText(KB_LOAD_TRACK_ON_SAMPLER1_DEFAULT);
    this->kb_load_track_on_sampler2->setText(KB_LOAD_TRACK_ON_SAMPLER2_DEFAULT);
    this->kb_load_track_on_sampler3->setText(KB_LOAD_TRACK_ON_SAMPLER3_DEFAULT);
    this->kb_load_track_on_sampler4->setText(KB_LOAD_TRACK_ON_SAMPLER4_DEFAULT);
    this->kb_help->setText(KB_HELP_DEFAULT);
}

void
Config_dialog::accept()
{
    //
    // Set all settings in configuration file if values are correct.
    //

    // Set base directory for track browser.
    if (QDir(this->base_dir_path->text()).exists() == true)
    {
        this->settings->set_tracks_base_dir_path(this->base_dir_path->text());
    }

    // Set gui style.
    this->settings->set_gui_style(this->gui_style_select->currentText());

    // Set autostart motion detection at startup.
    this->settings->set_autostart_motion_detection(this->autostart_detection_check->isChecked());

    // Set vinyl type.
    this->settings->set_vinyl_type(this->vinyl_type_select->currentText());

    // Set sound card settings.
    this->settings->set_sample_rate(this->sample_rate_select->currentText().toInt());

    // Set auto JACK port connections.
    this->settings->set_auto_jack_connections(this->auto_jack_connections_check->isChecked());

    // Set motion detection settings.
    this->settings->set_extreme_min(this->get_extreme_min_slider());
    this->settings->set_input_amplify_coeff(this->get_amplify_coeff_slider());
    this->settings->set_low_pass_filter_max_speed_usage(this->get_low_pass_filter_max_speed_usage_slider());
    this->settings->set_max_speed_diff(this->get_max_speed_diff_slider());
    this->settings->set_slow_speed_algo_usage(this->get_slow_speed_algo_usage_slider());
    this->settings->set_max_nb_speed_for_stability(this->get_max_nb_speed_for_stability_slider());
    this->settings->set_nb_cycle_before_changing_direction(this->get_nb_cycle_before_changing_direction_slider());

    // Set keyboard shortcuts.
    this->settings->set_keyboard_shortcut(KB_SWITCH_PLAYBACK,           this->kb_switch_playback->text());
    this->settings->set_keyboard_shortcut(KB_LOAD_TRACK_ON_DECK,        this->kb_load_track_on_deck->text());
    this->settings->set_keyboard_shortcut(KB_PLAY_BEGIN_TRACK_ON_DECK,  this->kb_play_begin_track_on_deck->text());
    this->settings->set_keyboard_shortcut(KB_GET_NEXT_TRACK_FROM_DECK,  this->kb_get_next_track_from_deck->text());
    this->settings->set_keyboard_shortcut(KB_SET_CUE_POINT1_ON_DECK,    this->kb_set_cue_point1_on_deck->text());
    this->settings->set_keyboard_shortcut(KB_PLAY_CUE_POINT1_ON_DECK,   this->kb_play_cue_point1_on_deck->text());
    this->settings->set_keyboard_shortcut(KB_SET_CUE_POINT2_ON_DECK,    this->kb_set_cue_point2_on_deck->text());
    this->settings->set_keyboard_shortcut(KB_PLAY_CUE_POINT2_ON_DECK,   this->kb_play_cue_point2_on_deck->text());
    this->settings->set_keyboard_shortcut(KB_SET_CUE_POINT3_ON_DECK,    this->kb_set_cue_point3_on_deck->text());
    this->settings->set_keyboard_shortcut(KB_PLAY_CUE_POINT3_ON_DECK,   this->kb_play_cue_point3_on_deck->text());
    this->settings->set_keyboard_shortcut(KB_SET_CUE_POINT4_ON_DECK,    this->kb_set_cue_point4_on_deck->text());
    this->settings->set_keyboard_shortcut(KB_PLAY_CUE_POINT4_ON_DECK,   this->kb_play_cue_point4_on_deck->text());
    this->settings->set_keyboard_shortcut(KB_FULLSCREEN,                this->kb_fullscreen->text());
    this->settings->set_keyboard_shortcut(KB_COLLAPSE_BROWSER,          this->kb_collapse_browse->text());
    this->settings->set_keyboard_shortcut(KB_LOAD_TRACK_ON_SAMPLER1,    this->kb_load_track_on_sampler1->text());
    this->settings->set_keyboard_shortcut(KB_LOAD_TRACK_ON_SAMPLER2,    this->kb_load_track_on_sampler2->text());
    this->settings->set_keyboard_shortcut(KB_LOAD_TRACK_ON_SAMPLER3,    this->kb_load_track_on_sampler3->text());
    this->settings->set_keyboard_shortcut(KB_LOAD_TRACK_ON_SAMPLER4,    this->kb_load_track_on_sampler4->text());
    this->settings->set_keyboard_shortcut(KB_HELP,                      this->kb_help->text());

    // Close window.
    this->done(QDialog::Accepted);
}

void
Config_dialog::reject()
{
    // Close window.
    this->done(QDialog::Rejected);
}

ShortcutQLabel::ShortcutQLabel(QWidget *parent) : QLabel(parent)
{
    qDebug() << "ShortcutQLabel::ShortcutQLabel: create object...";

    this->capturing = false;

    qDebug() << "ShortcutQLabel::ShortcutQLabel: create object done.";

    return;
}

ShortcutQLabel::~ShortcutQLabel()
{
    qDebug() << "ShortcutQLabel::ShortcutQLabel: delete object...";

    qDebug() << "ShortcutQLabel::ShortcutQLabel: delete object done.";

    return;
}

void
ShortcutQLabel::set_old_text()
{
    this->setText(this->old_text);
}

void
ShortcutQLabel::start_capture()
{
    // Store current text to get it back if cancel.
    this->old_text = this->text();

    // Ask to press a key.
    this->setText(tr("Press a key or Esc to cancel"));

    // Capture keyboard.
    this->capturing = true;
    this->grabKeyboard();
    this->grabMouse();

    qDebug() << "ShortcutQLabel::start_capture: capturing keyboard... ";

    return;
}

void
ShortcutQLabel::cancel_capture()
{
    this->capturing = false;
    this->releaseKeyboard();
    this->releaseMouse();

    // Keep previous shortcut.
    this->set_old_text();

    qDebug() << "ShortcutQLabel::cancel_capture: keyboard capture canceled";

    return;
}

void
ShortcutQLabel::finish_capture(int in_key)
{
    this->capturing = false;
    this->releaseKeyboard();
    this->releaseMouse();

    // Send an event to main config tab to check if this shortcut is not a duplicate.
    emit this->new_value(QKeySequence(in_key).toString(QKeySequence::NativeText));

    qDebug() << "ShortcutQLabel::cancel_capture: keyboard capture done with " << QKeySequence(in_key).toString(QKeySequence::NativeText);

    return;
}

void
ShortcutQLabel::mousePressEvent(QMouseEvent *in_mouse_event)

{
    qDebug() << "ShortcutQLabel::mousePressEvent: x=" << in_mouse_event->x() << "  capturing keyboard... ";

    if (this->capturing == false)
    {
        this->start_capture();
    }
    else
    {
        // Was capturing, user clicked some where, so cancel capture.
        this->cancel_capture();
    }
}


void
ShortcutQLabel::keyPressEvent(QKeyEvent *in_key_event)
{
    if (this->capturing == true)
    {
        int keyInt = in_key_event->key();
        Qt::Key key = static_cast<Qt::Key>(keyInt);

        if (key == Qt::Key_Escape)
        {
            // Cancel capture.
            this->cancel_capture();
        }
        else
        {
            if (key != Qt::Key_unknown &&
                key != Qt::Key_Control &&
                key != Qt::Key_Shift &&
                key != Qt::Key_Alt &&
                key != Qt::Key_Meta)
            {
                // check for a combination of user clicks
                Qt::KeyboardModifiers modifiers = in_key_event->modifiers();
                if(modifiers & Qt::ShiftModifier)
                    keyInt += Qt::SHIFT;
                if(modifiers & Qt::ControlModifier)
                    keyInt += Qt::CTRL;
                if(modifiers & Qt::AltModifier)
                    keyInt += Qt::ALT;
                if(modifiers & Qt::MetaModifier)
                    keyInt += Qt::META;

                qDebug() << "ShortcutQLabel::keyPressEvent: Keysequence:" << QKeySequence(keyInt).toString(QKeySequence::NativeText);

                // Stop capture and show key sequence.
                this->finish_capture(keyInt);
            }
        }
    }
}
