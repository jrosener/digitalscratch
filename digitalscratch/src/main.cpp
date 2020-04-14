/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*---------------------------------------------------------------( main.cpp )-*/
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
#include <QThread>
#include <QTranslator>
#include <QLocale>

#include <digital_scratch.h>

#include "app/application_logging.h"
#include "app/application_const.h"
#include "gui/gui.h"
#include "tracks/audio_track.h"
#include "tracks/audio_file_decoding_process.h"
#include "player/deck_playback_process.h"
#include "player/playback_parameters.h"
#include "player/control_and_playback_process.h"
#include "audiodev/audio_io_control_rules.h"
#include "audiodev/jack_client_control_rules.h"
#include "control/timecode_control_process.h"
#include "control/dicer_control_process.h"
#include "singleton.h"

int main(int argc, char *argv[])
{
    // Create application.
    QApplication app(argc, argv);

    // Log settings.
    qSetMessagePattern("[%{type}/%{category}/thread:%{qthreadptr}]\t%{function}@%{line} | %{message}");

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
        else
        {
            qCInfo(DS_APPSETTINGS) << "Run" << settings->get_extern_prog() << "success";
        }
    }
                                     
    // Translate UI to the language defined in application settings.
    QLocale::Language lang = QLocale(settings->get_language()).language();
    QTranslator translator;
    switch(lang)
    {
        case QLocale::France:
        case QLocale::French:
            if (translator.load(":/translations/digitalscratch_fr.qm") == false)
            {
                qCWarning(DS_APPSETTINGS) << "No french translation file found";
            }
            break;
        default:
            qCWarning(DS_APPSETTINGS) << "No translation file for language =" << settings->get_language();
            break;
    }
    app.installTranslator(&translator);

    // Create tracks, sampler, decoder process,... for each deck.
    QList<QSharedPointer<Audio_track>>                        ats;
    QList<QSharedPointer<Audio_file_decoding_process>>        dec_procs;
    QList<QSharedPointer<Playback_parameters>>                play_params;
    QList<QSharedPointer<Timecode_control_process>>           tcode_controls;
    QList<QSharedPointer<Manual_control_process>>             manual_controls;
    QList<QList<QSharedPointer<Audio_track>>>                 at_samplers;
    QList<QList<QSharedPointer<Audio_file_decoding_process>>> dec_sampler_procs;
    QList<QSharedPointer<Deck_playback_process>>              at_playbacks;
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
                                                                                            settings->get_vinyl_type(i),
                                                                                            settings->get_sample_rate()));
        tcode_controls << tcode_control;

        // Process which get playback parameters from keyboard or gui buttons.
        QSharedPointer<Manual_control_process> manual_control(new Manual_control_process(play_param));
        manual_controls << manual_control;

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
        QSharedPointer<Deck_playback_process> at_playback(new Deck_playback_process(at,
                                                                                    at_sampler,
                                                                                    play_param));
        at_playbacks << at_playback;
    }

    // Access sound card.
    QSharedPointer<Audio_IO_control_rules> sound_card(new Jack_client_control_rules(settings->get_nb_decks() * 2));
    sound_card->set_capture(true);

    // Sound capture and playback process.
    QSharedPointer<Control_and_playback_process> control_and_playback(new Control_and_playback_process(tcode_controls,
                                                                                                       manual_controls,
                                                                                                       at_playbacks,
                                                                                                       sound_card,
                                                                                                       settings->get_nb_decks()));

    // Novation Dicer external controller.
    // Run write/read commands to/from Dicers in another thread.
    QSharedPointer<Dicer_control_process> dicer_control(new Dicer_control_process());
    QThread *dicer_control_thread = new QThread();
    dicer_control->moveToThread(dicer_control_thread);
    QObject::connect(dicer_control_thread, SIGNAL(started()), dicer_control.data(), SLOT(start()));
    QObject::connect(dicer_control.data(), SIGNAL(terminated()), dicer_control_thread, SLOT(quit()));
    QObject::connect(dicer_control_thread, SIGNAL(finished()), dicer_control.data(), SLOT(deleteLater()));
    QObject::connect(dicer_control_thread, SIGNAL(finished()), dicer_control_thread, SLOT(deleteLater()));

    // Create GUI.
    Gui gui(ats,
            at_samplers,
            dec_procs,
            dec_sampler_procs,
            play_params,
            tcode_controls,
            manual_controls,
            dicer_control,
            at_playbacks,
            sound_card,
            control_and_playback);

    // Run all control and playback processing stuff in another thread than the Gui.
    QThread *control_and_playback_thread = new QThread();
    sound_card->moveToThread(control_and_playback_thread);
    control_and_playback->moveToThread(control_and_playback_thread);
    for (auto i = 0; i < settings->get_nb_decks(); i++)
    {
        tcode_controls[i]->moveToThread(control_and_playback_thread);
        manual_controls[i]->moveToThread(control_and_playback_thread);
        at_playbacks[i]->moveToThread(control_and_playback_thread);
    }
    QObject::connect(control_and_playback_thread, SIGNAL(started()), control_and_playback.data(), SLOT(init()));
    QObject::connect(control_and_playback.data(), SIGNAL(terminated()), control_and_playback_thread, SLOT(quit()));
    QObject::connect(control_and_playback_thread, SIGNAL(finished()), control_and_playback.data(), SLOT(deleteLater()));
    QObject::connect(control_and_playback_thread, SIGNAL(finished()), control_and_playback_thread, SLOT(deleteLater()));

    // Custom app close (needed if user click on 'X').
    app.connect(&app, SIGNAL(aboutToQuit()), control_and_playback.data(), SLOT(kill()));

    // Start application.
    control_and_playback_thread->start();
    dicer_control_thread->start();
    app.exec();

    return 0;
}
