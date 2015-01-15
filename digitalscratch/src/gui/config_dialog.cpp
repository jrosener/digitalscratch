/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*------------------------------------------------------( config_dialog.cpp )-*/
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
#include <QSizePolicy>
#include <QButtonGroup>
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
    this->extern_prog      = new QLineEdit(this);
    this->gui_style_select = new QComboBox(this);
    QList<QString> available_gui_styles = this->settings->get_available_gui_styles();
    for (int i = 0; i < available_gui_styles.size(); i++)
    {
        this->gui_style_select->addItem(available_gui_styles.at(i));
    }
    this->nb_decks_select = new QComboBox(this);
    QList<unsigned short int> available_nb_decks = this->settings->get_available_nb_decks();
    for (int i = 0; i < available_nb_decks.size(); i++)
    {
        this->nb_decks_select->addItem(QString::number(available_nb_decks.at(i)));
    }

    // Init sound card parameters widgets.
    this->sample_rate_select = new QComboBox(this);
    QList<unsigned int> available_sample_rates = this->settings->get_available_sample_rates();
    for (int i = 0; i < available_sample_rates.size(); i++)
    {
        this->sample_rate_select->addItem(QString::number(available_sample_rates.at(i)));
    }
    this->device_jack_check = new QCheckBox(this);
    this->device_jack_check->setTristate(false);
    this->auto_jack_connections_check = new QCheckBox(this);
    this->auto_jack_connections_check->setTristate(false);
    this->device_internal_check = new QCheckBox(this);
    this->device_internal_check->setTristate(false);
    this->device_internal_select = new QComboBox(this);
    QList<QString> available_sound_cards = this->settings->get_available_internal_sound_cards();
    for (int i = 0; i < available_sound_cards.size(); i++)
    {
        this->device_internal_select->addItem(available_sound_cards.at(i));
    }

    // Init motion detection parameters widgets.
    this->amplify_coeff                            = new QSlider(Qt::Horizontal, this);
    this->amplify_coeff_value                      = new QLabel(this);
    this->min_amplitude_for_normal_speed           = new QSlider(Qt::Horizontal, this);
    this->min_amplitude_for_normal_speed_value     = new QLabel(this);
    this->min_amplitude                            = new QSlider(Qt::Horizontal, this);
    this->min_amplitude_value                      = new QLabel(this);
    this->vinyl_type_select                        = new QComboBox(this);
    QList<QString> available_vinyl_types           = this->settings->get_available_vinyl_types();
    for (int i = 0; i < available_vinyl_types.size(); i++)
    {
        this->vinyl_type_select->addItem(available_vinyl_types.at(i));
    }
    this->rpm_select                               = new QComboBox(this);
    QList<unsigned short int> available_rpms       = this->settings->get_available_rpms();
    for (int i = 0; i < available_rpms.size(); i++)
    {
        this->rpm_select->addItem(QString::number(available_rpms.at(i)));
    }
    this->autostart_detection_check = new QCheckBox(this);
    this->autostart_detection_check->setTristate(false);

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
    this->kb_file_search               = new ShortcutQLabel(this);
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
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, [this](){this->accept();});
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, [this](){this->reject();});

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
    QObject::connect(base_dir_button, &QPushButton::clicked, [this](){this->show_browse_window();});

    // Player tab: select GUI style.
    QLabel *gui_style_label = new QLabel(tr("GUI style: "), this);

    // Player tab: select number of decks.
    QLabel *nb_decks_label = new QLabel(tr("Number of decks (restart required): "), this);

    // Run external prog at startup.
    QLabel *extern_prog_label = new QLabel(tr("External prog to run at startup: "), this);
    this->extern_prog->setMinimumWidth(300);
    QPushButton *extern_prog_button = new QPushButton(tr("Browse..."), this);
    QObject::connect(extern_prog_button, &QPushButton::clicked, [this](){this->show_browse_extern_prog_window();});

    // Player tab: setup layout.
    QGridLayout *player_tab_layout = new QGridLayout(this);
    player_tab_layout->addWidget(base_dir_label,         0, 0);
    player_tab_layout->addWidget(this->base_dir_path,    0, 1);
    player_tab_layout->addWidget(base_dir_button,        0, 2);
    player_tab_layout->addWidget(gui_style_label,        1, 0);
    player_tab_layout->addWidget(this->gui_style_select, 1, 1);
    player_tab_layout->addWidget(nb_decks_label,         2, 0);
    player_tab_layout->addWidget(this->nb_decks_select,  2, 1);
    player_tab_layout->addWidget(extern_prog_label,      3, 0);
    player_tab_layout->addWidget(this->extern_prog,      3, 1);
    player_tab_layout->addWidget(extern_prog_button,     3, 2);

    // Create tab.
    QWidget *player_tab = new QWidget(this);
    player_tab->setLayout(player_tab_layout);

    return player_tab;
}

void Config_dialog::fill_tab_player()
{
    this->base_dir_path->setText(this->settings->get_tracks_base_dir_path());
    this->gui_style_select->setCurrentIndex(this->gui_style_select->findText(this->settings->get_gui_style()));
    this->nb_decks_select->setCurrentIndex(this->nb_decks_select->findText(QString::number(this->settings->get_nb_decks())));
    this->extern_prog->setText(this->settings->get_extern_prog());
}

QWidget *Config_dialog::init_tab_sound_card()
{
    // Sound card tab: setup main layout.
    QVBoxLayout *sound_card_layout = new QVBoxLayout(this);

    // Select sample rate.
    QLabel *sample_rate_label = new QLabel(tr("Sample rate (restart required): "), this);
    QHBoxLayout *sample_rate_layout = new QHBoxLayout();
    sample_rate_label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sample_rate_layout->addWidget(sample_rate_label, 0, Qt::AlignLeft);
    sample_rate_layout->addWidget(this->sample_rate_select, 10, Qt::AlignLeft);
    sample_rate_layout->addStretch(10);
    sound_card_layout->addLayout(sample_rate_layout);

    // Select sound device.
    QGridLayout *device_layout = new QGridLayout();
    device_layout->setColumnStretch(3, 10);
    sound_card_layout->addLayout(device_layout);
    QLabel *device_label = new QLabel(tr("Sound device access: "), this);
    device_label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    device_layout->addWidget(device_label, 0, 0, Qt::AlignLeft);

    // Select sound device : choice 1 (jack).
    this->device_jack_check->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    device_layout->addWidget(this->device_jack_check, 0, 1, Qt::AlignLeft);
    QLabel *jack_label = new QLabel(tr("JACK"), this);
    jack_label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    device_layout->addWidget(jack_label, 0, 2, Qt::AlignLeft);
    QLabel *auto_jack_connections_label = new QLabel(tr("Connect automatically JACK ports: "), this);
    auto_jack_connections_label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    device_layout->addWidget(auto_jack_connections_label, 1, 2, Qt::AlignLeft);
    this->auto_jack_connections_check->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    device_layout->addWidget(this->auto_jack_connections_check, 1, 3, Qt::AlignLeft);

    // Select sound device : choice 2 (internal).
    this->device_internal_check->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    device_layout->addWidget(device_internal_check, 2, 1, Qt::AlignLeft);
    QLabel *internal_label = new QLabel(tr("Internal"), this);
    internal_label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    device_layout->addWidget(internal_label, 2, 2, Qt::AlignLeft);
    device_internal_select->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    device_layout->addWidget(device_internal_select, 3, 2, Qt::AlignLeft);

    // Make device choices exclusive.
    QButtonGroup *device_choices = new QButtonGroup(this);
    device_choices->addButton(this->device_jack_check);
    device_choices->addButton(this->device_internal_check);
    device_choices->setExclusive(true);

    // Create tab.
    QWidget *soundcard_tab = new QWidget(this);
    soundcard_tab->setLayout(sound_card_layout);

    return soundcard_tab;
}

void Config_dialog::fill_tab_sound_card()
{
    this->sample_rate_select->setCurrentIndex(this->sample_rate_select->findText(QString::number(this->settings->get_sample_rate())));
    this->auto_jack_connections_check->setChecked(this->settings->get_auto_jack_connections());
    if (this->settings->get_sound_driver() == SOUND_DRIVER_INTERNAL)
    {
        this->device_internal_check->setChecked(true);
    }
    else
    {
        this->device_jack_check->setChecked(true);
    }
    this->device_internal_select->setCurrentIndex(this->device_internal_select->findText(this->settings->get_internal_sound_card()));
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

    QLabel *rpm_label = new QLabel(tr("RPM: "), this);
    motion_detect_layout->addWidget(rpm_label,        2, 0);
    motion_detect_layout->addWidget(this->rpm_select, 2, 1);

    QLabel *amplify_coeff_label = new QLabel(tr("Amplification factor for input signal:"), this);
    this->amplify_coeff->setMinimum(1);
    this->amplify_coeff->setMaximum(20);
    this->amplify_coeff->setSingleStep(1);
    motion_detect_layout->addWidget(amplify_coeff_label, 3, 0);
    motion_detect_layout->addWidget(this->amplify_coeff, 3, 1);
    motion_detect_layout->addWidget(this->amplify_coeff_value, 3, 2);
    QObject::connect(this->amplify_coeff, &QSlider::valueChanged, [this](int value){this->set_amplify_coeff_value(value);});

    QLabel *min_amplitude_for_normal_speed_label = new QLabel(tr("Minimal signal amplitude @ speed = 100%:"), this);
    this->min_amplitude_for_normal_speed->setMinimum(1);
    this->min_amplitude_for_normal_speed->setMaximum(99);
    this->min_amplitude_for_normal_speed->setSingleStep(1);
    motion_detect_layout->addWidget(min_amplitude_for_normal_speed_label, 4, 0);
    motion_detect_layout->addWidget(this->min_amplitude_for_normal_speed, 4, 1);
    motion_detect_layout->addWidget(this->min_amplitude_for_normal_speed_value, 4, 2);
    QObject::connect(this->min_amplitude_for_normal_speed, &QSlider::valueChanged, [this](int value){this->set_min_amplitude_for_normal_speed_value(value);});

    QLabel *min_amplitude_label = new QLabel(tr("Minimal signal amplitude:"), this);
    this->min_amplitude->setMinimum(1);
    this->min_amplitude->setMaximum(999);
    this->min_amplitude->setSingleStep(1);
    motion_detect_layout->addWidget(min_amplitude_label, 5, 0);
    motion_detect_layout->addWidget(this->min_amplitude, 5, 1);
    motion_detect_layout->addWidget(this->min_amplitude_value, 5, 2);
    QObject::connect(this->min_amplitude, &QSlider::valueChanged, [this](int value){this->set_min_amplitude_value(value);});

    QPushButton *motion_params_reset_to_default = new QPushButton(this);
    motion_params_reset_to_default->setText(tr("Reset to default"));
    motion_detect_layout->addWidget(motion_params_reset_to_default, 10, 0, Qt::AlignLeft);
    QObject::connect(motion_params_reset_to_default, &QPushButton::clicked, [this](){this->reset_motion_detection_params();});

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

    this->rpm_select->setCurrentIndex(this->rpm_select->findText(QString::number(this->settings->get_rpm())));

    this->set_amplify_coeff_slider(this->settings->get_input_amplify_coeff());
    this->set_amplify_coeff_value(this->amplify_coeff->value());

    this->set_min_amplitude_for_normal_speed_slider(this->settings->get_min_amplitude_for_normal_speed());
    this->set_min_amplitude_for_normal_speed_value(this->min_amplitude_for_normal_speed->value());

    this->set_min_amplitude_slider(this->settings->get_min_amplitude());
    this->set_min_amplitude_value(this->min_amplitude->value());
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
    this->amplify_coeff_value->setText(QString::number(this->get_amplify_coeff_slider()));
}

void Config_dialog::set_min_amplitude_for_normal_speed_slider(float in_value)
{
    this->min_amplitude_for_normal_speed->setValue(qRound(in_value * 100.0));
}

float Config_dialog::get_min_amplitude_for_normal_speed_slider()
{
   return this->min_amplitude_for_normal_speed->value() / 100.0;
}

void
Config_dialog::set_min_amplitude_for_normal_speed_value(int)
{
    this->min_amplitude_for_normal_speed_value->setText(QString::number(this->get_min_amplitude_for_normal_speed_slider()));
}

void Config_dialog::set_min_amplitude_slider(float in_value)
{
    this->min_amplitude->setValue(qRound(in_value * 1000.0));
}

float Config_dialog::get_min_amplitude_slider()
{
   return this->min_amplitude->value() / 1000.0;
}

void
Config_dialog::set_min_amplitude_value(int)
{
    this->min_amplitude_value->setText(QString::number(this->get_min_amplitude_slider()));
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

    QLabel *kb_file_search_label = new QLabel(tr("File search"), this);
    shortcuts_layout->addWidget(kb_file_search_label, 10, 3);
    shortcuts_layout->addWidget(this->kb_file_search, 10, 4, Qt::AlignVCenter);

    QPushButton *shortcut_reset_to_default = new QPushButton(this);
    shortcut_reset_to_default->setText(tr("Reset to default"));
    shortcuts_layout->addWidget(shortcut_reset_to_default, 11, 4, Qt::AlignRight);
    QObject::connect(shortcut_reset_to_default, &QPushButton::clicked, [this](){this->reset_shortcuts();});

    // Force space between shortcuts columns.
    shortcuts_layout->setColumnMinimumWidth(1, 150);
    shortcuts_layout->setColumnMinimumWidth(2, 50);
    shortcuts_layout->setColumnMinimumWidth(4, 150);

    // Signal send ShortCutQLabels when a new value is there.
    QObject::connect(this->kb_switch_playback,          &ShortcutQLabel::new_value, [this](QString in_value){this->validate_and_set_shortcut(in_value, this->kb_switch_playback);});
    QObject::connect(this->kb_load_track_on_deck,       &ShortcutQLabel::new_value, [this](QString in_value){this->validate_and_set_shortcut(in_value, this->kb_load_track_on_deck);});
    QObject::connect(this->kb_play_begin_track_on_deck, &ShortcutQLabel::new_value, [this](QString in_value){this->validate_and_set_shortcut(in_value, this->kb_play_begin_track_on_deck);});
    QObject::connect(this->kb_get_next_track_from_deck, &ShortcutQLabel::new_value, [this](QString in_value){this->validate_and_set_shortcut(in_value, this->kb_get_next_track_from_deck);});
    QObject::connect(this->kb_set_cue_point1_on_deck,   &ShortcutQLabel::new_value, [this](QString in_value){this->validate_and_set_shortcut(in_value, this->kb_set_cue_point1_on_deck);});
    QObject::connect(this->kb_play_cue_point1_on_deck,  &ShortcutQLabel::new_value, [this](QString in_value){this->validate_and_set_shortcut(in_value, this->kb_play_cue_point1_on_deck);});
    QObject::connect(this->kb_set_cue_point2_on_deck,   &ShortcutQLabel::new_value, [this](QString in_value){this->validate_and_set_shortcut(in_value, this->kb_set_cue_point2_on_deck);});
    QObject::connect(this->kb_play_cue_point2_on_deck,  &ShortcutQLabel::new_value, [this](QString in_value){this->validate_and_set_shortcut(in_value, this->kb_play_cue_point2_on_deck);});
    QObject::connect(this->kb_set_cue_point3_on_deck,   &ShortcutQLabel::new_value, [this](QString in_value){this->validate_and_set_shortcut(in_value, this->kb_set_cue_point3_on_deck);});
    QObject::connect(this->kb_play_cue_point3_on_deck,  &ShortcutQLabel::new_value, [this](QString in_value){this->validate_and_set_shortcut(in_value, this->kb_play_cue_point3_on_deck);});
    QObject::connect(this->kb_set_cue_point4_on_deck,   &ShortcutQLabel::new_value, [this](QString in_value){this->validate_and_set_shortcut(in_value, this->kb_set_cue_point4_on_deck);});
    QObject::connect(this->kb_play_cue_point4_on_deck,  &ShortcutQLabel::new_value, [this](QString in_value){this->validate_and_set_shortcut(in_value, this->kb_play_cue_point4_on_deck);});
    QObject::connect(this->kb_fullscreen,               &ShortcutQLabel::new_value, [this](QString in_value){this->validate_and_set_shortcut(in_value, this->kb_fullscreen);});
    QObject::connect(this->kb_collapse_browse,          &ShortcutQLabel::new_value, [this](QString in_value){this->validate_and_set_shortcut(in_value, this->kb_collapse_browse);});
    QObject::connect(this->kb_load_track_on_sampler1,   &ShortcutQLabel::new_value, [this](QString in_value){this->validate_and_set_shortcut(in_value, this->kb_load_track_on_sampler1);});
    QObject::connect(this->kb_load_track_on_sampler2,   &ShortcutQLabel::new_value, [this](QString in_value){this->validate_and_set_shortcut(in_value, this->kb_load_track_on_sampler2);});
    QObject::connect(this->kb_load_track_on_sampler3,   &ShortcutQLabel::new_value, [this](QString in_value){this->validate_and_set_shortcut(in_value, this->kb_load_track_on_sampler3);});
    QObject::connect(this->kb_load_track_on_sampler4,   &ShortcutQLabel::new_value, [this](QString in_value){this->validate_and_set_shortcut(in_value, this->kb_load_track_on_sampler4);});
    QObject::connect(this->kb_help,                     &ShortcutQLabel::new_value, [this](QString in_value){this->validate_and_set_shortcut(in_value, this->kb_help);});
    QObject::connect(this->kb_file_search,              &ShortcutQLabel::new_value, [this](QString in_value){this->validate_and_set_shortcut(in_value, this->kb_file_search);});

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
        (this->kb_help->text().compare(in_value,                     Qt::CaseInsensitive) == 0) ||
        (this->kb_file_search->text().compare(in_value,              Qt::CaseInsensitive) == 0))
    {
        return true;
    }

    return false;
}

void Config_dialog::validate_and_set_shortcut(const QString& in_value, ShortcutQLabel *in_label)
{
    // Get keyboard shortcut label to work on.
    if(in_label != 0)
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
            in_label->set_old_text();
        }
        else
        {
            // Shortcut is correct, set it in the label.
            in_label->setText(in_value);
        }
    }
}

void Config_dialog::fill_tab_shortcuts()
{
    this->kb_switch_playback->setText(this->settings->get_keyboard_shortcut(KB_SWITCH_PLAYBACK));
    this->kb_load_track_on_deck->setText(this->settings->get_keyboard_shortcut(KB_LOAD_TRACK_ON_DECK));
    this->kb_play_begin_track_on_deck->setText(this->settings->get_keyboard_shortcut(KB_PLAY_BEGIN_TRACK_ON_DECK));
    this->kb_get_next_track_from_deck->setText(this->settings->get_keyboard_shortcut(KB_SHOW_NEXT_KEYS));
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
    this->kb_file_search->setText(this->settings->get_keyboard_shortcut(KB_FILE_SEARCH));
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

bool
Config_dialog::show_browse_extern_prog_window()
{
    // Create browse window.
    QString file_path = QFileDialog::getOpenFileName(this,
                                                     tr("Select an external program"),
                                                     QDir::homePath());

    // Update base directory path.
    if (file_path != NULL)
    {
        this->extern_prog->setText(file_path);
    }

    return true;
}

void Config_dialog::reset_motion_detection_params()
{
    // Reset all motion detection parameters to their default values.
    this->autostart_detection_check->setChecked(this->settings->get_autostart_motion_detection_default());
    this->rpm_select->setCurrentIndex(this->rpm_select->findText(QString::number(this->settings->get_rpm_default())));
    this->set_amplify_coeff_slider(this->settings->get_input_amplify_coeff_default());
    this->set_min_amplitude_for_normal_speed_slider(this->settings->get_min_amplitude_for_normal_speed_default_from_vinyl_type(this->vinyl_type_select->currentText()));
    this->set_min_amplitude_slider(this->settings->get_min_amplitude_default_from_vinyl_type(this->vinyl_type_select->currentText()));
}

void Config_dialog::reset_shortcuts()
{
    // Reset all keyboard shortcuts to their default values.
    this->kb_switch_playback->setText(KB_SWITCH_PLAYBACK_DEFAULT);
    this->kb_load_track_on_deck->setText(KB_LOAD_TRACK_ON_DECK_DEFAULT);
    this->kb_play_begin_track_on_deck->setText(KB_PLAY_BEGIN_TRACK_ON_DECK_DEFAULT);
    this->kb_get_next_track_from_deck->setText(KB_SHOW_NEXT_KEYS_DEFAULT);
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
    this->kb_file_search->setText(KB_FILE_SEARCH_DEFAULT);
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

    // Set number of decks.
    this->settings->set_nb_decks(this->nb_decks_select->currentText().toInt());

    // External prog run at startup.
    this->settings->set_extern_prog(this->extern_prog->text());

    // Set autostart motion detection at startup.
    this->settings->set_autostart_motion_detection(this->autostart_detection_check->isChecked());

    // Set vinyl type.
    this->settings->set_vinyl_type(this->vinyl_type_select->currentText());

    // Set RPM.
    this->settings->set_rpm(this->rpm_select->currentText().toInt());

    // Set sound card settings.
    this->settings->set_sample_rate(this->sample_rate_select->currentText().toInt());
    if (this->device_internal_check->isChecked() == true)
    {
        this->settings->set_sound_driver(SOUND_DRIVER_INTERNAL);
    }
    else
    {
        this->settings->set_sound_driver(SOUND_DRIVER_JACK);
    }
    this->settings->set_internal_sound_card(this->device_internal_select->currentText());
    this->settings->set_auto_jack_connections(this->auto_jack_connections_check->isChecked());

    // Set motion detection settings.
    this->settings->set_input_amplify_coeff(this->get_amplify_coeff_slider());
    this->settings->set_min_amplitude_for_normal_speed(this->get_min_amplitude_for_normal_speed_slider());
    this->settings->set_min_amplitude(this->get_min_amplitude_slider());

    // Set keyboard shortcuts.
    this->settings->set_keyboard_shortcut(KB_SWITCH_PLAYBACK,           this->kb_switch_playback->text());
    this->settings->set_keyboard_shortcut(KB_LOAD_TRACK_ON_DECK,        this->kb_load_track_on_deck->text());
    this->settings->set_keyboard_shortcut(KB_PLAY_BEGIN_TRACK_ON_DECK,  this->kb_play_begin_track_on_deck->text());
    this->settings->set_keyboard_shortcut(KB_SHOW_NEXT_KEYS,            this->kb_get_next_track_from_deck->text());
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
    this->settings->set_keyboard_shortcut(KB_FILE_SEARCH,               this->kb_file_search->text());

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
    this->capturing = false;

    return;
}

ShortcutQLabel::~ShortcutQLabel()
{
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

    return;
}

void
ShortcutQLabel::mousePressEvent(QMouseEvent *in_mouse_event)

{
    Q_UNUSED(in_mouse_event);

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

                // Stop capture and show key sequence.
                this->finish_capture(keyInt);
            }
        }
    }
}
