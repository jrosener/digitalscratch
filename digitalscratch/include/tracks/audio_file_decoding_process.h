/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*------------------------------------------( audio_file_decoding_process.h )-*/
/*                                                                            */
/*  Copyright (C) 2003-2016                                                   */
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
/*    Behavior class: process to decode an audio file (mp3, flac, ogg,...)    */
/*                                                                            */
/*============================================================================*/

#pragma once

#include <iostream>
#include <QFileInfo>
#include <QObject>
#include <QFile>
#include <QString>
#include <QSharedPointer>

#include "tracks/audio_track.h"
#include "app/application_const.h"

using namespace std;

class Audio_file_decoding_process : public QObject
{
    Q_OBJECT

 private:
    QSharedPointer<Audio_track> at;
    QFile                       file;
    bool                        do_resample;
    unsigned int                decoded_sample_rate;

 public:
    Audio_file_decoding_process(const QSharedPointer<Audio_track> &at,
                                const bool &do_resample = true);
    virtual ~Audio_file_decoding_process();

    void clear();
    bool run(const QString &path,
             const QString &file_hash = "",
             const QString &music_key = "");    // Make decoding of the audio file.

 private:
    void resample_track();                    // Change sample rate of the audio track.
    bool decode();                            // Internal audio decoding.

 signals:
    void name_changed(const QString &name);
    void key_changed(const QString &key);
};
