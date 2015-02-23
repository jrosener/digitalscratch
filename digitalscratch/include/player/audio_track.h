/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------( audio_track.h )-*/
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
/*                    Class defining an audio track                           */
/*                                                                            */
/*============================================================================*/

#pragma once

#include <string>
#include <QObject>
#include <QString>
#include <application_const.h>
#include <application_logging.h>
using namespace std;

class Audio_track : public QObject
{
    Q_OBJECT

 private:
    unsigned int       sample_rate;               // Sample rate of decoded samples.
    short signed int  *samples;                   // Table of decoded samples.
    unsigned int       end_of_samples;            // The last filled sample in the table of samples.
    unsigned int       length;                    // Length of the track (ms).
    QString            name;                      // Name of the track.
    QString            path;                      // Path of the file.
    QString            filename;                  // Name of the file.
    unsigned int       max_nb_samples;            // Max number of decoded samples.
    QString            hash;                      // Hash of the first kbytes of the file.
    QString            music_key;                 // The main musical key of the track.
    QString            music_key_tag;             // The main musical key of the track (get from metadata tag).

 public:
    Audio_track(const unsigned int &sample_rate);       // Does not contains any samples.
    Audio_track(const short unsigned int &max_minutes,  // Contains the table of decoded audio samples.
                const unsigned int       &sample_rate);
    virtual ~Audio_track();

 public:
    void              reset();                                                // Reset internal track parameters.
    short signed int *get_samples() const;                                    // Get a pointer on table of samples.
    unsigned int      get_end_of_samples() const;                             // Get index of last used sample.
    bool              set_end_of_samples(const unsigned int &end_of_samples); // Set index of last used sample.
    unsigned int      get_max_nb_samples() const;                             // Get maximum number of samples.
    unsigned int      get_sample_rate() const;                                // Get sample rate.
    unsigned int      get_security_nb_samples() const;                        // Get number of samples used for decoding security purpose.
    unsigned int      get_length() const;                                     // Get length of the track (msec).
    QString           get_length_str() const;                                 // Get length of the track (min:sec:msec).
    QString           get_name() const;                                       // Get name of the track.
    bool              set_name(const QString &name);                          // Set name of the track (basically artist+track).
    QString           get_path() const;                                       // Get path of the track.
    bool              set_fullpath(const QString &fullpath);                  // Set full path of the track.
    QString           get_filename() const;                                   // Get name of the file.
    QString           get_hash() const;                                       // Get hash of the file.
    bool              set_hash(const QString &hash);                          // Set hash of the file.
    QString           get_music_key() const;                                  // Get music key of the track.
    bool              set_music_key(const QString &key);                      // Set music key of the track.
    QString           get_music_key_tag() const;                              // Get music key of the track (from tag).
    bool              set_music_key_tag(const QString &key_tag);              // Set music key of the track (from tag).

 signals:
    void name_changed(const QString &name);
    void key_changed(const QString &key);
};
