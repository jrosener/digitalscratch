#include <QString>
#include <QtTest>
#include <audio_file_decoding_process_test.h>
#include <audio_track.h>
#include <audio_file_decoding_process.h>

#define DATA_DIR     "./test/data/"
#define DATA_TRACK_1 "track_1.mp3"
#define DATA_TRACK_2 "track_2.mp3"
#define DATA_TRACK_3 "track_3.mp3"

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
    QVERIFY2(at->get_hash() == "aa85f1d25c890b7c4aef127db8d03786", "track 1 hash");

    at->reset();
    QVERIFY2(decoder->calculate_hash(QString(DATA_DIR) + QString(DATA_TRACK_2)) == true, "calculate hash track 2");
    QVERIFY2(at->get_hash() == "cba9df1db2d06fe1ffa9a67a61acc4e3", "track 2 hash");

    at->reset();
    QVERIFY2(decoder->calculate_hash(QString(DATA_DIR) + QString(DATA_TRACK_3)) == true, "calculate hash track 3");
    QVERIFY2(at->get_hash() == "7fd669c1266fc131f7ded0ae5c7eecba", "track 3 hash");

    // Cleanup.
    delete decoder;
    delete at;
}
