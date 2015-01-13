/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*---------------------------------------------------------------( main.cpp )-*/
/*                                                                            */
/*  Copyright (C) 2003-2014                                                   */
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
/*------------------------------------------------------------( Description )-*/
/*                                                                            */
/*           DigitalScratch player application starting point                 */
/*                                                                            */
/*============================================================================*/

#include <QtDebug>
#include <QApplication>
#include <QTextCodec>
#include <QProcess>
#include <QThreadPool>

#include "application_logging.h"
#include "gui.h"
#include "audio_track.h"
#include "audio_file_decoding_process.h"
#include "audio_track_playback_process.h"
#include "sound_driver_access_rules.h"
#include "jack_access_rules.h"
#include "playback_parameters.h"
#include "timecode_control_process.h"
#include "sound_capture_and_playback_process.h"
#include "singleton.h"
#include "application_const.h"

int main(int argc, char *argv[])
{
    // Logging settings.
    qSetMessagePattern("[%{type}] | %{category} | %{function}@%{line} | %{message}");
    QLoggingCategory::setFilterRules(QStringLiteral("*.debug=false\n \
                                                    ds.file.debug=true\n \
                                                    *.warning=true\n \
                                                    *.critical=true\n"));

    // Create application.
    QApplication app(argc, argv);

    // Set max number of simultaneous new threads.
#ifdef WIN32
    QThreadPool::globalInstance()->setMaxThreadCount(1);
#else
    if (QThreadPool::globalInstance()->maxThreadCount() > 1)
    {
        QThreadPool::globalInstance()->setMaxThreadCount(QThreadPool::globalInstance()->maxThreadCount() - 1);
    }
#endif

    // Application settings management.
    Application_settings *settings = &Singleton<Application_settings>::get_instance();

    // Execute user's defined program at startup.
    if (settings->get_extern_prog() != "")
    {
        QProcess process;
        process.start(settings->get_extern_prog());
        if (process.waitForFinished() == false)
        {
            qCCritical(DS_APPSETTINGS) << "Running" << settings->get_extern_prog() << "failed.";
            return -1;
        }
    }

    // Create tracks, sampler, decoder process,... for each deck.
    QList<QSharedPointer<Audio_track>>                        ats;
    QList<QSharedPointer<Audio_file_decoding_process>>        dec_procs;
    QList<QSharedPointer<Playback_parameters>>                play_params;
    QList<QSharedPointer<Timecode_control_process>>           tcode_controls;
    QList<QList<QSharedPointer<Audio_track>>>                 at_samplers;
    QList<QList<QSharedPointer<Audio_file_decoding_process>>> dec_sampler_procs;
    QList<QSharedPointer<Audio_track_playback_process>>       at_playbacks;
    int *dscratch_ids = new int[settings->get_nb_decks()];
    for (auto i = 0; i < settings->get_nb_decks(); i++)
    {
        // Track for a deck.
        QSharedPointer<Audio_track>                 at(new Audio_track(MAX_MINUTES_TRACK, settings->get_sample_rate()));
        QSharedPointer<Audio_file_decoding_process> dec_proc(new Audio_file_decoding_process(at));
        ats << at;
        dec_procs << dec_proc;

        // Playback parameters for a deck.
        QSharedPointer<Playback_parameters> play_param(new Playback_parameters);
        play_params << play_param;

        // Process which analyze captured timecode data for a deck.
        QSharedPointer<Timecode_control_process> tcode_control(new Timecode_control_process(play_param,
                                                                                            settings->get_vinyl_type(),
                                                                                            settings->get_sample_rate()));
        tcode_controls << tcode_control;
        dscratch_ids[i] = tcode_control->get_dscratch_id();

        // Set of samplers for a deck.
        QList<QSharedPointer<Audio_track>>                 at_sampler;
        QList<QSharedPointer<Audio_file_decoding_process>> dec_sampler_proc;
        for (auto j = 1; j <= settings->get_nb_samplers(); j++)
        {
            QSharedPointer<Audio_track>                 at_s(new Audio_track(MAX_MINUTES_SAMPLER, settings->get_sample_rate()));
            QSharedPointer<Audio_file_decoding_process> dec_s_proc(new Audio_file_decoding_process(at_s));
            at_sampler << at_s;
            dec_sampler_proc << dec_s_proc;
        }
        at_samplers << at_sampler;
        dec_sampler_procs << dec_sampler_proc;

        // Playback process for a deck.
        QSharedPointer<Audio_track_playback_process> at_playback(new Audio_track_playback_process(at,
                                                                                                  at_sampler,
                                                                                                  play_param));
        at_playbacks << at_playback;
    }

    // Access sound card.
    // TODO add settings to check if we want to use the timecode to get playback params or not (so no capture).
    QSharedPointer<Sound_driver_access_rules> sound_card(new Jack_access_rules(settings->get_nb_decks() * 2));
    sound_card->set_capture(true);

    // Sound capture and playback process.
    QSharedPointer<Sound_capture_and_playback_process> capture_and_playback(new Sound_capture_and_playback_process(tcode_controls,
                                                                                                                   at_playbacks,
                                                                                                                   sound_card,
                                                                                                                   settings->get_nb_decks()));
    // TODO if not using timecode to get playback params
    //Only_playback_process *playback_external_params = new Playback_process_with_external_parameters(playback, sound_card);

    // Create GUI.
    QSharedPointer<Gui> gui(new Gui(ats,
                                    at_samplers,
                                    dec_procs,
                                    dec_sampler_procs,
                                    play_params,
                                    at_playbacks,
                                    sound_card,
                                    capture_and_playback,
                                    dscratch_ids));
    Q_UNUSED(gui);

    // Forward the quit call.
    app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

    // Start application.
    app.exec();

    // Cleanup.
    delete[] dscratch_ids;

    return 0;
}
