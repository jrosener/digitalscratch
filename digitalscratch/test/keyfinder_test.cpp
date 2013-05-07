#include <QtTest>
#include <iostream>
#include <keyfinder_test.h>
#include <audio_track.h>
#include <audio_file_decoding_process.h>

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

Keyfinder_Test::Keyfinder_Test()
{
}

void Keyfinder_Test::initTestCase()
{
}

void Keyfinder_Test::cleanupTestCase()
{
}

void Keyfinder_Test::testCaseGetKey()
{
    // Create audio decoder.
    Audio_track *at = new Audio_track(15);
    Audio_file_decoding_process *dec = new Audio_file_decoding_process(at);

    // Decode and get music key of the track.
    at->reset();
    dec->run(QString(DATA_DIR) + QString(DATA_TRACK_REAL1));
    QVERIFY2(QString(kfinder_get_key(at->get_samples(), at->get_end_of_samples(), SAMPLE_RATE, 2)) == "Bbm",
             DATA_TRACK_REAL1); // From report: Bb(M?)

    at->reset();
    dec->run(QString(DATA_DIR) + QString(DATA_TRACK_REAL2));
    QVERIFY2(QString(kfinder_get_key(at->get_samples(), at->get_end_of_samples(), SAMPLE_RATE, 2)) == "Ebm",
             DATA_TRACK_REAL2); // From report: Ebm

    at->reset();
    dec->run(QString(DATA_DIR) + QString(DATA_TRACK_REAL3));
    QVERIFY2(QString(kfinder_get_key(at->get_samples(), at->get_end_of_samples(), SAMPLE_RATE, 2)) == "Gbm",
             DATA_TRACK_REAL3); // From report: Dm

    at->reset();
    dec->run(QString(DATA_DIR) + QString(DATA_TRACK_REAL4));
    QVERIFY2(QString(kfinder_get_key(at->get_samples(), at->get_end_of_samples(), SAMPLE_RATE, 2)) == "Dm",
             DATA_TRACK_REAL4); // From report: Dm

    at->reset();
    dec->run(QString(DATA_DIR) + QString(DATA_TRACK_REAL5));
    QVERIFY2(QString(kfinder_get_key(at->get_samples(), at->get_end_of_samples(), SAMPLE_RATE, 2)) == "Bbm",
             DATA_TRACK_REAL5); // From report: Bb(M?)

    at->reset();
    dec->run(QString(DATA_DIR) + QString(DATA_TRACK_REAL6));
    QVERIFY2(QString(kfinder_get_key(at->get_samples(), at->get_end_of_samples(), SAMPLE_RATE, 2)) == "Ebm",
             DATA_TRACK_REAL6); // From report  Abm

    // Cleanup.
    delete dec;
    delete at;
}

