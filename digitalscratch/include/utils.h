/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------( utils.h )-*/
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
/*                         Static utility functions                           */
/*                                                                            */
/*============================================================================*/

#ifndef UTILS_H_
#define UTILS_H_

#include <QString>
#include <application_const.h>
#include <audio_track.h>
using namespace std;

class Utils
{
 private:
    static QList<QString> minor_keys;
    static QList<QString> major_keys;

 private:
    static void setup_keys();

 public:
    // Get a MD5 hash from in_kbytes bytes of the specified file.
    static QString get_file_hash(QString in_path, unsigned int in_kbytes);

    // Get full text content of a file.
    static QString file_read_all_text(QString in_path);

    // Compute music key of an audio file.
    static QString get_file_music_key(QString in_path);

    // Convert music key as clock number.
    static QString convert_music_key_to_clock_number(QString in_key);

    // Get next music keys (as clock number).
    static void get_next_music_keys(QString  in_key,
                                    QString& next_key,
                                    QString& prev_key,
                                    QString& next_major_key);

    static QString get_str_time_from_sample_index(unsigned int in_sample_index,
                                                  unsigned int in_sample_rate,
                                                  bool         in_with_msec);

    // Get current CSS string stylesheet.
    static QString get_current_stylesheet_css();
};

#endif /* UTILS_H_ */
