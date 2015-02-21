#include <QtTest>
#include <singleton.h>
#include <QDesktopServices>
#include <utils.h>
#include <application_settings.h>
#include <sound_capture_and_playback_process_test.h>
#include <digital_scratch_api.h>
#include <playback_parameters.h>
#include <timecode_control_process.h>
#include <manual_control_process.h>
#include <audio_track_playback_process.h>
#include <sound_driver_access_rules.h>
#include <sound_capture_and_playback_process.h>
#include <jack_access_rules.h>
#include <audio_file_decoding_process.h>
#include <qeventloop.h>

#define DATA_DIR     "./test/data/"
#define DATA_TRACK_1 "b_comp_-_p_dust.mp3"
#define TIMECODE_1   "scratchlivecontrol-vinylrip-33rpm+0.mp3"

Sound_capture_and_playback_process_Test::Sound_capture_and_playback_process_Test()
{
}

void Sound_capture_and_playback_process_Test::initTestCase()
{
}

void Sound_capture_and_playback_process_Test::cleanupTestCase()
{
    // Cleanup.
}

void Sound_capture_and_playback_process_Test::testCaseRunWithJack()
{
    // Prepare full stack for analyzing timecode captured from sound card.
    Application_settings *settings = &Singleton<Application_settings>::get_instance();
    settings->set_sample_rate(44100);
    settings->set_vinyl_type(SERATO_VINYL);
    settings->set_nb_decks(1);

    QSharedPointer<Playback_parameters> play_param(new Playback_parameters);

    QSharedPointer<Timecode_control_process> tcode_control(new Timecode_control_process(play_param,
                                                                                        settings->get_vinyl_type(),
                                                                                        settings->get_sample_rate()));
    QList<QSharedPointer<Timecode_control_process>> tcode_controls = {tcode_control};

    QSharedPointer<Manual_control_process> manual_control(new Manual_control_process(play_param));
    QList<QSharedPointer<Manual_control_process>> manual_controls = {manual_control};

    QSharedPointer<Audio_track> at(new Audio_track(MAX_MINUTES_TRACK, settings->get_sample_rate()));
    Audio_file_decoding_process decoder(at, false);
    decoder.run(QString(DATA_DIR) + QString(DATA_TRACK_1), "", "");

    QList<QSharedPointer<Audio_track>> at_sampler;
    QSharedPointer<Audio_track> at_s(new Audio_track(MAX_MINUTES_SAMPLER, settings->get_sample_rate()));
    at_sampler << at_s;

    QSharedPointer<Audio_track_playback_process> at_playback(new Audio_track_playback_process(at, at_sampler, play_param));
    QList<QSharedPointer<Audio_track_playback_process>> at_playbacks = {at_playback};

    QSharedPointer<Sound_driver_access_rules> sound_card(new Jack_access_rules(settings->get_nb_decks() * 2));
    sound_card->set_capture(true);
    QVERIFY2(sound_card->use_timecode_from_file(QString(DATA_DIR) + QString(TIMECODE_1)) == true, "set the pre-recorded timecode"); // Use a fake timecode

    QSharedPointer<Sound_capture_and_playback_process> capture_and_play(new Sound_capture_and_playback_process(tcode_controls,
                                                                                                               manual_controls,
                                                                                                               at_playbacks,
                                                                                                               sound_card,
                                                                                                               settings->get_nb_decks()));
    capture_and_play->set_process_mode(TIMECODE, 0);

    // Start capture and playback through jack.
    QVERIFY2(sound_card->start((void*)capture_and_play.data()) == true, "start capture and playback through jack");

    // Wait 10 sec.
    QTest::qWait(100000);

    // Stop processing.
    sound_card->stop();
}
