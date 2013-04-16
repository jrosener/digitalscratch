#include <QString>
#include <QtTest>
#include <audio_track.h>
#include <audio_file_decoding_process.h>

#define DATA_DIR     "./test/data/"
#define DATA_TRACK_1 "track_1.mp3"
#define DATA_TRACK_2 "track_2.mp3"
#define DATA_TRACK_3 "track_3.mp3"

class Audio_track_Test : public QObject
{
    Q_OBJECT
    
public:
    Audio_track_Test();
    
private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testCaseCreate();
    void testCaseFillSamples();
};

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
    Audio_track *at = new Audio_track(15);

    // Reset track content.
    at->reset();
    QVERIFY2(at->get_name()           == "", "name is empty");
    QVERIFY2(at->get_length()         == 0,  "length is null");
    QVERIFY2(at->get_end_of_samples() == 0,  "end of sample is null");
    QVERIFY2(at->get_samples()[0]     == 0,  "samples are null");

    // Cleanup.
    delete at;
}

void Audio_track_Test::testCaseFillSamples()
{
    // Create a track.
    Audio_track *at = new Audio_track(15);

    // Create a track decoder and decode compressed audio data to the audio track object.
    Audio_file_decoding_process *decoder = new Audio_file_decoding_process(at);

    // Check decoded track content 1.
    at->reset();
    QVERIFY2(decoder->run(QString(DATA_DIR) + QString(DATA_TRACK_1)) == true, "decode audio track 1");
    QVERIFY2(at->get_name()           == DATA_TRACK_1, "track 1 name");
    QVERIFY2(at->get_length()         == 21622,        "track 1 length");
    QVERIFY2(at->get_end_of_samples() == 1907140,      "track 1 end of sample");
    QVERIFY2(at->get_samples()[0]     == -43,          "track 1 first sample");
    QVERIFY2(decoder->calculate_hash(QString(DATA_DIR) + QString(DATA_TRACK_1)) == true, "decode audio track 1");
    QVERIFY2(at->get_hash()           == "lalalala",   "track 1 hash");

    // Check decoded track content 2.
    at->reset();
    QVERIFY2(decoder->run(QString(DATA_DIR) + QString(DATA_TRACK_2)) == true, "decode audio track 2");
    QVERIFY2(at->get_name()           == DATA_TRACK_2, "track 2 name");
    QVERIFY2(at->get_length()         == 24109,        "track 2 length");
    QVERIFY2(at->get_end_of_samples() == 2126462,      "track 2 end of sample");
    QVERIFY2(at->get_samples()[0]     == 205,          "track 2 first sample");

    // Check decoded track content 3.
    at->reset();
    QVERIFY2(decoder->run(QString(DATA_DIR) + QString(DATA_TRACK_3)) == true, "decode audio track 3");
    QVERIFY2(at->get_name()           == DATA_TRACK_3, "track 3 name");
    QVERIFY2(at->get_length()         == 9073,         "track 3 length");
    QVERIFY2(at->get_end_of_samples() == 800280,       "track 3 end of sample");
    QVERIFY2(at->get_samples()[0]     == -5,           "track 3 first sample");

    // Cleanup.
    delete decoder;
    delete at;
}

QTEST_APPLESS_MAIN(Audio_track_Test)

#include "audio_track_test.moc"
