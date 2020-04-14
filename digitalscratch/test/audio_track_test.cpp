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
#include "audio_track_test.h"
#include "tracks/audio_track.h"
#include "tracks/audio_file_decoding_process.h"

#define DATA_DIR     "./test/data/"
#define DATA_TRACK_1 "track_1.mp3"
#define DATA_TRACK_2 "track_2.mp3"
#define DATA_TRACK_3 "track_éèà@ù&_3.mp3"


Audio_track_Test::Audio_track_Test()
{
}

void Audio_track_Test::initTestCase()
{
}

void Audio_track_Test::cleanupTestCase()
{
}

void Audio_track_Test::testCaseCreate()
{
    // Create an object.
    Audio_track *at = new Audio_track(15, 44100);

    // Reset track content.
    at->reset();
    QVERIFY2(at->get_name()           == "", "name is empty");
    QVERIFY2(at->get_length()         == 0,  "length is null");
    QVERIFY2(at->get_end_of_samples() == 0,  "end of sample is null");

    // Cleanup.
    delete at;
}

void Audio_track_Test::testCaseFillSamples()
{
    // Create a track.
    QSharedPointer<Audio_track> at(new Audio_track(15, 44100));

    // Create a track decoder and decode compressed audio data to the audio track object.
    Audio_file_decoding_process decoder(at, false);

    // Check decoded track content 1.
    at->reset();
    QVERIFY2(decoder.run(QString(DATA_DIR) + QString(DATA_TRACK_1), "", "") == true, "decode audio track 1");
    QVERIFY2(at->get_name() == DATA_TRACK_1, "track 1 name");
    QVERIFY2(at->get_length() > 0,           "track 1 length");
    QVERIFY2(at->get_end_of_samples() > 0,   "track 1 end of sample");

    // Check decoded track content 2.
    at->reset();
    QVERIFY2(decoder.run(QString(DATA_DIR) + QString(DATA_TRACK_2), "", "") == true, "decode audio track 2");
    QVERIFY2(at->get_name() == DATA_TRACK_2, "track 2 name");
    QVERIFY2(at->get_length() > 0,           "track 2 length");
    QVERIFY2(at->get_end_of_samples() > 0,   "track 2 end of sample");

    // Check decoded track content 3.
    at->reset();
    QVERIFY2(decoder.run(QString(DATA_DIR) + QString(DATA_TRACK_3), "", "") == true, "decode audio track 3");
    QVERIFY2(at->get_name() == DATA_TRACK_3, "track 3 name");
    QVERIFY2(at->get_length() > 0,           "track 3 length");
    QVERIFY2(at->get_end_of_samples() > 0,   "track 3 end of sample");
}

void Audio_track_Test::testCaseSetPath()
{
    // Create a track.
    Audio_track *at = new Audio_track(15, 44100);

    // Set path and check it.
   #ifdef WIN32
    QString path("C:/aé-.filepath/ù/ö");
   #else
    QString path("/aé-.filepath/ù/ö");
   #endif
    QString filename("filename.mp3");
    QVERIFY2(at->set_fullpath(path + '/' + filename) == true, "set path");
    QVERIFY2(at->get_path()     == path,     "get path");
    QVERIFY2(at->get_filename() == filename, "get filename");

    // Cleanup.
    delete at;
}

