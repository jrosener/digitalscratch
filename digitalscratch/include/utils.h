/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------( utils.h )-*/
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
/*                         Static utility functions                           */
/*                                                                            */
/*============================================================================*/

#pragma once

#include <QString>
#include <QLocale>

#include "app/application_const.h"

using namespace std;

class Utils
{
 public:
    static QStringList audio_file_extensions;

 private:
    static QList<QString> minor_keys;
    static QList<QString> major_keys;

 private:
    static void setup_keys();

 public:
    // Get a MD5 hash from in_kbytes bytes of the specified file.
    static QString get_file_hash(const QString &path, const unsigned int &kbytes = 1);

    // Get full text content of a file.
    static QString file_read_all_text(const QString &path);

    // Compute music key of an audio file.
    static QString get_file_music_key(const QString &path);

    // Convert music key as clock number.
    static QString convert_music_key_to_clock_number(const QString &key);

    // Get next music keys (as clock number).
    static void get_next_music_keys(const QString &key,
                                    QString &next_key,
                                    QString &prev_key,
                                    QString &next_major_key);

    static QString get_str_time_from_sample_index(const unsigned int &sample_index,
                                                  const unsigned int &sample_rate,
                                                  const bool         &with_msec);

    // Get current CSS string stylesheet.
    static QString get_current_stylesheet_css();
    
    // ISO639 language code conversion.
    static QString language_to_iso639_code(const QLocale::Language &lang);
};
