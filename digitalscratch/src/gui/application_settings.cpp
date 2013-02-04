/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*-----------------------------------------------( application_settings.cpp )-*/
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
/*                Manage software settings stored in config file              */
/*                                                                            */
/*============================================================================*/

#include "application_settings.h"
#include "digital_scratch_api.h"
#include <iostream>
#include <QDir>

Application_settings::Application_settings()
{
    this->settings = new QSettings(APPLICATION_NAME);

    this->available_gui_styles = new QList<QString>();
    this->available_gui_styles->append(GUI_STYLE_DEFAULT);
    this->available_gui_styles->append(GUI_STYLE_NB1);

    this->available_vinyl_types = new QList<QString>();
    this->available_vinyl_types->append(FINAL_SCRATCH_VINYL);
    this->available_vinyl_types->append(SERATO_VINYL);
    this->available_vinyl_types->append(MIXVIBES_VINYL);

    this->init_settings();
}

Application_settings::~Application_settings()
{
    delete this->settings;
    delete this->available_gui_styles;
    delete this->available_vinyl_types;
}

void
Application_settings::init_settings()
{
    //
    // General application parameters.
    //

    if (this->settings->contains(BASE_DIR_PATH_CFG) == false) {
        this->settings->setValue(BASE_DIR_PATH_CFG, this->get_tracks_base_dir_path_default());
    }
    if (this->settings->contains(GUI_STYLE_CFG) == false) {
        this->settings->setValue(GUI_STYLE_CFG, GUI_STYLE_DEFAULT);
    }

    //
    // Timecode signal detection parameters.
    //
    if (this->settings->contains(VINYL_TYPE_CFG) == false) {
        this->settings->setValue(VINYL_TYPE_CFG, this->get_vinyl_type_default());
    }
    if (this->settings->contains(EXTREME_MIN_CFG) == false) {
        this->settings->setValue(EXTREME_MIN_CFG, (new QString)->setNum(this->get_extreme_min_default(), 'f', 3));
    }
    if (this->settings->contains(MAX_NB_BUFFER_CFG) == false) {
        this->settings->setValue(MAX_NB_BUFFER_CFG, (new QString)->setNum(this->get_max_nb_buffer_default()));
    }
    if (this->settings->contains(MAX_BUFFER_COEFF_CFG) == false) {
        this->settings->setValue(MAX_BUFFER_COEFF_CFG, (new QString)->setNum(this->get_max_buffer_coeff_default()));
    }
    if (this->settings->contains(MAX_SPEED_DIFF_CFG) == false) {
        this->settings->setValue(MAX_SPEED_DIFF_CFG, (new QString)->setNum(this->get_max_speed_diff_default(), 'f', 3));
    }
    if (this->settings->contains(SLOW_SPEED_ALGO_USAGE_CFG) == false) {
        this->settings->setValue(SLOW_SPEED_ALGO_USAGE_CFG, (new QString)->setNum(this->get_slow_speed_algo_usage_default(), 'f', 3));
    }
    if (this->settings->contains(MAX_NB_SPEED_STABIL_CFG) == false) {
        this->settings->setValue(MAX_NB_SPEED_STABIL_CFG, (new QString)->setNum(this->get_max_nb_speed_for_stability_default()));
    }
    if (this->settings->contains(NB_CYCLE_CHANGING_DIR_CFG) == false) {
        this->settings->setValue(NB_CYCLE_CHANGING_DIR_CFG, (new QString)->setNum(this->get_nb_cycle_before_changing_direction_default()));
    }
    if (this->settings->contains(LOW_PASS_FILTER_MAX_SPEED_USAGE_CFG) == false) {
        this->settings->setValue(LOW_PASS_FILTER_MAX_SPEED_USAGE_CFG, (new QString)->setNum(this->get_low_pass_filter_max_speed_usage_default(), 'f', 3));
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
    if (this->settings->contains(KB_SET_CUE_POINT_ON_DECK) == false) {
        this->settings->setValue(KB_SET_CUE_POINT_ON_DECK, KB_SET_CUE_POINT_ON_DECK_DEFAULT);
    }
    if (this->settings->contains(KB_PLAY_CUE_POINT_ON_DECK) == false) {
        this->settings->setValue(KB_PLAY_CUE_POINT_ON_DECK, KB_PLAY_CUE_POINT_ON_DECK_DEFAULT);
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
}

QString
Application_settings::get_tracks_base_dir_path()
{
    return this->settings->value(BASE_DIR_PATH_CFG).toString();
}

void
Application_settings::set_tracks_base_dir_path(QString in_tracks_base_dir_path)
{
    this->settings->setValue(BASE_DIR_PATH_CFG, in_tracks_base_dir_path);
}

QString
Application_settings::get_tracks_base_dir_path_default()
{
    return QDir::homePath();
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

//
// Timecode signal detection settings.
//

float
Application_settings::get_extreme_min()
{
    return this->settings->value(EXTREME_MIN_CFG).toFloat();
}

float
Application_settings::get_extreme_min_default()
{
    return dscratch_get_default_extreme_min();
}

void
Application_settings::set_extreme_min(float in_extreme_min)
{
    QString value;
    value.setNum(in_extreme_min, 'f', 3);
    if (in_extreme_min > 0.0 && in_extreme_min < 1.0) // Range: ]0.0,1.0[
    {
        this->settings->setValue(EXTREME_MIN_CFG, value);
    }
}

int
Application_settings::get_max_nb_buffer()
{
    return this->settings->value(MAX_NB_BUFFER_CFG).toInt();
}

int
Application_settings::get_max_nb_buffer_default()
{
    return dscratch_get_default_max_nb_buffer();
}

void
Application_settings::set_max_nb_buffer(int in_nb_buffer)
{
    QString value;
    value.setNum(in_nb_buffer);
    if (in_nb_buffer > 0 && in_nb_buffer < 100) // Range: ]0,100[
    {
        this->settings->setValue(MAX_NB_BUFFER_CFG, value);
    }
}

int
Application_settings::get_max_buffer_coeff()
{
    return this->settings->value(MAX_BUFFER_COEFF_CFG).toInt();
}

int
Application_settings::get_max_buffer_coeff_default()
{
    return dscratch_get_default_max_buffer_coeff();
}

void
Application_settings::set_max_buffer_coeff(int in_buffer_coeff)
{
    QString value;
    value.setNum(in_buffer_coeff);
    if (in_buffer_coeff > 0 && in_buffer_coeff < 100) // Range: ]0,100[
    {
        this->settings->setValue(MAX_BUFFER_COEFF_CFG, value);
    }
}

float
Application_settings::get_low_pass_filter_max_speed_usage()
{
    return this->settings->value(LOW_PASS_FILTER_MAX_SPEED_USAGE_CFG).toFloat();
}

float
Application_settings::get_low_pass_filter_max_speed_usage_default()
{
    return dscratch_get_default_low_pass_filter_max_speed_usage();
}

void
Application_settings::set_low_pass_filter_max_speed_usage(float in_low_pass_filter_max_speed_usage)
{
    QString value;
    value.setNum(in_low_pass_filter_max_speed_usage, 'f', 3);
    if (in_low_pass_filter_max_speed_usage > 0.0 && in_low_pass_filter_max_speed_usage < 2.0) // Range: ]0.0,2.0[
    {
        this->settings->setValue(LOW_PASS_FILTER_MAX_SPEED_USAGE_CFG, value);
    }
}

// TODO: implement settings management for timecode position detection.
//dscratch_set_coeff_right_dist_min_bit1_to_bit1
//dscratch_set_coeff_left_dist_min_bit1_to_bit1
//dscratch_set_coeff_right_dist_max_bit0_to_bit0
//dscratch_set_coeff_left_dist_max_bit0_to_bit0

//
// Playback parameters settings.
//

float
Application_settings::get_max_speed_diff()
{
    return this->settings->value(MAX_SPEED_DIFF_CFG).toFloat();
}

float
Application_settings::get_max_speed_diff_default()
{
    return dscratch_get_default_max_speed_diff();
}

void
Application_settings::set_max_speed_diff(float in_diff)
{
    QString value;
    value.setNum(in_diff, 'f', 3);
    if (in_diff > 0.0 && in_diff < 1.0) // Range: ]0.0,1.0[
    {
        this->settings->setValue(MAX_SPEED_DIFF_CFG, value);
    }
}

float
Application_settings::get_slow_speed_algo_usage()
{
    return this->settings->value(SLOW_SPEED_ALGO_USAGE_CFG).toFloat();
}

float
Application_settings::get_slow_speed_algo_usage_default()
{
    return dscratch_get_default_slow_speed_algo_usage();
}

void
Application_settings::set_slow_speed_algo_usage(float in_speed)
{
    QString value;
    value.setNum(in_speed, 'f', 3);
    if (in_speed > 0.0 && in_speed < 1.0) // Range: ]0.0,1.0[
    {
        this->settings->setValue(SLOW_SPEED_ALGO_USAGE_CFG, value);
    }
}

int
Application_settings::get_max_nb_speed_for_stability()
{
    return this->settings->value(MAX_NB_SPEED_STABIL_CFG).toInt();
}

int
Application_settings::get_max_nb_speed_for_stability_default()
{
    return dscratch_get_default_max_nb_speed_for_stability();
}

void
Application_settings::set_max_nb_speed_for_stability(int in_nb_speed)
{
    QString value;
    value.setNum(in_nb_speed);
    if (in_nb_speed > 0 && in_nb_speed < 100) // Range: ]0,100[
    {
        this->settings->setValue(MAX_NB_SPEED_STABIL_CFG, value);
    }
}

int
Application_settings::get_nb_cycle_before_changing_direction()
{
    return this->settings->value(NB_CYCLE_CHANGING_DIR_CFG).toInt();
}

int
Application_settings::get_nb_cycle_before_changing_direction_default()
{
    return dscratch_get_default_nb_cycle_before_changing_direction();
}

void
Application_settings::set_nb_cycle_before_changing_direction(int in_nb_cycle)
{
    QString value;
    value.setNum(in_nb_cycle);
    if (in_nb_cycle > 0 && in_nb_cycle < 100) // Range: ]0,100[
    {
        this->settings->setValue(NB_CYCLE_CHANGING_DIR_CFG, value);
    }
}

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

