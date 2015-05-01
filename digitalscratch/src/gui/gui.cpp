/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------( gui.cpp )-*/
/*                                                                            */
/*  Copyright (C) 2003-2015                                                   */
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
/*                Creates GUI for DigitalScratch player                       */
/*                                                                            */
/*============================================================================*/

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QScrollBar>
#include <QHeaderView>
#include <QPushButton>
#include <QShortcut>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QtDebug>
#include <QtGlobal>
#include <iostream>
#include <QGraphicsScene>
#include <QFileSystemModel>
#include <QSignalMapper>
#include <QMessageBox>
#include <QApplication>
#include <QPushButton>
#include <QCommandLinkButton>
#include <QSplitter>
#include <QModelIndexList>
#include <QAction>
#include <QMenu>
#include <QMimeData>
#include <QSizePolicy>
#include <QtConcurrentRun>
#include <math.h>
#include <digital_scratch_api.h>
#include <keyfinder_api.h>
#include <samplerate.h>
#include <jack/jack.h>

#include "app/application_logging.h"
#include "gui/gui.h"
#include "tracks/audio_collection_model.h"
#include "tracks/playlist.h"
#include "tracks/playlist_persistence.h"
#include "utils.h"
#include "singleton.h"

#ifdef WIN32
extern "C"
{
    #include "libavcodec/version.h"
    #include "libavformat/version.h"
}
#else
extern "C"
{
    #ifndef INT64_C
    #define INT64_C(c) (c ## LL)
    #define UINT64_C(c) (c ## ULL)
    #endif
    #include "libavcodec/version.h"
    #include "libavformat/version.h"
}
#endif

Gui::Gui(QList<QSharedPointer<Audio_track>>                        &ats,
         QList<QList<QSharedPointer<Audio_track>>>                 &at_samplers,
         QList<QSharedPointer<Audio_file_decoding_process>>        &decs,
         QList<QList<QSharedPointer<Audio_file_decoding_process>>> &dec_samplers,
         QList<QSharedPointer<Playback_parameters>>                &params,
         QList<QSharedPointer<Timecode_control_process>>           &tcode_controls,
         QList<QSharedPointer<Manual_control_process>>             &manual_controls,
         QList<QSharedPointer<Deck_playback_process>>              &playbacks,
         QSharedPointer<Sound_driver_access_rules>                 &sound_card,
         QSharedPointer<Control_and_playback_process>              &control_and_playback)
{
    // Check input parameters.
    if (playbacks.count()           == 0       ||
        sound_card.data()           == nullptr ||
        control_and_playback.data() == nullptr)
    {
        qCCritical(DS_OBJECTLIFE) << "bad input parameters";
        return;
    }

    // Get app settings.
    this->settings = &Singleton<Application_settings>::get_instance();

    // Get decks/tracks and sound capture/playback engine.
    this->ats                     = ats;
    this->at_samplers             = at_samplers;
    this->decs                    = decs;
    this->dec_samplers            = dec_samplers;
    this->params                  = params;
    this->tcode_controls          = tcode_controls;
    this->manual_controls         = manual_controls;
    this->playbacks               = playbacks;
    this->nb_decks                = this->settings->get_nb_decks();
    this->nb_samplers             = this->settings->get_nb_samplers();
    this->sound_card              = sound_card;
    this->control_and_play        = control_and_playback;
    this->selected_deck           = 0;
    this->dscratch_handles        = new DSCRATCH_HANDLE[this->settings->get_nb_decks()];
    for (int i = 0; i < this->settings->get_nb_decks(); i++)
    {
        this->dscratch_handles[i] = this->tcode_controls[i]->get_dscratch_handle();
    }

    // Init pop-up dialogs.
    this->config_dialog                   = nullptr;
    this->refresh_audio_collection_dialog = nullptr;

    // Create and show the main window.
    if (this->create_main_window() != true)
    {
        qCCritical(DS_OBJECTLIFE) << "creation of main window failed";
        return;
    }

    // Apply application settings.
    if (this->apply_application_settings() != true)
    {
        qCCritical(DS_APPSETTINGS) << "can not apply application settings";
        return;
    }

    // Run motion detection if the setting auto_start_motion_detection=ON.
    if (this->settings->get_autostart_motion_detection() == true)
    {
        this->start_control_and_playback();
    }

    // Display audio file collection (takes time, that's why we are first showing the main window).
    this->display_audio_file_collection();

    return;
}

Gui::~Gui()
{
    // Store size/position of the main window (first go back from fullscreen or maximized mode).
    if (this->window->isFullScreen() == true)
    {
        this->window->showNormal();
    }
    this->settings->set_main_window_position(this->window->mapToGlobal(QPoint(0, 0)));
    this->settings->set_main_window_size(this->window->size());
    this->settings->set_browser_splitter_size(this->browser_splitter->saveState());

    // Cleanup.
    this->clean_decks_area();
    this->clean_samplers_area();
    this->clean_file_browser_area();
    delete[] this->dscratch_handles;
    delete this->window;

    return;
}

bool
Gui::apply_application_settings()
{
    // Apply windows style.
    this->window_style = this->settings->get_gui_style();
    if (this->apply_main_window_style() != true)
    {
        qCWarning(DS_APPSETTINGS) << "cannot set new style to main window";
    }
    this->browser_splitter->restoreState(this->settings->get_browser_splitter_size());

    // Show/hide samplers.
    if (this->settings->get_samplers_visible() == true)
    {
        this->show_samplers();
    }
    else
    {
        this->hide_samplers();
    }

    // Change base path for tracks browser.
    if (this->file_system_model->get_root_path() != this->settings->get_tracks_base_dir_path())
    {
        this->set_folder_browser_base_path(this->settings->get_tracks_base_dir_path());
        if (this->is_window_rendered == true) // Do not do it if main window is not already displayed.
        {
            this->set_file_browser_base_path(this->settings->get_tracks_base_dir_path());
        }
    }

    // Apply motion detection settings for all turntables.
    for (int i = 0; i < this->nb_decks; i++)
    {
        // FIXME : wrap these calls into Timecode_control_process and remove the digital_scratch_api dependency (this->dscratch_handles).
        if (dscratch_change_vinyl_type(this->dscratch_handles[i], this->settings->get_vinyl_type()) != DSCRATCH_SUCCESS)
        {
            qCWarning(DS_APPSETTINGS) << "cannot set vinyl type";
        }
        if (dscratch_set_rpm(this->dscratch_handles[i], this->settings->get_rpm()) != DSCRATCH_SUCCESS)
        {
            qCWarning(DS_APPSETTINGS) << "cannot set turntable RPM";
        }
        if (dscratch_set_input_amplify_coeff(this->dscratch_handles[i], this->settings->get_input_amplify_coeff()) != DSCRATCH_SUCCESS)
        {
            qCWarning(DS_APPSETTINGS) << "cannot set new input amplify coeff value";
        }
        if (dscratch_set_min_amplitude_for_normal_speed(this->dscratch_handles[i], this->settings->get_min_amplitude_for_normal_speed()) != DSCRATCH_SUCCESS)
        {
            qCWarning(DS_APPSETTINGS) << "cannot set new min amplitude for normal speed value";
        }
        if (dscratch_set_min_amplitude(this->dscratch_handles[i], this->settings->get_min_amplitude()) != DSCRATCH_SUCCESS)
        {
            qCWarning(DS_APPSETTINGS) << "cannot set new min amplitude value";
        }
    }

    // Change shortcuts.
    this->shortcut_switch_playback->setKey(QKeySequence(this->settings->get_keyboard_shortcut(KB_SWITCH_PLAYBACK)));
    this->shortcut_collapse_browser->setKey(QKeySequence(this->settings->get_keyboard_shortcut(KB_COLLAPSE_BROWSER)));
    this->shortcut_load_audio_file->setKey(QKeySequence(this->settings->get_keyboard_shortcut(KB_LOAD_TRACK_ON_DECK)));
    this->shortcut_go_to_begin->setKey(QKeySequence(this->settings->get_keyboard_shortcut(KB_PLAY_BEGIN_TRACK_ON_DECK)));
    if (this->shortcut_load_samples.size() >= 1)
        this->shortcut_load_samples[0]->setKey(QKeySequence(this->settings->get_keyboard_shortcut(KB_LOAD_TRACK_ON_SAMPLER1)));
    if (this->shortcut_load_samples.size() >= 2)
        this->shortcut_load_samples[1]->setKey(QKeySequence(this->settings->get_keyboard_shortcut(KB_LOAD_TRACK_ON_SAMPLER2)));
    if (this->shortcut_load_samples.size() >= 3)
        this->shortcut_load_samples[2]->setKey(QKeySequence(this->settings->get_keyboard_shortcut(KB_LOAD_TRACK_ON_SAMPLER3)));
    if (this->shortcut_load_samples.size() >= 4)
        this->shortcut_load_samples[3]->setKey(QKeySequence(this->settings->get_keyboard_shortcut(KB_LOAD_TRACK_ON_SAMPLER4)));
    this->shortcut_show_next_keys->setKey(QKeySequence(this->settings->get_keyboard_shortcut(KB_SHOW_NEXT_KEYS)));
    this->shortcut_fullscreen->setKey(QKeySequence(this->settings->get_keyboard_shortcut(KB_FULLSCREEN)));
    this->shortcut_help->setKey(QKeySequence(this->settings->get_keyboard_shortcut(KB_HELP)));
    this->shortcut_file_search->setKey(QKeySequence(this->settings->get_keyboard_shortcut(KB_FILE_SEARCH)));
    this->shortcut_file_search_press_enter->setKey(QKeySequence(Qt::Key_Enter));
    this->shortcut_file_search_press_esc->setKey(QKeySequence(Qt::Key_Escape));
    for (unsigned short int i = 0; i < MAX_NB_CUE_POINTS; i++)
    {
        this->shortcut_set_cue_points[i]->setKey(QKeySequence(this->settings->get_keyboard_shortcut(KB_SET_CUE_POINTS_ON_DECK[i])));
        this->shortcut_go_to_cue_points[i]->setKey(QKeySequence(this->settings->get_keyboard_shortcut(KB_PLAY_CUE_POINTS_ON_DECK[i])));
    }

    // Set shortcut value in help bottom span.
    this->set_help_shortcut_value();

    return true;
}

void
Gui::start_control_and_playback()
{
    // Start sound card for capture and playback.
    if (this->control_and_play->start() == false)
    {
        qCWarning(DS_SOUNDCARD) << "can not start sound card";
        this->start_capture_button->setChecked(false);
        this->stop_capture_button->setChecked(true);
    }
    else
    {
        this->start_capture_button->setChecked(true);
        this->stop_capture_button->setChecked(false);
    }

    return;
}

void
Gui::stop_control_and_playback()
{
    // Stop sound card for capture and playback.
    if (this->control_and_play->stop() == false)
    {
        qCWarning(DS_SOUNDCARD) << "can not stop sound card";
        this->stop_capture_button->setChecked(false);
        this->start_capture_button->setChecked(true);
    }
    else
    {
        this->stop_capture_button->setChecked(true);
        this->start_capture_button->setChecked(false);
    }

    return;
}

bool
Gui::can_stop_control_and_playback()
{
    // Show a pop-up asking to confirm to stop capture.
    QMessageBox msg_box;
    msg_box.setWindowTitle("DigitalScratch");
    msg_box.setText(tr("Do you really want to stop turntable motion detection ?"));
    msg_box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msg_box.setIcon(QMessageBox::Question);
    msg_box.setDefaultButton(QMessageBox::Cancel);
    msg_box.setStyleSheet(Utils::get_current_stylesheet_css());
    if (this->nb_decks > 1)
    {
        msg_box.setWindowIcon(QIcon(ICON_2));
    }
    else
    {
        msg_box.setWindowIcon(QIcon(ICON));
    }

    // Close request confirmed.
    if (msg_box.exec() == QMessageBox::Ok)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
Gui::show_config_window()
{
    // Create a configuration dialog.
    if (this->config_dialog != nullptr)
    {
        delete this->config_dialog;
    }
    this->config_dialog = new Config_dialog(this->window);

    // Apply application settings if dialog is closed by OK.
    if (this->config_dialog->show() == QDialog::Accepted)
    {
        if (this->apply_application_settings() != true)
        {
            qCCritical(DS_APPSETTINGS) << "can not apply settings";
            return false;
        }
    }

    // Cleanup.
    delete this->config_dialog;
    this->config_dialog = nullptr;

    return true;
}

void
Gui::set_fullscreen()
{
    if (this->window->isFullScreen() == false)
    {
        this->window->showFullScreen();
    }
    else
    {
        this->window->showNormal();
    }
}

void
Gui::show_help()
{
    if (this->help_groupbox->isHidden() == true)
    {
        this->help_groupbox->show();
    }
    else
    {
        this->help_groupbox->hide();
    }
}

void
Gui::set_focus_search_bar()
{
    this->file_search->setFocus();
    this->file_search->selectAll();
    this->search_from_begin = true;
}

void
Gui::press_enter_in_search_bar()
{
    this->search_from_begin = false;
    this->file_search_string(this->last_search_string);
}

void
Gui::press_esc_in_search_bar()
{
    this->file_browser->setFocus();
}


void
Gui::file_search_string(const QString &text)
{
    if (text != "")
    {
        // Store search text.
        this->last_search_string = text;

        // Search text in file browser (get a list of results).
        QModelIndexList items = this->file_system_model->search(text);

        if (this->search_from_begin == true)
        {
            // If we found file/dir name that match, return the first one.
            if (items.size() > 0)
            {
                // Select item in file browser.
                this->file_browser->setCurrentIndex(items[0]);
                this->file_browser_selected_index = 0;
            }
        }
        else
        {
            // If we found file/dir name that match, return the next one.
            if (items.size() > 0)
            {
                if ((int)this->file_browser_selected_index + 1 < items.size())
                {
                    // Select next item in file browser.
                    this->file_browser_selected_index++;
                }
                else
                {
                    // Wrap search, so select first item again.
                    this->file_browser_selected_index = 0;
                }
                this->file_browser->setCurrentIndex(items[this->file_browser_selected_index]);
            }

            this->search_from_begin = true;
        }
    }
}

void
Gui::analyze_audio_collection(const bool &is_all_files)
{
    // Analyzis not running, show a popup asking for a full refresh or only for new files.
    this->settings->set_audio_collection_full_refresh(is_all_files);

    // Show progress bar.
    this->progress_label->setText(tr("Analysing audio collection..."));
    this->progress_groupbox->show();

    // Compute data on file collection and store them to DB.
    this->file_system_model->concurrent_analyse_audio_collection();
}

void
Gui::update_refresh_progress_value(const unsigned int &value)
{
    this->progress_bar->setValue(value);

    if (this->file_system_model->concurrent_watcher_store->isRunning() == true)
    {
        // Refresh file browser during running file analyzis and storage.
        this->file_browser->update();
    }
}

void
Gui::on_finished_analyze_audio_collection()
{
    // Hide progress bar.
    this->progress_label->setText("");
    this->progress_groupbox->hide();

    // Refresh file browser.
    this->file_browser->setRootIndex(this->file_system_model->get_root_index());
    this->refresh_file_browser->setEnabled(true);
    this->refresh_file_browser->setChecked(false);
}

void
Gui::reject_refresh_audio_collection_dialog()
{
    this->refresh_file_browser->setEnabled(true);
    this->refresh_file_browser->setChecked(false);

    return;
}

void
Gui::close_refresh_audio_collection_dialog()
{
    if (this->refresh_audio_collection_dialog != nullptr)
    {
        this->refresh_audio_collection_dialog->done(QDialog::Rejected);
    }

    return;
}

void
Gui::accept_refresh_audio_collection_dialog_all_files()
{
    // Analyze all files of audio collection.
    this->analyze_audio_collection(true);

    if (this->refresh_audio_collection_dialog != nullptr)
    {
        this->refresh_audio_collection_dialog->done(QDialog::Accepted);
    }

    return;
}

void
Gui::accept_refresh_audio_collection_dialog_new_files()
{
    // Analyze all files of audio collection.
    this->analyze_audio_collection(false);

    if (this->refresh_audio_collection_dialog != nullptr)
    {
        this->refresh_audio_collection_dialog->done(QDialog::Accepted);
    }

    return;
}

bool
Gui::show_refresh_audio_collection_dialog()
{
    // Show dialog only if there is no other analyzis process running.
    if (this->file_system_model->concurrent_watcher_store->isRunning() == false)
    {
        // Create the dialog object.
        if (this->refresh_audio_collection_dialog != nullptr)
        {
            delete this->refresh_audio_collection_dialog;
        }
        this->refresh_audio_collection_dialog = new QDialog(this->window);

        // Set properties : title, icon.
        this->refresh_audio_collection_dialog->setWindowTitle(tr("Refresh audio collection"));
        if (this->nb_decks > 1)
        {
            this->refresh_audio_collection_dialog->setWindowIcon(QIcon(ICON_2));
        }
        else
        {
            this->refresh_audio_collection_dialog->setWindowIcon(QIcon(ICON));
        }

        // Main question.
        QLabel *main_question = new QLabel("<h3>" + tr("There are 2 possibilities to analyze the audio collection.") + "</h3>"
                                           + "<p>" + tr("Click the choice you would like") + "</p>",
                                           this->refresh_audio_collection_dialog);


        // Choice 1: => All files.
        QCommandLinkButton *choice_all_files_button = new QCommandLinkButton(tr("All files."),
                                                                             tr("Analyze full audio collection") + " (" + QString::number(this->file_system_model->get_nb_items()) + " " + tr("elements") + ")",
                                                                             this->refresh_audio_collection_dialog);
        QObject::connect(choice_all_files_button, &QCommandLinkButton::clicked,
                         [this](){this->accept_refresh_audio_collection_dialog_all_files();});

        // Choice 2: => New files.
        QCommandLinkButton *choice_new_files_button = new QCommandLinkButton(tr("New files."),
                                                                             tr("Analyze only files with missing data") + " (" + QString::number(this->file_system_model->get_nb_new_items()) + " " + tr("elements") + ")",
                                                                             this->refresh_audio_collection_dialog);
        QObject::connect(choice_new_files_button, &QCommandLinkButton::clicked,
                         [this](){this->accept_refresh_audio_collection_dialog_new_files();});

        // Close/cancel button.
        QDialogButtonBox *cancel_button = new QDialogButtonBox(QDialogButtonBox::Cancel);
        QObject::connect(cancel_button, &QDialogButtonBox::rejected,
                         [this](){this->close_refresh_audio_collection_dialog();});
        QObject::connect(refresh_audio_collection_dialog, &QDialog::rejected,
                         [this](){this->reject_refresh_audio_collection_dialog();});

        // Full dialog layout.
        QVBoxLayout *layout = new QVBoxLayout(this->refresh_audio_collection_dialog);
        layout->addWidget(main_question);
        layout->addWidget(choice_all_files_button);
        layout->addWidget(choice_new_files_button);
        layout->addWidget(cancel_button);

        // Put layout in dialog.
        this->refresh_audio_collection_dialog->setLayout(layout);
        layout->setSizeConstraint(QLayout::SetFixedSize);

        // Show dialog.
        this->refresh_audio_collection_dialog->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        this->refresh_audio_collection_dialog->adjustSize();
        this->refresh_audio_collection_dialog->exec();

        // Cleanup.
        delete this->refresh_audio_collection_dialog;
        this->refresh_audio_collection_dialog = nullptr;
    }
    else
    {
        // Analyzis already running. Cancel it.
        this->file_system_model->stop_concurrent_analyse_audio_collection();
    }

    return true;
}

void
Gui::show_hide_samplers()
{
    if (this->samplers_container->isVisible() == true)
    {
        // Hide samplers.
        this->hide_samplers();
    }
    else
    {
        // Show samplers.
        this->show_samplers();
    }
}

void
Gui::hide_samplers()
{
    this->show_hide_samplers_button->setChecked(false);
    this->settings->set_samplers_visible(false);
    this->samplers_container->hide();
    for (unsigned short int i = 0; i < this->nb_decks; i++)
    {
        this->file_browser_control_buttons[i]->hide_load_sample_buttons();
    }
}

void
Gui::show_samplers()
{
    this->show_hide_samplers_button->setChecked(true);
    this->settings->set_samplers_visible(true);
    this->samplers_container->show();
    for (unsigned short int i = 0; i < this->nb_decks; i++)
    {
        this->file_browser_control_buttons[i]->show_load_sample_buttons();
    }
}

bool
Gui::show_about_window()
{
    // Create about window.
    QDialog about_dialog;

    // Set properties : title, icon.
    about_dialog.setWindowTitle(tr("About DigitalScratch"));
    if (this->nb_decks > 1)
    {
        about_dialog.setWindowIcon(QIcon(ICON_2));
    }
    else
    {
        about_dialog.setWindowIcon(QIcon(ICON));
    }

    //
    // Set content (logo, name-version, description, credit, license, libraries).
    //
    QLabel logo;
    logo.setPixmap(QPixmap(LOGO));
    logo.setAlignment(Qt::AlignHCenter);

    QString version = QString("<h1>DigitalScratch ") + QString(STR(VERSION)) + QString("</h1>");
    QLabel name(tr(version.toUtf8()));
    name.setAlignment(Qt::AlignHCenter);
    name.setTextFormat(Qt::RichText);

    QLabel description(tr("A vinyl emulation software."));
    description.setAlignment(Qt::AlignHCenter);

    QLabel web_site("<a style=\"color: orange\" href=\"http://www.digital-scratch.org\">http://www.digital-scratch.org</a>");
    web_site.setAlignment(Qt::AlignHCenter);
    web_site.setTextFormat(Qt::RichText);
    web_site.setTextInteractionFlags(Qt::TextBrowserInteraction);
    web_site.setOpenExternalLinks(true);

    QLabel credit(tr("Copyright (C) 2003-2015 Julien Rosener"));
    credit.setAlignment(Qt::AlignHCenter);

    QLabel license(tr("This program is free software; you can redistribute it and/or modify <br/>\
                       it under the terms of the GNU General Public License as published by <br/>\
                       the Free Software Foundation; either version 3 of the License, or <br/>\
                       (at your option) any later version.<br/><br/>"));
    license.setTextFormat(Qt::RichText);
    license.setAlignment(Qt::AlignHCenter);

    QLabel built("<b>" + tr("Built with:") + "</b>");

    built.setTextFormat(Qt::RichText);
    QLabel qt_version((QString("- Qt v") + QString(qVersion())).toUtf8()
                       + ", <a style=\"color: grey\" href=\"http://www.qt.io\">http://www.qt.io</a>");
    qt_version.setTextFormat(Qt::RichText);
    qt_version.setTextInteractionFlags(Qt::TextBrowserInteraction);
    qt_version.setOpenExternalLinks(true);

    QLabel libdigitalscratch_version((QString("- libdigitalscratch v") + QString(dscratch_get_version())).toUtf8()
                                     + ", <a style=\"color: grey\" href=\"http://www.digital-scratch.org\">http://www.digital-scratch.org</a>");
    libdigitalscratch_version.setTextFormat(Qt::RichText);
    libdigitalscratch_version.setTextInteractionFlags(Qt::TextBrowserInteraction);
    libdigitalscratch_version.setOpenExternalLinks(true);

    QLabel libavcodec_version((QString("- libavcodec v")
                               + QString::number(LIBAVCODEC_VERSION_MAJOR) + QString(".") + QString::number(LIBAVCODEC_VERSION_MINOR) + QString(".") + QString::number(LIBAVCODEC_VERSION_MICRO)).toUtf8()
                               + ", <a style=\"color: grey\" href=\"http://www.libav.org\">http://www.libav.org</a>");
    libavcodec_version.setTextFormat(Qt::RichText);
    libavcodec_version.setTextInteractionFlags(Qt::TextBrowserInteraction);
    libavcodec_version.setOpenExternalLinks(true);

    QLabel libavformat_version((QString("- libavformat v")
                                + QString::number(LIBAVFORMAT_VERSION_MAJOR) + QString(".") + QString::number(LIBAVFORMAT_VERSION_MINOR) + QString(".") + QString::number(LIBAVFORMAT_VERSION_MICRO)).toUtf8()
                                + ", <a style=\"color: grey\" href=\"http://www.libav.org\">http://www.libav.org</a>");
    libavformat_version.setTextFormat(Qt::RichText);
    libavformat_version.setTextInteractionFlags(Qt::TextBrowserInteraction);
    libavformat_version.setOpenExternalLinks(true);

    QLabel libavutil_version((QString("- libavutil v")
                              + QString::number(LIBAVUTIL_VERSION_MAJOR) + QString(".") + QString::number(LIBAVUTIL_VERSION_MINOR) + QString(".") + QString::number(LIBAVUTIL_VERSION_MICRO)).toUtf8()
                              + ", <a style=\"color: grey\" href=\"http://www.libav.org\">http://www.libav.org</a>");
    libavutil_version.setTextFormat(Qt::RichText);
    libavutil_version.setTextInteractionFlags(Qt::TextBrowserInteraction);
    libavutil_version.setOpenExternalLinks(true);

    QLabel libsamplerate_version((QString("- ") + QString(src_get_version())).toUtf8()
                                  + ", <a style=\"color: grey\" href=\"http://www.mega-nerd.com/SRC/\">http://www.mega-nerd.com/SRC/</a>");
    libsamplerate_version.setTextFormat(Qt::RichText);
    libsamplerate_version.setTextInteractionFlags(Qt::TextBrowserInteraction);
    libsamplerate_version.setOpenExternalLinks(true);

    QLabel libjack_version((QString("- libjack v") + QString(jack_get_version_string())).toUtf8()
                            + ", <a style=\"color: grey\" href=\"http://jackaudio.org\">http://jackaudio.org</a>");
    libjack_version.setTextFormat(Qt::RichText);
    libjack_version.setTextInteractionFlags(Qt::TextBrowserInteraction);
    libjack_version.setOpenExternalLinks(true);

    QLabel libkeyfinder_version((QString("- libkeyfinder v") + QString(kfinder_get_version())).toUtf8()
                                 + ", <a style=\"color: grey\" href=\"http://www.ibrahimshaath.co.uk/keyfinder/\">http://www.ibrahimshaath.co.uk/keyfinder/</a>");
    libkeyfinder_version.setTextFormat(Qt::RichText);
    libkeyfinder_version.setTextInteractionFlags(Qt::TextBrowserInteraction);
    libkeyfinder_version.setOpenExternalLinks(true);

    QLabel icons_credits("<br/><b>" + tr("Credits:") + "</b>");
    icons_credits.setTextFormat(Qt::RichText);

    QLabel icons("- Devine icons: <a style=\"color: grey\" href=\"http://ipapun.deviantart.com\">http://ipapun.deviantart.com</a>");
    icons.setTextFormat(Qt::RichText);
    icons.setTextInteractionFlags(Qt::TextBrowserInteraction);
    icons.setOpenExternalLinks(true);

    QLabel help("<br/><b>" + tr("Help:") + "</b>");
    help.setTextFormat(Qt::RichText);

    QLabel wiki("- Online help: <a style=\"color: grey\" href=\"https://github.com/jrosener/digitalscratch/wiki\">https://github.com/jrosener/digitalscratch/wiki</a>");
    wiki.setTextFormat(Qt::RichText);
    wiki.setTextInteractionFlags(Qt::TextBrowserInteraction);
    wiki.setOpenExternalLinks(true);

    // Close button.
    QDialogButtonBox button(QDialogButtonBox::Close);
    QObject::connect(&button, &QDialogButtonBox::rejected, [&about_dialog](){about_dialog.close();});

    // Full window layout.
    QVBoxLayout layout;
    layout.addWidget(&logo, Qt::AlignHCenter);
    layout.addWidget(&name, Qt::AlignHCenter);
    layout.addWidget(&description, Qt::AlignHCenter);
    layout.addWidget(&web_site, Qt::AlignHCenter);
    layout.addWidget(&credit);
    layout.addWidget(&license);
    layout.addWidget(&built);
    layout.addWidget(&qt_version);
    layout.addWidget(&libdigitalscratch_version);
    layout.addWidget(&libavcodec_version);
    layout.addWidget(&libavformat_version);
    layout.addWidget(&libavutil_version);
    layout.addWidget(&libsamplerate_version);
    layout.addWidget(&libjack_version);
    layout.addWidget(&libkeyfinder_version);
    layout.addWidget(&icons_credits);
    layout.addWidget(&icons);
    layout.addWidget(&help);
    layout.addWidget(&wiki);
    layout.addWidget(&button);

    // Put layout in dialog.
    about_dialog.setLayout(&layout);
    layout.setSizeConstraint(QLayout::SetFixedSize);

    // Apply stylesheet.
    about_dialog.setStyleSheet(Utils::get_current_stylesheet_css());

    // Show dialog.
    about_dialog.exec();

    return true;
}

bool
Gui::show_error_window(const QString &error_message)
{
    // Prepare error window.
    QMessageBox msg_box;
    msg_box.setWindowTitle("DigitalScratch");
    msg_box.setText("<h2>" + tr("Error") + "</h2>"
                    + "<br/>" + error_message
                    + "<br/><br/>" + "Please fix this issue and restart DigitalScratch.");
    msg_box.setStandardButtons(QMessageBox::Close);
    msg_box.setIcon(QMessageBox::Critical);
    msg_box.setStyleSheet(Utils::get_current_stylesheet_css());
    if (this->nb_decks > 1)
    {
        msg_box.setWindowIcon(QIcon(ICON_2));
    }
    else
    {
        msg_box.setWindowIcon(QIcon(ICON));
    }

    // Show error dialog.
    msg_box.exec();

    return true;
}

bool
Gui::create_main_window()
{
    // Init main window.
    this->window             = new QWidget();
    this->is_window_rendered = false;
    this->window_style       = GUI_STYLE_DEFAULT;

    // Init areas (header buttons, decks, samplers, file browser, bottom help, bottom status bar).
    this->init_header_buttons();
    this->init_decks_area();
    this->init_samplers_area();
    this->init_file_browser_area();
    this->init_bottom_help();
    this->init_bottom_status();

    // Bind buttons/shortcuts to events.
    this->connect_header_buttons();
    this->connect_decks_area();
    this->connect_samplers_area();
    this->connect_decks_and_samplers_selection();
    this->connect_file_browser_area();

    // Create main window.
    this->window->setWindowTitle(tr("DigitalScratch") + " " + QString(STR(VERSION)));
    this->window->setMinimumSize(800, 480);
    if (this->nb_decks > 1)
    {
        this->window->setWindowIcon(QIcon(ICON_2));
    }
    else
    {
        this->window->setWindowIcon(QIcon(ICON));
    }

    // Create main vertical layout.
    QVBoxLayout *main_layout = new QVBoxLayout;

    // Set main layout in main window.
    this->window->setLayout(main_layout);

    // Put every components in main layout.
    main_layout->addLayout(this->header_layout,      5);
    main_layout->addLayout(this->decks_layout,       30);
    main_layout->addWidget(this->samplers_container, 5);
    main_layout->addLayout(this->file_layout,        65);
    main_layout->addLayout(this->bottom_layout,      0);
    main_layout->addLayout(this->status_layout,      0);

    // Display main window.
    this->window->show();

    // Set window position/size from last run.
    this->window->move(this->settings->get_main_window_position());
    this->window->resize(this->settings->get_main_window_size());

    // Open error window.
    QObject::connect(this->sound_card.data(), &Sound_driver_access_rules::error_msg,
                     [this](QString in_error_message){this->show_error_window(in_error_message);});
    return true;
}

void
Gui::init_header_buttons()
{
    // Create configuration button.
    this->config_button = new QPushButton("   " + tr("&Settings"));
    this->config_button->setToolTip(tr("Change application settings..."));
    this->config_button->setObjectName("Configuration_button");
    this->config_button->setFocusPolicy(Qt::NoFocus);

    // Create button to set full screen.
    this->fullscreen_button = new QPushButton("   " + tr("&Full-screen"));
    this->fullscreen_button->setToolTip("<p>" + tr("Toggle fullscreen mode") + "</p><em>" + this->settings->get_keyboard_shortcut(KB_FULLSCREEN) + "</em>");
    this->fullscreen_button->setObjectName("Fullscreen_button");
    this->fullscreen_button->setFocusPolicy(Qt::NoFocus);

    // Create Stop capture button.
    this->stop_capture_button = new QPushButton(tr("S&TOP"));
    this->stop_capture_button->setToolTip("<p>" + tr("Stop turntable motion detection") + "</p>");
    this->stop_capture_button->setObjectName("Capture_buttons");
    this->stop_capture_button->setFocusPolicy(Qt::NoFocus);
    this->stop_capture_button->setCheckable(true);
    this->stop_capture_button->setChecked(true);

    // Create DigitalScratch logo.
    this->logo = new QPushButton();
    this->logo->setToolTip(tr("About DigitalScratch..."));
    this->logo->setObjectName("Logo");
    this->logo->setIcon(QIcon(LOGO));
    this->logo->setIconSize(QSize(112, 35));
    this->logo->setMaximumWidth(112);
    this->logo->setMaximumHeight(35);
    this->logo->setFlat(true);
    this->logo->setFocusPolicy(Qt::NoFocus);

    // Create Start capture button.
    this->start_capture_button = new QPushButton(tr("ST&ART"));
    this->start_capture_button->setToolTip("<p>" + tr("Start turntable motion detection") + "</p>");
    this->start_capture_button->setObjectName("Capture_buttons");
    this->start_capture_button->setFocusPolicy(Qt::NoFocus);
    this->start_capture_button->setCheckable(true);
    this->start_capture_button->setChecked(false);

    // Create help button.
    this->help_button = new QPushButton("   " + tr("&Help"));
    this->help_button->setToolTip("<p>" + tr("Show/hide keyboard shortcuts") + "</p><em>" + this->settings->get_keyboard_shortcut(KB_HELP) + "</em>");
    this->help_button->setObjectName("Help_button");
    this->help_button->setFocusPolicy(Qt::NoFocus);

    // Create quit button.
    this->quit_button = new QPushButton("   " + tr("&Exit"));
    this->quit_button->setToolTip(tr("Exit DigitalScratch"));
    this->quit_button->setObjectName("Quit_button");
    this->quit_button->setFocusPolicy(Qt::NoFocus);

    // Create top horizontal layout.
    this->header_layout = new QHBoxLayout();

    // Put configuration button and logo in configuration layout.
    this->header_layout->addWidget(this->config_button,        1,   Qt::AlignLeft);
    this->header_layout->addWidget(this->fullscreen_button,    1,   Qt::AlignLeft);
    this->header_layout->addWidget(this->stop_capture_button,  100, Qt::AlignRight);
    this->header_layout->addWidget(this->logo,                 1,   Qt::AlignCenter);
    this->header_layout->addWidget(this->start_capture_button, 100, Qt::AlignLeft);
    this->header_layout->addWidget(this->help_button,          1,   Qt::AlignRight);
    this->header_layout->addWidget(this->quit_button,          1,   Qt::AlignRight);
}

void
Gui::connect_header_buttons()
{   
    // Open configuration window.
    QObject::connect(this->config_button, &QPushButton::clicked, [this](){this->show_config_window();});

    // Set full screen.
    QObject::connect(this->fullscreen_button, &QPushButton::clicked, [this](){this->set_fullscreen();});
    this->shortcut_fullscreen = new QShortcut(this->window);
    QObject::connect(this->shortcut_fullscreen, &QShortcut::activated, [this](){this->set_fullscreen();});

    // Stop capture.
    QObject::connect(this->stop_capture_button, &QPushButton::clicked, [this](){this->stop_control_and_playback();});

    // Open about window.
    QObject::connect(this->logo, &QPushButton::clicked, [this](){show_about_window();});

    // Start capture.
    QObject::connect(this->start_capture_button, &QPushButton::clicked, [this](){this->start_control_and_playback();});

    // Help button.
    QObject::connect(this->help_button, &QPushButton::clicked, [this](){this->show_help();});
    this->shortcut_help = new QShortcut(this->window);
    QObject::connect(this->shortcut_help, &QShortcut::activated, [this](){this->show_help();});

    // Quit application.
    QObject::connect(this->quit_button, &QPushButton::clicked, [this](){this->can_close();});
}

void
Gui::init_decks_area()
{
    // Create horizontal layout for decks.
    this->decks_layout = new QHBoxLayout;

    // Create deck area and put it in layout.
    this->decks.clear();
    for (unsigned short int i = 0; i < this->nb_decks; i++)
    {
        Deck *dk = new Deck(tr("Deck ") + QString::number(i+1), this->ats[i]);
        dk->init_display();
        this->decks.push_back(dk);
        this->decks_layout->addWidget(dk);
    }
}

void
Gui::clean_decks_area()
{
    for (unsigned short int i = 0; i < this->nb_decks; i++)
    {
        delete this->decks[i];
    }

    delete [] this->shortcut_set_cue_points;
    delete [] this->shortcut_go_to_cue_points;

    delete this->decks_layout;
}

void
Gui::connect_decks_area()
{
    for(unsigned short int i = 0; i < this->nb_decks; i++)
    {
        // Toggle timecode/manual mode.
        QObject::connect(this->decks[i]->timecode_button, &QPushButton::clicked,
                         [this, i](bool checked)
                         {
                             if (checked == true)
                             {
                                 this->decks[i]->set_speed_mode_timecode();
                                 this->decks[i]->thru_button->setChecked(false);
                                 this->control_and_play->set_process_mode(ProcessMode::TIMECODE, i);
                             }
                             else
                             {
                                 this->decks[i]->timecode_button->setChecked(true);
                             }
                        });
        QObject::connect(this->decks[i]->manual_button, &QPushButton::clicked,
                        [this, i](bool checked)
                        {
                            if (checked == true)
                            {
                                this->decks[i]->set_speed_mode_manual();
                                this->control_and_play->set_process_mode(ProcessMode::MANUAL, i);
                            }
                            else
                            {
                                this->decks[i]->manual_button->setChecked(true);
                            }
                        });

        // Display speed (manual controller).
        QObject::connect(this->manual_controls[i].data(), &Control_process::speed_changed,
                        [this, i](float in_speed)
                        {
                            this->update_speed_label(in_speed, i);
                        });

        // Display speed (timecode controller).
        QObject::connect(this->tcode_controls[i].data(), &Control_process::speed_changed,
                        [this, i](float in_speed)
                        {
                            this->update_speed_label(in_speed, i);
                        });

        // Manual mode only: reset speed to 100% when right clicking on speed label.
        QObject::connect(this->decks[i]->speed, &SpeedQLabel::right_clicked,
                        [this, i]()
                        {
                            if (this->control_and_play->get_process_mode(i) == ProcessMode::MANUAL)
                            {
                                this->speed_reset_to_100p(i);
                            }
                        });

        // Speed up/down 0.1%.
        QObject::connect(this->decks[i]->speed_up_button, &SpeedQPushButton::clicked,
                        [this, i]()
                        {
                            this->speed_up_down(0.001f, i);
                        });
        QObject::connect(this->decks[i]->speed_down_button, &SpeedQPushButton::clicked,
                        [this, i]()
                        {
                            this->speed_up_down(-0.001f, i);
                        });

        // Speed up/down 1%.
        QObject::connect(this->decks[i]->speed_up_button, &SpeedQPushButton::right_clicked,
                        [this, i]()
                        {
                            this->speed_up_down(0.01f, i);
                        });
        QObject::connect(this->decks[i]->speed_down_button, &SpeedQPushButton::right_clicked,
                        [this, i]()
                        {
                            this->speed_up_down(-0.01f, i);
                        });

        // Speed acceleration/deceleration 1%.
        QObject::connect(this->decks[i]->accel_up_button, &SpeedQPushButton::clicked,
                        [this, i]()
                        {
                            this->speed_accel(0.01f, i);
                        });
        QObject::connect(this->decks[i]->accel_down_button, &SpeedQPushButton::clicked,
                        [this, i]()
                        {
                            this->speed_accel(-0.01f, i);
                        });

        // Speed acceleration/deceleration 10%.
        QObject::connect(this->decks[i]->accel_up_button, &SpeedQPushButton::right_clicked,
                        [this, i]()
                        {
                            this->speed_accel(0.1f, i);
                        });
        QObject::connect(this->decks[i]->accel_down_button, &SpeedQPushButton::right_clicked,
                        [this, i]()
                        {
                            this->speed_accel(-0.1f, i);
                        });

        // Enable track file dropping.
        QObject::connect(this->decks[i], &Deck::file_dropped, [this, i](){this->select_and_run_audio_file_decoding_process(i);});

        // Remaining time.
        QObject::connect(this->playbacks[i].data(), &Deck_playback_process::remaining_time_changed,
                         [this, i](const unsigned int &remaining_time)
                         {
                            this->set_remaining_time(remaining_time, i);
                         });

        // Name of the track.
        QObject::connect(this->decs[i].data(), &Audio_file_decoding_process::name_changed, [this, i](QString name){this->decks[i]->track_name->setText(name);});

        // Thru button.
        QObject::connect(this->decks[i]->thru_button, &QPushButton::clicked,
                         [this, i](bool checked) {this->playback_thru(i, checked);});

        // Music key of the track.
        QObject::connect(this->decs[i].data(), &Audio_file_decoding_process::key_changed, [this, i](QString key){this->decks[i]->set_key(key);});

        // Move in track when slider is moved on waveform.
        QObject::connect(this->decks[i]->waveform, &Waveform::slider_position_changed, [this, i](float position){this->jump_to_position(position, i);});

        // Restart to begin.
        QObject::connect(this->decks[i]->restart_button, &QPushButton::clicked, [this, i](){this->go_to_begin(i);});

        // Cue points.
        for (unsigned short int j = 0; j < MAX_NB_CUE_POINTS; j++)
        {
            // Set cue point.
            QObject::connect(this->decks[i]->cue_set_buttons[j], &QPushButton::clicked, [this, i, j](){this->set_cue_point(i, j);});

            // Go to cue point.
            QObject::connect(this->decks[i]->cue_play_buttons[j], &QPushButton::clicked, [this, i, j](){this->go_to_cue_point(i, j);});

            // Delete cue point.
            QObject::connect(this->decks[i]->cue_del_buttons[j], &QPushButton::clicked, [this, i, j](){this->del_cue_point(i, j);});
        }
    }

    // Keyboard shortcut to restart to begin.
    this->shortcut_go_to_begin = new QShortcut(this->window);
    QObject::connect(this->shortcut_go_to_begin,  &QShortcut::activated, [this](){this->deck_go_to_begin();});

    // Keyboard shortcut and buttons to set and play cue points.
    this->shortcut_set_cue_points   = new QShortcut* [MAX_NB_CUE_POINTS];
    this->shortcut_go_to_cue_points = new QShortcut* [MAX_NB_CUE_POINTS];
    for (unsigned short int j = 0; j < MAX_NB_CUE_POINTS; j++)
    {
        // Shortcut: set cue point.
        this->shortcut_set_cue_points[j] = new QShortcut(this->window);
        QObject::connect(this->shortcut_set_cue_points[j],   &QShortcut::activated, [this, j](){this->deck_set_cue_point(j);});

        // Shortcut: go to cue point.
        this->shortcut_go_to_cue_points[j] = new QShortcut(this->window);
        QObject::connect(this->shortcut_go_to_cue_points[j], &QShortcut::activated, [this, j](){this->deck_go_to_cue_point(j);});
    }
}

void
Gui::init_samplers_area()
{
    // Create horizontal layout for samplers.
    this->samplers_layout = new QHBoxLayout();
    this->samplers_layout->setMargin(0);
    this->samplers_container = new QWidget();
    this->samplers_container->setLayout(samplers_layout);

    // Create sampler area and put it in layout.
    this->samplers.clear();
    for (unsigned short int i = 0; i < this->nb_decks; i++)
    {
        Sampler *sp = new Sampler(tr("Sampler ") + QString::number(i+1), this->nb_samplers);
        sp->init_display();
        this->samplers.push_back(sp);
        this->samplers_layout->addWidget(this->samplers[i]);
    }
}

void
Gui::clean_samplers_area()
{
    for (unsigned short int i = 0; i < this->nb_decks; i++)
    {
        delete this->samplers[i];
    }
}

void
Gui::connect_samplers_area()
{
    // Show/hide samplers.
    QObject::connect(this->show_hide_samplers_button, &QPushButton::clicked, [this](){this->show_hide_samplers();});

    for (unsigned short int i = 0; i < this->nb_decks; i++)
    {
        for (unsigned short int j = 0; j < this->nb_samplers; j++)
        {
            // Button: play sampler.
            QObject::connect(this->samplers[i]->buttons_play[j], &QPushButton::clicked,
                             [this, i, j](){this->sampler_button_play_clicked(i, j);});

            // Button: stop sampler.
            QObject::connect(this->samplers[i]->buttons_stop[j], &QPushButton::clicked,
                             [this, i, j](){this->sampler_button_stop_clicked(i, j);});

            // Button: del sampler.
            QObject::connect(this->samplers[i]->buttons_del[j], &QPushButton::clicked,
                             [this, i, j](){this->sampler_button_del_clicked(i, j);});

            // Track name.
            QObject::connect(this->dec_samplers[i][j].data(), &Audio_file_decoding_process::name_changed,
                            [this, i, j](QString text)
                            {
                                this->set_sampler_text(text, i, j);
                            });

            // Drag and drop: load file.
            QObject::connect(this->samplers[i]->drop_areas[j], &QSamplerContainerWidget::file_dropped_in_sampler,
                             [this, i, j]()
                             {
                                this->run_sampler_decoding_process(i, j);
                             });
        }

        // Remaining time for samplers.
        QObject::connect(this->playbacks[i].data(), &Deck_playback_process::sampler_remaining_time_changed,
                        [this, i](unsigned int remaining_time, int sampler_index)
                        {
                            this->set_sampler_remaining_time(remaining_time, i, sampler_index);
                        });

        // State for samplers.
        QObject::connect(this->playbacks[i].data(), &Deck_playback_process::sampler_state_changed,
                        [this, i](int sampler_index, bool state)
                        {
                            this->set_sampler_state(i, sampler_index, state);
                        });
    }
}

void
Gui::connect_decks_and_samplers_selection()
{
    for (unsigned short int i = 0; i < this->nb_decks; i++)
    {
        // Create mouse action to select deck/sampler.
        QObject::connect(this->decks[i],    &Deck::selected,    [this, i](){this->select_playback(i);});
        QObject::connect(this->samplers[i], &Sampler::selected, [this, i](){this->select_playback(i);});

        // Create mouse action when deck/sampler is hovered (mouse entering area).
        QObject::connect(this->decks[i],    &Deck::hover,    [this, i](){this->hover_playback(i);});
        QObject::connect(this->samplers[i], &Sampler::hover, [this, i](){this->hover_playback(i);});

        // Create mouse action when deck/sampler is unhovered (mouse leaving area).
        QObject::connect(this->decks[i],    &Deck::unhover,    [this, i](){this->unhover_playback(i);});
        QObject::connect(this->samplers[i], &Sampler::unhover, [this, i](){this->unhover_playback(i);});
    }

    // Preselect deck and sampler 1.
    this->decks[0]->setProperty("selected", true);
    this->samplers[0]->setProperty("selected", true);

    // Connect keyboard shortcut to switch selection of decks/samplers.
    this->shortcut_switch_playback = new QShortcut(this->window);
    QObject::connect(this->shortcut_switch_playback, &QShortcut::activated, [this](){this->switch_playback_selection();});
}

void
Gui::init_file_browser_area()
{
    // Create the folder browser.
    this->treeview_icon_provider = new TreeViewIconProvider();
    this->folder_system_model    = new QFileSystemModel();
    this->folder_system_model->setIconProvider(this->treeview_icon_provider);
    this->folder_browser         = new QTreeView();
    this->folder_browser->setModel(this->folder_system_model);
    this->folder_browser->setColumnHidden(1, true);
    this->folder_browser->setColumnHidden(2, true);
    this->folder_browser->setColumnHidden(3, true);
    this->folder_browser->setHeaderHidden(true);
    this->folder_browser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->folder_browser->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // Create the file browser (track browser).
    this->file_system_model = new Audio_collection_model();
    this->file_browser      = new QTreeView();
    this->file_browser->setModel(this->file_system_model);
    this->file_browser->setSelectionMode(QAbstractItemView::SingleSelection);
    this->file_browser->setDragEnabled(true);
    this->file_browser->setDragDropMode(QAbstractItemView::DragOnly);
    this->file_browser->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    this->file_browser->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    this->file_browser->header()->setSortIndicatorShown(true);
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    this->file_browser->header()->setClickable(true);
#else
    this->file_browser->header()->setSectionsClickable(true);
#endif

    // Create the track search bar.
    this->file_search                 = new QLineEdit();
    this->search_from_begin           = false;
    this->file_search->setPlaceholderText(tr("Search..."));
    this->file_browser_selected_index = 0;    

    // Create function buttons for file browser.
    this->refresh_file_browser = new QPushButton(tr("SCAN FILES"));
    this->refresh_file_browser->setObjectName("Right_vertic_button_scan_files");
    this->refresh_file_browser->setToolTip(tr("Analyze audio collection (get musical key)"));
    this->refresh_file_browser->setFocusPolicy(Qt::NoFocus);
    this->refresh_file_browser->setCheckable(true);

    // File browser buttons.
    QWidget *file_browser_buttons_widget = new QWidget();
    QHBoxLayout *file_browser_buttons_layout = new QHBoxLayout(file_browser_buttons_widget);
    for (unsigned short int i = 0; i < this->nb_decks; i++)
    {
        // Add a set of control button for one deck/samplers.
        FileBrowserControlButtons *control_buttons_layout = new FileBrowserControlButtons(i, this->nb_samplers);
        this->file_browser_control_buttons << control_buttons_layout;
        file_browser_buttons_layout->addLayout(control_buttons_layout);

        // Add a vertical separator.
        if (i < this->nb_decks-1)
        {
            QFrame* line = new QFrame();
            line->setFrameShape(QFrame::VLine);
            line->setObjectName("Separator_line");
            file_browser_buttons_layout->addWidget(line, 1);
        }
    }
    file_browser_buttons_widget->setFixedHeight(37);

    // Horizontal separator (between load buttons and file browser area).
    QFrame* horiz_line = new QFrame();
    horiz_line->setFrameShape(QFrame::HLine);
    horiz_line->setObjectName("Separator_line");

    // Build file browser and search area.
    QVBoxLayout *browser_search_layout = new QVBoxLayout();
    browser_search_layout->addWidget(this->file_browser);
    browser_search_layout->addWidget(this->file_search, 0, Qt::AlignBottom);
    browser_search_layout->setMargin(0);
    QWidget *file_browser_and_search_widget = new QWidget();
    file_browser_and_search_widget->setLayout(browser_search_layout);

    // Add a splitter beetween the folder browser and the file browser.
    this->browser_splitter = new QSplitter();
    this->browser_splitter->addWidget(this->folder_browser);
    this->browser_splitter->addWidget(file_browser_and_search_widget);
    this->browser_splitter->setStretchFactor(0, 1);
    this->browser_splitter->setStretchFactor(1, 4);

    // Vertical separator.
    QFrame* vertic_line = new QFrame();
    vertic_line->setFrameShape(QFrame::VLine);
    vertic_line->setObjectName("Separator_line");

    // Vertical set of buttons on the right: refresh file borwser (calculate music key).
    QVBoxLayout *action_buttons_layout = new QVBoxLayout();
    action_buttons_layout->addWidget(this->refresh_file_browser, 1, Qt::AlignTop);

    // Vertical set of buttons on the right: show/hide the sampler area.
    this->show_hide_samplers_button = new QPushButton(tr("SAMPLERS"));
    this->show_hide_samplers_button->setObjectName("Right_vertic_button_samplers");
    this->show_hide_samplers_button->setToolTip(tr("Show/hide samplers"));
    this->show_hide_samplers_button->setCheckable(true);
    this->show_hide_samplers_button->setChecked(true);
    action_buttons_layout->addWidget(this->show_hide_samplers_button, 100, Qt::AlignTop);

    // Layout of the bottom part of the file browser (folder browser, file browser, set of buttons).
    QWidget *bottom_container_widget = new QWidget();
    QHBoxLayout *bottom_container = new QHBoxLayout(bottom_container_widget);
    bottom_container->addWidget(this->browser_splitter, 100);
    bottom_container->addWidget(vertic_line, 1);
    bottom_container->addLayout(action_buttons_layout);

    // Main layout and group box.
    QVBoxLayout *file_browser_layout = new QVBoxLayout();
    file_browser_layout->addWidget(file_browser_buttons_widget, 1);
    file_browser_layout->addWidget(horiz_line, 1);
    file_browser_layout->addWidget(bottom_container_widget, 100);
    this->file_browser_gbox = new QGroupBox();
    this->file_browser_gbox->setLayout(file_browser_layout);
    this->set_file_browser_title(this->settings->get_tracks_base_dir_path());

    this->file_layout = new QHBoxLayout();
    this->file_layout->addWidget(this->file_browser_gbox, 50);
}

void
Gui::clean_file_browser_area()
{
    delete this->watcher_parse_directory;
    delete this->treeview_icon_provider;
    delete this->folder_system_model;
    delete this->file_system_model;
}

void
Gui::connect_file_browser_area()
{
    // Refresh track browser.
    QObject::connect(this->refresh_file_browser, &QPushButton::clicked, [this](){this->show_refresh_audio_collection_dialog();});

    for (unsigned short int i = 0; i < this->nb_decks; i++)
    {
        // Load track on deck.
        QObject::connect(this->file_browser_control_buttons[i]->load_track_on_deck_button, &QPushButton::clicked,
                         [this, i](){this->select_and_run_audio_file_decoding_process(i);});

        // Show next tracks (based on music key).
        QObject::connect(this->file_browser_control_buttons[i]->show_next_key_from_deck_button, &QPushButton::clicked,
                         [this, i](){this->select_and_show_next_keys(i);});

        // Load track in samplers.
        for (unsigned short int j = 0; j < this->nb_samplers; j++)
        {
            QObject::connect(this->file_browser_control_buttons[i]->load_sample_buttons[j], &QPushButton::clicked,
                             [this, i, j](){this->select_and_run_sample_decoding_process(i, j);});
        }
    }

    // Open folder or playlist from file browser on double click.
    QObject::connect(this->folder_browser, &QTreeView::doubleClicked, [this](const QModelIndex &index){this->on_file_browser_double_click(index);});

    // Resize column with file name when expanding/collapsing a directory.
    QObject::connect(this->file_browser, &QTreeView::expanded,  [this](const QModelIndex &index){this->on_file_browser_expand(index);});
    QObject::connect(this->file_browser, &QTreeView::collapsed, [this](const QModelIndex &index){this->on_file_browser_expand(index);});

    // Connect the keyboard shortcut that collapse tree.
    this->shortcut_collapse_browser = new QShortcut(this->file_browser);
    QObject::connect(this->shortcut_collapse_browser, &QShortcut::activated, [this](){this->file_browser->collapseAll();});

    // Connect the keyboard shortcut to start decoding process on selected file.
    this->shortcut_load_audio_file = new QShortcut(this->file_browser);
    QObject::connect(this->shortcut_load_audio_file, &QShortcut::activated, [this](){this->run_audio_file_decoding_process();});

    // Sort track browser when clicking on header.
    QObject::connect(this->file_browser->header(), &QHeaderView::sectionClicked, [this](int logicalIndex){this->on_file_browser_header_click(logicalIndex);});

    // Connect the keyboard shortcut to show next audio file according to current music key.
    this->shortcut_show_next_keys = new QShortcut(this->file_browser);
    QObject::connect(this->shortcut_show_next_keys, &QShortcut::activated, [this](){this->show_next_keys();});

    // Keyboard shortcut for loading track in sampler.
    for (unsigned short int j = 0; j < this->nb_samplers; j++)
    {
        QShortcut *shortcut_load_sample = new QShortcut(this->file_browser);
        this->shortcut_load_samples << shortcut_load_sample;
        QObject::connect(shortcut_load_sample, &QShortcut::activated, [this, j](){this->run_sampler_decoding_process(j);});
    }

    // Connect thread states for audio collection read and write to DB.
    QObject::connect(this->file_system_model->concurrent_watcher_read.data(), &QFutureWatcher<void>::finished, [this](){this->sync_file_browser_to_audio_collection();});
    QObject::connect(this->file_system_model->concurrent_watcher_store.data(), &QFutureWatcher<void>::finished, [this](){this->on_finished_analyze_audio_collection();});

    // Add context menu for file browser (load track).
    for (unsigned short int i = 0; i < this->nb_decks; i++)
    {
        QAction *load_action = new QAction(tr("Load on deck ") + QString::number(i+1), this);
        load_action->setStatusTip(tr("Load selected track to deck ") + QString::number(i+1));
        QObject::connect(load_action, &QAction::triggered, [this, i](){this->select_and_run_audio_file_decoding_process(i);});
        this->file_browser->addAction(load_action);
    }

    // Add context menu for file browser (load sample).
    this->file_browser->setContextMenuPolicy(Qt::ActionsContextMenu);
    for (unsigned short int i = 0; i < this->nb_decks; i++)
    {
        QMenu *load_on_sampler_submenu = new QMenu(this->file_browser);
        QString name("A");
        for (unsigned short int j = 0; j < this->nb_samplers; j++)
        {
            QAction *load_on_sampler_action = new QAction(tr("Sampler ") + name, this);
            load_on_sampler_action->setStatusTip(tr("Load selected track to sampler ")+ name);
            QObject::connect(load_on_sampler_action, &QAction::triggered, [this, i, j](){this->select_and_run_sample_decoding_process(i, j);});
            load_on_sampler_submenu->addAction(load_on_sampler_action);
            name[0].unicode()++; // Next sampler letter.
        }
        QAction *load_on_sampler_main_action = new QAction(tr("Load on sampler ") + QString::number(i+1), this);
        load_on_sampler_main_action->setMenu(load_on_sampler_submenu);
        load_on_sampler_main_action->setStatusTip(tr("Load selected track to sampler ") + QString::number(i+1));
        this->file_browser->addAction(load_on_sampler_main_action);
    }

    // Search bar for file browser.
    this->shortcut_file_search             = new QShortcut(this->window);
    this->shortcut_file_search_press_enter = new QShortcut(this->file_search);
    this->shortcut_file_search_press_esc   = new QShortcut(this->file_search);

    QObject::connect(this->shortcut_file_search, &QShortcut::activated, [this](){this->set_focus_search_bar();});
    QObject::connect(this->file_search, &QLineEdit::textChanged, [this](const QString &text){this->file_search_string(text);});
    QObject::connect(this->shortcut_file_search_press_enter, &QShortcut::activated, [this](){this->press_enter_in_search_bar();});
    QObject::connect(this->file_search, &QLineEdit::returnPressed, [this](){this->press_enter_in_search_bar();});
    QObject::connect(this->shortcut_file_search_press_esc, &QShortcut::activated, [this](){this->press_esc_in_search_bar();});

    // Progress for file analyzis and storage.
    QObject::connect(this->file_system_model->concurrent_watcher_store.data(), &QFutureWatcher<void>::progressRangeChanged,
                     [this](int minimum, int maximum){this->progress_bar->setRange(minimum, maximum);});
    QObject::connect(this->file_system_model->concurrent_watcher_store.data(), &QFutureWatcher<void>::progressValueChanged,
                     [this](int progressValue){this->update_refresh_progress_value(progressValue);});

    // Progress for reading file data from storage.
    QObject::connect(this->file_system_model->concurrent_watcher_read.data(), &QFutureWatcher<void>::progressRangeChanged,
                     [this](int minimum, int maximum){this->progress_bar->setRange(minimum, maximum);});
    QObject::connect(this->file_system_model->concurrent_watcher_read.data(), &QFutureWatcher<void>::progressValueChanged,
                     [this](int progressValue){this->update_refresh_progress_value(progressValue);});

    // Parse directory thread.
    this->watcher_parse_directory = new QFutureWatcher<void>;
    QObject::connect(this->watcher_parse_directory, &QFutureWatcher<void>::finished, [this](){this->run_concurrent_read_collection_from_db();});
}

void
Gui::init_bottom_help()
{
    // Create help labels and pixmaps.
    QLabel *help_display_lb        = new QLabel(tr("Display"));
    QLabel *help_fullscreen_lb     = new QLabel(tr("Fullscreen"));
    this->help_fullscreen_value    = new QLabel();
    QLabel *help_help_lb           = new QLabel(tr("Help"));
    this->help_help_value          = new QLabel();
    QLabel *help_switch_deck_lb    = new QLabel(tr("Switch selected playback"));
    this->help_switch_deck_value   = new QLabel();

    QLabel *help_deck_lb           = new QLabel(tr("Selected deck"));
    QLabel *help_load_deck_lb      = new QLabel(tr("Load/Restart track"));
    this->help_load_deck_value     = new QLabel();
    QLabel *help_next_track_lb     = new QLabel(tr("Highlight next tracks"));
    this->help_next_track_value    = new QLabel();
    QLabel *help_cue_lb            = new QLabel(tr("Set/Play cue point 1/2/3/4"));
    this->help_cue_value           = new QLabel();

    QLabel *help_sampler_lb        = new QLabel(tr("Selected sampler"));
    QLabel *help_sample_lb         = new QLabel(tr("Load sampler 1/2/3/4"));
    this->help_sample_value        = new QLabel();
    QLabel *help_online_lb         = new QLabel(tr("Online wiki help"));
    QLabel *help_url_lb            = new QLabel("<a style=\"color: white\" href=\"https://github.com/jrosener/digitalscratch/wiki\">https://github.com/jrosener/digitalscratch/wiki</a>");
    help_url_lb->setTextFormat(Qt::RichText);
    help_url_lb->setTextInteractionFlags(Qt::TextBrowserInteraction);
    help_url_lb->setOpenExternalLinks(true);

    QLabel *help_browser_lb        = new QLabel(tr("File browser"));
    QLabel *help_browse_lb1        = new QLabel(tr("Browse"));
    this->help_browse_value1       = new QLabel();
    QLabel *help_browse_lb2        = new QLabel(tr("Collapse all"));
    this->help_browse_value2       = new QLabel();

    this->set_help_shortcut_value();

    help_display_lb->setObjectName("Help_title");
    help_fullscreen_lb->setObjectName("Help");
    help_help_lb->setObjectName("Help");
    help_switch_deck_lb->setObjectName("Help");

    help_deck_lb->setObjectName("Help_title");
    help_load_deck_lb->setObjectName("Help");
    help_next_track_lb->setObjectName("Help");
    help_cue_lb->setObjectName("Help");

    help_sampler_lb->setObjectName("Help_title");
    help_sample_lb->setObjectName("Help");
    help_online_lb->setObjectName("Help_title");
    help_url_lb->setObjectName("Help_url");

    help_browser_lb->setObjectName("Help_title");
    help_browse_lb1->setObjectName("Help");
    help_browse_lb2->setObjectName("Help");

    // Main help layout.
    QGridLayout *help_layout = new QGridLayout();

    help_layout->addWidget(help_display_lb,           0, 0);
    help_layout->addWidget(help_fullscreen_lb,        1, 0);
    help_layout->addWidget(help_fullscreen_value,     1, 1, Qt::AlignLeft);
    help_layout->addWidget(help_help_lb,              2, 0);
    help_layout->addWidget(help_help_value,           2, 1, Qt::AlignLeft);
    help_layout->addWidget(help_switch_deck_lb,       3, 0);
    help_layout->addWidget(help_switch_deck_value,    3, 1, Qt::AlignLeft);

    help_layout->addWidget(help_deck_lb,              0, 2);
    help_layout->addWidget(help_load_deck_lb,         1, 2);
    help_layout->addWidget(help_load_deck_value,      1, 3, Qt::AlignLeft);
    help_layout->addWidget(help_next_track_lb,        2, 2);
    help_layout->addWidget(help_next_track_value,     2, 3, Qt::AlignLeft);
    help_layout->addWidget(help_cue_lb,               3, 2);
    help_layout->addWidget(help_cue_value,            3, 3, Qt::AlignLeft);

    help_layout->addWidget(help_sampler_lb,           0, 4);
    help_layout->addWidget(help_sample_lb,            1, 4);
    help_layout->addWidget(help_sample_value,         1, 5, Qt::AlignLeft);
    help_layout->addWidget(help_online_lb,            3, 4);
    help_layout->addWidget(help_url_lb,               3, 5, 1, 3, Qt::AlignLeft);

    help_layout->addWidget(help_browser_lb,           0, 6);
    help_layout->addWidget(help_browse_lb1,           1, 6);
    help_layout->addWidget(help_browse_value1,        1, 7, Qt::AlignLeft);
    help_layout->addWidget(help_browse_lb2,           2, 6);
    help_layout->addWidget(help_browse_value2,        2, 7, Qt::AlignLeft);

    help_layout->setColumnStretch(0, 1);
    help_layout->setColumnStretch(1, 5);
    help_layout->setColumnStretch(2, 1);
    help_layout->setColumnStretch(3, 5);
    help_layout->setColumnStretch(4, 1);
    help_layout->setColumnStretch(5, 5);
    help_layout->setColumnStretch(6, 1);
    help_layout->setColumnStretch(7, 5);

    // Create help group box.
    this->help_groupbox = new QGroupBox();
    this->help_groupbox->hide();
    this->help_groupbox->setObjectName("Help");

    // Put help horizontal layout in help group box.
    this->help_groupbox->setLayout(help_layout);

    // Create bottom horizontal layout.
    this->bottom_layout = new QHBoxLayout;

    // Put help group box and configuration in bottom layout.
    this->bottom_layout->addWidget(this->help_groupbox);
}

void
Gui::init_bottom_status()
{
    // Create groupbox for progress bar.
    this->progress_groupbox = new QGroupBox();
    this->progress_groupbox->hide();
    this->progress_groupbox->setObjectName("Progress");

    // Create progress bar.
    this->progress_bar = new QProgressBar(this->progress_groupbox);
    this->progress_bar->setObjectName("Progress");

    // Create cancel button.
    this->progress_cancel_button = new QPushButton(this->progress_groupbox);
    this->progress_cancel_button->setObjectName("Progress");
    this->progress_cancel_button->setFixedSize(16, 16);
    this->progress_cancel_button->setFocusPolicy(Qt::NoFocus);
    this->progress_cancel_button->setToolTip(tr("Cancel execution"));
    QObject::connect(this->progress_cancel_button, &QPushButton::clicked, [this](){this->on_progress_cancel_button_click();});

    // Create label.
    this->progress_label = new QLabel(this->progress_groupbox);
    this->progress_label->setObjectName("Progress");

    // Create layout for progress bar.
    QHBoxLayout *progress_layout = new QHBoxLayout;
    progress_layout->addWidget(this->progress_bar);
    progress_layout->addWidget(this->progress_cancel_button);
    progress_layout->addWidget(this->progress_label);
    this->progress_groupbox->setLayout(progress_layout);
    this->status_layout = new QHBoxLayout;
    this->status_layout->addWidget(this->progress_groupbox);
}

void
Gui::display_audio_file_collection()
{
    QCoreApplication::processEvents();
    this->set_file_browser_base_path(this->settings->get_tracks_base_dir_path());
    this->is_window_rendered = true;
}

void
Gui::set_help_shortcut_value()
{
    this->help_fullscreen_value->setText(this->settings->get_keyboard_shortcut(KB_FULLSCREEN));
    this->help_help_value->setText(this->settings->get_keyboard_shortcut(KB_HELP));
    this->help_switch_deck_value->setText(this->settings->get_keyboard_shortcut(KB_SWITCH_PLAYBACK));
    this->help_load_deck_value->setText(this->settings->get_keyboard_shortcut(KB_LOAD_TRACK_ON_DECK)
                                        + "/"
                                        + this->settings->get_keyboard_shortcut(KB_PLAY_BEGIN_TRACK_ON_DECK));
    this->help_next_track_value->setText(this->settings->get_keyboard_shortcut(KB_SHOW_NEXT_KEYS));
    this->help_cue_value->setText(this->settings->get_keyboard_shortcut(KB_SET_CUE_POINT1_ON_DECK)
                                  + "/"
                                  + this->settings->get_keyboard_shortcut(KB_PLAY_CUE_POINT1_ON_DECK)
                                  + ", " + this->settings->get_keyboard_shortcut(KB_SET_CUE_POINT2_ON_DECK)
                                  + "/"
                                  + this->settings->get_keyboard_shortcut(KB_PLAY_CUE_POINT2_ON_DECK)
                                  + ", " + this->settings->get_keyboard_shortcut(KB_SET_CUE_POINT3_ON_DECK)
                                  + "/"
                                  + this->settings->get_keyboard_shortcut(KB_PLAY_CUE_POINT3_ON_DECK)
                                  + ", " + this->settings->get_keyboard_shortcut(KB_SET_CUE_POINT4_ON_DECK)
                                  + "/"
                                  + this->settings->get_keyboard_shortcut(KB_PLAY_CUE_POINT4_ON_DECK));
    this->help_sample_value->setText(this->settings->get_keyboard_shortcut(KB_LOAD_TRACK_ON_SAMPLER1)
                                     + "/"
                                     + this->settings->get_keyboard_shortcut(KB_LOAD_TRACK_ON_SAMPLER2)
                                     + "/"
                                     + this->settings->get_keyboard_shortcut(KB_LOAD_TRACK_ON_SAMPLER3)
                                     + "/"
                                     + this->settings->get_keyboard_shortcut(KB_LOAD_TRACK_ON_SAMPLER4));
    this->help_browse_value1->setText("Up/Down/Left/Right");
    this->help_browse_value2->setText(this->settings->get_keyboard_shortcut(KB_COLLAPSE_BROWSER));
}

void
Gui::can_close()
{
    // Show a pop-up asking to confirm to close.
    QMessageBox msg_box;
    msg_box.setWindowTitle("DigitalScratch");
    msg_box.setText(tr("Do you really want to quit DigitalScratch ?"));
    msg_box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msg_box.setIcon(QMessageBox::Question);
    msg_box.setDefaultButton(QMessageBox::Cancel);
    msg_box.setStyleSheet(Utils::get_current_stylesheet_css());
    if (this->nb_decks > 1)
    {
        msg_box.setWindowIcon(QIcon(ICON_2));
    }
    else
    {
        msg_box.setWindowIcon(QIcon(ICON));
    }

    // Close request confirmed.
    if (msg_box.exec() == QMessageBox::Ok)
    {
        //this->sound_card->stop();
        this->window->close();
    }
}

void
Gui::force_close()
{
    // User probably clicked on the 'X' button, so close without confirmation.
    this->sound_card->stop();
    //this->window->close(); // Looks not necessary.
}


bool
Gui::apply_main_window_style()
{
    // Apply some GUI settings manually.
    if (this->window_style == QString(GUI_STYLE_NATIVE))
    {
        // Set manually some standard icons.
        for (unsigned short int i = 0; i < this->nb_decks; i++)
        {
            this->decks[i]->restart_button->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaSkipBackward));
            for (unsigned short int j = 0; j < MAX_NB_CUE_POINTS; j++)
            {
                this->decks[i]->cue_set_buttons[j]->setIcon(QIcon());
                this->decks[i]->cue_set_buttons[j]->setText("o");
                this->decks[i]->cue_play_buttons[j]->setIcon(QIcon());
                this->decks[i]->cue_play_buttons[j]->setText(">");
                this->decks[i]->cue_del_buttons[j]->setIcon(QIcon());
                this->decks[i]->cue_del_buttons[j]->setText("x");
            }
            this->file_browser_control_buttons[i]->load_track_on_deck_button->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowUp));
            this->file_browser_control_buttons[i]->show_next_key_from_deck_button->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowDown));
            for (unsigned short int j = 0; j < this->nb_samplers; j++)
            {
                this->samplers[i]->buttons_play[j]->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaPlay));
                this->samplers[i]->buttons_stop[j]->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaStop));
                this->samplers[i]->buttons_del[j]->setIcon(QApplication::style()->standardIcon(QStyle::SP_TrashIcon));
                this->file_browser_control_buttons[i]->load_sample_buttons[j]->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowUp));
            }

        }
        this->refresh_file_browser->setIcon(QApplication::style()->standardIcon(QStyle::SP_BrowserReload));        
        this->progress_cancel_button->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaStop));

        QFileIconProvider *icon_prov = this->folder_system_model->iconProvider();
        if (icon_prov != nullptr)
        {
            ((TreeViewIconProvider*)icon_prov)->set_default_icons();
        }
        this->file_system_model->set_icons((QApplication::style()->standardIcon(QStyle::SP_FileIcon).pixmap(10, 10)),
                                           (QApplication::style()->standardIcon(QStyle::SP_DirIcon).pixmap(10, 10)));
    }
    else
    {
        // Reset some icons (can not be done nicely in CSS).
        for (unsigned short int i = 0; i < this->nb_decks; i++)
        {
            this->decks[i]->restart_button->setIcon(QIcon());
            for (unsigned short int j = 0; j < MAX_NB_CUE_POINTS; j++)
            {
                this->decks[i]->cue_set_buttons[j]->setIcon(QIcon());
                this->decks[i]->cue_set_buttons[j]->setText("");
                this->decks[i]->cue_play_buttons[j]->setIcon(QIcon());
                this->decks[i]->cue_play_buttons[j]->setText("");
                this->decks[i]->cue_del_buttons[j]->setIcon(QIcon());
                this->decks[i]->cue_del_buttons[j]->setText("");
            }
            this->file_browser_control_buttons[i]->load_track_on_deck_button->setIcon(QIcon());
            this->file_browser_control_buttons[i]->show_next_key_from_deck_button->setIcon(QIcon());
            for (int j = 0; j < this->nb_samplers; j++)
            {
                this->samplers[i]->buttons_play[j]->setIcon(QIcon());
                this->samplers[i]->buttons_stop[j]->setIcon(QIcon());
                this->samplers[i]->buttons_del[j]->setIcon(QIcon());
                this->file_browser_control_buttons[i]->load_sample_buttons[j]->setIcon(QIcon());
            }
        }
        this->progress_cancel_button->setIcon(QIcon());

        // Set icon for file browser QTreeview (can not be done nicely in CSS).
        QFileIconProvider *icon_prov = this->folder_system_model->iconProvider();
        if (icon_prov != nullptr)
        {
            ((TreeViewIconProvider*)icon_prov)->set_icons(QIcon(QPixmap(PIXMAPS_PATH + this->window_style + ICON_DRIVE_SUFFIX).scaledToWidth(10,      Qt::SmoothTransformation)),
                                                          QIcon(QPixmap(PIXMAPS_PATH + this->window_style + ICON_FOLDER_SUFFIX).scaledToWidth(10,     Qt::SmoothTransformation)),
                                                          QIcon(QPixmap(PIXMAPS_PATH + this->window_style + ICON_AUDIO_FILE_SUFFIX).scaledToWidth(10, Qt::SmoothTransformation)),
                                                          QIcon(QPixmap(PIXMAPS_PATH + this->window_style + ICON_FILE_SUFFIX).scaledToWidth(10,       Qt::SmoothTransformation)));
        }
        this->file_system_model->set_icons(QPixmap(PIXMAPS_PATH + this->window_style + ICON_AUDIO_FILE_SUFFIX).scaledToWidth(10, Qt::SmoothTransformation),
                                           QPixmap(PIXMAPS_PATH + this->window_style + ICON_FOLDER_SUFFIX).scaledToWidth(10, Qt::SmoothTransformation));
    }

    // Change main window skin (using CSS).
    this->window->setStyleSheet(Utils::get_current_stylesheet_css());

    return true;
}

bool
Gui::set_folder_browser_base_path(const QString &path)
{
    // Change path of the model and set file extension filters.
    this->folder_system_model->setRootPath("");
    QStringList name_filter;
    name_filter << "*.m3u" << "*.pls" << "*.xspf";
    this->folder_system_model->setNameFilters(name_filter);
    this->folder_system_model->setNameFilterDisables(false);

    // Change root path of file browser.
    this->folder_browser->setRootIndex(this->folder_system_model->index(""));
    this->folder_browser->update();

    // Preselect the base path.
    this->folder_browser->collapseAll();
    this->folder_browser->setCurrentIndex(this->folder_system_model->index(path));
    this->folder_browser->setExpanded(this->folder_system_model->index(path), true);

    return true;
}

bool
Gui::set_file_browser_base_path(const QString &path)
{
    if (this->watcher_parse_directory->isRunning() == false)
    {
        // Hide file browser during directory analysis.
        this->file_browser->setVisible(false);

        // Stop any running file analysis.
        this->file_system_model->stop_concurrent_read_collection_from_db();
        this->file_system_model->stop_concurrent_analyse_audio_collection();

        // Show progress bar.
        this->progress_label->setText(tr("Opening ") + path + "...");
        this->progress_groupbox->show();
        this->progress_bar->setMinimum(0);
        this->progress_bar->setMaximum(0);

        // Set base path as title to file browser.
        this->set_file_browser_title(path);

        // Clear file browser.
        this->file_system_model->clear();

        // Change root path of file browser (do it in a non blocking external thread).
        QFuture<QModelIndex> future = QtConcurrent::run(this->file_system_model, &Audio_collection_model::set_root_path, path);
        this->watcher_parse_directory->setFuture(future);

        // Reset progress.
        this->progress_bar->reset();
    }

    return true;
}

void
Gui::run_concurrent_read_collection_from_db()
{
    // Show file browser again (file were analyse on disk).
    this->file_browser->setVisible(true);

    // Get file info from DB.
    this->file_system_model->concurrent_read_collection_from_db(); // Run in another thread.
                                                                   // Call sync_file_browser_to_audio_collection() when it's done.

    return;
}

bool
Gui::set_file_browser_playlist_tracks(const Playlist &playlist)
{
    if (this->watcher_parse_directory->isRunning() == false)
    {
        // Hide file browser during playlist analysis.
        this->file_browser->setVisible(false);

        // Stop any running file analysis.
        this->file_system_model->stop_concurrent_read_collection_from_db();
        this->file_system_model->stop_concurrent_analyse_audio_collection();

        // Show progress bar.
        this->progress_label->setText(tr("Opening ") + playlist.get_name() + "...");
        this->progress_groupbox->show();
        this->progress_bar->setMinimum(0);
        this->progress_bar->setMaximum(0);

        // Set base path as title to file browser.
        this->set_file_browser_title(playlist.get_name());

        // Clear file browser.
        this->file_system_model->clear();

        // Set list of tracks to the file browser.
        this->file_browser->setRootIndex(QModelIndex());
        this->file_system_model->set_playlist(playlist);

        // Reset progress.
        this->progress_bar->reset();

        // Get file info from DB.
        this->file_system_model->concurrent_read_collection_from_db(); // Run in another thread.
                                                                       // Call sync_file_browser_to_audio_collection() when it's done.

        // Show file browser again.
        this->file_browser->setVisible(true);
    }

    return true;
}

void
Gui::sync_file_browser_to_audio_collection()
{
    // Reset file browser root node to audio collection model's root.
    this->file_browser->update();
    this->file_browser->setRootIndex(this->file_system_model->get_root_index());
    this->resize_file_browser_columns();

    // Hide progress bar.
    this->progress_label->setText("");
    this->progress_groupbox->hide();
}

bool
Gui::set_file_browser_title(const QString &title)
{
    // Change file browser title (which contains base dir for tracks).
    this->file_browser_gbox->setTitle(tr("File browser") + " [" + title + "]");

    return true;
}

void
Gui::run_sampler_decoding_process(const unsigned short int &sampler_index)
{
    this->run_sampler_decoding_process_on_deck(this->get_selected_deck_index(), sampler_index);

    return;
}

void
Gui::run_sampler_decoding_process(const unsigned short &deck_index, const unsigned short &sampler_index)
{
    this->select_playback(deck_index);
    this->run_sampler_decoding_process(sampler_index);

    return;
}

void
Gui::run_sampler_decoding_process_on_deck(const unsigned short &deck_index,
                                          const unsigned short &sampler_index)
{
    // Select deck.
    this->select_playback(deck_index);

    // Get selected file path.
    Audio_collection_item *item = static_cast<Audio_collection_item*>((this->file_browser->currentIndex()).internalPointer());
    QFileInfo info(item->get_full_path());
    if (info.isFile() == true)
    {
        // Execute decoding.
        QList<QSharedPointer<Audio_file_decoding_process>> samplers;
        samplers = this->dec_samplers[deck_index];
        if (samplers[sampler_index]->run(info.absoluteFilePath(), "", "") == false)
        {
            qCWarning(DS_FILE) << "can not decode " << info.absoluteFilePath();
        }
        else
        {
            this->playbacks[deck_index]->reset_sampler(sampler_index);
            this->set_sampler_state(deck_index, sampler_index, false);
            this->playbacks[deck_index]->set_sampler_state(sampler_index, false);
        }
    }

    return;
}

void
Gui::select_and_run_sample_decoding_process(const unsigned short &deck_index,
                                            const unsigned short &sampler_index)
{
    // Check the button.
    this->file_browser_control_buttons[deck_index]->load_sample_buttons[sampler_index]->setEnabled(false);
    this->file_browser_control_buttons[deck_index]->load_sample_buttons[sampler_index]->setChecked(true);

    // Select deck and decode + play sample audio file.
    this->run_sampler_decoding_process_on_deck(deck_index, sampler_index);

    // Release the button.
    this->file_browser_control_buttons[deck_index]->load_sample_buttons[sampler_index]->setEnabled(true);
    this->file_browser_control_buttons[deck_index]->load_sample_buttons[sampler_index]->setChecked(false);
}

void
Gui::set_sampler_text(const QString &text, const unsigned short int &deck_index, const unsigned short int &sampler_index)
{
    this->samplers[deck_index]->tracknames[sampler_index]->setText(text);
    return;
}

void
Gui::sampler_button_play_clicked(const unsigned short &deck_index,
                                 const unsigned short &sampler_index)
{
    // First stop playback (and return to beginning of the song).
    this->set_sampler_state(deck_index, sampler_index, false);
    this->playbacks[deck_index]->set_sampler_state(sampler_index, false);

    // Then start playback again.
    this->set_sampler_state(deck_index, sampler_index, true);
    this->playbacks[deck_index]->set_sampler_state(sampler_index, true);

    // Select playback area (if not already done).
    this->select_playback(deck_index);

    return;
}

void
Gui::sampler_button_stop_clicked(const unsigned short &deck_index,
                                 const unsigned short &sampler_index)
{
    // Stop playback (and return to beginning of the song).
    this->set_sampler_state(deck_index, sampler_index, false);
    this->playbacks[deck_index]->set_sampler_state(sampler_index, false);

    // Select playback area (if not already done).
    this->select_playback(deck_index);

    return;
}

void
Gui::sampler_button_del_clicked(const unsigned short &deck_index,
                                const unsigned short &sampler_index)
{
    // Remove track loaded in the sampler.
    this->playbacks[deck_index]->del_sampler(sampler_index);
    this->set_sampler_state(deck_index, sampler_index, false);
    this->set_sampler_text(tr("--"), deck_index, sampler_index);

    // Select playback area (if not already done).
    this->select_playback(deck_index);

    return;
}

void
Gui::on_progress_cancel_button_click()
{
    // Stop any running file analysis.
    this->file_system_model->stop_concurrent_read_collection_from_db();
    this->file_system_model->stop_concurrent_analyse_audio_collection();
}

void
Gui::on_file_browser_expand(QModelIndex)
{
    this->resize_file_browser_columns();
}

void
Gui::on_file_browser_header_click(const int &index)
{    
    // Get the order.
    Qt::SortOrder order = this->file_browser->header()->sortIndicatorOrder();

    // Sort the items.
    this->file_browser->sortByColumn(index, order);
}

void
Gui::on_file_browser_double_click(QModelIndex in_model_index)
{
    if (this->watcher_parse_directory->isRunning() == false)
    {
        // Get path (file for a playlist, or just a directory).
        QString path = this->folder_system_model->filePath(in_model_index);

        QFileInfo file_info(path);
        if (file_info.isFile() == true)
        {
            Playlist playlist(file_info.absolutePath(), file_info.baseName());
            Playlist_persistence playlist_persist;

            // It is a m3u playlist, parse it and show track list in file browser.
            if (file_info.suffix().compare(QString("m3u"), Qt::CaseInsensitive) == 0)
            {
                // Open M3U playlist
                if (playlist_persist.read_m3u(path, playlist) == true)
                {
                    // Populate file browser.
                    this->set_file_browser_playlist_tracks(playlist);
                }
                else
                {
                    qCWarning(DS_FILE) << "can not open m3u playlist " << qPrintable(path);
                }
            }
            else if (file_info.suffix().compare(QString("pls"), Qt::CaseInsensitive) == 0)
            {
                // Open PLS playlist
                if (playlist_persist.read_pls(path, playlist) == true)
                {
                    // Populate file browser.
                    this->set_file_browser_playlist_tracks(playlist);
                }
                else
                {
                    qCWarning(DS_FILE) << "can not open pls playlist " << qPrintable(path);
                }
            }
        }
        else
        {
            // It is a directory, open selected directory in file browser.
            this->set_file_browser_base_path(path);
        }
    }
}

void
Gui::resize_file_browser_columns()
{
    this->file_browser->resizeColumnToContents(COLUMN_KEY);
    this->file_browser->resizeColumnToContents(COLUMN_FILE_NAME);
}

void
Gui::select_and_run_audio_file_decoding_process(const unsigned short int &deck_index)
{
    // Check the button.
    this->file_browser_control_buttons[deck_index]->load_track_on_deck_button->setEnabled(false);
    this->file_browser_control_buttons[deck_index]->load_track_on_deck_button->setChecked(true);

    // Select deck.
    this->select_playback(deck_index);

    // Decode and play selected audio file.
    this->run_audio_file_decoding_process();

    // Release the button.
    this->file_browser_control_buttons[deck_index]->load_track_on_deck_button->setEnabled(true);
    this->file_browser_control_buttons[deck_index]->load_track_on_deck_button->setChecked(false);
}

void
Gui::run_audio_file_decoding_process()
{
    // Force processing events to refresh main window before running decoding.
    QApplication::processEvents();

    // Get selected file path.
    Audio_collection_item *item = static_cast<Audio_collection_item*>((this->file_browser->currentIndex()).internalPointer());
    QFileInfo info(item->get_full_path());
    if (info.isFile() == true)
    {
        // Get selected deck/sampler.
        unsigned short int deck_index = this->get_selected_deck_index();
        QLabel    *deck_track_name = this->decks[deck_index]->track_name;
        QLabel   **deck_cue_point  = this->decks[deck_index]->cue_point_labels;
        Waveform  *deck_waveform   = this->decks[deck_index]->waveform;
        QSharedPointer<Audio_file_decoding_process> decode_process = this->decs[deck_index];

        // Execute decoding if not trying to open the existing track.
        if (info.fileName().compare(deck_track_name->text()) != 0 )
        {
            // Stop playback.
            this->playbacks[deck_index]->stop();

            // Clear audio track and waveform.
            decode_process->clear();
            deck_waveform->reset();
            deck_waveform->update();

            // Decode track.
            if (decode_process->run(info.absoluteFilePath(), item->get_file_hash(), item->get_data(COLUMN_KEY).toString()) == false)
            {
                qCWarning(DS_FILE) << "can not decode " << info.absoluteFilePath();
            }
        }

        // Force waveform computation.
        deck_waveform->reset();

        // Reset playback process.
        this->playbacks[deck_index]->reset();
        deck_waveform->move_slider(0.0);

        // Reset cue point.
        for (unsigned short int i = 0; i < MAX_NB_CUE_POINTS; i++)
        {
            deck_waveform->move_cue_slider(i, this->playbacks[deck_index]->get_cue_point(i));
            deck_cue_point[i]->setText(this->playbacks[deck_index]->get_cue_point_str(i));
        }

        // Update waveform.
        deck_waveform->update();
    }

    return;
}

void
Gui::set_remaining_time(const unsigned int &remaining_time, const unsigned short &deck_index)
{
    // Split remaining time (which is in msec) into minutes, seconds and milliseconds. 
    int remaining_time_by_1000 = remaining_time / 1000.0;
    div_t tmp_division;
    tmp_division = div(remaining_time_by_1000, 60);
    QString min  = QString::number(tmp_division.quot);
    QString sec  = QString::number(tmp_division.rem);
    QString msec = QString::number(remaining_time).right(2);

    // Add "0" in front of remaining time.
    if (min.size() == 1)
    {
        min = "0" + min;
    }
    if (sec.size() == 1)
    {
        sec = "0" + sec;
    }

    // Change displayed remaining time.
    if (min.compare(this->decks[deck_index]->rem_time_min->text()) != 0)
    {
        this->decks[deck_index]->rem_time_min->setText(min);
    }
    if (sec.compare(this->decks[deck_index]->rem_time_sec->text()) != 0)
    {
        this->decks[deck_index]->rem_time_sec->setText(sec);
    }
    this->decks[deck_index]->rem_time_msec->setText(msec);

    // Move slider on waveform when remaining time changed.
    this->decks[deck_index]->waveform->move_slider(this->playbacks[deck_index]->get_position());

    return;
}

void
Gui::set_sampler_remaining_time(const unsigned int   &remaining_time,
                                const unsigned short &deck_index,
                                const unsigned short &sampler_index)
{
    if (remaining_time == 0)
    {
        this->samplers[deck_index]->remaining_times[sampler_index]->setText("- 00");
    }
    else
    {
        // Split remaining time (which is in msec) into minutes, seconds and milliseconds.
        int remaining_time_by_1000 = remaining_time / 1000.0;
        div_t tmp_division;
        tmp_division = div(remaining_time_by_1000, 60);
        QString sec  = QString::number(tmp_division.rem);

        // Change displayed remaining time (if different than previous one).
        if (sec.compare(this->samplers[deck_index]->remaining_times[sampler_index]->text()) != 0)
        {
            if (sec.size() == 1)
            {
                this->samplers[deck_index]->remaining_times[sampler_index]->setText("- 0" + sec);
            }
            else
            {
                this->samplers[deck_index]->remaining_times[sampler_index]->setText("- " + sec);
            }
        }
    }

    return;
}

void
Gui::set_sampler_state(const unsigned short &deck_index,
                       const unsigned short &sampler_index,
                       const bool           &state)
{
    // Change state only if a sample is loaded and playing.
    if ((this->playbacks[deck_index]->is_sampler_loaded(sampler_index) == true) && (state == true))
    {
        this->samplers[deck_index]->buttons_play[sampler_index]->setChecked(true);
        this->samplers[deck_index]->buttons_stop[sampler_index]->setChecked(false);
    }
    else // Sampler is stopping or is not loaded, make play button inactive.
    {
        this->samplers[deck_index]->buttons_play[sampler_index]->setChecked(false);
        this->samplers[deck_index]->buttons_stop[sampler_index]->setChecked(true);
    }

    return;
}

void
Gui::update_speed_label(const float &speed, const unsigned short &deck_index)
{
    double percent = (double)(floorf((speed * 100.0) * 10.0) / 10.0);
    QString sp = QString("%1%2").arg(percent < 0 ? '-' : '+').arg(qAbs(percent), 5, 'f', 1, '0') + '%';

    this->decks[deck_index]->speed->setText(sp);
}

void
Gui::speed_reset_to_100p(const unsigned short &deck_index)
{
    // Select deck.
    this->select_playback(deck_index);

    // Increment speed.
    this->manual_controls[deck_index]->reset_speed_to_100p();
}

void
Gui::speed_up_down(const float &speed_inc, const unsigned short &deck_index)
{
    // Select deck.
    this->select_playback(deck_index);

    // Increment speed.
    this->manual_controls[deck_index]->inc_speed(speed_inc);
}

void
Gui::speed_accel(const float &speed_inc, const unsigned short &deck_index)
{
    // Select deck.
    this->select_playback(deck_index);

    // Increment speed.
    this->manual_controls[deck_index]->inc_temporary_speed(speed_inc, 100); // Maintain a little bit more speed during 100 cycles.
}

void
Gui::jump_to_position(const float &position, const unsigned short &deck_index)
{
    this->playbacks[deck_index]->jump_to_position(position);
    this->select_playback(deck_index);

    return;
}

void
Gui::deck_go_to_begin()
{
    this->playbacks[this->get_selected_deck_index()]->jump_to_position(0.0);

    return;
}

void
Gui::go_to_begin(const unsigned short &deck_index)
{
    // Select deck.
    this->select_playback(deck_index);

    // Jump to the beginning of the track.
    this->deck_go_to_begin();

    return;
}

void
Gui::deck_set_cue_point(const unsigned short &cue_point_number)
{
    unsigned short int deck_index = this->get_selected_deck_index();

    this->decks[deck_index]->waveform->move_cue_slider(cue_point_number, this->playbacks[deck_index]->get_position());
    this->playbacks[deck_index]->store_cue_point(cue_point_number);
    this->decks[deck_index]->cue_point_labels[cue_point_number]->setText(this->playbacks[deck_index]->get_cue_point_str(cue_point_number));

    return;
}

void
Gui::set_cue_point(const unsigned short &deck_index, const unsigned short &cue_point_number)
{
    // Select deck.
    this->select_playback(deck_index);

    // Set cue point.
    this->deck_set_cue_point(cue_point_number);

    return;
}

void
Gui::deck_go_to_cue_point(const unsigned short &cue_point_number)
{
    this->playbacks[this->get_selected_deck_index()]->jump_to_cue_point(cue_point_number);

    return;
}

void
Gui::go_to_cue_point(const unsigned short &deck_index, const unsigned short &cue_point_number)
{
    // Select deck.
    this->select_playback(deck_index);

    // Jump.
    this->deck_go_to_cue_point(cue_point_number);

    return;
}

void
Gui::deck_del_cue_point(const unsigned short &cue_point_number)
{
    unsigned short int deck_index = this->get_selected_deck_index();

    this->playbacks[deck_index]->delete_cue_point(cue_point_number);
    this->decks[deck_index]->waveform->move_cue_slider(cue_point_number, 0.0);
    this->decks[deck_index]->cue_point_labels[cue_point_number]->setText(this->playbacks[deck_index]->get_cue_point_str(cue_point_number));

    return;
}

void
Gui::del_cue_point(const unsigned short &deck_index, const unsigned short &cue_point_number)
{
    // Select deck.
    this->select_playback(deck_index);

    // Delete point.
    this->deck_del_cue_point(cue_point_number);

    return;
}

void
Gui::switch_playback_selection()
{
    // Highlight the next one (circular selection).
    if (this->selected_deck + 1 == this->nb_decks)
    {
        this->select_playback(0);
    }
    else
    {
        this->select_playback(this->selected_deck + 1);
    }

    return;
}

void
Gui::select_playback(const unsigned short &deck_index)
{
    if (deck_index != this->selected_deck)
    {
        this->selected_deck = deck_index;
        this->highlight_deck_sampler_area(deck_index);
    }

    return;
}

void
Gui::highlight_deck_sampler_area(const unsigned short int &deck_index)
{
    // Select one pair deck+sampler, deselect the other.
    for (unsigned short int i = 0; i < this->nb_decks; i++)
    {
        if (i == deck_index)
        {
            this->decks[i]->setProperty("selected", true);
            this->samplers[i]->setProperty("selected", true);
        }
        else
        {
            this->decks[i]->setProperty("selected", false);
            this->samplers[i]->setProperty("selected", false);
        }

        this->decks[i]->redraw();
        this->samplers[i]->redraw();
    }

    return;
}

unsigned short int
Gui::get_selected_deck_index()
{
    return this->selected_deck;
}

void
Gui::hover_playback(const unsigned short &deck_index)
{
    this->highlight_border_deck_sampler_area(deck_index, true);

    return;
}

void
Gui::unhover_playback(const unsigned short &deck_index)
{
    this->highlight_border_deck_sampler_area(deck_index, false);

    return;
}

void
Gui::highlight_border_deck_sampler_area(const unsigned short int &deck_index,
                                        const bool               &switch_on)
{
    // highlight pair deck+sampler.
    this->decks[deck_index]->setProperty("hover", switch_on);
    this->samplers[deck_index]->setProperty("hover", switch_on);

    // Redraw widget (necessary to reparse stylesheet).
    this->decks[deck_index]->redraw();
    this->samplers[deck_index]->redraw();

    return;
}

void
Gui::select_and_show_next_keys(const unsigned short &deck_index)
{
    // Check the button.
    this->file_browser_control_buttons[deck_index]->show_next_key_from_deck_button->setEnabled(false);
    this->file_browser_control_buttons[deck_index]->show_next_key_from_deck_button->setChecked(true);

    // Select deck.
    this->select_playback(deck_index);

    // Show next keys from deck.
    this->show_next_keys();

    // Release the button.
    this->file_browser_control_buttons[deck_index]->show_next_key_from_deck_button->setEnabled(true);
    this->file_browser_control_buttons[deck_index]->show_next_key_from_deck_button->setChecked(false);
}

void
Gui::show_next_keys()
{
    // Get music key of selected deck/sampler.
    QString deck_key = this->ats[this->get_selected_deck_index()]->get_music_key();

    // Get next and prev keys and iterate over the full audio collection for highlighting.
    if (deck_key.length() > 0)
    {
        // Collapse all tree.
        this->file_browser->collapseAll();

        // Get next keys and highlight.
        QString next_key;
        QString prev_key;
        QString oppos_key;
        Utils::get_next_music_keys(deck_key, next_key, prev_key, oppos_key);
        QList<QModelIndex> directories = this->file_system_model->set_next_keys(next_key, prev_key, oppos_key);

        // Expand directories containing file of next keys.
        foreach (QModelIndex index, directories)
        {
            this->file_browser->expand(index);
        }
    }

    return;
}

void
Gui::playback_thru(const unsigned short &deck_index, const bool &on_off)
{
    if (on_off == true)
    {
        this->control_and_play->set_process_mode(ProcessMode::THRU, deck_index);
    }
    else
    {
        this->control_and_play->set_process_mode(ProcessMode::TIMECODE, deck_index);
    }

}

PlaybackQGroupBox::PlaybackQGroupBox(const QString &title) : QGroupBox(title)
{
    // Init.
    this->l_selected = false;
    this->setAcceptDrops(true);

    return;
}

PlaybackQGroupBox::~PlaybackQGroupBox()
{
    return;
}

void
PlaybackQGroupBox::redraw()
{
    this->style()->unpolish(this);
    this->style()->polish(this);

    return;
}

void
PlaybackQGroupBox::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    emit this->selected();

    return;
}

void
PlaybackQGroupBox::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    emit this->hover();

    return;
}

void
PlaybackQGroupBox::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    emit this->unhover();

    return;
}

void
PlaybackQGroupBox::dragEnterEvent(QDragEnterEvent *event)
{
    // Only accept plain text to drop in.
    if (event->mimeData()->hasFormat("application/vnd.text.list"))
    {
        event->acceptProposedAction();
    }
}

void
PlaybackQGroupBox::dropEvent(QDropEvent *event)
{
    // Accept the drop action.
    event->acceptProposedAction();

    // Send a signal saying that a file was dropped into the groupbox.
    emit file_dropped();
}

Deck::Deck(const QString &title, const QSharedPointer<Audio_track> &at) : PlaybackQGroupBox(title)
{
    this->at = at;
    this->settings = &Singleton<Application_settings>::get_instance();
    this->setObjectName("DeckGBox");

    return;
}

Deck::~Deck()
{
    delete this->track_name;
    delete this->thru_button;
    delete this->key;
    delete this->waveform;
    delete this->remaining_time_layout;
    delete this->buttons_layout;
    delete this->speed;
    delete this->timecode_button;
    delete this->manual_button;
    delete this->speed_up_button;
    delete this->speed_down_button;
    delete this->accel_up_button;
    delete this->accel_down_button;
    delete this->restart_button;
    delete [] this->cue_set_buttons;
    delete [] this->cue_play_buttons;
    delete [] this->cue_del_buttons;
    delete [] this->cue_point_labels;

    return;
}

void
Deck::init_display()
{
    // Create track name, key and waveform.
    this->track_name = new QLabel(tr("T r a c k"));
    this->track_name->setObjectName("TrackName");
    this->thru_button = new QPushButton(tr("THRU"));
    this->thru_button->setObjectName("Thru_button");
    this->thru_button->setCheckable(true);
    this->thru_button->setChecked(false);
    this->key = new QLabel();
    this->key->setObjectName("KeyValue");
    this->set_key("");
    this->waveform = new Waveform(this->at);
    this->waveform->setObjectName("Waveform");

    // Create remaining time.
    this->remaining_time_layout = new QHBoxLayout;
    this->rem_time_minus = new QLabel("-");
    this->rem_time_min   = new QLabel("00");
    this->rem_time_sep1  = new QLabel(":");
    this->rem_time_sec   = new QLabel("00");
    this->rem_time_sep2  = new QLabel(":");
    this->rem_time_msec  = new QLabel("00");
    this->rem_time_minus->setObjectName("RemainingTime");
    this->rem_time_min->setObjectName("RemainingTime");
    this->rem_time_sep1->setObjectName("RemainingTime");
    this->rem_time_sec->setObjectName("RemainingTime");
    this->rem_time_sep2->setObjectName("RemainingTime");
    this->rem_time_msec->setObjectName("RemainingTimeMsec");
    this->remaining_time_layout->addWidget(this->rem_time_minus, 1, Qt::AlignBottom);
    this->remaining_time_layout->addWidget(this->rem_time_min,   1, Qt::AlignBottom);
    this->remaining_time_layout->addWidget(this->rem_time_sep1,  1, Qt::AlignBottom);
    this->remaining_time_layout->addWidget(this->rem_time_sec,   1, Qt::AlignBottom);
    this->remaining_time_layout->addWidget(this->rem_time_sep2,  1, Qt::AlignBottom);
    this->remaining_time_layout->addWidget(this->rem_time_msec,  1, Qt::AlignBottom);
    this->remaining_time_layout->addStretch(100);
    this->remaining_time_layout->addWidget(this->key,            1, Qt::AlignRight);

    // Create buttons area.
    this->buttons_layout = new QHBoxLayout();

    // Speed management.
    QVBoxLayout *timecode_speed_layout = new QVBoxLayout();
    QHBoxLayout *timecode_manual_layout = new QHBoxLayout();
    this->timecode_button = new QPushButton(tr("TIM"));
    this->timecode_button->setToolTip("<p>" + tr("Control speed with timecoded vinyl.") + "</p>");
    this->timecode_button->setObjectName("Timecode_manual_buttons");
    this->timecode_button->setFocusPolicy(Qt::NoFocus);
    this->timecode_button->setCheckable(true);
    this->timecode_button->setChecked(true);
    timecode_manual_layout->addWidget(this->timecode_button);
    this->manual_button = new QPushButton(tr("MAN"));
    this->manual_button->setToolTip("<p>" + tr("Control speed manually.") + "</p>");
    this->manual_button->setObjectName("Timecode_manual_buttons");
    this->manual_button->setFocusPolicy(Qt::NoFocus);
    this->manual_button->setCheckable(true);
    this->manual_button->setChecked(false);
    timecode_manual_layout->addWidget(this->manual_button);

    timecode_speed_layout->addLayout(timecode_manual_layout);
    timecode_speed_layout->addStretch(100);
    this->speed = new SpeedQLabel();
    this->speed->setText(tr("+000.0%"));
    this->speed->setObjectName("Speed_value");
    timecode_speed_layout->addWidget(this->speed);
    this->buttons_layout->addLayout(timecode_speed_layout);
    QGridLayout *speed_layout = new QGridLayout();
    this->speed_up_button = new SpeedQPushButton("+");
    this->speed_up_button->setToolTip("<p>" + tr("Speed up") + "</p><p><em>" + tr("left-click") + " = +0.1%</em></p><p><em>" + tr("right-click") + " = +1%</em></p>");
    this->speed_up_button->setObjectName("Speed_button");
    this->speed_up_button->setFocusPolicy(Qt::NoFocus);
    this->speed_up_button->setFixedSize(15, 15);
    speed_layout->addWidget(speed_up_button, 0, 1);
    this->speed_down_button = new SpeedQPushButton("-");
    this->speed_down_button->setToolTip("<p>" + tr("Slow down") + "</p><p><em>" + tr("left-click") + " = -0.1%</em></p><p><em>" + tr("right-click") + " = -1%</em></p>");
    this->speed_down_button->setObjectName("Speed_button");
    this->speed_down_button->setFocusPolicy(Qt::NoFocus);
    this->speed_down_button->setFixedSize(15, 15);
    speed_layout->addWidget(speed_down_button, 1, 1);
    this->accel_up_button = new SpeedQPushButton("↷");
    this->accel_up_button->setToolTip("<p>" + tr("Temporarily speed up") + "</p><p><em>" + tr("left-click") + " = +1%</em></p><p><em>" + tr("right-click") + " = +10%</em></p>");
    this->accel_up_button->setObjectName("Speed_button");
    this->accel_up_button->setFocusPolicy(Qt::NoFocus);
    this->accel_up_button->setFixedSize(15, 15);
    speed_layout->addWidget(accel_up_button, 0, 2);
    this->accel_down_button = new SpeedQPushButton("↶");
    this->accel_down_button->setToolTip("<p>" + tr("Temporarily slow down") + "</p><p><em>" + tr("left-click") + " = -1%</em></p><p><em>" + tr("right-click") + " = -10%</em></p>");
    this->accel_down_button->setObjectName("Speed_button");
    this->accel_down_button->setFocusPolicy(Qt::NoFocus);
    this->accel_down_button->setFixedSize(15, 15);
    speed_layout->addWidget(accel_down_button, 1, 2);
    this->buttons_layout->addLayout(speed_layout);
    this->buttons_layout->addStretch(1000);

    // Set default speed control mode.
    this->set_speed_mode_timecode();

    // Restart button.
    this->restart_button = new QPushButton();
    this->restart_button->setObjectName("Restart_button");
    this->restart_button->setToolTip("<p>" + tr("Jump to start") + "</p><em>" + this->settings->get_keyboard_shortcut(KB_PLAY_BEGIN_TRACK_ON_DECK) + "</em>");
    this->restart_button->setFixedSize(15, 15);
    this->restart_button->setFocusPolicy(Qt::NoFocus);
    this->restart_button->setCheckable(true);
    buttons_layout->addWidget(this->restart_button, 1, Qt::AlignLeft | Qt::AlignTop);

    // Cue point management.
    this->cue_set_buttons  = new QPushButton* [MAX_NB_CUE_POINTS];
    this->cue_play_buttons = new QPushButton* [MAX_NB_CUE_POINTS];
    this->cue_del_buttons  = new QPushButton* [MAX_NB_CUE_POINTS];
    this->cue_point_labels    = new QLabel* [MAX_NB_CUE_POINTS];
    for (unsigned short int i = 0; i < MAX_NB_CUE_POINTS; i++)
    {
        this->cue_set_buttons[i] = new QPushButton();
        this->cue_set_buttons[i]->setObjectName("Cue_set_button" + QString::number(i));
        this->cue_set_buttons[i]->setToolTip("<p>" + tr("Set cue point") + " " + QString::number(i+1) + "</p><em>" + this->settings->get_keyboard_shortcut(KB_SET_CUE_POINTS_ON_DECK[i]) + "</em>");
        this->cue_set_buttons[i]->setFixedSize(15, 15);
        this->cue_set_buttons[i]->setFocusPolicy(Qt::NoFocus);
        this->cue_set_buttons[i]->setCheckable(true);

        this->cue_play_buttons[i] = new QPushButton();
        this->cue_play_buttons[i]->setObjectName("Cue_play_button" + QString::number(i));
        this->cue_play_buttons[i]->setToolTip("<p>" + tr("Play from cue point") + " " + QString::number(i+1) + "</p><em>" + this->settings->get_keyboard_shortcut(KB_PLAY_CUE_POINTS_ON_DECK[i]) + "</em>");
        this->cue_play_buttons[i]->setFixedSize(15, 15);
        this->cue_play_buttons[i]->setFocusPolicy(Qt::NoFocus);
        this->cue_play_buttons[i]->setCheckable(true);

        this->cue_del_buttons[i] = new QPushButton();
        this->cue_del_buttons[i]->setObjectName("Cue_del_button" + QString::number(i));
        this->cue_del_buttons[i]->setToolTip("<p>" + tr("Delete cue point") + " " + QString::number(i+1));
        this->cue_del_buttons[i]->setFixedSize(15, 15);
        this->cue_del_buttons[i]->setFocusPolicy(Qt::NoFocus);
        this->cue_del_buttons[i]->setCheckable(true);

        this->cue_point_labels[i] = new QLabel("__:__:___");
        this->cue_point_labels[i]->setObjectName("Cue_point_label");
        this->cue_point_labels[i]->setAlignment(Qt::AlignCenter);
        this->cue_point_labels[i]->setFixedHeight(15);

        QHBoxLayout *cue_buttons_layout = new QHBoxLayout();
        cue_buttons_layout->addWidget(this->cue_set_buttons[i],  1, Qt::AlignRight);
        cue_buttons_layout->addWidget(this->cue_play_buttons[i], 1, Qt::AlignRight);
        cue_buttons_layout->addWidget(this->cue_del_buttons[i],  1, Qt::AlignRight);

        QVBoxLayout *cue_points_layout = new QVBoxLayout();
        cue_points_layout->addLayout(cue_buttons_layout);
        cue_points_layout->addWidget(this->cue_point_labels[i], Qt::AlignCenter);

        this->buttons_layout->addStretch(5);
        this->buttons_layout->addLayout(cue_points_layout, 1);
    }

    // Create main deck layout.
    QHBoxLayout *general_layout = new QHBoxLayout();
    QVBoxLayout *sub_layout     = new QVBoxLayout();
    QHBoxLayout *track_layout   = new QHBoxLayout();

    // Put track name, position and timecode info in sub layout.
    track_layout->addWidget(this->track_name,  95);
    track_layout->addWidget(this->thru_button, 5);
    sub_layout->addLayout(track_layout,                5);
    sub_layout->addLayout(this->remaining_time_layout, 5);
    sub_layout->addWidget(this->waveform,              85);
    sub_layout->addLayout(this->buttons_layout,        5);
    general_layout->addLayout(sub_layout,              90);

    // Put main layout into groub box.
    this->setLayout(general_layout);

    return;
}

void
Deck::set_speed_mode_timecode()
{
    // Switch to timecode mode.
    this->manual_button->setChecked(false);
    this->speed_up_button->hide();
    this->speed_down_button->hide();
    this->accel_up_button->hide();
    this->accel_down_button->hide();
    this->thru_button->show();
    this->speed->hide(); // FIXME: this is a test to validate that updating at a high frame rate this label will cause the app crash.
}

void
Deck::set_speed_mode_manual()
{
    // Switch to manual mode.
    this->timecode_button->setChecked(false);
    this->speed_up_button->show();
    this->speed_down_button->show();
    this->accel_up_button->show();
    this->accel_down_button->show();
    this->thru_button->hide();
    this->speed->show(); // FIXME: this is a test to validate that updating at a high frame rate this label will cause the app crash.
}

void
Deck::set_key(const QString &key)
{
    if (key.isEmpty() == false)
    {
        this->key->show();
        this->key->setText("♪ " + key);
    }
    else
    {
        this->key->hide();
        this->key->setText("");
    }
}

Sampler::Sampler(const QString &title,
                 const unsigned short int &nb_samplers) : PlaybackQGroupBox(title)
{
    this->settings = &Singleton<Application_settings>::get_instance();
    this->setObjectName("SamplerGBox");
    this->nb_samplers = nb_samplers;

    return;
}

Sampler::~Sampler()
{
    return;
}

void
Sampler::init_display()
{
    // Main sampler layout.
    QVBoxLayout *layout = new QVBoxLayout();

    // Play, stop, del buttons.
    QString name("A");
    for (unsigned short int i = 0; i < this->nb_samplers; i++)
    {
        QPushButton *play = new QPushButton();
        play->setObjectName("Sampler_play_buttons");
        play->setFixedSize(16, 16);
        play->setFocusPolicy(Qt::NoFocus);
        play->setCheckable(true);
        play->setToolTip(tr("Play sample from start"));
        this->buttons_play.push_back(play);

        QPushButton *stop = new QPushButton();
        stop->setObjectName("Sampler_stop_buttons");
        stop->setFixedSize(16, 16);
        stop->setFocusPolicy(Qt::NoFocus);
        stop->setCheckable(true);
        stop->setChecked(true);
        stop->setToolTip(tr("Stop sample"));
        this->buttons_stop.push_back(stop);

        QPushButton *del = new QPushButton();
        del->setObjectName("Sampler_del_buttons");
        del->setFixedSize(16, 16);
        del->setFocusPolicy(Qt::NoFocus);
        del->setCheckable(true);
        del->setChecked(true);
        del->setToolTip(tr("Delete sample"));
        this->buttons_del.push_back(del);

        this->tracknames.push_back(new QLabel(tr("--")));
        this->remaining_times.push_back(new QLabel("- 00"));

        QHBoxLayout *horz_layout = new QHBoxLayout();
        QLabel *name_label = new QLabel(name);
        name_label->setFixedWidth(15);
        horz_layout->addWidget(name_label,             1);
        horz_layout->addWidget(this->buttons_play[i],  1);
        horz_layout->addWidget(this->buttons_stop[i],  1);
        horz_layout->addWidget(this->buttons_del[i],   1);
        horz_layout->addWidget(this->remaining_times[i], 4);
        horz_layout->addWidget(this->tracknames[i],     95);
        horz_layout->setMargin(0);

        // Drag and drop areas.
        QSamplerContainerWidget *container = new QSamplerContainerWidget();
        this->drop_areas.push_back(container);
        container->setLayout(horz_layout);
        layout->addWidget(container);

        name[0].unicode()++; // Next sampler letter.
    }

    this->setLayout(layout);
}

FileBrowserControlButtons::FileBrowserControlButtons(const unsigned short int &deck_index,
                                                     const unsigned short int &nb_samplers) : QHBoxLayout()
{
    Application_settings *settings = &Singleton<Application_settings>::get_instance();

    this->load_track_on_deck_button = new QPushButton();
    this->load_track_on_deck_button->setObjectName("Load_track_button_" + QString::number(deck_index + 1));
    this->load_track_on_deck_button->setToolTip("<p>" + tr("Load selected track to deck ") + QString::number(deck_index + 1) + "</p><em>" + settings->get_keyboard_shortcut(KB_LOAD_TRACK_ON_DECK) + "</em>");
    this->load_track_on_deck_button->setFixedSize(24, 24);
    this->load_track_on_deck_button->setFocusPolicy(Qt::NoFocus);
    this->load_track_on_deck_button->setCheckable(true);
    this->addWidget(this->load_track_on_deck_button);

    this->show_next_key_from_deck_button = new QPushButton();
    this->show_next_key_from_deck_button->setObjectName("Show_next_key_button");
    this->show_next_key_from_deck_button->setToolTip("<p>" + tr("Show deck ") + QString::number(deck_index + 1) + tr(" next potential tracks") + "</p><em>" + settings->get_keyboard_shortcut(KB_SHOW_NEXT_KEYS) + "</em>");
    this->show_next_key_from_deck_button->setFixedSize(24, 24);
    this->show_next_key_from_deck_button->setFocusPolicy(Qt::NoFocus);
    this->show_next_key_from_deck_button->setCheckable(true);
    this->addWidget(this->show_next_key_from_deck_button);

    this->addSpacing(20);

    QString name("A");
    for (unsigned short int i = 0; i < nb_samplers; i++)
    {
        QPushButton* load_sample_button = new QPushButton();
        load_sample_button->setObjectName("Load_track_sample_button_" + name);

        load_sample_button->setFixedSize(20, 20);
        load_sample_button->setFocusPolicy(Qt::NoFocus);
        load_sample_button->setCheckable(true);
        this->load_sample_buttons << load_sample_button;
        this->addWidget(load_sample_button);

        QString shortcut = "";
        switch (i)
        {
            case 0: shortcut = KB_LOAD_TRACK_ON_SAMPLER1; break;
            case 1: shortcut = KB_LOAD_TRACK_ON_SAMPLER2; break;
            case 2: shortcut = KB_LOAD_TRACK_ON_SAMPLER3; break;
            case 3: shortcut = KB_LOAD_TRACK_ON_SAMPLER4; break;
        }
        if (shortcut.isEmpty() == false)
        {
            this->load_sample_buttons[i]->setToolTip("<p>" + tr("Load selected track to sampler ") + name + "</p><em>" + settings->get_keyboard_shortcut(shortcut) + "</em>");
        }
        else
        {
            this->load_sample_buttons[i]->setToolTip("<p>" + tr("Load selected track to sampler ") + name + "</p>");
        }

        name[0].unicode()++; // Next sampler letter.
    }

    return;
}

FileBrowserControlButtons::~FileBrowserControlButtons()
{
    return;
}

void FileBrowserControlButtons::hide_load_sample_buttons()
{
    for (unsigned short int i = 0; i < this->load_sample_buttons.size(); i++)
    {
        this->load_sample_buttons[i]->hide();
    }

    return;
}

void FileBrowserControlButtons::show_load_sample_buttons()
{
    for (unsigned short int i = 0; i < this->load_sample_buttons.size(); i++)
    {
        this->load_sample_buttons[i]->show();
    }

    return;
}

SpeedQPushButton::SpeedQPushButton(const QString &title) : QPushButton(title)
{
    // Init.
    this->setProperty("right_clicked", false);
    this->setProperty("pressed", false);

    return;
}

SpeedQPushButton::~SpeedQPushButton()
{
    return;
}

void
SpeedQPushButton::redraw()
{
    this->style()->unpolish(this);
    this->style()->polish(this);

    return;
}

void
SpeedQPushButton::mousePressEvent(QMouseEvent *event)
{
    // Force state "pressed" in style sheet even it is a right click event.
    this->setProperty("pressed", true);
    this->redraw();

    QPushButton::mousePressEvent(event);

    return;
}

void
SpeedQPushButton::mouseReleaseEvent(QMouseEvent *event)
{
    // Unpress the button.
    this->setProperty("pressed", false);
    this->redraw();

    QPushButton::mouseReleaseEvent(event);

    // Forward the right click event.
    if (event->button() == Qt::RightButton)
    {
        emit this->right_clicked();
    }

    return;
}

void
SpeedQLabel::mouseReleaseEvent(QMouseEvent *event)
{
    // Forward the right click event.
    if (event->button() == Qt::RightButton)
    {
        emit this->right_clicked();
    }

    return;
}

QSamplerContainerWidget::QSamplerContainerWidget() : QWidget()
{
    this->setAcceptDrops(true);

    return;
}

QSamplerContainerWidget::~QSamplerContainerWidget()
{
    return;
}

void
QSamplerContainerWidget::dragEnterEvent(QDragEnterEvent *event)
{
    // Only accept plain text to drop in.
    if (event->mimeData()->hasFormat("application/vnd.text.list"))
    {
        event->acceptProposedAction();
    }
}

void
QSamplerContainerWidget::dropEvent(QDropEvent *event)
{
    // Accept the drop action.
    event->acceptProposedAction();

    // Send a signal saying that a file was dropped into the sampler.
    emit file_dropped_in_sampler();
}

TreeViewIconProvider::TreeViewIconProvider()
{
    this->set_default_icons();
}

TreeViewIconProvider::~TreeViewIconProvider()
{
}

void TreeViewIconProvider::set_icons(const QIcon &drive,
                                     const QIcon &folder,
                                     const QIcon &file,
                                     const QIcon &other)
{
    this->drive  = drive;
    this->folder = folder;
    this->file   = file;
    this->other  = other;
}

void TreeViewIconProvider::set_default_icons()
{
    this->drive  = QIcon(QApplication::style()->standardIcon(QStyle::SP_DriveHDIcon));
    this->folder = QIcon(QApplication::style()->standardIcon(QStyle::SP_DirIcon));
    this->file   = QIcon(QApplication::style()->standardIcon(QStyle::SP_FileIcon));
    this->other  = QIcon(QApplication::style()->standardIcon(QStyle::SP_FileIcon));
}

QIcon TreeViewIconProvider::icon(IconType type) const
{
    if (type == TreeViewIconProvider::Drive)
    {
        return this->drive;
    }
    else if (type == TreeViewIconProvider::Folder)
    {
        return this->folder;
    }
    else if (type == TreeViewIconProvider::File)
    {
        return this->file;
    }

    return this->other;
}

QIcon TreeViewIconProvider::icon(const QFileInfo &info) const
{
    if (info.isRoot() == true)
    {
        return this->drive;
    }
    else if (info.isDir() == true)
    {
        return this->folder;
    }
    else if (info.isFile() == true)
    {
        return this->file;
    }

    return this->other;
}

QString TreeViewIconProvider::type(const QFileInfo &info) const
{
    if (info.isRoot() == true)
    {
        return QString("Drive");
    }
    else if (info.isDir() == true)
    {
        return QString("Folder");
    }
    else if (info.isFile() == true)
    {
        return QString("File");
    }

    return QString("File");
}
