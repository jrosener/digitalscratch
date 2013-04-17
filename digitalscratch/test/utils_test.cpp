#include <QtTest>
#include <utils_test.h>
#include <iostream>

#define DATA_DIR     "./test/data/"
#define DATA_TRACK_1 "track_1.mp3"
#define DATA_TRACK_2 "track_2.mp3"
#define DATA_TRACK_3 "track_3.mp3"
#ifdef WIN32
    #define MUSIC_PATH   "D:/musique"
#else
    #define MUSIC_PATH   "/home/julien/Music"
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
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8")); // Should be probably removed in Qt5.
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
