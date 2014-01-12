/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*--------------------------------------------------------------( utils.cpp )-*/
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

#include <utils.h>
#include <QCryptographicHash>
#include <algorithm>
#include <QtDebug>
#include <QFile>
#include <QStringList>
#include <audio_track.h>
#include <audio_file_decoding_process.h>
#include <audio_track_key_process.h>
#include <application_settings.h>
#include <singleton.h>

// Global static data.
QStringList Utils::audio_file_extensions = QStringList() << "ac3" << "flac" << "m4a" << "mp2" << "mp3" << "ogg" << "wav" << "wma";

// Static utils functions.
QString Utils::get_file_hash(QString in_path, unsigned int in_kbytes)
{   
    qDebug() << "Utils::get_file_hash...";

    // Init.
    QString hash("");

    // Check if path is defined.
    if (in_path == NULL)
    {
        qWarning() << "Utils::get_file_hash: path is NULL.";
        return "";
    }

    // Check number of kbytes.
    if (in_kbytes == 0)
    {
        qWarning() << "Utils::get_file_hash: nb bytes to hash is 0.";
        return "";
    }

    // Check if file exists.
    QFile file(in_path);
    if (file.exists() == false)
    {
        qWarning() << "Utils::get_file_hash: file " << in_path << " does not exists.";
        return "";
    }

    // Open file as binary.
    if (file.open(QIODevice::ReadOnly) == false)
    {
        return "";
    }

    // Get a hash of the first bytes of data (or the size of the file if it is less).
    QByteArray bin  = file.read(std::min((qint64)(in_kbytes*1024), file.size()));
    hash = QString(QCryptographicHash::hash(bin, QCryptographicHash::Md5).toHex());

    // Cleanup.
    file.close();

    qDebug() << "Utils::get_file_hash: done.";

    return hash;
}

QString Utils::file_read_all_text(QString in_path)
{
    QString result = "";

    QFile file(in_path);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text) == true)
    {
        result = QString(file.readAll());
        file.close();
    }

    return result;

}

QString Utils::get_file_music_key(QString in_path)
{
    // Init result.
    QString result = "";

    // Decode the audio track.
    Audio_track *at = new Audio_track(10, 44100); // Force 44100 to calculate music key.
    Audio_file_decoding_process *dec = new Audio_file_decoding_process(at, false);
    dec->run(in_path, "", "");

    // Compute the music key.
    Audio_track_key_process *key_proc = new Audio_track_key_process(at);
    if (key_proc->run() == true)
    {
        result = at->get_music_key();
    }
    else
    {
        qWarning() << "Utils::get_file_music_key: cannot get music key for " << in_path;
    }

    // Cleanup.
    delete dec;
    delete at;
    delete key_proc;
    
    // Return result.
    return result;
}

QString Utils::convert_music_key_to_clock_number(QString in_key)
{
    QMap<QString, QString> key_map; // Map music key to clock number.
    key_map.insert("AM",  "11B");
    key_map.insert("Am",  "8A");
    key_map.insert("BbM", "6B");
    key_map.insert("Bbm", "3A");
    key_map.insert("BM",  "1B");
    key_map.insert("Bm",  "10A");
    key_map.insert("CM",  "8B");
    key_map.insert("Cm",  "5A");
    key_map.insert("DbM", "3B");
    key_map.insert("Dbm", "12A");
    key_map.insert("DM",  "10B");
    key_map.insert("Dm",  "7A");
    key_map.insert("EbM", "5B");
    key_map.insert("Ebm", "2A");
    key_map.insert("EM",  "12B");
    key_map.insert("Em",  "9A");
    key_map.insert("FM",  "7B");
    key_map.insert("Fm",  "4A");
    key_map.insert("GbM", "2B");
    key_map.insert("Gbm", "11A");
    key_map.insert("GM",  "9B");
    key_map.insert("Gm",  "6A");
    key_map.insert("AbM", "4B");
    key_map.insert("Abm", "1A");

    return key_map.value(in_key, "");
}

QList<QString> Utils::minor_keys;
QList<QString> Utils::major_keys;

void Utils::setup_keys()
{
    minor_keys.append("1A");
    minor_keys.append("2A");
    minor_keys.append("3A");
    minor_keys.append("4A");
    minor_keys.append("5A");
    minor_keys.append("6A");
    minor_keys.append("7A");
    minor_keys.append("8A");
    minor_keys.append("9A");
    minor_keys.append("10A");
    minor_keys.append("11A");
    minor_keys.append("12A");

    major_keys.append("1B");
    major_keys.append("2B");
    major_keys.append("3B");
    major_keys.append("4B");
    major_keys.append("5B");
    major_keys.append("6B");
    major_keys.append("7B");
    major_keys.append("8B");
    major_keys.append("9B");
    major_keys.append("10B");
    major_keys.append("11B");
    major_keys.append("12B");
}

void Utils::get_next_music_keys(QString  in_key,
                                QString& next_key,
                                QString& prev_key,
                                QString& next_major_key)
{
    QString         result = "";
    QList<QString> *keys   = NULL;

    // Init minor/major key list if not already done.
    if (minor_keys.size() == 0)
    {
        Utils::setup_keys();
    }

    // Init returned keys.
    next_key       = "";
    prev_key       = "";
    next_major_key = "";

    if (in_key.length() >= 2)
    {
        int index = 0;

        // Try to find music key in minor keys.
        index = minor_keys.indexOf(in_key);

        if (index == -1)
        {
            // Key not found, try to find music key in major keys.
            index = major_keys.indexOf(in_key);

            if (index == -1)
            {
                qWarning() << "Utils::get_next_music_keys: cannot find next key of " << in_key;
            }
            else
            {
                // Key is major.
                keys = &major_keys;
            }
        }
        else
        {
            // Key is minor.
            keys = &minor_keys;
        }

        if (keys != NULL)
        {
            // Get the next key from minor or major list.
            if ((index + 1) > (keys->size() - 1))
            {
                next_key = keys->first(); // key is the last one, so the next one is the first in the list.
            }
            else
            {
                next_key = keys->at(index + 1);
            }

            // Get the previous key from minor or major list.
            if ((index - 1) < 0)
            {
                prev_key = keys->last(); // key is the first one, so the previous one is the last in the list.
            }
            else
            {
                prev_key = keys->at(index - 1);
            }

            // Get the opposite (minor/major) key.
            if (keys == &minor_keys)
            {
                next_major_key = major_keys.at(index);
            }
            else
            {
                next_major_key = minor_keys.at(index);
            }
        }
    }

    return;
}

QString Utils::get_str_time_from_sample_index(unsigned int in_sample_index,
                                              unsigned int in_sample_rate,
                                              bool         in_with_msec)
{
    // Calculate sample index position as min:sec:msec
    unsigned int msec = (unsigned int)(1000.0 * (float)(in_sample_index) / (2.0 * (float)in_sample_rate));
    int          sec  = msec / 1000.0;
    div_t        tmp_division = div(sec, 60);
    QString min_str  = QString::number(tmp_division.quot);
    QString sec_str  = QString::number(tmp_division.rem);
    QString msec_str = QString::number(msec).right(3);

    if (min_str.size() == 1)
    {
        min_str = "0" + min_str;
    }
    if (sec_str.size() == 1)
    {
        sec_str = "0" + sec_str;
    }
    QString pos = min_str + ":" + sec_str;
    if (in_with_msec == true)
    {
        if (msec_str.size() == 1)
        {
            msec_str = "0" + msec_str;
        }
        if (msec_str.size() == 2)
        {
            msec_str = "0" + msec_str;
        }
        pos += ":" + msec_str;
    }

    return pos;
}

QString Utils::get_current_stylesheet_css()
{
    QString result = "";

    // Get reference on application settings.
    Application_settings *settings = &Singleton<Application_settings>::get_instance();

    if (settings->get_gui_style() == QString(GUI_STYLE_DARK))
    {
        result = Utils::file_read_all_text(GUI_STYLE_DARK_CSS);
    }

    return result;
}
