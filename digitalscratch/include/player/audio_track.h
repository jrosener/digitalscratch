/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------( audio_track.h )-*/
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
/*                    Class defining an audio track                           */
/*                                                                            */
/*============================================================================*/

#ifndef AUDIO_TRACK_H_
#define AUDIO_TRACK_H_

#include <string>
#include <QObject>
#include <QString>
#include <application_const.h>
using namespace std;

#define SAMPLE_RATE         44100                  // Fixed sample rate.
#define SECURITY_NB_SAMPLES (2 * 10 * SAMPLE_RATE) // Number of samples added at the end of *samples for
                                                   //  decoding purpose.

class Audio_track : public QObject
{
    Q_OBJECT

 private:
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
    Audio_track();                                  // Does not contains any samples.
    Audio_track(short unsigned int in_max_minutes); // Contains the table of decoded audio samples.
    virtual ~Audio_track();

 public:
    void              reset();                                            // Reset internal track parameters.
    short signed int *get_samples();                                      // Get a pointer on table of samples.
    unsigned int      get_end_of_samples();                               // Get index of last used sample.
    bool              set_end_of_samples(unsigned int in_end_of_samples); // Set index of last used sample.
    unsigned int      get_max_nb_samples();                               // Get maximum number of samples.
    signed int        get_sample_rate();                                  // Get sample rate.
    unsigned int      get_security_nb_samples();                          // Get number of samples used for decoding security purpose.
    unsigned int      get_length();                                       // Get length of the track (msec).
    QString           get_name();                                         // Get name of the track.
    bool              set_name(QString in_name);                          // Set name of the track (basically artist+track).
    QString           get_path();                                         // Get path of the track.
    bool              set_fullpath(QString in_fullpath);                  // Set full path of the track.
    QString           get_filename();                                     // Get name of the file.
    QString           get_hash();                                         // Get hash of the file.
    bool              set_hash(QString in_hash);                          // Set hash of the file.
    QString           get_music_key();                                    // Get music key of the track.
    bool              set_music_key(QString in_key);                      // Set music key of the track.
    QString           get_music_key_tag();                                // Get music key of the track (from tag).
    bool              set_music_key_tag(QString in_key_tag);              // Set music key of the track (from tag).

 signals:
    void name_changed(QString in_name);
    void key_changed(QString in_key);
};

#endif /* AUDIO_TRACK_H_ */
