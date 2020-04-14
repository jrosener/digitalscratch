/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                     Digital Scratch Player Test                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
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
/*============================================================================*/

#include <QString>
#include <QtTest>
#include "audio_file_decoding_process_test.h"
#include "tracks/audio_track.h"
#include "tracks/audio_file_decoding_process.h"
#include "utils.h"

#define DATA_DIR     "./test/data/"
#define DATA_TRACK_1 "track_1.mp3"
#define DATA_TRACK_2 "b_comp_-_p_dust.mp3"

Audio_file_decoding_process_Test::Audio_file_decoding_process_Test()
{
}

void Audio_file_decoding_process_Test::initTestCase()
{
}

void Audio_file_decoding_process_Test::cleanupTestCase()
{
}

void Audio_file_decoding_process_Test::testCaseCreate()
{
    // Create an object.
    QSharedPointer<Audio_track> at(new Audio_track(15, 44100));
    Audio_file_decoding_process decoder(at, false);
}

void Audio_file_decoding_process_Test::testCaseRun()
{
    // Create the audio track.
    QSharedPointer<Audio_track> at(new Audio_track(15, 44100));
    Audio_file_decoding_process decoder(at, false);

    // Run decoding with bad parameters.
    QVERIFY2(decoder.run("", "abcd", "A1") == false, "bad file path");

    // Run decoding on small file.
    QFileInfo file_info = QFileInfo(QString(DATA_DIR) + QString(DATA_TRACK_1));
    QString   fullpath  = file_info.absoluteFilePath();
    QString   hash      = Utils::get_file_hash(fullpath);
    QString   key       = "A1";
    decoder.clear();
    QVERIFY2(decoder.run(fullpath, hash, key) == true,   "decode small mp3");
    QVERIFY2(at->get_path() == file_info.absolutePath(), "check path of small mp3");
    QVERIFY2(at->get_hash() == hash,                     "check file hash of small mp3");
    QVERIFY2(at->get_music_key() == key,                 "check music key of small mp3");

    // Run decoding on normal file.
    QFileInfo file_info_2 = QFileInfo(QString(DATA_DIR) + QString(DATA_TRACK_2));
    decoder.clear();
    QVERIFY2(decoder.run(file_info_2.absoluteFilePath(), "", "") == true,  "decode normal sized mp3");
}

