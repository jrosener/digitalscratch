#include <QtTest>
#include <data_persistence_test.h>
#include <data_persistence.h>
#include <singleton.h>
#include <QDesktopServices>
#include <utils.h>

#define DATA_DIR     "./test/data/"
#define DATA_TRACK_1 "track_1.mp3"
#define DATA_TRACK_2 "track_2.mp3"
#define DATA_TRACK_3 "track_3.mp3"

Data_persistence_Test::Data_persistence_Test()
{
}

void Data_persistence_Test::initTestCase()
{
    // Remove database file.
    QFile::remove(QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "/digitalscratch.sqlite");

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
    Audio_track *at = new Audio_track();
    Data_persistence *data_persist = &Singleton<Data_persistence>::get_instance();

    // Store track.
    QString fullpath = QString(DATA_DIR) + QString(DATA_TRACK_1);
    at->set_fullpath(fullpath);
    at->set_hash(Utils::get_file_hash(fullpath, FILE_HASH_SIZE));
    at->set_music_key("A1");
    QVERIFY2(data_persist->store_audio_track(at) == true, "audio track store");

    // Store another track.
    fullpath = QString(DATA_DIR) + QString(DATA_TRACK_2);
    at->set_fullpath(fullpath);
    at->set_hash(Utils::get_file_hash(fullpath, FILE_HASH_SIZE));
    at->set_music_key("A2");
    QVERIFY2(data_persist->store_audio_track(at) == true, "audio track store");

    // Modify last track.
    at->set_music_key("A21");
    QVERIFY2(data_persist->store_audio_track(at) == true, "audio track store");

    // Cleanup.
    delete at;
}

void Data_persistence_Test::testCaseGetAudioTrack()
{
    // Insert a test audio track.
    Data_persistence *data_persist = &Singleton<Data_persistence>::get_instance();
    Audio_track *at  = new Audio_track();
    QString fullpath = QString(DATA_DIR) + QString(DATA_TRACK_1);
    at->set_fullpath(fullpath);
    at->set_hash(Utils::get_file_hash(fullpath, FILE_HASH_SIZE));
    at->set_music_key("A1");
    QVERIFY2(data_persist->store_audio_track(at) == true, "audio track store");

    // Get this audio track.
    Audio_track *at_from_db = new Audio_track();
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

    // Cleanup.
    delete at;
    delete at_from_db;
}
