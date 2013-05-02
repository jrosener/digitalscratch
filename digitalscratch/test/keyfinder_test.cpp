#include <QtTest>
#include <iostream>
#include <keyfinder_test.h>
#include <audio_track.h>
#include <audio_file_decoding_process.h>

#define DATA_DIR     "./test/data/"
#define DATA_TRACK_1 "track_1.mp3"
#define DATA_TRACK_2 "track_2.mp3"
#define DATA_TRACK_3 "track_3.mp3"
#define DATA_TRACK_REAL1 "02_dig_your_own_hole.mp3"
#define DATA_TRACK_REAL2 "commix_-_be_true.mp3"
#define DATA_TRACK_REAL3 "b2_the_sound_of_the_big_babou.mp3"
#define DATA_TRACK_REAL4 "laurent_garnier_-_the_sound_of_the_big_babou.mp3"
#define DATA_TRACK_REAL5 "08-pendulum_-_tarantula_feat._fresh_pyda_and_tenor_fly.mp3"
#define DATA_TRACK_REAL6 "16-pendulum-tarantula.mp3"
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
    // Decode mp3 track (samples as integer).
    Audio_track *at = new Audio_track(10);
    Audio_file_decoding_process *dec = new Audio_file_decoding_process(at);
    dec->run(QString(DATA_DIR) + QString(DATA_TRACK_REAL5));

    // Convert samples from integer to float since Keyfinder uses float.
    short signed int *int_samples = at->get_samples();
    float *float_samples;
    float_samples = new float[at->get_max_nb_samples()];
    for (unsigned int i = 0; i < at->get_end_of_samples(); i++)
    {
        float_samples[i] = (float)int_samples[i] / 32768.0;
    }

    // Get music key of the track.
    QString key = get_key(float_samples, at->get_max_nb_samples(), SAMPLE_RATE, 2);

    // Cleanup.
    delete[] float_samples;
    delete dec;
    delete at;
}

