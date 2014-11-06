#include <QtTest>
#include <data_persistence_test.h>
#include <data_persistence.h>
#include <singleton.h>
#include <QDesktopServices>
#include <utils.h>
#include <audio_file_decoding_process.h>

#define DATA_DIR     "./test/data/"
#define DATA_TRACK_1 "track_1.mp3"
#define DATA_TRACK_2 "track_2.mp3"
#define DATA_TRACK_3 "track_éèà@ù&_3.mp3"
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
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    QFile::remove(QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "/digitalscratch.sqlite");
#else
    QFile::remove(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/digitalscratch.sqlite");
#endif

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
}

void Data_persistence_Test::testCaseGetAudioTrack()
{
    // Insert a test audio track.
    Data_persistence *data_persist = &Singleton<Data_persistence>::get_instance();
    QSharedPointer<Audio_track> at(new Audio_track(44100));
    QString fullpath = QString(DATA_DIR) + QString(DATA_TRACK_1);
    at->set_fullpath(fullpath);
    at->set_hash(Utils::get_file_hash(fullpath, FILE_HASH_SIZE));
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
    // Get it as well to check data.
    //

    // Init.
    Data_persistence *data_persist = &Singleton<Data_persistence>::get_instance();
    QSharedPointer<Audio_track> at_to_store(new Audio_track(44100));
    QSharedPointer<Audio_track> at_from_db(new Audio_track(44100));

    // Iterate recursively over files in directory.
    QDir dir(MUSIC_PATH);
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
    decoder.run(fullpath, Utils::get_file_hash(fullpath, FILE_HASH_SIZE), "A1");
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
    QVERIFY2(data_persist->get_cue_point(at,       MAX_NB_CUE_POINTS, position) == false, "too high cue point number");

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
