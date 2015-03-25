#include <QtTest>
#include <iostream>
#include <QtConcurrentMap>
#include <QStandardPaths>

#include "utils_test.h"

#define DATA_DIR     "./test/data/"
#define DATA_TRACK_1 "track_1.mp3"
#define DATA_TRACK_2 "track_2.mp3"
#define DATA_TRACK_3 "track_éèà@ù&_3.mp3"

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
    QVERIFY2(Utils::get_file_hash(QString(DATA_DIR) + QString(DATA_TRACK_1))
              == "10f96d453a96fd08874d1940be4fbeb1", "track 1 hash");

    QVERIFY2(Utils::get_file_hash(QString(DATA_DIR) + QString(DATA_TRACK_2))
              == "d34d5bc6792be6d1e5a70919e053cff6", "track 2 hash");

    QVERIFY2(Utils::get_file_hash(QString(DATA_DIR) + QString(DATA_TRACK_3))
              == "e5f389b7d18f81df5a6144cbaae21bb9", "track 3 hash");

    // Check bad input parameters.
    QVERIFY2(Utils::get_file_hash("", 200) == "", "path does not exist, no hash");
    QVERIFY2(Utils::get_file_hash(nullptr, 200) == "", "path is null, no hash");
    QVERIFY2(Utils::get_file_hash(QString(DATA_DIR) + QString(DATA_TRACK_1), 0) == "", "nb kbytes is 0");
}

void Utils_Test::testCaseGetFileHashCharge()
{
    //
    // Get hash of all mp3 files in big directory. Then check there is no duplicate hash.
    //

    // Iterate recursively over files in directory.
    QString music_path = QStandardPaths::locate(QStandardPaths::MusicLocation, QString(), QStandardPaths::LocateDirectory);
    if ((music_path.isEmpty() == false) && (music_path != ".")) // Skip the test if ther is no default music directory.
    {
        QDir dir(music_path);
        if (dir.exists() == true)
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
                        // Get hash of this mp3.
                        hash = Utils::get_file_hash(i.filePath());

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
                QVERIFY2(values.size() == 1, qPrintable("In " + dir.absolutePath() + " same hash for several audio tracks: " + dup_file_hashes));
            }
        }
    }
}

void Utils_Test::testCaseGetFileMusicKey()
{
    // Prepare list of audio file for getting music key concurrently.
    QList<QString> filepaths = QList<QString>();
    filepaths << QString(DATA_DIR) + QString(DATA_TRACK_1);
    filepaths << QString(DATA_DIR) + QString(DATA_TRACK_2);
    filepaths << QString(DATA_DIR) + QString(DATA_TRACK_3);
    
    // Get music key for list of audio file.
    QList<QString> keys = QtConcurrent::blockingMapped(filepaths, Utils::get_file_music_key);
    
    // Check keys.
    QVERIFY2(keys.length() == 3, "size of list of keys");
    QVERIFY2(keys[0] == "1A",  qPrintable(QString(DATA_TRACK_1) + QString(" key: ") + QString(keys[0])));
    QVERIFY2(keys[1] == "10A", qPrintable(QString(DATA_TRACK_1) + QString(" key: ") + QString(keys[1])));
    QVERIFY2(keys[2] == "1A",  qPrintable(QString(DATA_TRACK_1) + QString(" key: ") + QString(keys[2])));
}

void Utils_Test::testCaseGetNextMusicKeys()
{
    QString next  = "";
    QString prev  = "";
    QString oppos = "";

    Utils::get_next_music_keys("1A", next, prev, oppos);
    QVERIFY2(next  == "2A",  "1A next key");
    QVERIFY2(prev  == "12A", "1A prev key");
    QVERIFY2(oppos == "1B",  "1A oppos key");

    Utils::get_next_music_keys("12B", next, prev, oppos);
    QVERIFY2(next  == "1B",  "12B next key");
    QVERIFY2(prev  == "11B", "12B prev key");
    QVERIFY2(oppos == "12A", "12B oppos key");
}
