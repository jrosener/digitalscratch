/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                     Digital Scratch Player Test                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/*                                                                            */
/*  Copyright (C) 2003-2020                                                   */
/*                Julien Rosener <julien.rosener@digital-scratch.org>         */
/*                                                                            */
/*----------------------------------------------------------------( License )-*/
/*                                                                            */
/*  This program is free software: you can redistribute it and/or modify      */
/*  it under the terms of the GNU General Public License as published by      */
/*  the Free Software Foundation, either version 3 of the License, or         */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  This package is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU General Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with this program. If not, see <http://www.gnu.org/licenses/>.      */
/*                                                                            */
/*============================================================================*/

#include <QtTest>
#include <QDesktopServices>
#include <qeventloop.h>

#include <digital_scratch.h>
#include "singleton.h"
#include "utils.h"
#include "app/application_settings.h"
#include "player/playback_parameters.h"
#include "player/deck_playback_process.h"
#include "player/control_and_playback_process.h"
#include "control/timecode_control_process.h"
#include "control/manual_control_process.h"
#include "audiodev/audio_io_control_rules.h"
#include "audiodev/jack_client_control_rules.h"
#include "tracks/audio_file_decoding_process.h"
#include "control_and_playback_process_test.h"

#define DATA_DIR     "./test/data/"
#define DATA_TRACK_1 "b_comp_-_p_dust.mp3"
#define TIMECODE_1   "scratchlivecontrol-vinylrip-33rpm+0.mp3"
#define TIMECODE_2   "timecode-serato-5min-full_control.mp3"

Control_and_playback_process_Test::Control_and_playback_process_Test()
{
}

void Control_and_playback_process_Test::initTestCase()
{
}

void Control_and_playback_process_Test::cleanupTestCase()
{
    // Cleanup.
}

void Control_and_playback_process_Test::testCaseRunWithJack_1deck()
{
    // Prepare full stack for analyzing timecode captured from sound card.
    Application_settings *settings = &Singleton<Application_settings>::get_instance();
    settings->set_sample_rate(44100);
    settings->set_vinyl_type(0, SERATO);
    settings->set_nb_decks(1);

    QSharedPointer<Playback_parameters> play_param(new Playback_parameters);

    QSharedPointer<Timecode_control_process> tcode_control(new Timecode_control_process(play_param,
                                                                                        settings->get_vinyl_type(0),
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

    QSharedPointer<Deck_playback_process> at_playback(new Deck_playback_process(at, at_sampler, play_param));
    QList<QSharedPointer<Deck_playback_process>> at_playbacks = {at_playback};

    QSharedPointer<Audio_IO_control_rules> sound_card(new Jack_client_control_rules(settings->get_nb_decks() * 2));
    sound_card->set_capture(true);
    QVERIFY2(sound_card->use_timecode_from_file(QString(DATA_DIR) + QString(TIMECODE_1)) == true, "set the pre-recorded timecode"); // Use a fake timecode

    QSharedPointer<Control_and_playback_process> capture_and_play(new Control_and_playback_process(tcode_controls,
                                                                                                  manual_controls,
                                                                                                  at_playbacks,
                                                                                                  sound_card,
                                                                                                  settings->get_nb_decks()));
    capture_and_play->set_process_mode(ProcessMode::TIMECODE, 0);

    // Start capture and playback through jack.
    QVERIFY2(sound_card->start((void*)capture_and_play.data()) == true, "start capture and playback through jack");

    // Wait 10 sec.
    QTest::qWait(10000);

    // TODO: For the moment, the test is a manual test: just listen to the 10sec of music.

    // Stop processing.
    sound_card->stop();
}

void Control_and_playback_process_Test::testCaseRunWithJack_2decks()
{
    // Prepare full stack for analyzing timecode captured from sound card.
    Application_settings *settings = &Singleton<Application_settings>::get_instance();
    settings->set_sample_rate(44100);
    settings->set_nb_decks(2);
    settings->set_vinyl_type(0, SERATO);
    settings->set_vinyl_type(1, SERATO);

    QSharedPointer<Playback_parameters> play_param_1(new Playback_parameters);
    QSharedPointer<Timecode_control_process> tcode_control_1(new Timecode_control_process(play_param_1,
                                                                                          settings->get_vinyl_type(0),
                                                                                          settings->get_sample_rate()));
    QSharedPointer<Playback_parameters> play_param_2(new Playback_parameters);
    QSharedPointer<Timecode_control_process> tcode_control_2(new Timecode_control_process(play_param_2,
                                                                                          settings->get_vinyl_type(1),
                                                                                          settings->get_sample_rate()));
    QList<QSharedPointer<Timecode_control_process>> tcode_controls = {tcode_control_1, tcode_control_2};

    QSharedPointer<Manual_control_process> manual_control_1(new Manual_control_process(play_param_1));
    QSharedPointer<Manual_control_process> manual_control_2(new Manual_control_process(play_param_2));
    QList<QSharedPointer<Manual_control_process>> manual_controls = {manual_control_1, manual_control_2};

    QSharedPointer<Audio_track> at_1(new Audio_track(MAX_MINUTES_TRACK, settings->get_sample_rate()));
    Audio_file_decoding_process decoder_1(at_1, false);
    decoder_1.run(QString(DATA_DIR) + QString(DATA_TRACK_1), "", "");

    QSharedPointer<Audio_track> at_2(new Audio_track(MAX_MINUTES_TRACK, settings->get_sample_rate()));
    Audio_file_decoding_process decoder_2(at_2, false);
    decoder_2.run(QString(DATA_DIR) + QString(DATA_TRACK_1), "", "");

    QSharedPointer<Audio_track> at_s_1(new Audio_track(MAX_MINUTES_SAMPLER, settings->get_sample_rate()));
    QList<QSharedPointer<Audio_track>> at_sampler_1 = {at_s_1};
    QSharedPointer<Audio_track> at_s_2(new Audio_track(MAX_MINUTES_SAMPLER, settings->get_sample_rate()));
    QList<QSharedPointer<Audio_track>> at_sampler_2 = {at_s_2};

    QSharedPointer<Deck_playback_process> at_playback_1(new Deck_playback_process(at_1, at_sampler_1, play_param_1));
    QSharedPointer<Deck_playback_process> at_playback_2(new Deck_playback_process(at_2, at_sampler_2, play_param_2));
    QList<QSharedPointer<Deck_playback_process>> at_playbacks = {at_playback_1, at_playback_2};

    QSharedPointer<Audio_IO_control_rules> sound_card(new Jack_client_control_rules(settings->get_nb_decks() * 2));
    sound_card->set_capture(true);
    QVERIFY2(sound_card->use_timecode_from_file(QString(DATA_DIR) + QString(TIMECODE_2)) == true, "set the pre-recorded timecode"); // Use a fake timecode

    QSharedPointer<Control_and_playback_process> capture_and_play(new Control_and_playback_process(tcode_controls,
                                                                                                   manual_controls,
                                                                                                   at_playbacks,
                                                                                                   sound_card,
                                                                                                   settings->get_nb_decks()));
    capture_and_play->set_process_mode(ProcessMode::TIMECODE, 0);

    // Start capture and playback through jack.
    QVERIFY2(sound_card->start((void*)capture_and_play.data()) == true, "start capture and playback through jack");

    // Wait 10 sec.
    QTest::qWait(10000);

    // TODO: For the moment, the test is a manual test: just listen to the 10sec of music.

    // Stop processing.
    sound_card->stop();
}
