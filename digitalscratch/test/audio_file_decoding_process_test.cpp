#include <QString>
#include <QtTest>
#include <audio_file_decoding_process_test.h>
#include <audio_track.h>
#include <audio_file_decoding_process.h>

#define DATA_DIR     "./test/data/"
#define DATA_TRACK_1 "track_1.mp3"
#define DATA_TRACK_2 "track_2.mp3"
#define DATA_TRACK_3 "track_3.mp3"
#define MUSIC_PATH   "/home/julien/Music"

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
    Audio_track *at = new Audio_track(15);
    Audio_file_decoding_process *decoder = new Audio_file_decoding_process(at);

    // Cleanup.
    delete decoder;
    delete at;
}

void Audio_file_decoding_process_Test::testCaseCalculateHash()
{
    // Create a decoder object.
    Audio_track *at = new Audio_track(15);
    Audio_file_decoding_process *decoder = new Audio_file_decoding_process(at);

    // Get hash of files.
    at->reset();
    QVERIFY2(decoder->calculate_hash(QString(DATA_DIR) + QString(DATA_TRACK_1)) == true, "calculate hash track 1");
    QVERIFY2(at->get_hash() == "e9e6e98ef4c8a32ac55e7e1155882b57", "track 1 hash");

    at->reset();
    QVERIFY2(decoder->calculate_hash(QString(DATA_DIR) + QString(DATA_TRACK_2)) == true, "calculate hash track 2");
    QVERIFY2(at->get_hash() == "557a8a86952a10e5eca44cd24021bd32", "track 2 hash");

    at->reset();
    QVERIFY2(decoder->calculate_hash(QString(DATA_DIR) + QString(DATA_TRACK_3)) == true, "calculate hash track 3");
    QVERIFY2(at->get_hash() == "e47ee9c41c0279a6addf528414739db4", "track 3 hash");

    // Wrong parameters.
    at->reset();
    QVERIFY2(decoder->calculate_hash(QString("path does not exists")) == false, "path does not exists");
    QVERIFY2(at->get_hash() == "", "path does not exist, no hash");

    at->reset();
    QVERIFY2(decoder->calculate_hash(NULL) == false, "path is null");
    QVERIFY2(at->get_hash() == "", "path is null, no hash");

    // Cleanup.
    delete decoder;
    delete at;
}

void Audio_file_decoding_process_Test::testCaseChargeCalculateHash()
{
    // Create a decoder object.
    Audio_track *at = new Audio_track(15);
    Audio_file_decoding_process *decoder = new Audio_file_decoding_process(at);

    //
    // Get hash of all mp3 files in big directory. Then check there is no duplicate hash.
    //

    // Iterate recursively over files in directory.
    QDir dir(MUSIC_PATH);
    QDirIterator iterator(dir.absolutePath(), QDirIterator::Subdirectories);
    QMultiMap<QString, QString> map;
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8")); // Should be probably removed in Qt5.
    while (iterator.hasNext())
    {
        // Go to the next element (file or directory).
        iterator.next();
        if (iterator.fileInfo().isDir() == false)
        {
            // The element is a file.
            QString filename = iterator.fileName();
            if (filename.endsWith(".mp3") == true)
            {
                //cout << qPrintable(iterator.filePath()) << endl;

                // Get hash of this mp3.
                //at->reset(); // takes too much time (std::fill)
                QVERIFY2(decoder->calculate_hash(iterator.filePath()) == true, qPrintable("calculate hash on " + iterator.filePath()));

                // Store the hash in a map.
                map.insertMulti(at->get_hash(), iterator.filePath());
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
            for (int idx = 0; idx < values.size(); ++idx)
            {
                dup_file_hashes += values.at(idx) + " | ";
            }
        }
        QVERIFY2(values.size() == 1, qPrintable("same hash for several audio tracks: " + dup_file_hashes));
    }

    // Cleanup.
    delete decoder;
    delete at;
}
