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
#ifdef WIN32
    #define MUSIC_PATH   "D:/musique"
#else
    #define MUSIC_PATH   "/home/julien/Music/drum_n_bass"
#endif

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
    Audio_track *at = new Audio_track();
    Data_persistence *data_persist = &Singleton<Data_persistence>::get_instance();

    // Store track.
    QString fullpath = QString(DATA_DIR) + QString(DATA_TRACK_1);
    at->set_fullpath(fullpath);
    at->set_hash(Utils::get_file_hash(fullpath, FILE_HASH_SIZE));
    at->set_music_key("A1");
    QVERIFY2(data_persist->store_audio_track(at) == true, "audio track 1 store");

    // Store another track.
    fullpath = QString(DATA_DIR) + QString(DATA_TRACK_2);
    at->set_fullpath(fullpath);
    at->set_hash(Utils::get_file_hash(fullpath, FILE_HASH_SIZE));
    at->set_music_key("A2");
    QVERIFY2(data_persist->store_audio_track(at) == true, "audio track 2 store");

    // Modify last track.
    at->set_music_key("A21");
    QVERIFY2(data_persist->store_audio_track(at) == true, "change key and store");

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

void Data_persistence_Test::testCaseStoreAndGetATCharge()
{
    //
    // For each audio file in big directory, create an Audio Track and store it.
    // Get it as well to check data.
    //

    // Init.
    Data_persistence *data_persist = &Singleton<Data_persistence>::get_instance();
    Audio_track      *at_to_store  = new Audio_track();
    Audio_track      *at_from_db   = new Audio_track();

    // Iterate recursively over files in directory.
    QDir dir(MUSIC_PATH);
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
                hash = Utils::get_file_hash(i.filePath(), FILE_HASH_SIZE);
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

    // Cleanup.
    delete at_to_store;
    delete at_from_db;
}
