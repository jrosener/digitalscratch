/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                     Digital Scratch Player Test                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/*                                                                            */
/*  Copyright (C) 2003-2017                                                   */
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

#include <QtTest>
#include <QDesktopServices>
#include "data_persistence_test.h"
#include "singleton.h"
#include "utils.h"
#include "tracks/audio_file_decoding_process.h"
#include "tracks/data_persistence.h"

#define DATA_DIR     "./test/data/"
#define DATA_TRACK_1 "track_1.mp3"
#define DATA_TRACK_2 "track_2.mp3"
#define DATA_TRACK_3 "track_éèà@ù&_3.mp3"

Data_persistence_Test::Data_persistence_Test()
{
}

void Data_persistence_Test::initTestCase()
{
    // Remove database file.
    QFile::remove(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/digitalscratch.sqlite");

    // Get/create a data persistence static instance.
    Data_persistence *data_persist = &Singleton<Data_persistence>::get_instance();

    // Init DB.
    QVERIFY2(data_persist->is_initialized == true, "DB initialized");
}

void Data_persistence_Test::cleanupTestCase()
{
    // Cleanup.
}

void Data_persistence_Test::testCaseStoreAudioTrack()
{
    QSharedPointer<Audio_track> at(new Audio_track(44100));
    Data_persistence *data_persist = &Singleton<Data_persistence>::get_instance();

    // Store track.
    QString fullpath = QString(DATA_DIR) + QString(DATA_TRACK_1);
    at->set_fullpath(fullpath);
    at->set_hash(Utils::get_file_hash(fullpath));
    at->set_music_key("A1");
    QVERIFY2(data_persist->store_audio_track(at) == true, "audio track 1 store");

    // Store another track.
    fullpath = QString(DATA_DIR) + QString(DATA_TRACK_2);
    at->set_fullpath(fullpath);
    at->set_hash(Utils::get_file_hash(fullpath));
    at->set_music_key("A2");
    QVERIFY2(data_persist->store_audio_track(at) == true, "audio track 2 store");

    // Modify last track.
    at->set_music_key("A21");
    QVERIFY2(data_persist->store_audio_track(at) == true, "change key and store");
}

void Data_persistence_Test::testCaseGetAudioTrack()
{
    // Insert a test audio track.
    Data_persistence *data_persist = &Singleton<Data_persistence>::get_instance();
    QSharedPointer<Audio_track> at(new Audio_track(44100));
    QString fullpath = QString(DATA_DIR) + QString(DATA_TRACK_1);
    at->set_fullpath(fullpath);
    at->set_hash(Utils::get_file_hash(fullpath));
    at->set_music_key("A1");
    QVERIFY2(data_persist->store_audio_track(at) == true, "audio track store");

    // Get this audio track.
    QSharedPointer<Audio_track> at_from_db(new Audio_track(44100));
    at_from_db->set_hash(at->get_hash());
    QVERIFY2(data_persist->get_audio_track(at_from_db) == true,  "get audio track");
    QVERIFY2(at_from_db->get_path()      == at->get_path(),      "path from DB");
    QVERIFY2(at_from_db->get_filename()  == at->get_filename(),  "filename from DB");
    QVERIFY2(at_from_db->get_music_key() == at->get_music_key(), "key from DB");

    // Get not exising audio track.
    at_from_db->reset();
    at_from_db->set_hash("1234567890");
    QVERIFY2(data_persist->get_audio_track(at_from_db) == false,  "get audio track, not found");
    QVERIFY2(at_from_db->get_path()      == "", "no path from DB");
    QVERIFY2(at_from_db->get_filename()  == "", "no filename from DB");
    QVERIFY2(at_from_db->get_music_key() == "", "no key from DB");
}

void Data_persistence_Test::testCaseStoreAndGetATCharge()
{
    //
    // For each audio file in big directory, create an Audio Track and store it.
    // Then get it to check data.
    //

    // Init.
    Data_persistence *data_persist = &Singleton<Data_persistence>::get_instance();
    QSharedPointer<Audio_track> at_to_store(new Audio_track(44100));
    QSharedPointer<Audio_track> at_from_db(new Audio_track(44100));

    // Iterate recursively over files in directory.
    QDir dir(QStandardPaths::locate(QStandardPaths::MusicLocation, QString(), QStandardPaths::LocateDirectory));
    if (dir.exists() == true) // Skip the test if there are no test data.
    {
        QDirIterator i(dir.absolutePath(), QDirIterator::Subdirectories);
        QMultiMap<QString, QString> map;
        QString hash("");
        while (i.hasNext())
        {
            // Go to the next element (file or directory).
            i.next();
            if (i.fileInfo().isDir() == false)
            {
                // The element is a file.
                QString filename = i.fileName();
                if (filename.endsWith(".mp3") == true)
                {
                    // Prepare the audio track.
                    at_to_store->reset();
                    hash = Utils::get_file_hash(i.filePath());
                    at_to_store->set_hash(hash);
                    at_to_store->set_fullpath(i.filePath());
                    at_to_store->set_music_key("A1");

                    // Store the audio track.
                    QVERIFY2(data_persist->store_audio_track(at_to_store) == true, qPrintable(QString("store track ") + i.filePath()));

                    // Update audio track.
                    at_to_store->set_music_key("B1");
                    QVERIFY2(data_persist->store_audio_track(at_to_store) == true, qPrintable(QString("update track ") + i.filePath()));

                    // Get data in another Audio track object and compare values.
                    at_from_db->reset();
                    at_from_db->set_hash(hash);
                    QVERIFY2(data_persist->get_audio_track(at_from_db) == true,           qPrintable(QString("get audio track") + i.filePath()));
                    QVERIFY2(at_from_db->get_path()      == at_to_store->get_path(),      qPrintable(QString("path from DB") + i.filePath()));
                    QVERIFY2(at_from_db->get_filename()  == at_to_store->get_filename(),  qPrintable(QString("filename from DB") + i.filePath()));
                    QVERIFY2(at_from_db->get_music_key() == at_to_store->get_music_key(), qPrintable(QString("key from DB") + i.filePath()));
                }
            }
        }
    }
}

void Data_persistence_Test::testCaseStoreAndGetCuePoint()
{
    // Get DB instance.
    Data_persistence *data_persist = &Singleton<Data_persistence>::get_instance();

    // Precondition: store a track (if not already there).
    QSharedPointer<Audio_track> at(new Audio_track(15, 44100));
    Audio_file_decoding_process decoder(at, false);
    QString fullpath = QString(DATA_DIR) + QString(DATA_TRACK_1);
    decoder.run(fullpath, Utils::get_file_hash(fullpath), "A1");
    QVERIFY2(data_persist->store_audio_track(at) == true, "store audio track");

    // Store cue point: wrong params.
    QSharedPointer<Audio_track> at_wrong(new Audio_track(15, 44100));
    QVERIFY2(data_persist->store_cue_point(at_wrong, 0, 1234)                  == false, "wrong audio track");
    QVERIFY2(data_persist->store_cue_point(at,       MAX_NB_CUE_POINTS , 1234) == false, "too high cue point number");
    QVERIFY2(data_persist->store_cue_point(at,       0, at->get_length() + 1)  == false, "bad cue point position");

    // Store a cue point for this track.
    QVERIFY2(data_persist->store_cue_point(at, 0, 1234)  == true,  "store cue point 1");
    QVERIFY2(data_persist->store_cue_point(at, 1, 4567)  == true,  "store cue point 2");
    QVERIFY2(data_persist->store_cue_point(at, 2, 8910)  == true,  "store cue point 3");
    QVERIFY2(data_persist->store_cue_point(at, 3, 1112)  == true,  "store cue point 4");

    // Get cue point: wrong params.
    unsigned int position = 0;
    QVERIFY2(data_persist->get_cue_point(at_wrong, 0, position) == false, "wrong audio track");
    QVERIFY2(data_persist->get_cue_point(at, MAX_NB_CUE_POINTS, position) == false, "too high cue point number");

    // Get cue point.
    QVERIFY2(data_persist->get_cue_point(at, 0, position) == true,  "get cue point 1");
    QVERIFY2(position == 1234, "position 1");

    // Update cue point.
    QVERIFY2(data_persist->store_cue_point(at, 0, 1314)  == true,  "update cue point 1");

    // Get updated cue point.
    QVERIFY2(data_persist->get_cue_point(at, 0, position) == true,  "get updated cue point 1");
    QVERIFY2(position == 1314, "updated position 1");

    // Delete cue point.
    QVERIFY2(data_persist->delete_cue_point(at, 1)        == true,  "delete updated cue point 2");
    QVERIFY2(data_persist->get_cue_point(at, 1, position) == false, "get cue point 2");
}

void Data_persistence_Test::testCasePersistTag()
{
    Data_persistence *data_persist = &Singleton<Data_persistence>::get_instance();

    // Store new tag.
    QVERIFY2(data_persist->store_tag("tag1") == true, "store new tag1");
    QVERIFY2(data_persist->store_tag("tag2") == true, "store new tag2");
    // Try to store existing tag (should not fail).
    QVERIFY2(data_persist->store_tag("tag1") == true, "store existing tag1");

    // Get tag list.
    QStringList tags;
    data_persist->get_full_tag_list(tags);
    QVERIFY2(tags.size() == 2, "nb tags = 2");
    QVERIFY2(tags[0] == "tag1", "tag[0] = tag1");
    QVERIFY2(tags[1] == "tag2", "tag[1] = tag2");

    // Rename existing tag.
    QVERIFY2(data_persist->rename_tag("tag1", "tag renamed") == true, "rename existing tag1");
    // Try renaming missing tag (should fail).
    QVERIFY2(data_persist->rename_tag("tag missing", "tag renamed") == false, "rename missing tag");

    // Get tag list.
    tags.clear();
    data_persist->get_full_tag_list(tags);
    QVERIFY2(tags.size() == 2, "nb tags = 2");
    QVERIFY2(tags[0] == "tag renamed", "tag[0] = tag renamed");
    QVERIFY2(tags[1] == "tag2", "tag[1] = tag2");

    // Delete existing tag.
    QVERIFY2(data_persist->delete_tag("tag renamed") == true, "delete existing tag renamed");
    QVERIFY2(data_persist->delete_tag("tag2") == true, "delete existing tag2");
    // Try deleting missing tag.
    QVERIFY2(data_persist->delete_tag("tag2") == false, "delete missing tag2");

    // Get tag list.
    tags.clear();
    data_persist->get_full_tag_list(tags);
    QVERIFY2(tags.size() == 0, "nb tags = 0");

    // Store again 2 tags.
    QVERIFY2(data_persist->store_tag("house") == true, "store new tag house");
    QVERIFY2(data_persist->store_tag("techno") == true, "store new tag techno");

    // Store 2 tracks.
    QSharedPointer<Audio_track> at1(new Audio_track(44100));
    QString fullpath = QString(DATA_DIR) + QString(DATA_TRACK_1);
    at1->set_fullpath(fullpath);
    at1->set_hash(Utils::get_file_hash(fullpath));
    at1->set_music_key("A1");
    QVERIFY2(data_persist->store_audio_track(at1) == true, "audio track 1 store");
    QSharedPointer<Audio_track> at2(new Audio_track(44100));
    fullpath = QString(DATA_DIR) + QString(DATA_TRACK_2);
    at2->set_fullpath(fullpath);
    at2->set_hash(Utils::get_file_hash(fullpath));
    at2->set_music_key("A2");
    QVERIFY2(data_persist->store_audio_track(at2) == true, "audio track 2 store");

    // Add tags to track and check them.
    QVERIFY2(data_persist->add_tag_to_track(at1, "techno") == true, "Add tag techno to track 1");
    QVERIFY2(data_persist->add_tag_to_track(at1, "techno") == true, "Add tag techno twice to track 1");
    QVERIFY2(data_persist->add_tag_to_track(at1, "house") == true, "Add tag house to track 1");
    tags.clear();
    data_persist->get_tags_from_track(at1, tags);
    QVERIFY2(tags.size() == 2, "nb tags = 2");
    QVERIFY2(tags[0] == "house", "tags[0] = house");
    QVERIFY2(tags[1] == "techno",  "tags[1] = techno");
    QVERIFY2(data_persist->rem_tag_from_track(at1, "house") == true, "Delete tag house from track 1");
    tags.clear();
    data_persist->get_tags_from_track(at1, tags);
    QVERIFY2(tags.size() == 1, "nb tags = 1");
    QVERIFY2(tags[0] == "techno", "tags[0] = techno");

    QVERIFY2(data_persist->add_tag_to_track(at2, "house") == true, "Add tag house to track 2");
    tags.clear();
    data_persist->get_tags_from_track(at2, tags);
    QVERIFY2(tags.size() == 1, "nb tags = 1");
    QVERIFY2(tags[0] == "house", "tags[0] = house");

    // Get track list of a specified tag.
    QStringList tracklist;
    tracklist.clear();
    data_persist->get_tracks_from_tag("techno", tracklist);
    QVERIFY2(tracklist.size() == 1, "nb techno tracks = 1");
    QVERIFY2(tracklist[0] == QFileInfo(QString(DATA_DIR) + QString(DATA_TRACK_1)).absoluteFilePath(), "tracklist[0] = track_1.mp3");

    tracklist.clear();
    data_persist->get_tracks_from_tag("house", tracklist);
    QVERIFY2(tracklist.size() == 1, "nb house tracks = 1");
    QVERIFY2(tracklist[0] == QFileInfo(QString(DATA_DIR) + QString(DATA_TRACK_2)).absoluteFilePath(), "tracklist[0] = track_2.mp3");

    // Reorganize position of tracks in tag tracklists.
    QSharedPointer<Audio_track> at3(new Audio_track(44100));
    fullpath = QString(DATA_DIR) + QString(DATA_TRACK_3);
    at3->set_fullpath(fullpath);
    at3->set_hash(Utils::get_file_hash(fullpath));
    at3->set_music_key("A1");
    QVERIFY2(data_persist->store_audio_track(at3) == true, "audio track 3 store");
    data_persist->store_tag("dnb");
    data_persist->add_tag_to_track(at1, "house");
    data_persist->add_tag_to_track(at1, "techno");
    data_persist->add_tag_to_track(at1, "dnb");
    data_persist->add_tag_to_track(at2, "house");
    data_persist->add_tag_to_track(at2, "techno");
    data_persist->add_tag_to_track(at3, "house");
    QVERIFY2(data_persist->switch_track_positions_in_tag_list("house", at1, at2) == true, "switch position of track_1 and track_2 for the house tag");
    tracklist.clear();
    data_persist->get_tracks_from_tag("house", tracklist);
    QVERIFY2(tracklist.size() == 3, "nb house tracks = 3");
    QVERIFY2(tracklist[0] == QFileInfo(QString(DATA_DIR) + QString(DATA_TRACK_1)).absoluteFilePath(), "tracklist[0] = track_1.mp3");
    QVERIFY2(tracklist[1] == QFileInfo(QString(DATA_DIR) + QString(DATA_TRACK_2)).absoluteFilePath(), "tracklist[0] = track_2.mp3");
}

