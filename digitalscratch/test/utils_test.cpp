#include <QtTest>
#include <utils_test.h>
#include <iostream>

#define DATA_DIR     "./test/data/"
#define DATA_TRACK_1 "track_1.mp3"
#define DATA_TRACK_2 "track_2.mp3"
#define DATA_TRACK_3 "track_3.mp3"
#define DATA_TRACK_REAL1 "laurent_garnier_-_the_sound_of_the_big_babou.mp3"
#define DATA_TRACK_REAL2 "02_dig_your_own_hole.mp3"
#define DATA_TRACK_REAL3 "08-pendulum_-_tarantula_feat._fresh_pyda_and_tenor_fly.mp3"
#define DATA_TRACK_REAL4 "16-pendulum-tarantula.mp3"
#define DATA_TRACK_REAL5 "b2_the_sound_of_the_big_babou.mp3"
#define DATA_TRACK_REAL6 "commix_-_be_true.mp3"
#ifdef WIN32
    #define MUSIC_PATH   "D:/musique"
#else
    #define MUSIC_PATH   "/home/julien/Music/drum_n_bass"
#endif

Utils_Test::Utils_Test()
{
}

void Utils_Test::initTestCase()
{
}

void Utils_Test::cleanupTestCase()
{
}

void Utils_Test::testCaseGetFileHash()
{
    // Get hash of test files.
    QVERIFY2(Utils::get_file_hash(QString(DATA_DIR) + QString(DATA_TRACK_1), FILE_HASH_SIZE)
              == "18cc3115b3b4f43e71a4b8859d18fcc1", "track 1 hash");

    QVERIFY2(Utils::get_file_hash(QString(DATA_DIR) + QString(DATA_TRACK_2), FILE_HASH_SIZE)
              == "4fb8a48432061b1defa514e73087aace", "track 2 hash");

    QVERIFY2(Utils::get_file_hash(QString(DATA_DIR) + QString(DATA_TRACK_3), FILE_HASH_SIZE)
              == "f610ccedaf72485853b3829b3b88a38c", "track 3 hash");

    // Check bad input parameters.
    QVERIFY2(Utils::get_file_hash("", 200) == "",   "path does not exist, no hash");
    QVERIFY2(Utils::get_file_hash(NULL, 200) == "", "path is null, no hash");
    QVERIFY2(Utils::get_file_hash(QString(DATA_DIR) + QString(DATA_TRACK_1), 0) == "", "nb kbytes is 0");
}

void Utils_Test::testCaseGetFileHashCharge()
{
    //
    // Get hash of all mp3 files in big directory. Then check there is no duplicate hash.
    //

    // Iterate recursively over files in directory.
    QDir dir(MUSIC_PATH);
    QDirIterator i(dir.absolutePath(), QDirIterator::Subdirectories);
    QMultiMap<QString, QString> map;
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8")); // FIXME: Should be probably removed in Qt5.
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
                // Get hash of this mp3.
                hash = Utils::get_file_hash(i.filePath(), FILE_HASH_SIZE);

                // Store the hash in a map.
                map.insertMulti(hash, i.filePath());
            }
      }
    }

    // Check if there is no duplicate hash.
    QMapIterator<QString, QString> j(map);
    while (j.hasNext())
    {
        j.next();
        QList<QString> values = map.values(j.key());
        QString dup_file_hashes = "";
        if (values.size() > 1)
        {
            // Duplicate hash found.
            for (int k = 0; k < values.size(); ++k)
            {
                dup_file_hashes += values.at(k) + " | ";
            }
        }
        QVERIFY2(values.size() == 1, qPrintable("same hash for several audio tracks: " + dup_file_hashes));
    }
}

void Utils_Test::testCaseGetFileMusicKey()
{
    // Prepare list of audio file for getting music key concurrently.
    QList<QString> filepaths = QList<QString>();
    filepaths << QString(DATA_DIR) + QString(DATA_TRACK_REAL1);
    filepaths << QString(DATA_DIR) + QString(DATA_TRACK_REAL2);
    filepaths << QString(DATA_DIR) + QString(DATA_TRACK_REAL3);
    filepaths << QString(DATA_DIR) + QString(DATA_TRACK_REAL4);
    filepaths << QString(DATA_DIR) + QString(DATA_TRACK_REAL5);
    filepaths << QString(DATA_DIR) + QString(DATA_TRACK_REAL6);
    
    // Get music key for list of audio file.
    QList<QString> keys = QtConcurrent::blockingMapped(filepaths, Utils::get_file_music_key);
    
    // Check keys.
    QVERIFY2(keys.length() == 6, "size of list of keys");
    QVERIFY2(keys[0] == "Bbm", DATA_TRACK_REAL1);
    QVERIFY2(keys[1] == "Ebm", DATA_TRACK_REAL2);
    QVERIFY2(keys[2] == "Gbm", DATA_TRACK_REAL3);
    QVERIFY2(keys[3] == "Dm",  DATA_TRACK_REAL4);
    QVERIFY2(keys[4] == "Bbm", DATA_TRACK_REAL5);
    QVERIFY2(keys[5] == "Ebm", DATA_TRACK_REAL6);
}
