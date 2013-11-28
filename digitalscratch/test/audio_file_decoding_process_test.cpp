#include <QString>
#include <QtTest>
#include <audio_file_decoding_process_test.h>
#include <audio_track.h>
#include <audio_file_decoding_process.h>


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
    Audio_track *at = new Audio_track(15, 44100);
    Audio_file_decoding_process *decoder = new Audio_file_decoding_process(at);

    // Cleanup.
    delete decoder;
    delete at;
}
