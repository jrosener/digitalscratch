/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------( gui.cpp )-*/
/*                                                                            */
/*  Copyright (C) 2003-2013                                                   */
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
#include <FLAC/format.h>
#include <samplerate.h>
#include <jack/jack.h>
#include <QSignalMapper>
#include <QMessageBox>
#include <QApplication>
#include <QPushButton>
#include <QCommandLinkButton>

#include "gui.h"
#include "digital_scratch_api.h"
#include "audio_collection_model.h"
#include "utils.h"
#include "singleton.h"

Gui::Gui(Audio_track                    *in_at_1,
         Audio_track                    *in_at_2,
         Audio_track                  ***in_at_samplers,
         unsigned short int              in_nb_samplers,
         Audio_file_decoding_process    *in_dec_1,
         Audio_file_decoding_process    *in_dec_2,
         Audio_file_decoding_process  ***in_dec_samplers,
         Playback_parameters            *in_params_1,
         Playback_parameters            *in_params_2,
         Audio_track_playback_process   *in_playback,
         unsigned short int              in_nb_decks,
         Sound_card_access_rules        *in_sound_card,
         int                            *in_dscratch_ids)
{
    qDebug() << "Gui::Gui: create object...";

    // Set attributes.
    this->window_style = GUI_STYLE_DEFAULT;
    this->window       = new QWidget;
    if (in_at_1          == NULL ||
        in_at_2          == NULL ||
        in_at_samplers   == NULL ||
        in_dec_1         == NULL ||
        in_dec_2         == NULL ||
        in_dec_samplers  == NULL ||
        in_params_1      == NULL ||
        in_params_2      == NULL ||
        in_playback      == NULL ||
        in_sound_card    == NULL)
    {
        qFatal("Gui::Gui: Incorrect parameters.");
        return;
    }
    else
    {
        this->settings       = &Singleton<Application_settings>::get_instance();
        this->at_1           = in_at_1;
        this->at_2           = in_at_2;
        this->at_1_samplers  = in_at_samplers[0];
        this->at_2_samplers  = in_at_samplers[1];
        this->nb_samplers    = in_nb_samplers;
        this->dec_1          = in_dec_1;
        this->dec_2          = in_dec_2;
        this->dec_1_samplers = in_dec_samplers[0];
        this->dec_2_samplers = in_dec_samplers[1];
        this->params_1       = in_params_1;
        this->params_2       = in_params_2;
        this->playback       = in_playback;
        this->nb_decks       = in_nb_decks;
        this->sound_card     = in_sound_card;
        this->dscratch_ids   = in_dscratch_ids;
    }

    // Creates dynamic widgets.
    this->file_system_model = new Audio_collection_model();

    this->file_browser = new QTreeView();
    this->file_browser->setModel(this->file_system_model);
    this->file_browser_gbox = new QGroupBox();

    this->decks_remaining_time    = new Remaining_time* [2];
    this->decks_remaining_time[0] = new Remaining_time();
    this->decks_remaining_time[1] = new Remaining_time();

    // Init dialogs.
    this->config_dialog = NULL;
    this->refresh_audio_collection_dialog = NULL;
    this->about_dialog = NULL;

    // Init shortcuts.
    this->shortcut_switch_playback    = new QShortcut(this->window);
    this->shortcut_collapse_browser   = new QShortcut(this->file_browser);
    this->shortcut_load_audio_file    = new QShortcut(this->file_browser);
    this->shortcut_go_to_begin        = new QShortcut(this->window);
    this->shortcut_set_cue_point      = new QShortcut(this->window);
    this->shortcut_go_to_cue_point    = new QShortcut(this->window);
    this->shortcut_load_sample_file_1 = new QShortcut(this->file_browser);
    this->shortcut_load_sample_file_2 = new QShortcut(this->file_browser);
    this->shortcut_load_sample_file_3 = new QShortcut(this->file_browser);
    this->shortcut_load_sample_file_4 = new QShortcut(this->file_browser);
    this->shortcut_show_next_keys     = new QShortcut(this->file_browser);
    this->shortcut_fullscreen         = new QShortcut(this->window);
    this->shortcut_help               = new QShortcut(this->window);

    // Create main window.
    if (this->create_main_window() != true)
    {
        qFatal("Gui::Gui: Creation of main window failed.");
        return;
    }

    // Apply previous window position.
    this->window->move(this->settings->get_main_window_position());

    // Apply previous window size.
    this->window->resize(this->settings->get_main_window_size());

    // Apply application settings.
    if (this->apply_application_settings() != true)
    {
        qFatal("Gui::Gui: Can not apply application settings.");
        return;
    }

    qDebug() << "Gui::Gui: create object done.";

    return;
}

Gui::~Gui()
{
    qDebug() << "Gui::Gui: delete object...";

    // Store size/position of the main window (first go back from fullscreen or maximized mode).
    this->window->showNormal();
    this->settings->set_main_window_position(this->window->pos());
    this->settings->set_main_window_size(this->window->size());

    // Stop running threads.
    if (this->file_system_model->concurrent_watcher_store->isRunning() == true)
    {
        this->file_system_model->concurrent_watcher_store->cancel();
        this->file_system_model->concurrent_watcher_store->waitForFinished();
    }
    if (this->file_system_model->concurrent_watcher_read->isRunning() == true)
    {
        this->file_system_model->concurrent_watcher_read->cancel();
        this->file_system_model->concurrent_watcher_read->waitForFinished();
    }
    delete this->file_system_model;

    // Cleanup.
    delete this->file_browser;
    delete this->config_dialog;
    delete this->window;
    delete [] this->decks_remaining_time;

    qDebug() << "Gui::Gui: delete object done.";

    return;
}

bool
Gui::apply_application_settings()
{
    qDebug() << "Gui::apply_application_settings...";

    // Apply windows style.
    this->window_style = this->settings->get_gui_style();
    if (this->apply_main_window_style() != true)
    {
        qWarning() << "Gui::apply_application_settings: Cannot set new style to main window";
    }

    // Change base path for tracks browser.
    this->set_file_browser_base_path(this->settings->get_tracks_base_dir_path());

    // Apply motion detection settings for all turntables.
    for (int i = 0; i < this->nb_decks; i++)
    {
        if (dscratch_change_vinyl_type(this->dscratch_ids[i],
                                       (char*)this->settings->get_vinyl_type().toStdString().c_str()) != 0)
        {
            qWarning() << "Gui::apply_application_settings: Cannot set vinyl type";
        }
        if (dscratch_set_extreme_min(this->dscratch_ids[i], this->settings->get_extreme_min()) != 0)
        {
            qWarning() << "Gui::apply_application_settings: Cannot set new extreme min value";
        }
        if (dscratch_set_max_nb_buffer(this->dscratch_ids[i], this->settings->get_max_nb_buffer()) != 0)
        {
            qWarning() << "Gui::apply_application_settings: Cannot set new max nb buffer value";
        }
        if (dscratch_set_max_buffer_coeff(this->dscratch_ids[i], this->settings->get_max_buffer_coeff()) != 0)
        {
            qWarning() << "Gui::apply_application_settings: Cannot set new max buffer coeff value";
        }
        if (dscratch_set_max_speed_diff(this->dscratch_ids[i], this->settings->get_max_speed_diff()) != 0)
        {
            qWarning() << "Gui::apply_application_settings: Cannot set new max speed diff value";
        }
        if (dscratch_set_slow_speed_algo_usage(this->dscratch_ids[i], this->settings->get_slow_speed_algo_usage()) != 0)
        {
            qWarning() << "Gui::apply_application_settings: Cannot set new slow speed algo usage value";
        }
        if (dscratch_set_max_nb_speed_for_stability(this->dscratch_ids[i], this->settings->get_max_nb_speed_for_stability()) != 0)
        {
            qWarning() << "Gui::apply_application_settings: Cannot set new max nb speed for stability value";
        }
        if (dscratch_set_nb_cycle_before_changing_direction(this->dscratch_ids[i], this->settings->get_nb_cycle_before_changing_direction()) != 0)
        {
            qWarning() << "Gui::apply_application_settings: Cannot set new nb cycle before changing direction value";
        }
        if (dscratch_set_low_pass_filter_max_speed_usage(this->dscratch_ids[i], this->settings->get_low_pass_filter_max_speed_usage()) != 0)
        {
            qWarning() << "Gui::apply_application_settings: Cannot set new low pass filter max speed usage value";
        }
    }

    // Change shortcuts.
    this->shortcut_switch_playback->setKey(QKeySequence(this->settings->get_keyboard_shortcut(KB_SWITCH_PLAYBACK)));
    this->shortcut_collapse_browser->setKey(QKeySequence(this->settings->get_keyboard_shortcut(KB_COLLAPSE_BROWSER)));
    this->shortcut_load_audio_file->setKey(QKeySequence(this->settings->get_keyboard_shortcut(KB_LOAD_TRACK_ON_DECK)));
    this->shortcut_go_to_begin->setKey(QKeySequence(this->settings->get_keyboard_shortcut(KB_PLAY_BEGIN_TRACK_ON_DECK)));
    this->shortcut_set_cue_point->setKey(QKeySequence(this->settings->get_keyboard_shortcut(KB_SET_CUE_POINT_ON_DECK)));
    this->shortcut_go_to_cue_point->setKey(QKeySequence(this->settings->get_keyboard_shortcut(KB_PLAY_CUE_POINT_ON_DECK)));
    this->shortcut_load_sample_file_1->setKey(QKeySequence(this->settings->get_keyboard_shortcut(KB_LOAD_TRACK_ON_SAMPLER1)));
    this->shortcut_load_sample_file_2->setKey(QKeySequence(this->settings->get_keyboard_shortcut(KB_LOAD_TRACK_ON_SAMPLER2)));
    this->shortcut_load_sample_file_3->setKey(QKeySequence(this->settings->get_keyboard_shortcut(KB_LOAD_TRACK_ON_SAMPLER3)));
    this->shortcut_load_sample_file_4->setKey(QKeySequence(this->settings->get_keyboard_shortcut(KB_LOAD_TRACK_ON_SAMPLER4)));
    this->shortcut_show_next_keys->setKey(QKeySequence(this->settings->get_keyboard_shortcut(KB_SHOW_NEXT_KEYS)));
    this->shortcut_fullscreen->setKey(QKeySequence(this->settings->get_keyboard_shortcut(KB_FULLSCREEN)));
    this->shortcut_help->setKey(QKeySequence(this->settings->get_keyboard_shortcut(KB_HELP)));

    qDebug() << "Gui::apply_application_settings done.";

    return true;
}

bool
Gui::show_config_window()
{
    qDebug() << "Gui::show_config_window...";

    // Create a configuration dialog.
    if (this->config_dialog != NULL)
    {
        delete this->config_dialog;
    }
    this->config_dialog = new Config_dialog(this->window);

    // Apply application settings if dialog is closed by OK.
    if (this->config_dialog->show() == QDialog::Accepted)
    {
        if (this->apply_application_settings() != true)
        {
            qFatal("Can not apply settings.");
            return false;
        }
    }

    qDebug() << "Gui::show_config_window done.";

    return true;
}

void
Gui::set_fullscreen()
{
    qDebug() << "Gui::set_fullscreen...";

    if (this->window->isFullScreen() == false)
    {
        this->window->showFullScreen();
    }
    else
    {
        this->window->showNormal();
    }

    qDebug() << "Gui::set_fullscreen done.";
}

void
Gui::show_help()
{
    qDebug() << "Gui::show_help...";

    if (this->help_groupbox->isHidden() == true)
    {
        this->help_groupbox->show();
    }
    else
    {
        this->help_groupbox->hide();
    }

    qDebug() << "Gui::show_help done.";
}

void
Gui::analyze_audio_collection(bool is_all_files)
{
    // Analyzis not running, show a popup asking for a full refresh or only for new files.
    this->settings->set_audio_collection_full_refresh(is_all_files);

    // Show progress bar.
    this->refresh_file_browser_progress->setVisible(true);

    // Compute data on file collection and store them to DB.
    this->file_system_model->concurrent_analyse_audio_collection();
}

void
Gui::update_refresh_progress_value(int in_value)
{
    this->refresh_file_browser_progress->setValue(in_value);
    this->file_browser->update();
}

void
Gui::on_finished_analyze_audio_collection()
{
    // Hide progress bar.
    this->refresh_file_browser_progress->setVisible(false);

    // Refresh file browser.
    this->file_browser->setRootIndex(this->file_system_model->get_root_index());
    this->refresh_file_browser->setEnabled(true);
    this->refresh_file_browser->setChecked(false);
}

void
Gui::reject_refresh_audio_collection_dialog()
{
    qDebug() << "Gui::reject_refresh_audio_collection_dialog...";

    this->refresh_file_browser->setEnabled(true);
    this->refresh_file_browser->setChecked(false);

    qDebug() << "Gui::reject_refresh_audio_collection_dialog done.";

    return;
}

void
Gui::close_refresh_audio_collection_dialog()
{
    qDebug() << "Gui::close_refresh_audio_collection_dialog...";

    if (this->refresh_audio_collection_dialog != NULL)
    {
        this->refresh_audio_collection_dialog->done(QDialog::Rejected);
    }

    qDebug() << "Gui::close_refresh_audio_collection_dialog done.";

    return;
}

void
Gui::accept_refresh_audio_collection_dialog_all_files()
{
    qDebug() << "Gui::accept_refresh_audio_collection_dialog_all_files...";

    // Analyze all files of audio collection.
    this->analyze_audio_collection(true);

    if (this->refresh_audio_collection_dialog != NULL)
    {
        this->refresh_audio_collection_dialog->done(QDialog::Accepted);
    }

    qDebug() << "Gui::accept_refresh_audio_collection_dialog_all_files done.";

    return;
}

void
Gui::accept_refresh_audio_collection_dialog_new_files()
{
    qDebug() << "Gui::accept_refresh_audio_collection_dialog_new_files...";

    // Analyze all files of audio collection.
    this->analyze_audio_collection(false);

    if (this->refresh_audio_collection_dialog != NULL)
    {
        this->refresh_audio_collection_dialog->done(QDialog::Accepted);
    }

    qDebug() << "Gui::accept_refresh_audio_collection_dialog_new_files done.";

    return;
}

bool
Gui::show_refresh_audio_collection_dialog()
{
    qDebug() << "Gui::show_refresh_audio_collection_dialog...";

    // Show dialog only if there is no other analyzis process running.
    if (this->file_system_model->concurrent_watcher_store->isRunning() == false)
    {
        // Create the dialog object.
        if (this->refresh_audio_collection_dialog != NULL)
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
        QObject::connect(choice_all_files_button, SIGNAL(clicked()),
                         this, SLOT(accept_refresh_audio_collection_dialog_all_files()));


        // Choice 2: => New files.
        QCommandLinkButton *choice_new_files_button = new QCommandLinkButton(tr("New files."),
                                                                             tr("Analyze only files with missing data") + " (" + QString::number(this->file_system_model->get_nb_new_items()) + " " + tr("elements") + ")",
                                                                             this->refresh_audio_collection_dialog);
        QObject::connect(choice_new_files_button, SIGNAL(clicked()),
                         this, SLOT(accept_refresh_audio_collection_dialog_new_files()));

        // Close/cancel button.
        QDialogButtonBox *cancel_button = new QDialogButtonBox(QDialogButtonBox::Cancel);
        QObject::connect(cancel_button, SIGNAL(rejected()), this, SLOT(close_refresh_audio_collection_dialog()));
        QObject::connect(this->refresh_audio_collection_dialog, SIGNAL(rejected()), this, SLOT(reject_refresh_audio_collection_dialog()));

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
    }
    else
    {
        // Analyzis already running. Cancel it.
        this->file_system_model->concurrent_watcher_store->cancel();
        this->file_system_model->concurrent_watcher_store->waitForFinished();
    }

    qDebug() << "Gui::show_refresh_audio_collection_dialog done.";

    return true;
}

void
Gui::done_about_window()
{
    qDebug() << "Gui::done_about_window...";

    if (this->about_dialog != NULL)
    {
        this->about_dialog->done(QDialog::Accepted);
    }

    qDebug() << "Gui::done_about_window done.";

    return;
}

bool
Gui::show_about_window()
{
    qDebug() << "Gui::show_about_window...";

    // Create about window.
    this->about_dialog = new QDialog(this->window);

    // Set properties : title, icon.
    this->about_dialog->setWindowTitle(tr("About DigitalScratch"));
    if (this->nb_decks > 1)
    {
        this->about_dialog->setWindowIcon(QIcon(ICON_2));
    }
    else
    {
        this->about_dialog->setWindowIcon(QIcon(ICON));
    }


    //
    // Set content (logo, name-version, description, credit, license, libraries).
    //
    QLabel *logo = new QLabel();
    logo->setPixmap(QPixmap(LOGO));
    logo->setAlignment(Qt::AlignHCenter);

    QString version = QString("<h1>DigitalScratch ") + QString(STR(VERSION)) + QString("</h1>");
    QLabel *name = new QLabel(tr(version.toUtf8()));
    name->setAlignment(Qt::AlignHCenter);
    name->setTextFormat(Qt::RichText);

    QLabel *description = new QLabel(tr("A vinyl emulation software."));
    description->setAlignment(Qt::AlignHCenter);

    QLabel *web_site = new QLabel("<a style=\"color: orange\" href=\"http://www.digital-scratch.org\">http://www.digital-scratch.org</a>");
    web_site->setAlignment(Qt::AlignHCenter);
    web_site->setTextFormat(Qt::RichText);
    web_site->setTextInteractionFlags(Qt::TextBrowserInteraction);
    web_site->setOpenExternalLinks(true);

    QLabel *credit = new QLabel(tr("Copyright (C) 2003-2013 Julien Rosener"));
    credit->setAlignment(Qt::AlignHCenter);

    QLabel *license = new QLabel(tr("This program is free software; you can redistribute it and/or modify <br/> \
                                     it under the terms of the GNU General Public License as published by <br/> \
                                     the Free Software Foundation; either version 3 of the License, or <br/> \
                                     (at your option) any later version.<br/><br/>"));
    license->setTextFormat(Qt::RichText);

    QLabel *built = new QLabel("<b>" + tr("Built with:") + "</b>");
    built->setTextFormat(Qt::RichText);
    QLabel *libdigitalscratch_version = new QLabel((QString("- libdigitalscratch v") + QString(dscratch_get_version())).toUtf8());
    QLabel *libmpg123_version = new QLabel("- libmpg123");
    QLabel *libFLAC_version = new QLabel((QString("- libFLAC v") + QString(FLAC__VERSION_STRING)).toUtf8());
    QLabel *libsamplerate_version = new QLabel((QString("- ") + QString(src_get_version())).toUtf8());
    QLabel *libjack_version = new QLabel((QString("- libjack v") + QString(jack_get_version_string())).toUtf8());
    QLabel *qt_version = new QLabel((QString("- Qt v") + QString(qVersion())).toUtf8());

    QLabel *credits = new QLabel("<br/><b>" + tr("Credits:") + "</b>");
    credits->setTextFormat(Qt::RichText);
    QLabel *icons = new QLabel("- Devine icons: <a style=\"color: grey\" href=\"http://ipapun.deviantart.com\">http://ipapun.deviantart.com</a>");
    icons->setTextFormat(Qt::RichText);
    icons->setTextInteractionFlags(Qt::TextBrowserInteraction);
    icons->setOpenExternalLinks(true);

    // Close button.
    QDialogButtonBox *button = new QDialogButtonBox(QDialogButtonBox::Close);
    QObject::connect(button, SIGNAL(rejected()), this, SLOT(done_about_window()));

    // Full window layout.
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(logo, Qt::AlignHCenter);
    layout->addWidget(name, Qt::AlignHCenter);
    layout->addWidget(description, Qt::AlignHCenter);
    layout->addWidget(web_site, Qt::AlignHCenter);
    layout->addWidget(credit);
    layout->addWidget(license);
    layout->addWidget(built);
    layout->addWidget(libdigitalscratch_version);
    layout->addWidget(libmpg123_version);
    layout->addWidget(libFLAC_version);
    layout->addWidget(libsamplerate_version);
    layout->addWidget(libjack_version);
    layout->addWidget(qt_version);
    layout->addWidget(credits);
    layout->addWidget(icons);
    layout->addWidget(button);

    // Put layout in dialog.
    this->about_dialog->setLayout(layout);
    layout->setSizeConstraint(QLayout::SetFixedSize);

    // Show dialog.
    this->about_dialog->exec();

    qDebug() << "Gui::show_about_window done.";

    return true;
}

void
Gui::done_error_window()
{
    qDebug() << "Gui::done_error_window...";

    if (this->error_dialog != NULL)
    {
        this->error_dialog->done(QDialog::Accepted);
    }

    qDebug() << "Gui::done_error_window done.";

    return;
}

bool
Gui::show_error_window(QString in_error_message)
{
    qDebug() << "Gui::show_error_window...";

    // Prepare error window.
    QMessageBox msg_box;
    msg_box.setWindowTitle("DigitalScratch");
    msg_box.setText("<h2>" + tr("Error") + "</h2>"
                    + "<br/>" + in_error_message
                    + "<br/><br/>" + "Please fix this issue and restart DigitalScratch.");
    msg_box.setStandardButtons(QMessageBox::Close);
    msg_box.setIcon(QMessageBox::Critical);
    msg_box.setStyleSheet(this->get_stylesheet_css());
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

    qDebug() << "Gui::show_error_window done.";

    return true;
}

QString
Gui::get_stylesheet_css()
{
    QString result = "";

    if (this->window_style == QString(GUI_STYLE_DARK))
    {
        result = Utils::file_read_all_text(GUI_STYLE_DARK_CSS);
    }

    return result;
}

bool
Gui::create_main_window()
{
    qDebug() << "Gui::create_main_window...";

    ////////////////////////////////////////////////////////////////////////////
    // Configuration + logo.
    ////////////////////////////////////////////////////////////////////////////

    // Create configuration button.
    QPushButton *config_button = new QPushButton("   " + tr("&Settings"));
    config_button->setToolTip(tr("Change application settings..."));
    config_button->setObjectName("Configuration_button");
    config_button->setFocusPolicy(Qt::NoFocus);


    // Create button to set full screen.
    QPushButton *fullscreen_button = new QPushButton("   " + tr("&Full-screen"));
    fullscreen_button->setToolTip(tr("Toggle fullscreen mode"));
    fullscreen_button->setObjectName("Fullscreen_button");
    fullscreen_button->setFocusPolicy(Qt::NoFocus);

    // Create DigitalScratch logo.
    QPushButton *logo = new QPushButton();
    logo->setToolTip(tr("About DigitalScratch..."));
    logo->setObjectName("Logo");
    logo->setIcon(QIcon(LOGO));
    logo->setIconSize(QSize(112, 35));
    logo->setMaximumWidth(112);
    logo->setMaximumHeight(35);
    logo->setFlat(true);
    logo->setFocusPolicy(Qt::NoFocus);

    // Create help button.
    QPushButton *help_button = new QPushButton("   " + tr("&Help"));
    help_button->setToolTip(tr("Show/hide keyboard shortcuts"));
    help_button->setObjectName("Help_button");
    help_button->setFocusPolicy(Qt::NoFocus);

    // Create quit button.
    QPushButton *quit_button = new QPushButton("   " + tr("&Exit"));
    quit_button->setToolTip(tr("Exit DigitalScratch"));
    quit_button->setObjectName("Quit_button");
    quit_button->setFocusPolicy(Qt::NoFocus);

    // Create top horizontal layout.
    QHBoxLayout *top_layout = new QHBoxLayout();

    // Put configuration button and logo in configuration layout.
    top_layout->addWidget(config_button,     1,   Qt::AlignLeft);
    top_layout->addWidget(fullscreen_button, 1,   Qt::AlignLeft);
    top_layout->addWidget(logo,              100, Qt::AlignCenter);
    top_layout->addWidget(help_button,       1,   Qt::AlignRight);
    top_layout->addWidget(quit_button,       1,   Qt::AlignRight);

    ////////////////////////////////////////////////////////////////////////////
    // Decks.
    ////////////////////////////////////////////////////////////////////////////

    // Create track name, key, position and timecode infos.
    this->deck1_track_name = new QLabel();
    this->deck1_key        = new QLabel();
    this->deck1_waveform   = new Waveform(this->at_1, this->window);
    this->deck1_key->setObjectName("KeyValue");

    // TODO: work on a vertical waveform.
    //this->deck1_vertical_waveform       = new Vertical_waveform(this->at_1);
    //this->deck1_scene = new QGraphicsScene();
    //this->deck1_scene->setSceneRect(0, 0, 100, 100);
    //this->deck1_scene->addWidget(this->deck1_vertical_waveform);
    //this->deck1_scene->addWidget(this->deck1_track_name);
    //this->deck1_scene->addRect(QRectF(0, 0, 50, 50));
    //this->deck1_scene->addText("TEST");
    //this->deck1_view = new QGraphicsView(this->deck1_scene);

    QLabel *deck1_tcode_speed           = new QLabel(tr("Speed (%): "));
    QLabel *deck1_tcode_speed_value     = new QLabel(tr("000.0"));
    deck1_tcode_speed->setObjectName("SpeedLabel");
    deck1_tcode_speed_value->setObjectName("SpeedValue");
    // TODO: work on position detection
    //QLabel *deck1_tcode_position        = new QLabel(tr("Position (ms): "));
    //QLabel *deck1_tcode_position_value  = new QLabel(tr("000.000"));
    QLabel *deck1_tcode_amplitude       = new QLabel(tr("Volume (%): "));
    QLabel *deck1_tcode_amplitude_value = new QLabel(tr("000.0"));
    deck1_tcode_amplitude->setObjectName("VolumeLabel");
    deck1_tcode_amplitude_value->setObjectName("VolumeValue");
    QHBoxLayout *decks1_tcode_infos = new QHBoxLayout;
    decks1_tcode_infos->addWidget(deck1_tcode_speed, 1);
    decks1_tcode_infos->addWidget(deck1_tcode_speed_value, 10);
    // TODO: work on position detection
    //decks1_tcode_infos->addWidget(deck1_tcode_position, 1);
    //decks1_tcode_infos->addWidget(deck1_tcode_position_value, 10);
    decks1_tcode_infos->addWidget(deck1_tcode_amplitude, 1);
    decks1_tcode_infos->addWidget(deck1_tcode_amplitude_value, 10);
    QHBoxLayout *deck1_remaining_time_layout = new QHBoxLayout;
    deck1_remaining_time_layout->addWidget(this->decks_remaining_time[0]->minus, 1,   Qt::AlignBottom);
    deck1_remaining_time_layout->addWidget(this->decks_remaining_time[0]->min,   1,   Qt::AlignBottom);
    deck1_remaining_time_layout->addWidget(this->decks_remaining_time[0]->sep1,  1,   Qt::AlignBottom);
    deck1_remaining_time_layout->addWidget(this->decks_remaining_time[0]->sec,   1,   Qt::AlignBottom);
    deck1_remaining_time_layout->addWidget(this->decks_remaining_time[0]->sep2,  1,   Qt::AlignBottom);
    deck1_remaining_time_layout->addWidget(this->decks_remaining_time[0]->msec,  1,   Qt::AlignBottom);
    deck1_remaining_time_layout->addWidget(this->deck1_key,                      100, Qt::AlignRight);

    this->deck1_track_name->setObjectName("TrackName");
    this->deck1_waveform->setObjectName("Waveform");


    this->deck2_track_name = new QLabel();
    this->deck2_key        = new QLabel();
    this->deck2_waveform   = new Waveform(this->at_2, this->window);
    this->deck2_key->setObjectName("KeyValue");

    // TODO: work on a vertical waveform.
    //this->deck2_vertical_waveform       = new Vertical_waveform(this->at_2, this->window);

    QLabel *deck2_tcode_speed           = new QLabel(tr("Speed (%): "));
    QLabel *deck2_tcode_speed_value     = new QLabel(tr("000.0"));
    deck2_tcode_speed->setObjectName("SpeedLabel");
    deck2_tcode_speed_value->setObjectName("SpeedValue");
    // TODO: work on position detection
    //QLabel *deck2_tcode_position        = new QLabel(tr("Position (ms): "));
    //QLabel *deck2_tcode_position_value  = new QLabel(tr("000.000"));
    QLabel *deck2_tcode_amplitude       = new QLabel(tr("Volume (%): "));
    QLabel *deck2_tcode_amplitude_value = new QLabel(tr("000.0"));
    deck2_tcode_amplitude->setObjectName("VolumeLabel");
    deck2_tcode_amplitude_value->setObjectName("VolumeValue");
    QHBoxLayout *decks2_tcode_infos = new QHBoxLayout;
    decks2_tcode_infos->addWidget(deck2_tcode_speed, 1);
    decks2_tcode_infos->addWidget(deck2_tcode_speed_value, 10);
    // TODO: work on position detection
    //decks2_tcode_infos->addWidget(deck2_tcode_position, 1);
    //decks2_tcode_infos->addWidget(deck2_tcode_position_value, 10);
    decks2_tcode_infos->addWidget(deck2_tcode_amplitude, 1);
    decks2_tcode_infos->addWidget(deck2_tcode_amplitude_value, 10);
    QHBoxLayout *deck2_remaining_time_layout = new QHBoxLayout;
    deck2_remaining_time_layout->addWidget(this->decks_remaining_time[1]->minus, 1,   Qt::AlignBottom);
    deck2_remaining_time_layout->addWidget(this->decks_remaining_time[1]->min,   1,   Qt::AlignBottom);
    deck2_remaining_time_layout->addWidget(this->decks_remaining_time[1]->sep1,  1,   Qt::AlignBottom);
    deck2_remaining_time_layout->addWidget(this->decks_remaining_time[1]->sec,   1,   Qt::AlignBottom);
    deck2_remaining_time_layout->addWidget(this->decks_remaining_time[1]->sep2,  1,   Qt::AlignBottom);
    deck2_remaining_time_layout->addWidget(this->decks_remaining_time[1]->msec,  1,   Qt::AlignBottom);
    deck2_remaining_time_layout->addWidget(this->deck2_key,                      100, Qt::AlignRight);

    this->deck2_track_name->setObjectName("TrackName");
    this->deck2_waveform->setObjectName("Waveform");

    // Create horizontal and vertical layout for each deck.
    QHBoxLayout *deck1_general_layout = new QHBoxLayout();
    QHBoxLayout *deck2_general_layout = new QHBoxLayout();
    QVBoxLayout *deck1_layout = new QVBoxLayout();
    QVBoxLayout *deck2_layout = new QVBoxLayout();

    // Put track name, position and timecode info in decks layout.
    deck1_layout->addWidget(deck1_track_name, 10);
    deck1_layout->addLayout(deck1_remaining_time_layout, 10);
    deck1_layout->addWidget(this->deck1_waveform, 70);
    deck1_layout->addLayout(decks1_tcode_infos, 10);
    deck1_general_layout->addLayout(deck1_layout, 90);
    // TODO: work on a vertical waveform.
    //deck1_general_layout->addWidget(this->deck1_view, 10);

    deck2_layout->addWidget(deck2_track_name, 10);
    deck2_layout->addLayout(deck2_remaining_time_layout, 10);
    deck2_layout->addWidget(this->deck2_waveform, 70);
    deck2_layout->addLayout(decks2_tcode_infos, 10);
    deck2_general_layout->addLayout(deck2_layout, 90);
    // TODO: work on a vertical waveform.
    //deck2_general_layout->addWidget(this->deck2_vertical_waveform, 10);

    // Create deck group boxes.
    this->deck1_gbox = new PlaybackQGroupBox(tr("Deck 1"));
    this->deck1_gbox->setObjectName("DeckGBox");
    this->deck2_gbox = new PlaybackQGroupBox(tr("Deck 2"));
    this->deck2_gbox->setObjectName("DeckGBox");

    // Put horizontal layouts in group boxes.
    this->deck1_gbox->setLayout(deck1_general_layout);
    this->deck2_gbox->setLayout(deck2_general_layout);

    // Create horizontal layout for 2 decks.
    QHBoxLayout *decks_layout = new QHBoxLayout;

    // Put decks in layout.
    decks_layout->addWidget(this->deck1_gbox);
    if (this->nb_decks > 1)
    {
        decks_layout->addWidget(this->deck2_gbox);
    }

    ////////////////////////////////////////////////////////////////////////////
    // Samplers.
    ////////////////////////////////////////////////////////////////////////////

    // Sampler 1 group box
    this->sampler1_buttons_play  = new QPushButton*[this->nb_samplers];
    this->sampler1_buttons_stop  = new QPushButton*[this->nb_samplers];
    this->sampler1_trackname     = new QLabel*[this->nb_samplers];
    this->sampler1_remainingtime = new QLabel*[this->nb_samplers];
    QGridLayout *sampler1_layout = new QGridLayout();
    QString      sampler1_name("A");
    for (int i = 0; i < this->nb_samplers; i++)
    {
        this->sampler1_buttons_play[i] = new QPushButton();
        this->sampler1_buttons_play[i]->setObjectName("Sampler_play_buttons");
        this->sampler1_buttons_play[i]->setIconSize(QSize(12, 12));
        this->sampler1_buttons_play[i]->setMaximumWidth(24);
        this->sampler1_buttons_play[i]->setMaximumHeight(24);
        this->sampler1_buttons_play[i]->setFocusPolicy(Qt::NoFocus);
        this->sampler1_buttons_play[i]->setCheckable(true);
        this->sampler1_buttons_stop[i] = new QPushButton();
        this->sampler1_buttons_stop[i]->setObjectName("Sampler_stop_buttons");
        this->sampler1_buttons_stop[i]->setIconSize(QSize(12, 12));
        this->sampler1_buttons_stop[i]->setMaximumWidth(24);
        this->sampler1_buttons_stop[i]->setMaximumHeight(24);
        this->sampler1_buttons_stop[i]->setFocusPolicy(Qt::NoFocus);
        this->sampler1_buttons_stop[i]->setCheckable(true);
        this->sampler1_buttons_stop[i]->setChecked(true);
        this->sampler1_trackname[i] = new QLabel(tr("--"));
        this->sampler1_remainingtime[i] = new QLabel("- 00");

        QLabel *sampler1_name_label = new QLabel(sampler1_name);
        sampler1_layout->addWidget(sampler1_name_label,             i, 0);
        sampler1_layout->addWidget(this->sampler1_buttons_play[i],  i, 1);
        sampler1_layout->addWidget(this->sampler1_buttons_stop[i],  i, 2);
        sampler1_layout->addWidget(this->sampler1_remainingtime[i], i, 3);
        sampler1_layout->addWidget(this->sampler1_trackname[i],     i, 4);

        sampler1_name[0].unicode()++; // Next sampler letter.
    }
    sampler1_layout->setColumnStretch(0, 1);
    sampler1_layout->setColumnStretch(1, 1);
    sampler1_layout->setColumnStretch(2, 1);
    sampler1_layout->setColumnStretch(3, 4);
    sampler1_layout->setColumnStretch(4, 95);
    this->sampler1_gbox = new PlaybackQGroupBox(tr("Sample player 1"));
    this->sampler1_gbox->setObjectName("SamplerGBox");
    this->sampler1_gbox->setLayout(sampler1_layout);

    // Sampler 2 group box
    this->sampler2_buttons_play  = new QPushButton*[this->nb_samplers];
    this->sampler2_buttons_stop  = new QPushButton*[this->nb_samplers];
    this->sampler2_trackname     = new QLabel*[this->nb_samplers];
    this->sampler2_remainingtime = new QLabel*[this->nb_samplers];
    QGridLayout *sampler2_layout = new QGridLayout();
    QString      sampler2_name("A");
    for (int i = 0; i < this->nb_samplers; i++)
    {
        this->sampler2_buttons_play[i] = new QPushButton();
        this->sampler2_buttons_play[i]->setObjectName("Sampler_play_buttons");
        this->sampler2_buttons_play[i]->setIconSize(QSize(12, 12));
        this->sampler2_buttons_play[i]->setMaximumWidth(24);
        this->sampler2_buttons_play[i]->setMaximumHeight(24);
        this->sampler2_buttons_play[i]->setFocusPolicy(Qt::NoFocus);
        this->sampler2_buttons_play[i]->setCheckable(true);
        this->sampler2_buttons_stop[i] = new QPushButton();
        this->sampler2_buttons_stop[i]->setObjectName("Sampler_stop_buttons");
        this->sampler2_buttons_stop[i]->setIconSize(QSize(12, 12));
        this->sampler2_buttons_stop[i]->setMaximumWidth(24);
        this->sampler2_buttons_stop[i]->setMaximumHeight(24);
        this->sampler2_buttons_stop[i]->setFocusPolicy(Qt::NoFocus);
        this->sampler2_buttons_stop[i]->setCheckable(true);
        this->sampler2_buttons_stop[i]->setChecked(true);
        this->sampler2_trackname[i] = new QLabel(tr("--"));
        this->sampler2_remainingtime[i] = new QLabel("- 00");

        QLabel *sampler2_name_label = new QLabel(sampler2_name);
        sampler2_layout->addWidget(sampler2_name_label,             i, 0);
        sampler2_layout->addWidget(this->sampler2_buttons_play[i],  i, 1);
        sampler2_layout->addWidget(this->sampler2_buttons_stop[i],  i, 2);
        sampler2_layout->addWidget(this->sampler2_remainingtime[i], i, 3);
        sampler2_layout->addWidget(this->sampler2_trackname[i],     i, 4);

        sampler2_name[0].unicode()++; // Next sampler letter.
    }
    sampler2_layout->setColumnStretch(0, 1);
    sampler2_layout->setColumnStretch(1, 1);
    sampler2_layout->setColumnStretch(2, 1);
    sampler2_layout->setColumnStretch(3, 4);
    sampler2_layout->setColumnStretch(4, 95);
    this->sampler2_gbox = new PlaybackQGroupBox(tr("Sample player 2"));
    this->sampler2_gbox->setObjectName("SamplerGBox");
    this->sampler2_gbox->setLayout(sampler2_layout);

    // Samplers layout.
    QHBoxLayout *sampler_layout = new QHBoxLayout();
    sampler_layout->addWidget(this->sampler1_gbox);
    if (this->nb_decks > 1)
    {
        sampler_layout->addWidget(this->sampler2_gbox);
    }

    ////////////////////////////////////////////////////////////////////////////
    // Deck and sampler selection.
    ////////////////////////////////////////////////////////////////////////////

    // Connect keyboard shortcut to switch selection of decks/samplers.
    QObject::connect(this->shortcut_switch_playback, SIGNAL(activated()), this, SLOT(switch_playback_selection()));

    // Create mouse action to select deck/sampler.
    QObject::connect(this->deck1_gbox, SIGNAL(selected()), this, SLOT(select_playback_1()));
    QObject::connect(this->deck2_gbox, SIGNAL(selected()), this, SLOT(select_playback_2()));
    QObject::connect(this->sampler1_gbox, SIGNAL(selected()), this, SLOT(select_playback_1()));
    QObject::connect(this->sampler2_gbox, SIGNAL(selected()), this, SLOT(select_playback_2()));

    // Create mouse action when deck/sampler is hovered (mouse entering area).
    QSignalMapper *deck_enter_signal_mapper = new QSignalMapper(this);
    deck_enter_signal_mapper->setMapping(this->deck1_gbox, 0);
    QObject::connect(this->deck1_gbox, SIGNAL(hover()), deck_enter_signal_mapper, SLOT(map()));
    deck_enter_signal_mapper->setMapping(this->deck2_gbox, 1);
    QObject::connect(this->deck2_gbox, SIGNAL(hover()), deck_enter_signal_mapper, SLOT(map()));
    QObject::connect(deck_enter_signal_mapper, SIGNAL(mapped(int)), this, SLOT(hover_playback(int)));

    QSignalMapper *sampler_enter_signal_mapper = new QSignalMapper(this);
    sampler_enter_signal_mapper->setMapping(this->sampler1_gbox, 0);
    QObject::connect(this->sampler1_gbox, SIGNAL(hover()), sampler_enter_signal_mapper, SLOT(map()));
    sampler_enter_signal_mapper->setMapping(this->sampler2_gbox, 1);
    QObject::connect(this->sampler2_gbox, SIGNAL(hover()), sampler_enter_signal_mapper, SLOT(map()));
    QObject::connect(sampler_enter_signal_mapper, SIGNAL(mapped(int)), this, SLOT(hover_playback(int)));

    // Create mouse action when deck/sampler is unhovered (mouse leaving area).
    QSignalMapper *deck_leave_signal_mapper = new QSignalMapper(this);
    deck_leave_signal_mapper->setMapping(this->deck1_gbox, 0);
    QObject::connect(this->deck1_gbox, SIGNAL(unhover()), deck_leave_signal_mapper, SLOT(map()));
    deck_leave_signal_mapper->setMapping(this->deck2_gbox, 1);
    QObject::connect(this->deck2_gbox, SIGNAL(unhover()), deck_leave_signal_mapper, SLOT(map()));
    QObject::connect(deck_leave_signal_mapper, SIGNAL(mapped(int)), this, SLOT(unhover_playback(int)));

    QSignalMapper *sampler_leave_signal_mapper = new QSignalMapper(this);
    sampler_leave_signal_mapper->setMapping(this->sampler1_gbox, 0);
    QObject::connect(this->sampler1_gbox, SIGNAL(unhover()), sampler_leave_signal_mapper, SLOT(map()));
    sampler_leave_signal_mapper->setMapping(this->sampler2_gbox, 1);
    QObject::connect(this->sampler2_gbox, SIGNAL(unhover()), sampler_leave_signal_mapper, SLOT(map()));
    QObject::connect(sampler_leave_signal_mapper, SIGNAL(mapped(int)), this, SLOT(unhover_playback(int)));

    // Preselect deck and sampler 1.
    this->deck1_gbox->setProperty("selected", true);
    this->sampler1_gbox->setProperty("selected", true);


    ////////////////////////////////////////////////////////////////////////////
    // File browser.
    ////////////////////////////////////////////////////////////////////////////

    // Customize file browser display.
    this->file_browser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Resize column with file name when expanding/collapsing a directory.
    QObject::connect(this->file_browser, SIGNAL(expanded(QModelIndex)),  this, SLOT(on_file_browser_expand(QModelIndex)));
    QObject::connect(this->file_browser, SIGNAL(collapsed(QModelIndex)), this, SLOT(on_file_browser_expand(QModelIndex)));

    // Connect the keyboard shortcut that collapse tree.
    QObject::connect(this->shortcut_collapse_browser, SIGNAL(activated()), this->file_browser, SLOT(collapseAll()));

    // Connect the keyboard shortcut to start decoding process on selected file.
    QObject::connect(this->shortcut_load_audio_file, SIGNAL(activated()), this, SLOT(run_audio_file_decoding_process()));

    // Connect keyboard shortcuts to start decoding for the sampler.
    QObject::connect(this->shortcut_load_sample_file_1, SIGNAL(activated()), this, SLOT(run_sample_1_decoding_process()));
    QObject::connect(this->shortcut_load_sample_file_2, SIGNAL(activated()), this, SLOT(run_sample_2_decoding_process()));
    QObject::connect(this->shortcut_load_sample_file_3, SIGNAL(activated()), this, SLOT(run_sample_3_decoding_process()));
    QObject::connect(this->shortcut_load_sample_file_4, SIGNAL(activated()), this, SLOT(run_sample_4_decoding_process()));

    // Connect the keyboard shortcut to show next audio file according to current music key.
    QObject::connect(this->shortcut_show_next_keys, SIGNAL(activated()), this, SLOT(show_next_keys()));

    // Connect thread states for audio collection read and write to DB.
    QObject::connect(this->file_system_model->concurrent_watcher_read,  SIGNAL(finished()), this, SLOT(sync_file_browser_to_audio_collection()));
    QObject::connect(this->file_system_model->concurrent_watcher_store, SIGNAL(finished()), this, SLOT(on_finished_analyze_audio_collection()));

    // Create function buttons for file browser.
    this->refresh_file_browser = new QPushButton();
    this->refresh_file_browser->setObjectName("Refresh_browser_button");
    this->refresh_file_browser->setToolTip(tr("Analyze audio collection (get musical key)"));
    this->refresh_file_browser->setFixedSize(24, 24);
    this->refresh_file_browser->setFocusPolicy(Qt::NoFocus);
    this->refresh_file_browser->setCheckable(true);
    QObject::connect(this->refresh_file_browser, SIGNAL(clicked()), this, SLOT(show_refresh_audio_collection_dialog()));

    this->load_track_on_deck1_button = new QPushButton();
    this->load_track_on_deck1_button->setObjectName("Load_track_button_1");
    this->load_track_on_deck1_button->setToolTip(tr("Load selected track to deck 1"));
    this->load_track_on_deck1_button->setFixedSize(24, 24);
    this->load_track_on_deck1_button->setFocusPolicy(Qt::NoFocus);
    this->load_track_on_deck1_button->setCheckable(true);
    QObject::connect(this->load_track_on_deck1_button, SIGNAL(clicked()), this, SLOT(select_and_run_audio_file_decoding_process_deck1()));

    this->show_next_key_from_deck1_button = new QPushButton();
    this->show_next_key_from_deck1_button->setObjectName("Show_next_key_button");
    this->show_next_key_from_deck1_button->setToolTip(tr("Show deck 1 next potential tracks"));
    this->show_next_key_from_deck1_button->setFixedSize(24, 24);
    this->show_next_key_from_deck1_button->setFocusPolicy(Qt::NoFocus);
    this->show_next_key_from_deck1_button->setCheckable(true);
    QObject::connect(this->show_next_key_from_deck1_button, SIGNAL(clicked()), this, SLOT(select_and_show_next_keys_deck1()));

    this->load_sample1_1_button = new QPushButton();
    this->load_sample1_1_button->setObjectName("Load_track_sample_button_a");
    this->load_sample1_1_button->setToolTip(tr("Load selected track to sample A"));
    this->load_sample1_1_button->setFixedSize(20, 20);
    this->load_sample1_1_button->setFocusPolicy(Qt::NoFocus);
    this->load_sample1_1_button->setCheckable(true);
    QObject::connect(this->load_sample1_1_button, SIGNAL(clicked()), this, SLOT(select_and_run_sample1_decoding_process_deck1()));

    this->load_sample1_2_button = new QPushButton();
    this->load_sample1_2_button->setObjectName("Load_track_sample_button_b");
    this->load_sample1_2_button->setToolTip(tr("Load selected track to sample B"));
    this->load_sample1_2_button->setFixedSize(20, 20);
    this->load_sample1_2_button->setFocusPolicy(Qt::NoFocus);
    this->load_sample1_2_button->setCheckable(true);
    QObject::connect(this->load_sample1_2_button, SIGNAL(clicked()), this, SLOT(select_and_run_sample2_decoding_process_deck1()));

    this->load_sample1_3_button = new QPushButton();
    this->load_sample1_3_button->setObjectName("Load_track_sample_button_c");
    this->load_sample1_3_button->setToolTip(tr("Load selected track to sample C"));
    this->load_sample1_3_button->setFixedSize(20, 20);
    this->load_sample1_3_button->setFocusPolicy(Qt::NoFocus);
    this->load_sample1_3_button->setCheckable(true);
    QObject::connect(this->load_sample1_3_button, SIGNAL(clicked()), this, SLOT(select_and_run_sample3_decoding_process_deck1()));

    this->load_sample1_4_button = new QPushButton();
    this->load_sample1_4_button->setObjectName("Load_track_sample_button_d");
    this->load_sample1_4_button->setToolTip(tr("Load selected track to sample D"));
    this->load_sample1_4_button->setFixedSize(20, 20);
    this->load_sample1_4_button->setFocusPolicy(Qt::NoFocus);
    this->load_sample1_4_button->setCheckable(true);
    QObject::connect(this->load_sample1_4_button, SIGNAL(clicked()), this, SLOT(select_and_run_sample4_decoding_process_deck1()));

    if (this->nb_decks > 1)
    {
        this->load_sample2_1_button = new QPushButton();
        this->load_sample2_1_button->setObjectName("Load_track_sample_button_a");
        this->load_sample2_1_button->setToolTip(tr("Load selected track to sample A"));
        this->load_sample2_1_button->setFixedSize(20, 20);
        this->load_sample2_1_button->setFocusPolicy(Qt::NoFocus);
        this->load_sample2_1_button->setCheckable(true);
        QObject::connect(this->load_sample2_1_button, SIGNAL(clicked()), this, SLOT(select_and_run_sample1_decoding_process_deck2()));

        this->show_next_key_from_deck2_button = new QPushButton();
        this->show_next_key_from_deck2_button->setObjectName("Show_next_key_button");
        this->show_next_key_from_deck2_button->setToolTip(tr("Show deck 2 next potential tracks"));
        this->show_next_key_from_deck2_button->setFixedSize(24, 24);
        this->show_next_key_from_deck2_button->setFocusPolicy(Qt::NoFocus);
        this->show_next_key_from_deck2_button->setCheckable(true);
        QObject::connect(this->show_next_key_from_deck2_button, SIGNAL(clicked()), this, SLOT(select_and_show_next_keys_deck2()));

        this->load_sample2_2_button = new QPushButton();
        this->load_sample2_2_button->setObjectName("Load_track_sample_button_b");
        this->load_sample2_2_button->setToolTip(tr("Load selected track to sample B"));
        this->load_sample2_2_button->setFixedSize(20, 20);
        this->load_sample2_2_button->setFocusPolicy(Qt::NoFocus);
        this->load_sample2_2_button->setCheckable(true);
        QObject::connect(this->load_sample2_2_button, SIGNAL(clicked()), this, SLOT(select_and_run_sample2_decoding_process_deck2()));

        this->load_sample2_3_button = new QPushButton();
        this->load_sample2_3_button->setObjectName("Load_track_sample_button_c");
        this->load_sample2_3_button->setToolTip(tr("Load selected track to sample C"));
        this->load_sample2_3_button->setFixedSize(20, 20);
        this->load_sample2_3_button->setFocusPolicy(Qt::NoFocus);
        this->load_sample2_3_button->setCheckable(true);
        QObject::connect(this->load_sample2_3_button, SIGNAL(clicked()), this, SLOT(select_and_run_sample3_decoding_process_deck2()));

        this->load_sample2_4_button = new QPushButton();
        this->load_sample2_4_button->setObjectName("Load_track_sample_button_d");
        this->load_sample2_4_button->setToolTip(tr("Load selected track to sample D"));
        this->load_sample2_4_button->setFixedSize(20, 20);
        this->load_sample2_4_button->setFocusPolicy(Qt::NoFocus);
        this->load_sample2_4_button->setCheckable(true);
        QObject::connect(this->load_sample2_4_button, SIGNAL(clicked()), this, SLOT(select_and_run_sample4_decoding_process_deck2()));

        this->load_track_on_deck2_button = new QPushButton();
        this->load_track_on_deck2_button->setObjectName("Load_track_button_2");
        this->load_track_on_deck2_button->setToolTip(tr("Load selected track to deck 2"));
        this->load_track_on_deck2_button->setFixedSize(24, 24);
        this->load_track_on_deck2_button->setFocusPolicy(Qt::NoFocus);
        this->load_track_on_deck2_button->setCheckable(true);
        QObject::connect(this->load_track_on_deck2_button, SIGNAL(clicked()), this, SLOT(select_and_run_audio_file_decoding_process_deck2()));
    }

    // Create progress bar for the refresh button.
    this->refresh_file_browser_progress= new QProgressBar();
    this->refresh_file_browser_progress->hide();
    this->refresh_file_browser_progress->setFixedSize(20, 4);
    this->refresh_file_browser_progress->setTextVisible(false);
    QObject::connect(this->file_system_model->concurrent_watcher_store, SIGNAL(progressRangeChanged(int,int)),
                     this->refresh_file_browser_progress,               SLOT(setRange(int,int)));
    QObject::connect(this->file_system_model->concurrent_watcher_store, SIGNAL(progressValueChanged(int)),
                     this,                                              SLOT(update_refresh_progress_value(int)));

    QWidget *file_browser_buttons_widget = new QWidget();
    QGridLayout *file_browser_buttons_layout         = new QGridLayout(file_browser_buttons_widget);
    QHBoxLayout *file_browser_sample1_buttons_layout = new QHBoxLayout();
    QHBoxLayout *file_browser_sample2_buttons_layout = new QHBoxLayout();
    file_browser_sample1_buttons_layout->addWidget(this->load_sample1_1_button);
    file_browser_sample1_buttons_layout->addWidget(this->load_sample1_2_button);
    file_browser_sample1_buttons_layout->addWidget(this->load_sample1_3_button);
    file_browser_sample1_buttons_layout->addWidget(this->load_sample1_4_button);
    file_browser_sample2_buttons_layout->addWidget(this->load_sample2_1_button);
    file_browser_sample2_buttons_layout->addWidget(this->load_sample2_2_button);
    file_browser_sample2_buttons_layout->addWidget(this->load_sample2_3_button);
    file_browser_sample2_buttons_layout->addWidget(this->load_sample2_4_button);
    file_browser_buttons_layout->addWidget(this->load_track_on_deck1_button,     0, 0, Qt::AlignLeft);
    file_browser_buttons_layout->addWidget(this->show_next_key_from_deck1_button,0, 1, Qt::AlignLeft);
    file_browser_buttons_layout->addLayout(file_browser_sample1_buttons_layout,  0, 2, Qt::AlignRight);
    file_browser_buttons_layout->addWidget(this->refresh_file_browser,           0, 3, Qt::AlignCenter);
    file_browser_buttons_layout->addWidget(this->refresh_file_browser_progress,  1, 3, Qt::AlignCenter);
    file_browser_buttons_layout->addLayout(file_browser_sample2_buttons_layout,  0, 4, Qt::AlignRight);
    file_browser_buttons_layout->addWidget(this->show_next_key_from_deck2_button,0, 5, Qt::AlignRight);
    file_browser_buttons_layout->addWidget(this->load_track_on_deck2_button,     0, 6, Qt::AlignRight);
    file_browser_buttons_layout->setColumnStretch(0, 1);
    file_browser_buttons_layout->setColumnStretch(1, 100);
    file_browser_buttons_layout->setColumnStretch(2, 1);
    file_browser_buttons_layout->setColumnStretch(3, 10);
    file_browser_buttons_layout->setColumnStretch(4, 1);
    file_browser_buttons_layout->setColumnStretch(5, 100);
    file_browser_buttons_layout->setColumnStretch(6, 1);
    file_browser_buttons_widget->setFixedHeight(37);

    // Create layout and group box for file browser.
    QVBoxLayout *file_browser_layout = new QVBoxLayout();
    file_browser_layout->addWidget(file_browser_buttons_widget);
    file_browser_layout->addWidget(this->file_browser);
    this->set_file_browser_title();
    this->file_browser_gbox->setLayout(file_browser_layout);


    ////////////////////////////////////////////////////////////////////////////
    // Playlist.
    ////////////////////////////////////////////////////////////////////////////

    // TODO implement playlist
    //QGroupBox *playlist_gbox = new QGroupBox(tr("Playlist"));
    //playlist_gbox->setLayout(playlist_layout);


    ////////////////////////////////////////////////////////////////////////////
    // General file layout.
    ////////////////////////////////////////////////////////////////////////////

    QHBoxLayout *file_layout = new QHBoxLayout();
    file_layout->addWidget(this->file_browser_gbox, 50);
    // TODO implement playlist
    // file_layout->addWidget(playlist_gbox, 50);


    ////////////////////////////////////////////////////////////////////////////
    // Help.
    ////////////////////////////////////////////////////////////////////////////

    // Create help labels and pixmaps.
    QLabel *help_switch_deck_lb    = new QLabel(tr("Switch playback"));
    QLabel *help_switch_deck_value = new QLabel(this->settings->get_keyboard_shortcut(KB_SWITCH_PLAYBACK));
    QLabel *help_deck_lb           = new QLabel(tr("Load/Restart on deck"));
    QLabel *help_deck_value        = new QLabel(this->settings->get_keyboard_shortcut(KB_LOAD_TRACK_ON_DECK)
                                                + "/"
                                                + this->settings->get_keyboard_shortcut(KB_PLAY_BEGIN_TRACK_ON_DECK));
    QLabel *help_cue_lb            = new QLabel(tr("Set/Play cue point on deck"));
    QLabel *help_cue_value         = new QLabel(this->settings->get_keyboard_shortcut(KB_SET_CUE_POINT_ON_DECK)
                                                + "/"
                                                + this->settings->get_keyboard_shortcut(KB_PLAY_CUE_POINT_ON_DECK));
    QLabel *help_sample_lb         = new QLabel(tr("Load on sampler 1/2/3/4"));
    QLabel *help_sample_value      = new QLabel(this->settings->get_keyboard_shortcut(KB_LOAD_TRACK_ON_SAMPLER1)
                                                + "/"
                                                + this->settings->get_keyboard_shortcut(KB_LOAD_TRACK_ON_SAMPLER2)
                                                + "/"
                                                + this->settings->get_keyboard_shortcut(KB_LOAD_TRACK_ON_SAMPLER3)
                                                + "/"
                                                + this->settings->get_keyboard_shortcut(KB_LOAD_TRACK_ON_SAMPLER4));
    QLabel *help_fullscreen_lb     = new QLabel(tr("Fullscreen"));
    QLabel *help_fullscreen_value  = new QLabel(this->settings->get_keyboard_shortcut(KB_FULLSCREEN));
    QLabel *help_help_lb           = new QLabel(tr("Help"));
    QLabel *help_help_value        = new QLabel(this->settings->get_keyboard_shortcut(KB_HELP));
    QLabel *help_browse_lb1        = new QLabel(tr("Browse files"));
    QLabel *help_browse_value1     = new QLabel("Up/Down/Left/Right");
    QLabel *help_browse_lb2        = new QLabel(tr("Collapse file tree"));
    QLabel *help_browse_value2     = new QLabel(this->settings->get_keyboard_shortcut(KB_COLLAPSE_BROWSER));

    help_switch_deck_lb->setObjectName("Help");
    help_cue_lb->setObjectName("Help");
    help_deck_lb->setObjectName("Help");
    help_sample_lb->setObjectName("Help");
    help_fullscreen_lb->setObjectName("Help");
    help_help_lb->setObjectName("Help");
    help_browse_lb1->setObjectName("Help");
    help_browse_lb2->setObjectName("Help");

    // Setup layout.
    QGridLayout *help_layout = new QGridLayout();
    help_layout->addWidget(help_switch_deck_lb,    0, 0);
    help_layout->addWidget(help_switch_deck_value, 0, 1, Qt::AlignLeft);
    help_layout->addWidget(help_sample_lb,         1, 0);
    help_layout->addWidget(help_sample_value,      1, 1, Qt::AlignLeft);
    help_layout->addWidget(help_deck_lb,           0, 2);
    help_layout->addWidget(help_deck_value,        0, 3, Qt::AlignLeft);
    help_layout->addWidget(help_cue_lb,            1, 2);
    help_layout->addWidget(help_cue_value,         1, 3, Qt::AlignLeft);
    help_layout->addWidget(help_fullscreen_lb,     0, 4);
    help_layout->addWidget(help_fullscreen_value,  0, 5, Qt::AlignLeft);
    help_layout->addWidget(help_help_lb,           1, 4);
    help_layout->addWidget(help_help_value,        1, 5, Qt::AlignLeft);
    help_layout->addWidget(help_browse_lb1,        0, 6);
    help_layout->addWidget(help_browse_value1,     0, 7, Qt::AlignLeft);
    help_layout->addWidget(help_browse_lb2,        1, 6);
    help_layout->addWidget(help_browse_value2,     1, 7, Qt::AlignLeft);

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

    ////////////////////////////////////////////////////////////////////////////
    // Bottom bar.
    ////////////////////////////////////////////////////////////////////////////

    // Create bottom horizontal layout.
    QHBoxLayout *bottom_layout = new QHBoxLayout;

    // Put help group box and configuration in bottom layout.
    bottom_layout->addWidget(this->help_groupbox);

    ////////////////////////////////////////////////////////////////////////////
    // Make connections.
    ////////////////////////////////////////////////////////////////////////////

    // Open configuration window.
    QObject::connect(config_button, SIGNAL(clicked()), this, SLOT(show_config_window()));

    // Set full screen.
    QObject::connect(fullscreen_button,         SIGNAL(clicked()),   this, SLOT(set_fullscreen()));
    QObject::connect(this->shortcut_fullscreen, SIGNAL(activated()), this, SLOT(set_fullscreen()));

    // Open about window.
    QObject::connect(logo, SIGNAL(clicked()), this, SLOT(show_about_window()));

    // Help button.
    QObject::connect(help_button,         SIGNAL(clicked()),   this, SLOT(show_help()));
    QObject::connect(this->shortcut_help, SIGNAL(activated()), this, SLOT(show_help()));

    // Quit application.
    QObject::connect(quit_button, SIGNAL(clicked()), this, SLOT(can_close()));

    // Open error window.
    QObject::connect(this->sound_card, SIGNAL(error_msg(QString)),
                     this,             SLOT(show_error_window(QString)));

    // Remaining time for each deck.
    QObject::connect(this->playback, SIGNAL(remaining_time_changed(unsigned int, int)),
                     this,           SLOT(set_remaining_time(unsigned int, int)));

    // Name of the track for each deck.
    QObject::connect(this->at_1,             SIGNAL(name_changed(QString)),
                     this->deck1_track_name, SLOT(setText(const QString&)));
    QObject::connect(this->at_2,             SIGNAL(name_changed(QString)),
                     this->deck2_track_name, SLOT(setText(const QString&)));

    // Music key of the track for each deck.
    QObject::connect(this->at_1,      SIGNAL(key_changed(QString)),
                     this->deck1_key, SLOT(setText(const QString&)));
    QObject::connect(this->at_2,      SIGNAL(key_changed(QString)),
                     this->deck2_key, SLOT(setText(const QString&)));

    // Name of samplers.
    QObject::connect(this->at_1_samplers[0], SIGNAL(name_changed(QString)),
                     this,                   SLOT(set_sampler_1_1_text(const QString&)));
    QObject::connect(this->at_1_samplers[1], SIGNAL(name_changed(QString)),
                     this,                   SLOT(set_sampler_1_2_text(const QString&)));
    QObject::connect(this->at_1_samplers[2], SIGNAL(name_changed(QString)),
                     this,                   SLOT(set_sampler_1_3_text(const QString&)));
    QObject::connect(this->at_1_samplers[3], SIGNAL(name_changed(QString)),
                     this,                   SLOT(set_sampler_1_4_text(const QString&)));

    QObject::connect(this->at_2_samplers[0], SIGNAL(name_changed(QString)),
                     this,                   SLOT(set_sampler_2_1_text(const QString&)));
    QObject::connect(this->at_2_samplers[1], SIGNAL(name_changed(QString)),
                     this,                   SLOT(set_sampler_2_2_text(const QString&)));
    QObject::connect(this->at_2_samplers[2], SIGNAL(name_changed(QString)),
                     this,                   SLOT(set_sampler_2_3_text(const QString&)));
    QObject::connect(this->at_2_samplers[3], SIGNAL(name_changed(QString)),
                     this,                   SLOT(set_sampler_2_4_text(const QString&)));

    // Start stop samplers.
    QObject::connect(sampler1_buttons_play[0], SIGNAL(clicked()), this, SLOT(on_sampler_button_1_1_play_click()));
    QObject::connect(sampler1_buttons_play[1], SIGNAL(clicked()), this, SLOT(on_sampler_button_1_2_play_click()));
    QObject::connect(sampler1_buttons_play[2], SIGNAL(clicked()), this, SLOT(on_sampler_button_1_3_play_click()));
    QObject::connect(sampler1_buttons_play[3], SIGNAL(clicked()), this, SLOT(on_sampler_button_1_4_play_click()));

    QObject::connect(sampler2_buttons_play[0], SIGNAL(clicked()), this, SLOT(on_sampler_button_2_1_play_click()));
    QObject::connect(sampler2_buttons_play[1], SIGNAL(clicked()), this, SLOT(on_sampler_button_2_2_play_click()));
    QObject::connect(sampler2_buttons_play[2], SIGNAL(clicked()), this, SLOT(on_sampler_button_2_3_play_click()));
    QObject::connect(sampler2_buttons_play[3], SIGNAL(clicked()), this, SLOT(on_sampler_button_2_4_play_click()));

    QObject::connect(sampler1_buttons_stop[0], SIGNAL(clicked()), this, SLOT(on_sampler_button_1_1_stop_click()));
    QObject::connect(sampler1_buttons_stop[1], SIGNAL(clicked()), this, SLOT(on_sampler_button_1_2_stop_click()));
    QObject::connect(sampler1_buttons_stop[2], SIGNAL(clicked()), this, SLOT(on_sampler_button_1_3_stop_click()));
    QObject::connect(sampler1_buttons_stop[3], SIGNAL(clicked()), this, SLOT(on_sampler_button_1_4_stop_click()));

    QObject::connect(sampler2_buttons_stop[0], SIGNAL(clicked()), this, SLOT(on_sampler_button_2_1_stop_click()));
    QObject::connect(sampler2_buttons_stop[1], SIGNAL(clicked()), this, SLOT(on_sampler_button_2_2_stop_click()));
    QObject::connect(sampler2_buttons_stop[2], SIGNAL(clicked()), this, SLOT(on_sampler_button_2_3_stop_click()));
    QObject::connect(sampler2_buttons_stop[3], SIGNAL(clicked()), this, SLOT(on_sampler_button_2_4_stop_click()));

    // Remaining time for samplers.
    QObject::connect(this->playback, SIGNAL(sampler_remaining_time_changed(unsigned int, int, int)),
                     this,           SLOT(set_sampler_remaining_time(unsigned int, int, int)));

    // State for samplers.
    QObject::connect(this->playback, SIGNAL(sampler_state_changed(int, int, bool)),
                     this,           SLOT(set_sampler_state(int, int, bool)));

    // Timecode informations (speed + position + volume), for each deck.
    QObject::connect(this->params_1,          SIGNAL(speed_changed(double)),
                     deck1_tcode_speed_value, SLOT(setNum(double)));
    QObject::connect(this->params_2,          SIGNAL(speed_changed(double)),
                     deck2_tcode_speed_value, SLOT(setNum(double)));
    // TODO: work on position detection
    //QObject::connect(this->params_1,             SIGNAL(position_changed(double)),
    //                 deck1_tcode_position_value, SLOT(setNum(double)));
    //QObject::connect(this->params_2,             SIGNAL(position_changed(double)),
    //                 deck2_tcode_position_value, SLOT(setNum(double)));
    QObject::connect(this->params_1,              SIGNAL(volume_changed(double)),
                     deck1_tcode_amplitude_value, SLOT(setNum(double)));
    QObject::connect(this->params_2,              SIGNAL(volume_changed(double)),
                     deck2_tcode_amplitude_value, SLOT(setNum(double)));

    // Move in track when slider is moved on waveform.
    QObject::connect(this->deck1_waveform, SIGNAL(slider_position_changed(float)),
                     this,                 SLOT(deck1_jump_to_position(float)));
    QObject::connect(this->deck2_waveform, SIGNAL(slider_position_changed(float)),
                     this,                 SLOT(deck2_jump_to_position(float)));

    // Keyboard shortcut to go back to the beginning of the track.
    QObject::connect(this->shortcut_go_to_begin, SIGNAL(activated()), this, SLOT(deck_go_to_begin()));

    // Keyboard shortcut to set a cue point.
    QObject::connect(this->shortcut_set_cue_point, SIGNAL(activated()), this, SLOT(deck_set_cue_point()));

    // Keyboard shortcut to play from a cue point.
    QObject::connect(this->shortcut_go_to_cue_point, SIGNAL(activated()), this, SLOT(deck_go_to_cue_point()));


    ////////////////////////////////////////////////////////////////////////////
    // Main window.
    ////////////////////////////////////////////////////////////////////////////

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
    main_layout->addLayout(top_layout,     5);
    main_layout->addLayout(decks_layout,   30);
    main_layout->addLayout(sampler_layout, 5);
    main_layout->addLayout(file_layout,    65);
    main_layout->addLayout(bottom_layout,  0);

    // Display main window.
    this->window->show();

    qDebug() << "Gui::create_main_window done.";

    return true;
}

void
Gui::can_close()
{
    qDebug() << "Gui::can_close...";

    // Show a pop-up asking to confirm to close.
    QMessageBox msg_box;
    msg_box.setWindowTitle("DigitalScratch");
    msg_box.setText(tr("Do you really want to quit DigitalScratch ?"));
    msg_box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msg_box.setIcon(QMessageBox::Question);
    msg_box.setDefaultButton(QMessageBox::Cancel);
    msg_box.setStyleSheet(this->get_stylesheet_css());
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
        this->window->close();
    }

    qDebug() << "Gui::can_close done";
}

bool
Gui::apply_main_window_style()
{
    qDebug() << "Gui::apply_main_window_style...";

    // Apply some GUI settings manually.
    if (this->window_style == QString(GUI_STYLE_DEFAULT))
    {
        // Set manually some standard icons.
        for (int i = 0; i < this->nb_samplers; i++)
        {
            this->sampler1_buttons_play[i]->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaPlay));
            this->sampler2_buttons_play[i]->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaPlay));
            this->sampler1_buttons_stop[i]->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaStop));
            this->sampler2_buttons_stop[i]->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaStop));
        }
        this->refresh_file_browser->setIcon(QApplication::style()->standardIcon(QStyle::SP_BrowserReload));
        this->load_track_on_deck1_button->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowUp));
        this->load_sample1_1_button->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowUp));
        this->load_sample1_2_button->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowUp));
        this->load_sample1_3_button->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowUp));
        this->load_sample1_4_button->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowUp));
        this->load_track_on_deck2_button->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowUp));
        this->load_sample2_1_button->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowUp));
        this->load_sample2_2_button->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowUp));
        this->load_sample2_3_button->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowUp));
        this->load_sample2_4_button->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowUp));
        this->show_next_key_from_deck1_button->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowDown));
        this->show_next_key_from_deck2_button->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowDown));
        this->file_system_model->set_icons((QApplication::style()->standardIcon(QStyle::SP_FileIcon).pixmap(10, 10)),
                                           (QApplication::style()->standardIcon(QStyle::SP_DirIcon).pixmap(10, 10)));
    }
    else
    {
        // Reset some icons (can not be done nicely in CSS).
        for (int i = 0; i < this->nb_samplers; i++)
        {
            this->sampler1_buttons_play[i]->setIcon(QIcon());
            this->sampler2_buttons_play[i]->setIcon(QIcon());
            this->sampler1_buttons_stop[i]->setIcon(QIcon());
            this->sampler2_buttons_stop[i]->setIcon(QIcon());
        }
        this->refresh_file_browser->setIcon(QIcon());
        this->load_track_on_deck1_button->setIcon(QIcon());
        this->load_sample1_1_button->setIcon(QIcon());
        this->load_sample1_2_button->setIcon(QIcon());
        this->load_sample1_3_button->setIcon(QIcon());
        this->load_sample1_4_button->setIcon(QIcon());
        this->load_track_on_deck2_button->setIcon(QIcon());
        this->load_sample2_1_button->setIcon(QIcon());
        this->load_sample2_2_button->setIcon(QIcon());
        this->load_sample2_3_button->setIcon(QIcon());
        this->load_sample2_4_button->setIcon(QIcon());
        this->show_next_key_from_deck1_button->setIcon(QIcon());
        this->show_next_key_from_deck2_button->setIcon(QIcon());

        // Set icon for file browser QTreeview (can not be done nicely in CSS).
        this->file_system_model->set_icons(QPixmap(PIXMAPS_PATH + this->window_style + ICON_AUDIO_FILE_SUFFIX).scaledToWidth(10, Qt::SmoothTransformation),
                                           QPixmap(PIXMAPS_PATH + this->window_style + ICON_FOLDER_SUFFIX).scaledToWidth(10, Qt::SmoothTransformation));
    }

    // Change main window skin (using CSS).
    this->window->setStyleSheet(this->get_stylesheet_css());

    qDebug() << "Gui::apply_main_window_style done.";

    return true;
}

bool
Gui::set_file_browser_base_path(QString in_path)
{
    qDebug() << "Gui::set_file_browser_base_path...";

    // Set base path as title to file browser.
    this->set_file_browser_title();

    // Change root path of file browser.
    this->file_browser->setRootIndex(QModelIndex());
    this->file_system_model->set_root_path(in_path);
    this->file_system_model->concurrent_read_collection_from_db(); // Run in another thread.
                                                                   // Call sync_file_browser_to_audio_collection() when it's done.

    qDebug() << "Gui::set_file_browser_base_path done.";

    return true;
}

void
Gui::sync_file_browser_to_audio_collection()
{
    // Reset file browser root node to audio collection model's root.
    this->file_browser->setRootIndex(this->file_system_model->get_root_index());
    this->resize_file_browser_columns();
}

bool
Gui::set_file_browser_title()
{
    qDebug() << "Gui::set_file_browser_title...";

    // Change file browser title (which contains base dir for tracks).
    this->file_browser_gbox->setTitle(tr("File browser") + " [" + this->settings->get_tracks_base_dir_path() + "]");

    qDebug() << "Gui::set_file_browser_title done.";

    return true;
}

void
Gui::run_sampler_decoding_process(unsigned short int in_deck_index,
                                  unsigned short int in_sampler_index)
{
    qDebug() << "Gui::run_sampler_decoding_process...";

    // Get selected file path.
    Audio_collection_item *item = static_cast<Audio_collection_item*>((this->file_browser->currentIndex()).internalPointer());
    QFileInfo info(item->get_full_path());
    qDebug() << "Gui::run_sampler_decoding_process: selected item: " << info.absoluteFilePath();

    // Execute decoding.
    Audio_file_decoding_process **samplers = NULL;
    if (in_deck_index == 0)
    {
        samplers = this->dec_1_samplers;
    }
    else
    {
        samplers = this->dec_2_samplers;
    }
    if (samplers[in_sampler_index]->run(info.absoluteFilePath(), "") == false)
    {
        qWarning() << "Gui::run_sampler_decoding_process: can not decode " << info.absoluteFilePath();
    }
    else
    {
        this->playback->reset_sampler(in_deck_index, in_sampler_index);
        this->set_sampler_state(in_deck_index, in_sampler_index, false);
        this->playback->set_sampler_state(in_deck_index, in_sampler_index, false);
    }

    qDebug() << "Gui::run_sampler_decoding_process done.";

    return;
}

void
Gui::select_and_run_sample1_decoding_process_deck1()
{
    // Check the button.
    this->load_sample1_1_button->setEnabled(false);
    this->load_sample1_1_button->setChecked(true);

    // Select deck 1.
    this->highlight_deck_sampler_area(0);

    // Decode and play sample audio file on sampler 1.
    this->run_sample_1_decoding_process();

    // Release the button.
    this->load_sample1_1_button->setEnabled(true);
    this->load_sample1_1_button->setChecked(false);
}

void
Gui::select_and_run_sample1_decoding_process_deck2()
{
    // Check the button.
    this->load_sample2_1_button->setEnabled(false);
    this->load_sample2_1_button->setChecked(true);

    // Select deck 2.
    this->highlight_deck_sampler_area(1);

    // Decode and play sample audio file on sampler 1.
    this->run_sample_1_decoding_process();

    // Release the button.
    this->load_sample2_1_button->setEnabled(true);
    this->load_sample2_1_button->setChecked(false);
}

void
Gui::select_and_run_sample2_decoding_process_deck1()
{
    // Check the button.
    this->load_sample1_2_button->setEnabled(false);
    this->load_sample1_2_button->setChecked(true);

    // Select deck 1.
    this->highlight_deck_sampler_area(0);

    // Decode and play sample audio file on sampler 2.
    this->run_sample_2_decoding_process();

    // Release the button.
    this->load_sample1_2_button->setEnabled(true);
    this->load_sample1_2_button->setChecked(false);
}

void
Gui::select_and_run_sample2_decoding_process_deck2()
{
    // Check the button.
    this->load_sample2_2_button->setEnabled(false);
    this->load_sample2_2_button->setChecked(true);

    // Select deck 2.
    this->highlight_deck_sampler_area(1);

    // Decode and play sample audio file on sampler 2.
    this->run_sample_2_decoding_process();

    // Release the button.
    this->load_sample2_2_button->setEnabled(true);
    this->load_sample2_2_button->setChecked(false);
}

void
Gui::select_and_run_sample3_decoding_process_deck1()
{
    // Check the button.
    this->load_sample1_3_button->setEnabled(false);
    this->load_sample1_3_button->setChecked(true);

    // Select deck 1.
    this->highlight_deck_sampler_area(0);

    // Decode and play sample audio file on sampler 3.
    this->run_sample_3_decoding_process();

    // Release the button.
    this->load_sample1_3_button->setEnabled(true);
    this->load_sample1_3_button->setChecked(false);
}

void
Gui::select_and_run_sample3_decoding_process_deck2()
{
    // Check the button.
    this->load_sample2_3_button->setEnabled(false);
    this->load_sample2_3_button->setChecked(true);

    // Select deck 2.
    this->highlight_deck_sampler_area(1);

    // Decode and play sample audio file on sampler 3.
    this->run_sample_3_decoding_process();

    // Release the button.
    this->load_sample2_3_button->setEnabled(true);
    this->load_sample2_3_button->setChecked(false);
}

void
Gui::select_and_run_sample4_decoding_process_deck1()
{
    // Check the button.
    this->load_sample1_4_button->setEnabled(false);
    this->load_sample1_4_button->setChecked(true);

    // Select deck 1.
    this->highlight_deck_sampler_area(0);

    // Decode and play sample audio file on sampler 4.
    this->run_sample_4_decoding_process();

    // Release the button.
    this->load_sample1_4_button->setEnabled(true);
    this->load_sample1_4_button->setChecked(false);
}

void
Gui::select_and_run_sample4_decoding_process_deck2()
{
    // Check the button.
    this->load_sample2_4_button->setEnabled(false);
    this->load_sample2_4_button->setChecked(true);

    // Select deck 2.
    this->highlight_deck_sampler_area(1);

    // Decode and play sample audio file on sampler 4.
    this->run_sample_4_decoding_process();

    // Release the button.
    this->load_sample2_4_button->setEnabled(true);
    this->load_sample2_4_button->setChecked(false);
}

void
Gui::run_sample_1_decoding_process()
{
    if ((this->nb_decks > 1) && (this->deck2_gbox->is_selected() == true))
    {
        this->run_sampler_decoding_process(1, 0);
    }
    else
    {
        this->run_sampler_decoding_process(0, 0);
    }

    return;
}

void
Gui::run_sample_2_decoding_process()
{
    if ((this->nb_decks > 1) && (this->deck2_gbox->is_selected() == true))
    {
        this->run_sampler_decoding_process(1, 1);
    }
    else
    {
        this->run_sampler_decoding_process(0, 1);
    }

    return;
}

void
Gui::run_sample_3_decoding_process()
{
    if ((this->nb_decks > 1) && (this->deck2_gbox->is_selected() == true))
    {
        this->run_sampler_decoding_process(1, 2);
    }
    else
    {
        this->run_sampler_decoding_process(0, 2);
    }

    return;
}

void
Gui::run_sample_4_decoding_process()
{
    if ((this->nb_decks > 1) && (this->deck2_gbox->is_selected() == true))
    {
        this->run_sampler_decoding_process(1, 3);
    }
    else
    {
        this->run_sampler_decoding_process(0, 3);
    }

    return;
}

void
Gui::set_sampler_1_1_text(QString in_text)
{
    this->sampler1_trackname[0]->setText(in_text);
    return;
}

void
Gui::set_sampler_1_2_text(QString in_text)
{
    this->sampler1_trackname[1]->setText(in_text);
    return;
}

void
Gui::set_sampler_1_3_text(QString in_text)
{
    this->sampler1_trackname[2]->setText(in_text);
    return;
}

void
Gui::set_sampler_1_4_text(QString in_text)
{
    this->sampler1_trackname[3]->setText(in_text);
    return;
}

void
Gui::set_sampler_2_1_text(QString in_text)
{
    this->sampler2_trackname[0]->setText(in_text);
    return;
}

void
Gui::set_sampler_2_2_text(QString in_text)
{
    this->sampler2_trackname[1]->setText(in_text);
    return;
}

void
Gui::set_sampler_2_3_text(QString in_text)
{
    this->sampler2_trackname[2]->setText(in_text);
    return;
}

void
Gui::set_sampler_2_4_text(QString in_text)
{
    this->sampler2_trackname[3]->setText(in_text);
    return;
}

void
Gui::on_sampler_button_1_1_play_click()
{
    this->on_sampler_button_play_click(0, 0);
    return;
}

void
Gui::on_sampler_button_1_2_play_click()
{
    this->on_sampler_button_play_click(0, 1);
    return;
}

void
Gui::on_sampler_button_1_3_play_click()
{
    this->on_sampler_button_play_click(0, 2);
    return;
}

void
Gui::on_sampler_button_1_4_play_click()
{
    this->on_sampler_button_play_click(0, 3);
    return;
}

void
Gui::on_sampler_button_2_1_play_click()
{
    this->on_sampler_button_play_click(1, 0);
    return;
}

void
Gui::on_sampler_button_2_2_play_click()
{
    this->on_sampler_button_play_click(1, 1);
    return;
}

void
Gui::on_sampler_button_2_3_play_click()
{
    this->on_sampler_button_play_click(1, 2);
    return;
}

void
Gui::on_sampler_button_2_4_play_click()
{
    this->on_sampler_button_play_click(1, 3);
    return;
}

void
Gui::on_sampler_button_play_click(unsigned short int in_deck_index,
                                  unsigned short int in_sampler_index)
{
    qDebug() << "Gui::on_sampler_button_play_click...";

    // First stop playback (and return to beginning of the song).
    this->set_sampler_state(in_deck_index, in_sampler_index, false);
    this->playback->set_sampler_state(in_deck_index, in_sampler_index, false);

    // Then start playback again.
    this->set_sampler_state(in_deck_index, in_sampler_index, true);
    this->playback->set_sampler_state(in_deck_index, in_sampler_index, true);

    // Select playback area (if not already done).
    this->highlight_deck_sampler_area(in_deck_index);

    qDebug() << "Gui::on_sampler_button_play_click done.";

    return;
}

void
Gui::on_sampler_button_1_1_stop_click()
{
    this->on_sampler_button_stop_click(0, 0);
    return;
}

void
Gui::on_sampler_button_1_2_stop_click()
{
    this->on_sampler_button_stop_click(0, 1);
    return;
}

void
Gui::on_sampler_button_1_3_stop_click()
{
    this->on_sampler_button_stop_click(0, 2);
    return;
}

void
Gui::on_sampler_button_1_4_stop_click()
{
    this->on_sampler_button_stop_click(0, 3);
    return;
}

void
Gui::on_sampler_button_2_1_stop_click()
{
    this->on_sampler_button_stop_click(1, 0);
    return;
}

void
Gui::on_sampler_button_2_2_stop_click()
{
    this->on_sampler_button_stop_click(1, 1);
    return;
}

void
Gui::on_sampler_button_2_3_stop_click()
{
    this->on_sampler_button_stop_click(1, 2);
    return;
}

void
Gui::on_sampler_button_2_4_stop_click()
{
    this->on_sampler_button_stop_click(1, 3);
    return;
}

void
Gui::on_sampler_button_stop_click(unsigned short int in_deck_index,
                                  unsigned short int in_sampler_index)
{
    qDebug() << "Gui::on_sampler_button_stop_click...";

    // Stop playback (and return to beginning of the song).
    this->set_sampler_state(in_deck_index, in_sampler_index, false);
    this->playback->set_sampler_state(in_deck_index, in_sampler_index, false);

    // Select playback area (if not already done).
    this->highlight_deck_sampler_area(in_deck_index);

    qDebug() << "Gui::on_sampler_button_play_click done.";

    return;
}

void
Gui::on_file_browser_expand(QModelIndex)
{
    qDebug() << "Gui::on_file_browser_expand_collapse...";
    this->resize_file_browser_columns();
    qDebug() << "Gui::on_file_browser_expand_collapse...";
}

void
Gui::resize_file_browser_columns()
{
    qDebug() << "Gui::resize_file_browser_columns...";
    this->file_browser->resizeColumnToContents(COLUMN_FILE_NAME);
    qDebug() << "Gui::resize_file_browser_columns...";
}

void
Gui::select_and_run_audio_file_decoding_process_deck1()
{
    // Check the button.
    this->load_track_on_deck1_button->setEnabled(false);
    this->load_track_on_deck1_button->setChecked(true);

    // Select deck 1.
    this->highlight_deck_sampler_area(0);

    // Decode and play selected audio file on deck 1.
    this->run_audio_file_decoding_process();

    // Release the button.
    this->load_track_on_deck1_button->setEnabled(true);
    this->load_track_on_deck1_button->setChecked(false);
}

void
Gui::select_and_run_audio_file_decoding_process_deck2()
{
    // Check the button.
    this->load_track_on_deck2_button->setEnabled(false);
    this->load_track_on_deck2_button->setChecked(true);

    // Select deck 2.
    this->highlight_deck_sampler_area(1);

    // Decode and play selected audio file on deck 2.
    this->run_audio_file_decoding_process();

    // Release the button.
    this->load_track_on_deck2_button->setEnabled(true);
    this->load_track_on_deck2_button->setChecked(false);
}

void
Gui::run_audio_file_decoding_process()
{
    qDebug() << "Gui::run_audio_file_decoding_process...";

    // Get selected file path.
    Audio_collection_item *item = static_cast<Audio_collection_item*>((this->file_browser->currentIndex()).internalPointer());
    QFileInfo info(item->get_full_path());
    qDebug() << "Gui::run_audio_file_decoding_process: selected item: " << info.absoluteFilePath();

    // Get selected deck/sampler.
    unsigned short int deck_index = 0;
    QLabel   *deck_track_name = this->deck1_track_name;
    Waveform *deck_waveform   = this->deck1_waveform;
    Audio_file_decoding_process *decode_process = this->dec_1;
    if ((this->nb_decks > 1) && (this->deck2_gbox->is_selected() == true))
    {
        deck_index = 1;
        deck_track_name = this->deck2_track_name;
        deck_waveform   = this->deck2_waveform;
        decode_process  = this->dec_2;
    }

    // Execute decoding if not trying to open the existing track.
    if (info.fileName().compare(deck_track_name->text()) != 0 ) {
        if (decode_process->run(info.absoluteFilePath(), item->get_data(COLUMN_KEY).toString()) == false)
        {
            qWarning() << "Gui::run_audio_file_decoding_process: can not decode " << info.absoluteFilePath();
        }
        else
        {
            this->playback->reset(deck_index);

            // Reset cue point.
            deck_waveform->move_cue_slider(this->playback->get_position(deck_index));
        }
    }
    else
    {
        this->playback->reset(deck_index);

        // Reset cue point.
        deck_waveform->move_cue_slider(this->playback->get_position(deck_index));
    }

    // Update waveforms.
    deck_waveform->update();
//    this->deck1_vertical_waveform->update();

    qDebug() << "Gui::run_audio_file_decoding_process done.";

    return;
}

void
Gui::set_remaining_time(unsigned int in_remaining_time, int in_deck_index)
{
    qDebug() << "Gui::set_remaining_time...";

    // Split remaining time (which is in msec) into minutes, seconds and milliseconds.
    int remaining_time_by_1000 = in_remaining_time / 1000.0;
    div_t tmp_division;
    tmp_division = div(remaining_time_by_1000, 60);
    QString min  = QString::number(tmp_division.quot);
    QString sec  = QString::number(tmp_division.rem);
    QString msec = QString::number(in_remaining_time).right(3);

    // Change displayed remaining time.
    if (min.compare(this->decks_remaining_time[in_deck_index]->min->text()) != 0)
    {
        if (min.size() == 1)
        {
            this->decks_remaining_time[in_deck_index]->min->setText("0" + min);
        }
        else
        {
            this->decks_remaining_time[in_deck_index]->min->setText(min);
        }
    }
    if (sec.compare(this->decks_remaining_time[in_deck_index]->sec->text()) != 0)
    {
        if (sec.size() == 1)
        {
            this->decks_remaining_time[in_deck_index]->sec->setText("0" + sec);
        }
        else
        {
            this->decks_remaining_time[in_deck_index]->sec->setText(sec);
        }
    }
    if (msec.compare(this->decks_remaining_time[in_deck_index]->msec->text()) != 0)
    {
        this->decks_remaining_time[in_deck_index]->msec->setText(msec);
    }

    // Move slider on waveform when remaining time changed.
    if (in_deck_index == 0)
    {

       this->deck1_waveform->move_slider(this->playback->get_position(0));
    }
    else
    {
        this->deck2_waveform->move_slider(this->playback->get_position(1));
    }

    qDebug() << "Gui::set_remaining_time done.";

    return;
}

void
Gui::set_sampler_remaining_time(unsigned int in_remaining_time,
                                int          in_deck_index,
                                int          in_sampler_index)
{
    qDebug() << "Gui::set_sampler_remaining_time...";

    // Split remaining time (which is in msec) into minutes, seconds and milliseconds.
    int remaining_time_by_1000 = in_remaining_time / 1000.0;
    div_t tmp_division;
    tmp_division = div(remaining_time_by_1000, 60);
    QString sec  = QString::number(tmp_division.rem);

    // Change displayed remaining time (if different than previous one).
    if (in_deck_index == 0 &&
        sec.compare(this->sampler1_remainingtime[in_sampler_index]->text()) != 0)
    {
        if (sec.size() == 1)
        {
            this->sampler1_remainingtime[in_sampler_index]->setText("- 0" + sec);
        }
        else
        {
            this->sampler1_remainingtime[in_sampler_index]->setText("- " + sec);
        }
    }
    if (in_deck_index == 1 &&
        sec.compare(this->sampler2_remainingtime[in_sampler_index]->text()) != 0)
    {
        if (sec.size() == 1)
        {
            this->sampler2_remainingtime[in_sampler_index]->setText("- 0" + sec);
        }
        else
        {
            this->sampler2_remainingtime[in_sampler_index]->setText("- " + sec);
        }
    }

    qDebug() << "Gui::set_sampler_remaining_time done.";

    return;
}

void
Gui::set_sampler_state(int in_deck_index,
                       int in_sampler_index,
                       bool in_state)
{
    qDebug() << "Gui::set_sampler_state...";

    // Change state only if a sample is loaded and playing.
    if ((this->playback->is_sampler_loaded(in_deck_index, in_sampler_index) == true) && (in_state == true))
    {
        if (in_deck_index == 0)
        {
            this->sampler1_buttons_play[in_sampler_index]->setChecked(true);
            this->sampler1_buttons_stop[in_sampler_index]->setChecked(false);
        }
        else
        {
            this->sampler2_buttons_play[in_sampler_index]->setChecked(true);
            this->sampler2_buttons_stop[in_sampler_index]->setChecked(false);
        }

    }
    else // Sampler is stopping or is not loaded, make play button inactive.
    {
        if (in_deck_index == 0)
        {
            this->sampler1_buttons_play[in_sampler_index]->setChecked(false);
            this->sampler1_buttons_stop[in_sampler_index]->setChecked(true);
        }
        else
        {
            this->sampler2_buttons_play[in_sampler_index]->setChecked(false);
            this->sampler2_buttons_stop[in_sampler_index]->setChecked(true);
        }
    }


    qDebug() << "Gui::set_sampler_state done.";

    return;
}

void
Gui::deck1_jump_to_position(float in_position)
{
    qDebug() << "Gui::deck1_jump_to_position...";

    this->playback->jump_to_position(0, in_position);
    this->highlight_deck_sampler_area(0);

    qDebug() << "Gui::deck1_jump_to_position done.";

    return;
}

void
Gui::deck2_jump_to_position(float in_position)
{
    qDebug() << "Gui::deck2_jump_to_position...";

    this->playback->jump_to_position(1, in_position);
    this->highlight_deck_sampler_area(1);

    qDebug() << "Gui::deck2_jump_to_position done.";

    return;
}

void
Gui::deck_go_to_begin()
{
    qDebug() << "Gui::deck_go_to_begin...";

    if ((this->nb_decks > 1) && (this->deck2_gbox->is_selected() == true))
    {
        this->playback->jump_to_position(1, 0.0);
    }
    else
    {
        this->playback->jump_to_position(0, 0.0);
    }

    qDebug() << "Gui::deck_go_to_begin done.";

    return;
}

void
Gui::deck_set_cue_point()
{
    qDebug() << "Gui::deck_set_cue_point...";

    if ((this->nb_decks > 1) && (this->deck2_gbox->is_selected() == true))
    {
        this->deck2_waveform->move_cue_slider(this->playback->get_position(1));
        this->playback->store_cue_point(1);
    }
    else
    {
        this->deck1_waveform->move_cue_slider(this->playback->get_position(0));
        this->playback->store_cue_point(0);
    }

    qDebug() << "Gui::deck_set_cue_point done.";

    return;
}

void
Gui::deck_go_to_cue_point()
{
    qDebug() << "Gui::deck_go_to_cue_point...";

    if ((this->nb_decks > 1) && (this->deck2_gbox->is_selected() == true))
    {
        this->playback->jump_to_cue_point(1);
    }
    else
    {
        this->playback->jump_to_cue_point(0);
    }

    qDebug() << "Gui::deck_go_to_cue_point done.";

    return;
}

void
Gui::switch_playback_selection()
{
    qDebug() << "Gui::select_deck...";

    // Switch deck selection.
    if (this->nb_decks > 1)
    {
        if (this->deck1_gbox->property("selected").toBool() == true)
        {
            // Select deck/sample #2
            this->highlight_deck_sampler_area(1);
        }
        else
        {
            // Select deck/sample #1
            this->highlight_deck_sampler_area(0);
        }
    }

    qDebug() << "Gui::select_deck done.";

    return;
}

void
Gui::select_playback_1()
{
    qDebug() << "Gui::select_playback_1...";

    // Select deck/sample #1
    this->highlight_deck_sampler_area(0);

    qDebug() << "Gui::select_playback_1 done.";

    return;
}

void
Gui::select_playback_2()
{
    qDebug() << "Gui::select_playback_2...";

    if (this->nb_decks > 1)
    {
        // Select deck/sample #2
        this->highlight_deck_sampler_area(1);
    }

    qDebug() << "Gui::select_playback_2 done.";

    return;
}

void
Gui::highlight_deck_sampler_area(unsigned short int in_deck_index)
{
    qDebug() << "Gui::highlight_deck_sampler_area...";

    bool switch_on = false;
    if (in_deck_index == 0)
    {
        switch_on = true;
    }

    // Select correct pair deck+sampler.
    this->deck1_gbox->setProperty("selected", switch_on);
    this->deck2_gbox->setProperty("selected", !switch_on);
    this->sampler1_gbox->setProperty("selected", switch_on);
    this->sampler2_gbox->setProperty("selected", !switch_on);

    // Redraw widget (necessary to reparse stylesheet).
    this->deck1_gbox->redraw();
    this->deck2_gbox->redraw();
    this->sampler1_gbox->redraw();
    this->sampler2_gbox->redraw();

    qDebug() << "Gui::highlight_deck_sampler_area done.";

    return;
}

void
Gui::hover_playback(int in_deck_index)
{
    qDebug() << "Gui::hover_playback...";

    this->highlight_border_deck_sampler_area(in_deck_index, true);

    qDebug() << "Gui::hover_playback done.";

    return;
}

void
Gui::unhover_playback(int in_deck_index)
{
    qDebug() << "Gui::unhover_playback...";

    this->highlight_border_deck_sampler_area(in_deck_index, false);

    qDebug() << "Gui::unhover_playback done.";

    return;
}

void
Gui::highlight_border_deck_sampler_area(unsigned short int in_deck_index,
                                        bool               switch_on)
{
    qDebug() << "Gui::highlight_border_deck_sampler_area...";

    if (in_deck_index == 0)
    {
        // highlight pair deck+sampler.
        this->deck1_gbox->setProperty("hover", switch_on);
        this->sampler1_gbox->setProperty("hover", switch_on);

        // Redraw widget (necessary to reparse stylesheet).
        this->deck1_gbox->redraw();
        this->sampler1_gbox->redraw();
    }

    if (in_deck_index == 1)
    {
        // highlight pair deck+sampler.
        this->deck2_gbox->setProperty("hover", switch_on);
        this->sampler2_gbox->setProperty("hover", switch_on);

        // Redraw widget (necessary to reparse stylesheet).
        this->deck2_gbox->redraw();
        this->sampler2_gbox->redraw();
    }

    qDebug() << "Gui::highlight_border_deck_sampler_area done.";

    return;
}

void
Gui::select_and_show_next_keys_deck1()
{
    // Check the button.
    this->show_next_key_from_deck1_button->setEnabled(false);
    this->show_next_key_from_deck1_button->setChecked(true);

    // Select deck 1.
    this->highlight_deck_sampler_area(0);

    // Show next keys from deck 1.
    this->show_next_keys();

    // Release the button.
    this->show_next_key_from_deck1_button->setEnabled(true);
    this->show_next_key_from_deck1_button->setChecked(false);
}

void
Gui::select_and_show_next_keys_deck2()
{
    // Check the button.
    this->show_next_key_from_deck2_button->setEnabled(false);
    this->show_next_key_from_deck2_button->setChecked(true);

    // Select deck 2.
    this->highlight_deck_sampler_area(1);

    // Show next keys from deck 2.
    this->show_next_keys();

    // Release the button.
    this->show_next_key_from_deck2_button->setEnabled(true);
    this->show_next_key_from_deck2_button->setChecked(false);
}

void
Gui::show_next_keys()
{
    qDebug() << "Gui::show_next_keys...";

    // Get music key of selected deck/sampler.
    QString deck_key = this->deck1_key->text();
    if ((this->nb_decks > 1) && (this->deck2_gbox->is_selected() == true))
    {
        deck_key = this->deck2_key->text();
    }

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

    qDebug() << "Gui::show_next_keys done.";

    return;
}

PlaybackQGroupBox::PlaybackQGroupBox(const QString &title) : QGroupBox(title)
{
    qDebug() << "PlaybackQGroupBox::PlaybackQGroupBox: create object...";

    // Init.
    this->l_selected = false;

    qDebug() << "PlaybackQGroupBox::PlaybackQGroupBox: create object done.";

    return;
}

PlaybackQGroupBox::~PlaybackQGroupBox()
{
    qDebug() << "PlaybackQGroupBox::PlaybackQGroupBox: delete object...";

    qDebug() << "PlaybackQGroupBox::PlaybackQGroupBox: delete object done.";

    return;
}

void
PlaybackQGroupBox::redraw()
{
    qDebug() << "PlaybackQGroupBox::redraw...";

    this->style()->unpolish(this);
    this->style()->polish(this);

    qDebug() << "PlaybackQGroupBox::redraw done.";

    return;
}

void
PlaybackQGroupBox::mousePressEvent(QMouseEvent *in_mouse_event)
{
    qDebug() << "PlaybackQGroupBox::mousePressEvent...";

    qDebug() << "PlaybackQGroupBox::pressEvent: x = " << in_mouse_event->x();
    emit this->selected();

    qDebug() << "PlaybackQGroupBox::mousePressEvent done.";

    return;
}

void
PlaybackQGroupBox::enterEvent(QEvent *in_event)
{
    qDebug() << "PlaybackQGroupBox::enterEvent...";

    qDebug() << "PlaybackQGroupBox::enterEvent: type = " << in_event->type();
    emit this->hover();


    qDebug() << "PlaybackQGroupBox::enterEvent done.";

    return;
}

void
PlaybackQGroupBox::leaveEvent(QEvent *in_event)
{
    qDebug() << "PlaybackQGroupBox::leaveEvent...";

    qDebug() << "PlaybackQGroupBox::leaveEvent: type = " << in_event->type();
    emit this->unhover();

    qDebug() << "PlaybackQGroupBox::leaveEvent done.";

    return;
}
