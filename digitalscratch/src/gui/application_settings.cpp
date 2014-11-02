/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*-----------------------------------------------( application_settings.cpp )-*/
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
/*                Manage software settings stored in config file              */
/*                                                                            */
/*============================================================================*/

#include "application_settings.h"
#include "digital_scratch_api.h"
#include <iostream>
#include <QDir>
#include <QSize>
#include <QPoint>

Application_settings::Application_settings()
{
    this->settings = new QSettings(APPLICATION_NAME);

    this->available_gui_styles = new QList<QString>();
    this->available_gui_styles->append(GUI_STYLE_NATIVE);
    this->available_gui_styles->append(GUI_STYLE_DARK);

    this->available_vinyl_types = new QList<QString>();
    this->available_vinyl_types->append(FINAL_SCRATCH_VINYL);
    this->available_vinyl_types->append(SERATO_VINYL);
    this->available_vinyl_types->append(MIXVIBES_VINYL);

    this->available_rpms = new QList<unsigned short int>();
    this->available_rpms->append(RPM_33);
    this->available_rpms->append(RPM_45);

    this->available_nb_decks = new QList<unsigned int>();
    this->available_nb_decks->append(1);
    this->available_nb_decks->append(2);

    this->available_sample_rates = new QList<unsigned int>();
    this->available_sample_rates->append(44100);
    this->available_sample_rates->append(48000);
    this->available_sample_rates->append(96000);

    this->available_sound_cards = Audio_device_access_rules::get_device_list();

    this->audio_collection_full_refresh = true;

    this->init_settings();
}

Application_settings::~Application_settings()
{
    delete this->settings;
    delete this->available_gui_styles;
    delete this->available_vinyl_types;
    delete this->available_rpms;
    delete this->available_sample_rates;
    delete this->available_sound_cards;
}

void
Application_settings::init_settings()
{
    //
    // General application parameters.
    //
    if (this->settings->contains(MAIN_WIN_SIZE_CFG) == false) {
        this->settings->setValue(MAIN_WIN_SIZE_CFG, this->get_main_window_size_default());
    }
    if (this->settings->contains(MAIN_WIN_POS_CFG) == false) {
        this->settings->setValue(MAIN_WIN_POS_CFG, this->get_main_window_position_default());
    }
    if (this->settings->contains(BASE_DIR_PATH_CFG) == false) {
        this->settings->setValue(BASE_DIR_PATH_CFG, this->get_tracks_base_dir_path_default());
    }
    if (this->settings->contains(EXTERN_PROG_CFG) == false) {
        this->settings->setValue(EXTERN_PROG_CFG, this->get_extern_prog_default());
    }
    if (this->settings->contains(GUI_STYLE_CFG) == false) {
        this->settings->setValue(GUI_STYLE_CFG, this->get_gui_style_default());
    }
    if (this->settings->contains(SAMPLERS_VISIBLE_CFG) == false) {
        this->settings->setValue(SAMPLERS_VISIBLE_CFG, this->get_samplers_visible_default());
    }
    if (this->settings->contains(NB_DECKS_CFG) == false) {
        this->settings->setValue(NB_DECKS_CFG, this->get_nb_decks_default());
    }

    //
    // Sound card settings.
    //
    if (this->settings->contains(SAMPLE_RATE_CFG) == false) {
        this->settings->setValue(SAMPLE_RATE_CFG, this->get_sample_rate_default());
    }
    if (this->settings->contains(AUTO_JACK_CONNECTIONS_CFG) == false) {
        this->settings->setValue(AUTO_JACK_CONNECTIONS_CFG, this->get_auto_jack_connections_default());
    }
    if (this->settings->contains(SOUND_DRIVER_CFG) == false) {
        this->settings->setValue(SOUND_DRIVER_CFG, this->get_sound_driver_default());
    }

    //
    // Timecode signal detection parameters.
    //
    if (this->settings->contains(AUTOSTART_MOTION_DETECTION_CFG) == false) {
        this->settings->setValue(AUTOSTART_MOTION_DETECTION_CFG, this->get_autostart_motion_detection_default());
    }
    if (this->settings->contains(VINYL_TYPE_CFG) == false) {
        this->settings->setValue(VINYL_TYPE_CFG, this->get_vinyl_type_default());
    }
    if (this->settings->contains(INPUT_AMPLIFY_COEFF) == false) {
        this->settings->setValue(INPUT_AMPLIFY_COEFF, (new QString)->setNum(this->get_input_amplify_coeff_default()));
    }
    if (this->settings->contains(MIN_AMPLITUDE_NORMAL_SPEED) == false) {
        this->settings->setValue(MIN_AMPLITUDE_NORMAL_SPEED, (new QString)->setNum(this->get_min_amplitude_for_normal_speed_default_from_vinyl_type(this->settings->value(VINYL_TYPE_CFG).toString())));
    }
    if (this->settings->contains(MIN_AMPLITUDE) == false) {
        this->settings->setValue(MIN_AMPLITUDE, (new QString)->setNum(this->get_min_amplitude_default_from_vinyl_type(this->settings->value(VINYL_TYPE_CFG).toString())));
    }

    //
    // Keyboard shortcuts.
    //
    if (this->settings->contains(KB_SWITCH_PLAYBACK) == false) {
        this->settings->setValue(KB_SWITCH_PLAYBACK, KB_SWITCH_PLAYBACK_DEFAULT);
    }
    if (this->settings->contains(KB_LOAD_TRACK_ON_DECK) == false) {
        this->settings->setValue(KB_LOAD_TRACK_ON_DECK, KB_LOAD_TRACK_ON_DECK_DEFAULT);
    }
    if (this->settings->contains(KB_PLAY_BEGIN_TRACK_ON_DECK) == false) {
        this->settings->setValue(KB_PLAY_BEGIN_TRACK_ON_DECK, KB_PLAY_BEGIN_TRACK_ON_DECK_DEFAULT);
    }
    if (this->settings->contains(KB_SET_CUE_POINT1_ON_DECK) == false) {
        this->settings->setValue(KB_SET_CUE_POINT1_ON_DECK, KB_SET_CUE_POINT1_ON_DECK_DEFAULT);
    }
    if (this->settings->contains(KB_PLAY_CUE_POINT1_ON_DECK) == false) {
        this->settings->setValue(KB_PLAY_CUE_POINT1_ON_DECK, KB_PLAY_CUE_POINT1_ON_DECK_DEFAULT);
    }
    if (this->settings->contains(KB_SET_CUE_POINT2_ON_DECK) == false) {
        this->settings->setValue(KB_SET_CUE_POINT2_ON_DECK, KB_SET_CUE_POINT2_ON_DECK_DEFAULT);
    }
    if (this->settings->contains(KB_PLAY_CUE_POINT2_ON_DECK) == false) {
        this->settings->setValue(KB_PLAY_CUE_POINT2_ON_DECK, KB_PLAY_CUE_POINT2_ON_DECK_DEFAULT);
    }
    if (this->settings->contains(KB_SET_CUE_POINT3_ON_DECK) == false) {
        this->settings->setValue(KB_SET_CUE_POINT3_ON_DECK, KB_SET_CUE_POINT3_ON_DECK_DEFAULT);
    }
    if (this->settings->contains(KB_PLAY_CUE_POINT3_ON_DECK) == false) {
        this->settings->setValue(KB_PLAY_CUE_POINT3_ON_DECK, KB_PLAY_CUE_POINT3_ON_DECK_DEFAULT);
    }
    if (this->settings->contains(KB_SET_CUE_POINT4_ON_DECK) == false) {
        this->settings->setValue(KB_SET_CUE_POINT4_ON_DECK, KB_SET_CUE_POINT4_ON_DECK_DEFAULT);
    }
    if (this->settings->contains(KB_PLAY_CUE_POINT4_ON_DECK) == false) {
        this->settings->setValue(KB_PLAY_CUE_POINT4_ON_DECK, KB_PLAY_CUE_POINT4_ON_DECK_DEFAULT);
    }
    if (this->settings->contains(KB_COLLAPSE_BROWSER) == false) {
        this->settings->setValue(KB_COLLAPSE_BROWSER, KB_COLLAPSE_BROWSER_DEFAULT);
    }
    if (this->settings->contains(KB_LOAD_TRACK_ON_SAMPLER1) == false) {
        this->settings->setValue(KB_LOAD_TRACK_ON_SAMPLER1, KB_LOAD_TRACK_ON_SAMPLER1_DEFAULT);
    }
    if (this->settings->contains(KB_LOAD_TRACK_ON_SAMPLER2) == false) {
        this->settings->setValue(KB_LOAD_TRACK_ON_SAMPLER2, KB_LOAD_TRACK_ON_SAMPLER2_DEFAULT);
    }
    if (this->settings->contains(KB_LOAD_TRACK_ON_SAMPLER3) == false) {
        this->settings->setValue(KB_LOAD_TRACK_ON_SAMPLER3, KB_LOAD_TRACK_ON_SAMPLER3_DEFAULT);
    }
    if (this->settings->contains(KB_LOAD_TRACK_ON_SAMPLER4) == false) {
        this->settings->setValue(KB_LOAD_TRACK_ON_SAMPLER4, KB_LOAD_TRACK_ON_SAMPLER4_DEFAULT);
    }
    if (this->settings->contains(KB_SHOW_NEXT_KEYS) == false) {
        this->settings->setValue(KB_SHOW_NEXT_KEYS, KB_SHOW_NEXT_KEYS_DEFAULT);
    }
    if (this->settings->contains(KB_FULLSCREEN) == false) {
        this->settings->setValue(KB_FULLSCREEN, KB_FULLSCREEN_DEFAULT);
    }
    if (this->settings->contains(KB_HELP) == false) {
        this->settings->setValue(KB_HELP, KB_HELP_DEFAULT);
    }
    if (this->settings->contains(KB_FILE_SEARCH) == false) {
        this->settings->setValue(KB_FILE_SEARCH, KB_FILE_SEARCH_DEFAULT);
    }
}

void
Application_settings::set_main_window_size(QSize in_size)
{
    this->settings->setValue(MAIN_WIN_SIZE_CFG,
                             QString::number(in_size.width()) + "x" + QString::number(in_size.height()));
}

QSize
Application_settings::get_main_window_size()
{
    QString size(this->settings->value(MAIN_WIN_SIZE_CFG).toString());

    if (size.length() < 3)
    {
        // Bad size.
        size = MAIN_WIN_SIZE_CFG_DEFAULT;
    }

    int width  = size.split("x")[0].toInt();
    int height = size.split("x")[1].toInt();
    return QSize(width, height);
}

QString
Application_settings::get_main_window_size_default()
{
    return MAIN_WIN_SIZE_CFG_DEFAULT;
}

void
Application_settings::set_main_window_position(QPoint in_pos)
{
    this->settings->setValue(MAIN_WIN_POS_CFG,
                             QString::number(in_pos.x()) + "," + QString::number(in_pos.y()));
}

QPoint
Application_settings::get_main_window_position()
{
    QString pos(this->settings->value(MAIN_WIN_POS_CFG).toString());

    if (pos.length() < 3)
    {
        // Bad position.
        pos = MAIN_WIN_POS_CFG_DEFAULT;
    }

    int x = pos.split(",")[0].toInt();
    int y = pos.split(",")[1].toInt();

    return QPoint(x, y);
}

QString
Application_settings::get_main_window_position_default()
{
    return MAIN_WIN_POS_CFG_DEFAULT;
}

void
Application_settings::set_tracks_base_dir_path(QString in_tracks_base_dir_path)
{
    this->settings->setValue(BASE_DIR_PATH_CFG, in_tracks_base_dir_path);
}

QString
Application_settings::get_tracks_base_dir_path()
{
    return this->settings->value(BASE_DIR_PATH_CFG).toString();
}

QString
Application_settings::get_tracks_base_dir_path_default()
{
    return QDir::homePath();
}

void
Application_settings::set_browser_splitter_size(QByteArray in_state)
{
    this->settings->setValue(BROWSER_SPLITTER_SIZE_CFG, in_state);
}

QByteArray
Application_settings::get_browser_splitter_size()
{
    return this->settings->value(BROWSER_SPLITTER_SIZE_CFG).toByteArray();
}

void
Application_settings::set_extern_prog(QString in_extern_prog_path)
{
    this->settings->setValue(EXTERN_PROG_CFG, in_extern_prog_path);
}

QString
Application_settings::get_extern_prog()
{
    return this->settings->value(EXTERN_PROG_CFG).toString();
}

QString
Application_settings::get_extern_prog_default()
{
    return "";
}

QList<QString>*
Application_settings::get_available_gui_styles()
{
    return this->available_gui_styles;
}

QString
Application_settings::get_gui_style()
{
    return this->settings->value(GUI_STYLE_CFG).toString();
}

QString
Application_settings::get_gui_style_default()
{
    return GUI_STYLE_DEFAULT;
}

void
Application_settings::set_gui_style(QString in_gui_style)
{
    this->settings->setValue(GUI_STYLE_CFG, in_gui_style);
}

short signed int
Application_settings::get_nb_decks()
{
    return this->settings->value(NB_DECKS_CFG).toInt();
}

signed short int
Application_settings::get_nb_decks_default()
{
    return NB_DECKS_DEFAULT;
}

void
Application_settings::set_nb_decks(signed short in_nb_decks)
{
    this->settings->setValue(NB_DECKS_CFG, in_nb_decks);
}

QList<unsigned int>*
Application_settings::get_available_nb_decks()
{
    return this->available_nb_decks;
}


//
// Timecode signal detection settings.
//

int
Application_settings::get_input_amplify_coeff()
{
    return this->settings->value(INPUT_AMPLIFY_COEFF).toInt();
}

int
Application_settings::get_input_amplify_coeff_default()
{
    return dscratch_get_default_input_amplify_coeff();
}

void
Application_settings::set_input_amplify_coeff(int in_coeff)
{
    QString value;
    value.setNum(in_coeff);
    if (in_coeff > 0 && in_coeff < 1000) // Range: ]0,1000[
    {
        this->settings->setValue(INPUT_AMPLIFY_COEFF, value);
    }
}

float
Application_settings::get_min_amplitude_for_normal_speed()
{
    return this->settings->value(MIN_AMPLITUDE_NORMAL_SPEED).toFloat();
}

float
Application_settings::get_min_amplitude_for_normal_speed_default()
{
    return dscratch_get_default_min_amplitude_for_normal_speed();
}

float
Application_settings::get_min_amplitude_for_normal_speed_default_from_vinyl_type(QString vinyl_type)
{
    return dscratch_get_default_min_amplitude_for_normal_speed_from_vinyl_type(vinyl_type.toLocal8Bit().data());
}

void
Application_settings::set_min_amplitude_for_normal_speed(float in_amplitude)
{
    QString value;
    value.setNum(in_amplitude);
    if (in_amplitude > 0.0 && in_amplitude < 1.0) // Range: ]0,1[
    {
        this->settings->setValue(MIN_AMPLITUDE_NORMAL_SPEED, value);
    }
}

float
Application_settings::get_min_amplitude()
{
    return this->settings->value(MIN_AMPLITUDE).toFloat();
}

float
Application_settings::get_min_amplitude_default()
{
    return dscratch_get_default_min_amplitude();
}

float
Application_settings::get_min_amplitude_default_from_vinyl_type(QString vinyl_type)
{
    return dscratch_get_default_min_amplitude_from_vinyl_type(vinyl_type.toLocal8Bit().data());
}

void
Application_settings::set_min_amplitude(float in_amplitude)
{
    QString value;
    value.setNum(in_amplitude);
    if (in_amplitude > 0.0 && in_amplitude < 1.0) // Range: ]0,1[
    {
        this->settings->setValue(MIN_AMPLITUDE, value);
    }
}

void
Application_settings::set_samplers_visible(bool is_visible)
{
    this->settings->setValue(SAMPLERS_VISIBLE_CFG, is_visible);
}

bool
Application_settings::get_samplers_visible()
{
    return this->settings->value(SAMPLERS_VISIBLE_CFG).toBool();
}

bool
Application_settings::get_samplers_visible_default()
{
    return SAMPLERS_VISIBLE_DEFAULT;
}

//
// Playback parameters settings.
//
void
Application_settings::set_keyboard_shortcut(QString in_kb_shortcut_path, QString in_value)
{
    if (in_value.isEmpty() == false)
    {
        this->settings->setValue(in_kb_shortcut_path, in_value);
    }
}

QString
Application_settings::get_keyboard_shortcut(QString in_kb_shortcut_path)
{
    return this->settings->value(in_kb_shortcut_path).toString();
}

unsigned int
Application_settings::get_sample_rate()
{
    return this->settings->value(SAMPLE_RATE_CFG).toUInt();
}

unsigned int
Application_settings::get_sample_rate_default()
{
    return SAMPLE_RATE_DEFAULT;
}

void
Application_settings::set_sample_rate(unsigned int in_sample_rate)
{
    this->settings->setValue(SAMPLE_RATE_CFG, in_sample_rate);
}

bool
Application_settings::get_auto_jack_connections()
{
    return this->settings->value(AUTO_JACK_CONNECTIONS_CFG).toBool();
}

bool
Application_settings::get_auto_jack_connections_default()
{
    return AUTO_JACK_CONNECTIONS_DEFAULT;
}

void
Application_settings::set_auto_jack_connections(bool in_autoconnect)
{
    this->settings->setValue(AUTO_JACK_CONNECTIONS_CFG, in_autoconnect);
}

void
Application_settings::set_sound_driver(QString in_driver)
{
    this->settings->setValue(SOUND_DRIVER_CFG, in_driver);
}

QString
Application_settings::get_sound_driver()
{
    return this->settings->value(SOUND_DRIVER_CFG).toString();
}

QString
Application_settings::get_sound_driver_default()
{
    return SOUND_DRIVER_DEFAULT;
}

void
Application_settings::set_internal_sound_card(QString in_card)
{
    this->settings->setValue(SOUND_CARD_CFG, in_card);
}

QString
Application_settings::get_internal_sound_card()
{
    return this->settings->value(SOUND_CARD_CFG).toString();
}

QString
Application_settings::get_internal_sound_card_default()
{
    return SOUND_CARD_DEFAULT;
}

QList<QString>*
Application_settings::get_available_internal_sound_cards()
{
    return this->available_sound_cards;
}

bool
Application_settings::get_autostart_motion_detection()
{
    return this->settings->value(AUTOSTART_MOTION_DETECTION_CFG).toBool();
}

bool
Application_settings::get_autostart_motion_detection_default()
{
    return AUTOSTART_MOTION_DETECTION_DEFAULT;
}

void
Application_settings::set_autostart_motion_detection(bool in_autostart)
{
    this->settings->setValue(AUTOSTART_MOTION_DETECTION_CFG, in_autostart);
}

QString
Application_settings::get_vinyl_type()
{
    return this->settings->value(VINYL_TYPE_CFG).toString();
}

QString
Application_settings::get_vinyl_type_default()
{
    return dscratch_get_default_vinyl_type();
}

void
Application_settings::set_vinyl_type(QString in_vinyl_type)
{
    this->settings->setValue(VINYL_TYPE_CFG, in_vinyl_type);
}

QList<QString>*
Application_settings::get_available_vinyl_types()
{
    return this->available_vinyl_types;
}

void
Application_settings::set_rpm(unsigned short int in_rpm)
{
    QString value;
    value.setNum(in_rpm);
    if (in_rpm == 33 || in_rpm == 45) // Range: 33 or 45.
    {
        this->settings->setValue(RPM_CFG, value);
    }
}

unsigned short int
Application_settings::get_rpm()
{
    if (this->settings->value(RPM_CFG).toInt() == 45)
        return 45;
    else
        return 33;
}

unsigned short int
Application_settings::get_rpm_default()
{
    return dscratch_get_default_rpm();
}

QList<unsigned short int>*
Application_settings::get_available_rpms()
{
    return this->available_rpms;
}

QList<unsigned int>*
Application_settings::get_available_sample_rates()
{
    return this->available_sample_rates;
}

void
Application_settings::set_audio_collection_full_refresh(bool in_full_refresh)
{
    this->audio_collection_full_refresh = in_full_refresh;
}

bool
Application_settings::get_audio_collection_full_refresh()
{
    return this->audio_collection_full_refresh;
}
