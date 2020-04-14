/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*-----------------------------------------------( application_settings.cpp )-*/
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
/*                Manage software settings stored in config file              */
/*                                                                            */
/*============================================================================*/

#include <iostream>
#include <QDir>
#include <QSize>
#include <QPoint>
#include <QStandardPaths>

#include "app/application_settings.h"
#include "utils.h"

Application_settings::Application_settings() : settings(APPLICATION_NAME)
{
    this->available_gui_styles << GUI_STYLE_NATIVE << GUI_STYLE_DARK;
    this->available_languages << "en" << "fr";
    for (int i = 0; i < NB_DSCRATCH_VINYLS; i++)
    {
       this->available_vinyl_types.insert(static_cast<dscratch_vinyls_t>(i),
                                          dscratch_get_vinyl_name_from_type(static_cast<dscratch_vinyls_t>(i)));
    }
    this->available_rpms << RPM_33 << RPM_45;
    this->available_nb_decks << 1 << 2 << 3;
    this->available_sample_rates << 44100 << 48000 << 96000;

    // TODO: add hardware sound card support.
//    this->available_sound_cards = Sound_card_control_rules::get_device_list();

    this->audio_collection_full_refresh = true;

    this->init_settings();
}

Application_settings::~Application_settings()
{
}

void
Application_settings::init_settings()
{
    //
    // General application parameters.
    //
    if (this->settings.contains(MAIN_WIN_SIZE_CFG) == false) {
        this->settings.setValue(MAIN_WIN_SIZE_CFG, this->get_main_window_size_default());
    }
    if (this->settings.contains(MAIN_WIN_POS_CFG) == false) {
        this->settings.setValue(MAIN_WIN_POS_CFG, this->get_main_window_position_default());
    }
    if (this->settings.contains(BASE_DIR_PATH_CFG) == false) {
        this->settings.setValue(BASE_DIR_PATH_CFG, this->get_tracks_base_dir_path_default());
    }
    if (this->settings.contains(TRACKLIST_PATH_CFG) == false) {
        this->settings.setValue(TRACKLIST_PATH_CFG, this->get_tracklist_path_default());
    }
    if (this->settings.contains(EXTERN_PROG_CFG) == false) {
        this->settings.setValue(EXTERN_PROG_CFG, this->get_extern_prog_default());
    }
    if (this->settings.contains(GUI_STYLE_CFG) == false) {
        this->settings.setValue(GUI_STYLE_CFG, this->get_gui_style_default());
    }
    if (this->settings.contains(LANG_CFG) == false) {
        this->settings.setValue(LANG_CFG, this->get_language_default());
    }
    if (this->settings.contains(SAMPLERS_VISIBLE_CFG) == false) {
        this->settings.setValue(SAMPLERS_VISIBLE_CFG, this->get_samplers_visible_default());
    }
    if (this->settings.contains(NB_DECKS_CFG) == false) {
        this->settings.setValue(NB_DECKS_CFG, this->get_nb_decks_default());
    }
    if (this->settings.contains(NB_SAMPLERS_CFG) == false) {
        this->settings.setValue(NB_SAMPLERS_CFG, this->get_nb_samplers_default());
    }

    //
    // Sound card settings.
    //
    if (this->settings.contains(SAMPLE_RATE_CFG) == false) {
        this->settings.setValue(SAMPLE_RATE_CFG, this->get_sample_rate_default());
    }
    if (this->settings.contains(AUTO_JACK_CONNECTIONS_CFG) == false) {
        this->settings.setValue(AUTO_JACK_CONNECTIONS_CFG, this->get_auto_jack_connections_default());
    }
    if (this->settings.contains(SOUND_DRIVER_CFG) == false) {
        this->settings.setValue(SOUND_DRIVER_CFG, this->get_sound_driver_default());
    }

    //
    // Timecode signal detection parameters.
    //
    for (int i = 0; i < 3; i++)
    {
        if (this->settings.contains(QString(DECK_INDEX) + QString::number(i) + "/" + QString(VINYL_TYPE_CFG)) == false)
        {
            this->settings.setValue(QString(DECK_INDEX) + QString::number(i) + "/" + QString(VINYL_TYPE_CFG),
                                    this->get_vinyl_type_default());
        }
        if (this->settings.contains(QString(DECK_INDEX) + QString::number(i) + "/" + QString(RPM_CFG)) == false)
        {
            this->settings.setValue(QString(DECK_INDEX) + QString::number(i) + "/" + QString(RPM_CFG),
                                    (new QString)->setNum(this->get_rpm_default()));
        }
    }

    //
    // Keyboard shortcuts.
    //
    if (this->settings.contains(KB_SWITCH_PLAYBACK) == false) {
        this->settings.setValue(KB_SWITCH_PLAYBACK, KB_SWITCH_PLAYBACK_DEFAULT);
    }
    if (this->settings.contains(KB_LOAD_TRACK_ON_DECK) == false) {
        this->settings.setValue(KB_LOAD_TRACK_ON_DECK, KB_LOAD_TRACK_ON_DECK_DEFAULT);
    }
    if (this->settings.contains(KB_PLAY_BEGIN_TRACK_ON_DECK) == false) {
        this->settings.setValue(KB_PLAY_BEGIN_TRACK_ON_DECK, KB_PLAY_BEGIN_TRACK_ON_DECK_DEFAULT);
    }
    if (this->settings.contains(KB_SET_CUE_POINT1_ON_DECK) == false) {
        this->settings.setValue(KB_SET_CUE_POINT1_ON_DECK, KB_SET_CUE_POINT1_ON_DECK_DEFAULT);
    }
    if (this->settings.contains(KB_PLAY_CUE_POINT1_ON_DECK) == false) {
        this->settings.setValue(KB_PLAY_CUE_POINT1_ON_DECK, KB_PLAY_CUE_POINT1_ON_DECK_DEFAULT);
    }
    if (this->settings.contains(KB_SET_CUE_POINT2_ON_DECK) == false) {
        this->settings.setValue(KB_SET_CUE_POINT2_ON_DECK, KB_SET_CUE_POINT2_ON_DECK_DEFAULT);
    }
    if (this->settings.contains(KB_PLAY_CUE_POINT2_ON_DECK) == false) {
        this->settings.setValue(KB_PLAY_CUE_POINT2_ON_DECK, KB_PLAY_CUE_POINT2_ON_DECK_DEFAULT);
    }
    if (this->settings.contains(KB_SET_CUE_POINT3_ON_DECK) == false) {
        this->settings.setValue(KB_SET_CUE_POINT3_ON_DECK, KB_SET_CUE_POINT3_ON_DECK_DEFAULT);
    }
    if (this->settings.contains(KB_PLAY_CUE_POINT3_ON_DECK) == false) {
        this->settings.setValue(KB_PLAY_CUE_POINT3_ON_DECK, KB_PLAY_CUE_POINT3_ON_DECK_DEFAULT);
    }
    if (this->settings.contains(KB_SET_CUE_POINT4_ON_DECK) == false) {
        this->settings.setValue(KB_SET_CUE_POINT4_ON_DECK, KB_SET_CUE_POINT4_ON_DECK_DEFAULT);
    }
    if (this->settings.contains(KB_PLAY_CUE_POINT4_ON_DECK) == false) {
        this->settings.setValue(KB_PLAY_CUE_POINT4_ON_DECK, KB_PLAY_CUE_POINT4_ON_DECK_DEFAULT);
    }
    if (this->settings.contains(KB_LOAD_TRACK_ON_SAMPLER1) == false) {
        this->settings.setValue(KB_LOAD_TRACK_ON_SAMPLER1, KB_LOAD_TRACK_ON_SAMPLER1_DEFAULT);
    }
    if (this->settings.contains(KB_LOAD_TRACK_ON_SAMPLER2) == false) {
        this->settings.setValue(KB_LOAD_TRACK_ON_SAMPLER2, KB_LOAD_TRACK_ON_SAMPLER2_DEFAULT);
    }
    if (this->settings.contains(KB_LOAD_TRACK_ON_SAMPLER3) == false) {
        this->settings.setValue(KB_LOAD_TRACK_ON_SAMPLER3, KB_LOAD_TRACK_ON_SAMPLER3_DEFAULT);
    }
    if (this->settings.contains(KB_LOAD_TRACK_ON_SAMPLER4) == false) {
        this->settings.setValue(KB_LOAD_TRACK_ON_SAMPLER4, KB_LOAD_TRACK_ON_SAMPLER4_DEFAULT);
    }
    if (this->settings.contains(KB_SHOW_NEXT_KEYS) == false) {
        this->settings.setValue(KB_SHOW_NEXT_KEYS, KB_SHOW_NEXT_KEYS_DEFAULT);
    }
    if (this->settings.contains(KB_FULLSCREEN) == false) {
        this->settings.setValue(KB_FULLSCREEN, KB_FULLSCREEN_DEFAULT);
    }
    if (this->settings.contains(KB_HELP) == false) {
        this->settings.setValue(KB_HELP, KB_HELP_DEFAULT);
    }
    if (this->settings.contains(KB_FILE_SEARCH) == false) {
        this->settings.setValue(KB_FILE_SEARCH, KB_FILE_SEARCH_DEFAULT);
    }
}

bool
Application_settings::import_from_ini_file(const QString &file_path)
{
    bool res = true;

    if (QFileInfo::exists(file_path) == true)
    {
        QSettings import_settings(file_path, QSettings::IniFormat);

        QStringList keys({BASE_DIR_PATH_CFG,          // Application and main window.
                          TRACKLIST_PATH_CFG,
                          GUI_STYLE_CFG,
                          SAMPLERS_VISIBLE_CFG,
                          EXTERN_PROG_CFG,
                          NB_DECKS_CFG,
                          NB_SAMPLERS_CFG,
                          LANG_CFG,
                          SAMPLE_RATE_CFG,            // Sound caracteristics.
                          AUTO_JACK_CONNECTIONS_CFG,
                          SOUND_DRIVER_CFG,
                          VINYL_TYPE_CFG,             // Decks: motion detection.
                          RPM_CFG,
                          MAX_SPEED_DIFF_CFG,         // Playback parameters.
                          SLOW_SPEED_ALGO_USAGE_CFG,
                          MAX_NB_SPEED_STABIL_CFG,
                          NB_CYCLE_CHANGING_DIR_CFG,
                          KB_SWITCH_PLAYBACK,         // Keyboard shortcuts.
                          KB_LOAD_TRACK_ON_DECK,
                          KB_PLAY_BEGIN_TRACK_ON_DECK,
                          KB_SET_CUE_POINT1_ON_DECK,
                          KB_PLAY_CUE_POINT1_ON_DECK,
                          KB_SET_CUE_POINT2_ON_DECK,
                          KB_PLAY_CUE_POINT2_ON_DECK,
                          KB_SET_CUE_POINT3_ON_DECK,
                          KB_PLAY_CUE_POINT3_ON_DECK,
                          KB_SET_CUE_POINT4_ON_DECK,
                          KB_PLAY_CUE_POINT4_ON_DECK,
                          KB_LOAD_TRACK_ON_SAMPLER1,
                          KB_LOAD_TRACK_ON_SAMPLER2,
                          KB_LOAD_TRACK_ON_SAMPLER3,
                          KB_LOAD_TRACK_ON_SAMPLER4,
                          KB_SHOW_NEXT_KEYS,
                          KB_FULLSCREEN,
                          KB_HELP,
                          KB_FILE_SEARCH
                          });

        foreach (auto k, keys)
        {
            if (import_settings.contains(k) == true)
            {
                this->settings.setValue(k, import_settings.value(k));
            }
        }
    }
    else
    { // Import file does not exists.
        res = false;
    }

    return res;
}

QString
Application_settings::get_ini_config_file()
{
    return this->settings.fileName();
}

void
Application_settings::set_main_window_size(const QSize &size)
{
    this->settings.setValue(MAIN_WIN_SIZE_CFG,
                             QString::number(size.width()) + "x" + QString::number(size.height()));
}

QSize
Application_settings::get_main_window_size()
{
    QString size(this->settings.value(MAIN_WIN_SIZE_CFG).toString());

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
Application_settings::set_main_window_position(const QPoint &pos)
{
    this->settings.setValue(MAIN_WIN_POS_CFG,
                            QString::number(pos.x()) + "," + QString::number(pos.y()));
}

QPoint
Application_settings::get_main_window_position()
{
    QString pos(this->settings.value(MAIN_WIN_POS_CFG).toString());

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
Application_settings::set_tracks_base_dir_path(const QString &path)
{
    this->settings.setValue(BASE_DIR_PATH_CFG, path);
}

QString
Application_settings::get_tracks_base_dir_path()
{
    return this->settings.value(BASE_DIR_PATH_CFG).toString();
}

QString
Application_settings::get_tracks_base_dir_path_default()
{
    return QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
}

void
Application_settings::set_tracklist_path(const QString &path)
{
    this->settings.setValue(TRACKLIST_PATH_CFG, path);
}

QString
Application_settings::get_tracklist_path()
{
    return this->settings.value(TRACKLIST_PATH_CFG).toString();
}

QString
Application_settings::get_tracklist_path_default()
{
    return QDir::cleanPath(QStandardPaths::writableLocation(QStandardPaths::MusicLocation) + QDir::separator() + "tracklist_backup");
}

void
Application_settings::set_browser_splitter_size(const QByteArray &state)
{
    this->settings.setValue(BROWSER_SPLITTER_SIZE_CFG, state);
}

QByteArray
Application_settings::get_browser_splitter_size()
{
    return this->settings.value(BROWSER_SPLITTER_SIZE_CFG).toByteArray();
}

void
Application_settings::set_extern_prog(QString in_extern_prog_path)
{
    this->settings.setValue(EXTERN_PROG_CFG, in_extern_prog_path);
}

QString
Application_settings::get_extern_prog()
{
    return this->settings.value(EXTERN_PROG_CFG).toString();
}

QString
Application_settings::get_extern_prog_default()
{
    return "";
}

QList<QString>
Application_settings::get_available_gui_styles()
{
    return this->available_gui_styles;
}

QString
Application_settings::get_gui_style()
{
    return this->settings.value(GUI_STYLE_CFG).toString();
}

QString
Application_settings::get_gui_style_default()
{
    return GUI_STYLE_DEFAULT;
}

void
Application_settings::set_gui_style(const QString &style)
{
    this->settings.setValue(GUI_STYLE_CFG, style);
}

unsigned short int
Application_settings::get_nb_decks()
{
    unsigned short int value = static_cast<unsigned short int>(this->settings.value(NB_DECKS_CFG).toUInt());

    // Range is ]0;3]
    if ((value == 0) || (value > 3))
    {
        return this->get_nb_decks_default();
    }
    else
    {
        return value;
    }
}

unsigned short int
Application_settings::get_nb_decks_default()
{
    return NB_DECKS_DEFAULT;
}

void
Application_settings::set_nb_decks(const unsigned short int &nb_decks)
{
    this->settings.setValue(NB_DECKS_CFG, nb_decks);
}

QList<unsigned short int>
Application_settings::get_available_nb_decks()
{
    return this->available_nb_decks;
}

unsigned short int
Application_settings::get_nb_samplers()
{
    return static_cast<unsigned short int>(this->settings.value(NB_SAMPLERS_CFG).toUInt());
}

unsigned short int
Application_settings::get_nb_samplers_default()
{
    return NB_SAMPLERS_DEFAULT;
}

void
Application_settings::set_nb_samplers(const unsigned short int &nb_samplers)
{
    this->settings.setValue(NB_SAMPLERS_CFG, nb_samplers);
}

void
Application_settings::set_language(const QString &iso639_lang)
{
    this->settings.setValue(LANG_CFG, iso639_lang);
}

QString
Application_settings::get_language()
{
    return Utils::language_to_iso639_code(QLocale(this->settings.value(LANG_CFG).toString()).language());
}

QString
Application_settings::get_language_default()
{
    return Utils::language_to_iso639_code(QLocale::system().language());
}

QList<QString>
Application_settings::get_available_languages()
{
    return this->available_languages;
}

//
// Timecode signal detection settings.
//

dscratch_vinyls_t
Application_settings::get_vinyl_type(const unsigned short int &deck_index)
{
    return static_cast<dscratch_vinyls_t>(this->settings.value(QString(DECK_INDEX) + QString::number(deck_index)
                                                               + "/" + QString(VINYL_TYPE_CFG)).toInt());
}

dscratch_vinyls_t
Application_settings::get_vinyl_type_default()
{
    return dscratch_get_default_vinyl_type();
}

void
Application_settings::set_vinyl_type(const unsigned short int &deck_index, dscratch_vinyls_t type)
{
    this->settings.setValue(QString(DECK_INDEX) + QString::number(deck_index)
                            + "/" + QString(VINYL_TYPE_CFG), type);
}

QMap<dscratch_vinyls_t, QString>
Application_settings::get_available_vinyl_types()
{
    return this->available_vinyl_types;
}

void
Application_settings::set_rpm(const unsigned short int &deck_index, const dscratch_vinyl_rpm_t &rpm)
{
    QString value;
    value.setNum(rpm);
    if (rpm == RPM_33 || rpm == RPM_45) // Supported speeds are 33 and 45 rpm, nothing else.
    {
        this->settings.setValue(QString(DECK_INDEX) + QString::number(deck_index)
                                + "/" + QString(RPM_CFG), value);
    }
}

dscratch_vinyl_rpm_t
Application_settings::get_rpm(const unsigned short int &deck_index)
{
    if (this->settings.value(QString(DECK_INDEX) + QString::number(deck_index)
                             + "/" + QString(RPM_CFG)).toInt() == RPM_45)
        return RPM_45;
    else
        return RPM_33;
}

dscratch_vinyl_rpm_t
Application_settings::get_rpm_default()
{
    return dscratch_get_default_rpm();
}

QList<unsigned short int>
Application_settings::get_available_rpms()
{
    return this->available_rpms;
}

void
Application_settings::set_samplers_visible(const bool &is_visible)
{
    this->settings.setValue(SAMPLERS_VISIBLE_CFG, is_visible);
}

bool
Application_settings::get_samplers_visible()
{
    return this->settings.value(SAMPLERS_VISIBLE_CFG).toBool();
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
Application_settings::set_keyboard_shortcut(const QString &kb_shortcut_path, const QString &value)
{
    if (value.isEmpty() == false)
    {
        this->settings.setValue(kb_shortcut_path, value);
    }
}

QString
Application_settings::get_keyboard_shortcut(QString in_kb_shortcut_path)
{
    return this->settings.value(in_kb_shortcut_path).toString();
}

unsigned int
Application_settings::get_sample_rate()
{
    return this->settings.value(SAMPLE_RATE_CFG).toUInt();
}

unsigned int
Application_settings::get_sample_rate_default()
{
    return SAMPLE_RATE_DEFAULT;
}

void
Application_settings::set_sample_rate(const unsigned int &sample_rate)
{
    this->settings.setValue(SAMPLE_RATE_CFG, sample_rate);
}

bool
Application_settings::get_auto_jack_connections()
{
    return this->settings.value(AUTO_JACK_CONNECTIONS_CFG).toBool();
}

bool
Application_settings::get_auto_jack_connections_default()
{
    return AUTO_JACK_CONNECTIONS_DEFAULT;
}

void
Application_settings::set_auto_jack_connections(const bool &do_autoconnect)
{
    this->settings.setValue(AUTO_JACK_CONNECTIONS_CFG, do_autoconnect);
}

void
Application_settings::set_sound_driver(const QString &driver)
{
    this->settings.setValue(SOUND_DRIVER_CFG, driver);
}

QString
Application_settings::get_sound_driver()
{
    return this->settings.value(SOUND_DRIVER_CFG).toString();
}

QString
Application_settings::get_sound_driver_default()
{
    return SOUND_DRIVER_DEFAULT;
}

void
Application_settings::set_internal_sound_card(const QString &card)
{
    this->settings.setValue(SOUND_CARD_CFG, card);
}

QString
Application_settings::get_internal_sound_card()
{
    return this->settings.value(SOUND_CARD_CFG).toString();
}

QString
Application_settings::get_internal_sound_card_default()
{
    return SOUND_CARD_DEFAULT;
}

QList<QString>
Application_settings::get_available_internal_sound_cards()
{
    return this->available_sound_cards;
}

QList<unsigned int>
Application_settings::get_available_sample_rates()
{
    return this->available_sample_rates;
}

void
Application_settings::set_audio_collection_full_refresh(const bool &full_refresh)
{
    this->audio_collection_full_refresh = full_refresh;
}

bool
Application_settings::get_audio_collection_full_refresh()
{
    return this->audio_collection_full_refresh;
}
