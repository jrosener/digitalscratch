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
#include "gui.h"
#include "audio_track.h"
#include "audio_file_decoding_process.h"
#include "audio_track_playback_process.h"
#include "sound_card_access_rules.h"
#include "jack_access_rules.h"
#include "playback_parameters.h"
#include "timecode_analyzis_process.h"
#include <QThreadPool>
#include "sound_capture_and_playback_process.h"
#include <singleton.h>
#include <application_const.h>

int main(int argc, char *argv[])
{
    unsigned short int nb_decks    = 2;
    unsigned short int nb_samplers = 4;

    // Get number of decks from command line, default is 2.
    if ((argc == 2) && (atoi(argv[1]) <= 2) && (atoi(argv[1]) > 0))
    {
        nb_decks = atoi(argv[1]);
    }
    qDebug() << "Main::main Number of decks:" << nb_decks;

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
        QProcess *process = new QProcess();
        process->start(settings->get_extern_prog());
    }

    // Tracks.
    Audio_track *at_1 = new Audio_track(MAX_MINUTES_TRACK, settings->get_sample_rate());
    Audio_track *at_2 = NULL;
    if (nb_decks > 1) at_2 = new Audio_track(MAX_MINUTES_TRACK, settings->get_sample_rate());
    else              at_2 = new Audio_track(settings->get_sample_rate());
    Audio_track *ats[] = { at_1, at_2 };

    // Samplers.
    Audio_track *at_1_sampler_1  = new Audio_track(MAX_MINUTES_SAMPLER, settings->get_sample_rate());
    Audio_track *at_1_sampler_2  = new Audio_track(MAX_MINUTES_SAMPLER, settings->get_sample_rate());
    Audio_track *at_1_sampler_3  = new Audio_track(MAX_MINUTES_SAMPLER, settings->get_sample_rate());
    Audio_track *at_1_sampler_4  = new Audio_track(MAX_MINUTES_SAMPLER, settings->get_sample_rate());
    Audio_track *at_1_samplers[] = { at_1_sampler_1, at_1_sampler_2, at_1_sampler_3, at_1_sampler_4 };

    Audio_track *at_2_sampler_1  = NULL;
    Audio_track *at_2_sampler_2  = NULL;
    Audio_track *at_2_sampler_3  = NULL;
    Audio_track *at_2_sampler_4  = NULL;
    if (nb_decks > 1)
    {
        at_2_sampler_1  = new Audio_track(MAX_MINUTES_SAMPLER, settings->get_sample_rate());
        at_2_sampler_2  = new Audio_track(MAX_MINUTES_SAMPLER, settings->get_sample_rate());
        at_2_sampler_3  = new Audio_track(MAX_MINUTES_SAMPLER, settings->get_sample_rate());
        at_2_sampler_4  = new Audio_track(MAX_MINUTES_SAMPLER, settings->get_sample_rate());
    }
    else
    {
        at_2_sampler_1  = new Audio_track(settings->get_sample_rate());
        at_2_sampler_2  = new Audio_track(settings->get_sample_rate());
        at_2_sampler_3  = new Audio_track(settings->get_sample_rate());
        at_2_sampler_4  = new Audio_track(settings->get_sample_rate());
    }
    Audio_track *at_2_samplers[] = { at_2_sampler_1, at_2_sampler_2, at_2_sampler_3, at_2_sampler_4 };

    Audio_track **at_samplers[] = { at_1_samplers, at_2_samplers };

    // Decoding processes.
    Audio_file_decoding_process *dec_1  = new Audio_file_decoding_process(at_1);
    Audio_file_decoding_process *dec_2  = new Audio_file_decoding_process(at_2);

    // Decoding processes for samplers.
    Audio_file_decoding_process *dec_1_sampler_1  = new Audio_file_decoding_process(at_1_sampler_1);
    Audio_file_decoding_process *dec_1_sampler_2  = new Audio_file_decoding_process(at_1_sampler_2);
    Audio_file_decoding_process *dec_1_sampler_3  = new Audio_file_decoding_process(at_1_sampler_3);
    Audio_file_decoding_process *dec_1_sampler_4  = new Audio_file_decoding_process(at_1_sampler_4);
    Audio_file_decoding_process *dec_1_samplers[] = { dec_1_sampler_1, dec_1_sampler_2, dec_1_sampler_3, dec_1_sampler_4 };

    Audio_file_decoding_process *dec_2_sampler_1  = new Audio_file_decoding_process(at_2_sampler_1);
    Audio_file_decoding_process *dec_2_sampler_2  = new Audio_file_decoding_process(at_2_sampler_2);
    Audio_file_decoding_process *dec_2_sampler_3  = new Audio_file_decoding_process(at_2_sampler_3);
    Audio_file_decoding_process *dec_2_sampler_4  = new Audio_file_decoding_process(at_2_sampler_4);
    Audio_file_decoding_process *dec_2_samplers[] = { dec_2_sampler_1, dec_2_sampler_2, dec_2_sampler_3, dec_2_sampler_4 };

    Audio_file_decoding_process **dec_samplers[] = { dec_1_samplers, dec_2_samplers };

    // Playback parameters.
    Playback_parameters *params_1 = new Playback_parameters();
    Playback_parameters *params_2 = new Playback_parameters();
    Playback_parameters *params[] = { params_1, params_2 };

    // Process which analyze captured timecode data.
    Timecode_analyzis_process *tcode_analyzis = new Timecode_analyzis_process(params,
                                                                              nb_decks,
                                                                              settings->get_vinyl_type(),
                                                                              settings->get_sample_rate());
    int *dscratch_ids;
    dscratch_ids = new int[nb_decks];
    for (int i = 0; i < nb_decks; i++)
    {
        dscratch_ids[i] = tcode_analyzis->get_dscratch_id(i);
    }

    // Playback process.
    Audio_track_playback_process *playback = new Audio_track_playback_process(ats, at_samplers, params, nb_decks, nb_samplers);

    // Access sound card.
    Sound_card_access_rules *sound_card = new Jack_access_rules(nb_decks * 2);

    // Sound capture and playback process.
    Sound_capture_and_playback_process *capture_and_playback = new Sound_capture_and_playback_process(tcode_analyzis, playback, sound_card);

    // Create GUI.
    Gui *gui = new Gui(at_1, at_2,
                       at_samplers, 4,
                       dec_1, dec_2,
                       dec_samplers,
                       params_1, params_2,
                       playback,
                       nb_decks,
                       sound_card,
                       capture_and_playback,
                       dscratch_ids);

    // Forward the quit call.
    app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

    // Start application.
    app.exec();

    // Cleanup.
    delete gui;
    delete sound_card;
    delete capture_and_playback;
    delete tcode_analyzis;
    delete playback;
    delete params_1;
    delete params_2;
    delete dec_1;
    delete dec_2;
    delete at_1;
    delete at_2;
    delete[] dscratch_ids;

    return 0;
}
