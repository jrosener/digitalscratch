/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*------------------------------------------------------( config_dialog.cpp )-*/
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
/*                Creates configuration GUI for Digital-scratch player        */
/*                                                                            */
/*============================================================================*/

#include <QIcon>
#include <QDialog>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QLabel>
#include <QGridLayout>
#include <QSettings>
#include <QPushButton>
#include <QtDebug>
#include "config_dialog.h"
#include <digital_scratch_api.h>
#include <iostream>

Config_dialog::Config_dialog(QWidget              *parent,
                             Application_settings *in_settings) : QDialog(parent)
{
    this->settings = in_settings;

    // Init player parameters.
    this->base_dir_path    = new QLineEdit(this);
    this->gui_style_select = new QComboBox(this);
    QList<QString> *available_gui_styles = this->settings->get_available_gui_styles();
    for (int i = 0; i < available_gui_styles->size(); i++)
    {
        this->gui_style_select->addItem(available_gui_styles->at(i));
    }

    // Init motion detection parameters widgets.
    this->extreme_min                        = new QLineEdit(this);
    this->max_speed_diff                     = new QLineEdit(this);
    this->slow_speed_algo_usage              = new QLineEdit(this);
    this->max_nb_speed_for_stability         = new QLineEdit(this);
    this->nb_cycle_before_changing_direction = new QLineEdit(this);
    this->low_pass_filter_max_speed_usage    = new QLineEdit(this);
    this->vinyl_type_select                  = new QComboBox(this);
    QList<QString> *available_vinyl_types    = this->settings->get_available_vinyl_types();
    for (int i = 0; i < available_vinyl_types->size(); i++)
    {
        this->vinyl_type_select->addItem(available_vinyl_types->at(i));
    }

    // Init keyboard shortcuts widgets.
    this->kb_switch_playback          = new ShortcutQLabel(this);
    this->kb_load_track_on_deck       = new ShortcutQLabel(this);
    this->kb_play_begin_track_on_deck = new ShortcutQLabel(this);
    this->kb_set_cue_point_on_deck    = new ShortcutQLabel(this);
    this->kb_play_cue_point_on_deck   = new ShortcutQLabel(this);
    this->kb_collapse_browse          = new ShortcutQLabel(this);
    this->kb_load_track_on_sampler1   = new ShortcutQLabel(this);
    this->kb_load_track_on_sampler2   = new ShortcutQLabel(this);
    this->kb_load_track_on_sampler3   = new ShortcutQLabel(this);
    this->kb_load_track_on_sampler4   = new ShortcutQLabel(this);    

    return;
}

Config_dialog::~Config_dialog()
{
    return;
}

bool
Config_dialog::show_browse_window()
{
    // Create browse window.
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    tr("Select music directory"),
                                                    this->settings->get_tracks_base_dir_path(),
                                                    QFileDialog::ShowDirsOnly);

    // Update base directory path.
    if (dir != NULL)
    {
        this->base_dir_path->setText(dir);
    }

    return true;
}

void Config_dialog::reset_motion_detection_params()
{
    // Reset all motion detection parameters to their default values.
    this->vinyl_type_select->setCurrentIndex(this->vinyl_type_select->findText(this->settings->get_vinyl_type_default()));
    this->extreme_min->setText((new QString)->setNum(this->settings->get_extreme_min_default(), 'f', 3));
    this->max_speed_diff->setText((new QString)->setNum(this->settings->get_max_speed_diff_default(), 'f', 3));
    this->slow_speed_algo_usage->setText((new QString)->setNum(this->settings->get_slow_speed_algo_usage_default(), 'f', 3));
    this->max_nb_speed_for_stability->setText((new QString)->setNum(this->settings->get_max_nb_speed_for_stability_default()));
    this->nb_cycle_before_changing_direction->setText((new QString)->setNum(this->settings->get_nb_cycle_before_changing_direction_default()));
    this->low_pass_filter_max_speed_usage->setText((new QString)->setNum(this->settings->get_low_pass_filter_max_speed_usage_default(), 'f', 3));
}

void Config_dialog::reset_shortcuts()
{
    // Reset all keyboard shortcuts to their default values.
    this->kb_switch_playback->setText(KB_SWITCH_PLAYBACK_DEFAULT);
    this->kb_load_track_on_deck->setText(KB_LOAD_TRACK_ON_DECK_DEFAULT);
    this->kb_play_begin_track_on_deck->setText(KB_PLAY_BEGIN_TRACK_ON_DECK_DEFAULT);
    this->kb_set_cue_point_on_deck->setText(KB_SET_CUE_POINT_ON_DECK_DEFAULT);
    this->kb_play_cue_point_on_deck->setText(KB_PLAY_CUE_POINT_ON_DECK_DEFAULT);
    this->kb_collapse_browse->setText(KB_COLLAPSE_BROWSER_DEFAULT);
    this->kb_load_track_on_sampler1->setText(KB_LOAD_TRACK_ON_SAMPLER1_DEFAULT);
    this->kb_load_track_on_sampler2->setText(KB_LOAD_TRACK_ON_SAMPLER2_DEFAULT);
    this->kb_load_track_on_sampler3->setText(KB_LOAD_TRACK_ON_SAMPLER3_DEFAULT);
    this->kb_load_track_on_sampler4->setText(KB_LOAD_TRACK_ON_SAMPLER4_DEFAULT);
}

void
Config_dialog::accept()
{
    //
    // Set all settings in configuration file if values are correct.
    //

    // Set base directory for track browser.
    if (QDir(this->base_dir_path->text()).exists() == true)
    {
        this->settings->set_tracks_base_dir_path(this->base_dir_path->text());
    }

    // Set gui style.
    this->settings->set_gui_style(this->gui_style_select->currentText());

    // Set vinyl type.
    this->settings->set_vinyl_type(this->vinyl_type_select->currentText());

    // Set motion detection settings (check range).
    this->settings->set_extreme_min(this->extreme_min->text().toFloat());
    this->settings->set_max_speed_diff(this->max_speed_diff->text().toFloat());
    this->settings->set_slow_speed_algo_usage(this->slow_speed_algo_usage->text().toFloat());
    this->settings->set_max_nb_speed_for_stability(this->max_nb_speed_for_stability->text().toInt());
    this->settings->set_nb_cycle_before_changing_direction(this->nb_cycle_before_changing_direction->text().toInt());
    this->settings->set_low_pass_filter_max_speed_usage(this->low_pass_filter_max_speed_usage->text().toFloat());

    // Set keyboard shortcuts.
    this->settings->set_keyboard_shortcut(KB_SWITCH_PLAYBACK,          this->kb_switch_playback->text());
    this->settings->set_keyboard_shortcut(KB_LOAD_TRACK_ON_DECK,       this->kb_load_track_on_deck->text());
    this->settings->set_keyboard_shortcut(KB_PLAY_BEGIN_TRACK_ON_DECK, this->kb_play_begin_track_on_deck->text());
    this->settings->set_keyboard_shortcut(KB_SET_CUE_POINT_ON_DECK,    this->kb_set_cue_point_on_deck->text());
    this->settings->set_keyboard_shortcut(KB_PLAY_CUE_POINT_ON_DECK,   this->kb_play_cue_point_on_deck->text());
    this->settings->set_keyboard_shortcut(KB_COLLAPSE_BROWSER,         this->kb_collapse_browse->text());
    this->settings->set_keyboard_shortcut(KB_LOAD_TRACK_ON_SAMPLER1,   this->kb_load_track_on_sampler1->text());
    this->settings->set_keyboard_shortcut(KB_LOAD_TRACK_ON_SAMPLER2,   this->kb_load_track_on_sampler2->text());
    this->settings->set_keyboard_shortcut(KB_LOAD_TRACK_ON_SAMPLER3,   this->kb_load_track_on_sampler3->text());
    this->settings->set_keyboard_shortcut(KB_LOAD_TRACK_ON_SAMPLER4,   this->kb_load_track_on_sampler4->text());

    // Close window.
    this->done(QDialog::Accepted);
}

void
Config_dialog::reject()
{
    // Close window.
    this->done(QDialog::Rejected);
}

int
Config_dialog::show()
{
    // Create dialog.
    this->setWindowTitle(tr("Configuration"));

    // Set window icon
    this->setWindowIcon(QIcon(ICON));

    //
    // Player tab: base directory to browse.
    //
    QLabel *base_dir_label = new QLabel(tr("Base music directory: "), this);
    this->base_dir_path->setMinimumWidth(300);
    QPushButton *base_dir_button = new QPushButton(tr("Browse..."), this);
    QObject::connect(base_dir_button, SIGNAL(clicked()), this, SLOT(show_browse_window()));

    // Player tab: select GUI style.
    QLabel *gui_style_label = new QLabel(tr("GUI style: "), this);

    // Player tab: setup layout.
    QGridLayout *player_tab_layout = new QGridLayout(this);
    player_tab_layout->addWidget(base_dir_label,         0, 0);
    player_tab_layout->addWidget(this->base_dir_path,    0, 1);
    player_tab_layout->addWidget(base_dir_button,        0, 2);
    player_tab_layout->addWidget(gui_style_label,        1, 0);
    player_tab_layout->addWidget(this->gui_style_select, 1, 1);
    QWidget *player_tab = new QWidget(this);
    player_tab->setLayout(player_tab_layout);


    //
    // Motion detection tab: provide coded vinyl configuration parameters.
    //
    QGridLayout *motion_detect_layout = new QGridLayout(this);

    QLabel *vinyl_type_label = new QLabel(tr("Vinyl type: "), this);
    motion_detect_layout->addWidget(vinyl_type_label,        0, 0);
    motion_detect_layout->addWidget(this->vinyl_type_select, 0, 1);

    QLabel *extreme_min_label = new QLabel(tr("Minimal detected signal: ]0.0,1.0["), this);
    this->extreme_min->setMinimumWidth(300);
    motion_detect_layout->addWidget(extreme_min_label, 1, 0);
    motion_detect_layout->addWidget(this->extreme_min, 1, 1);

    QLabel *low_pass_filter_max_speed_usage_label = new QLabel(tr("Enable low pass filter on recorded timecode under this speed: ]0.0,2.0["), this);
    this->low_pass_filter_max_speed_usage->setMinimumWidth(300);
    motion_detect_layout->addWidget(low_pass_filter_max_speed_usage_label, 2, 0);
    motion_detect_layout->addWidget(this->low_pass_filter_max_speed_usage, 2, 1);

    QLabel *max_speed_diff_label = new QLabel(tr("Maximum speed difference allowed beetween 2 speeds: ]0.0,1.0["), this);
    this->max_speed_diff->setMinimumWidth(300);
    motion_detect_layout->addWidget(max_speed_diff_label, 3, 0);
    motion_detect_layout->addWidget(this->max_speed_diff, 3, 1);

    QLabel *slow_speed_algo_usage_label = new QLabel(tr("Enable slow speed algo if speed is under this value: ]0.0,1.0["), this);
    this->slow_speed_algo_usage->setMinimumWidth(300);
    motion_detect_layout->addWidget(slow_speed_algo_usage_label, 4, 0);
    motion_detect_layout->addWidget(this->slow_speed_algo_usage, 4, 1);

    QLabel *max_nb_speed_for_stability_label = new QLabel(tr("Maximum number of speeds used by speed stability algo: ]0,100["), this);
    this->max_nb_speed_for_stability->setMinimumWidth(300);
    motion_detect_layout->addWidget(max_nb_speed_for_stability_label, 5, 0);
    motion_detect_layout->addWidget(this->max_nb_speed_for_stability, 5, 1);

    QLabel *nb_cycle_before_changing_direction_label = new QLabel(tr("Number of cycles used to keep old direction before switching to new one: ]0,100["), this);
    this->nb_cycle_before_changing_direction->setMinimumWidth(300);
    motion_detect_layout->addWidget(nb_cycle_before_changing_direction_label, 6, 0);
    motion_detect_layout->addWidget(this->nb_cycle_before_changing_direction, 6, 1);

    this->motion_params_reset_to_default = new QPushButton(this);
    this->motion_params_reset_to_default->setText(tr("Reset to default"));
    motion_detect_layout->addWidget(this->motion_params_reset_to_default, 7, 0, Qt::AlignLeft);
    QObject::connect(this->motion_params_reset_to_default, SIGNAL(clicked()), this, SLOT(reset_motion_detection_params()));

    QWidget *motion_detect_tab = new QWidget(this);
    motion_detect_tab->setLayout(motion_detect_layout);


    //
    // Keyboard shortcuts tab.
    //
    QGridLayout *shortcuts_layout = new QGridLayout(this);

    QLabel *kb_switch_playback_label = new QLabel(tr("Switch playback"), this);
    shortcuts_layout->addWidget(kb_switch_playback_label, 1, 0);
    shortcuts_layout->addWidget(this->kb_switch_playback, 1, 1);

    QLabel *kb_load_track_on_deck_label = new QLabel(tr("Load track"), this);
    shortcuts_layout->addWidget(kb_load_track_on_deck_label, 2, 0);
    shortcuts_layout->addWidget(this->kb_load_track_on_deck, 2, 1);

    QLabel *kb_play_begin_track_on_deck_label = new QLabel(tr("Restart track"), this);
    shortcuts_layout->addWidget(kb_play_begin_track_on_deck_label, 3, 0);
    shortcuts_layout->addWidget(this->kb_play_begin_track_on_deck, 3, 1);

    QLabel *kb_set_cue_point_on_deck_label = new QLabel(tr("Set cue point"), this);
    shortcuts_layout->addWidget(kb_set_cue_point_on_deck_label, 4, 0);
    shortcuts_layout->addWidget(this->kb_set_cue_point_on_deck, 4, 1);

    QLabel *kb_play_cue_point_on_deck_label = new QLabel(tr("Play from cue point"), this);
    shortcuts_layout->addWidget(kb_play_cue_point_on_deck_label, 5, 0);
    shortcuts_layout->addWidget(this->kb_play_cue_point_on_deck, 5, 1);

    QLabel *kb_collapse_browse_label = new QLabel(tr("Collapse file browser"), this);
    shortcuts_layout->addWidget(kb_collapse_browse_label, 1, 3);
    shortcuts_layout->addWidget(this->kb_collapse_browse, 1, 4);

    QLabel *kb_load_track_on_sampler1_label = new QLabel(tr("Load track on sampler 1"), this);
    shortcuts_layout->addWidget(kb_load_track_on_sampler1_label, 2, 3);
    shortcuts_layout->addWidget(this->kb_load_track_on_sampler1, 2, 4);

    QLabel *kb_load_track_on_sampler2_label = new QLabel(tr("Load track on sampler 2"), this);
    shortcuts_layout->addWidget(kb_load_track_on_sampler2_label, 3, 3);
    shortcuts_layout->addWidget(this->kb_load_track_on_sampler2, 3, 4);

    QLabel *kb_load_track_on_sampler3_label = new QLabel(tr("Load track on sampler 3"), this);
    shortcuts_layout->addWidget(kb_load_track_on_sampler3_label, 4, 3);
    shortcuts_layout->addWidget(this->kb_load_track_on_sampler3, 4, 4);

    QLabel *kb_load_track_on_sampler4_label = new QLabel(tr("Load track on sampler 4"), this);
    shortcuts_layout->addWidget(kb_load_track_on_sampler4_label, 5, 3);
    shortcuts_layout->addWidget(this->kb_load_track_on_sampler4, 5, 4);

    shortcuts_layout->setColumnStretch(0, 10);
    shortcuts_layout->setColumnStretch(1, 15);
    shortcuts_layout->setColumnStretch(2, 10);
    shortcuts_layout->setColumnStretch(3, 10);
    shortcuts_layout->setColumnStretch(4, 15);

    this->shortcut_reset_to_default = new QPushButton(this);
    this->shortcut_reset_to_default->setText(tr("Reset to default"));
    shortcuts_layout->addWidget(this->shortcut_reset_to_default, 6, 0, Qt::AlignLeft);
    QObject::connect(this->shortcut_reset_to_default, SIGNAL(clicked()), this, SLOT(reset_shortcuts()));

    QWidget *shortcuts_tab = new QWidget(this);
    shortcuts_tab->setLayout(shortcuts_layout);


    //
    // Main window.
    //

    // Create 3 tabs: player, sound card and motion detection.
    QTabWidget *tabs = new QTabWidget(this);
    tabs->insertTab(0, player_tab, tr("Player"));
    tabs->insertTab(1, motion_detect_tab, tr("Motion detection"));
    tabs->insertTab(2, shortcuts_tab, tr("Shortcuts"));

    // 2 buttons: OK and Cancel.
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |
                                                       QDialogButtonBox::Cancel,
                                                       Qt::Horizontal,
                                                       this);
    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    // Create main vertical layout.
    QVBoxLayout *main_layout = new QVBoxLayout(this);
    main_layout->addWidget(tabs);
    main_layout->addWidget(buttonBox);

    // Put main layout in window.
    this->setLayout(main_layout);

    // Fill widgets with content of configuration file.
    this->base_dir_path->setText(this->settings->get_tracks_base_dir_path());
    this->gui_style_select->setCurrentIndex(this->gui_style_select->findText(this->settings->get_gui_style()));
    this->vinyl_type_select->setCurrentIndex(this->vinyl_type_select->findText(this->settings->get_vinyl_type()));
    this->extreme_min->setText((new QString)->setNum(this->settings->get_extreme_min(), 'f', 3));
    this->max_speed_diff->setText((new QString)->setNum(this->settings->get_max_speed_diff(), 'f', 3));
    this->slow_speed_algo_usage->setText((new QString)->setNum(this->settings->get_slow_speed_algo_usage(), 'f', 3));
    this->max_nb_speed_for_stability->setText((new QString)->setNum(this->settings->get_max_nb_speed_for_stability()));
    this->nb_cycle_before_changing_direction->setText((new QString)->setNum(this->settings->get_nb_cycle_before_changing_direction()));
    this->low_pass_filter_max_speed_usage->setText((new QString)->setNum(this->settings->get_low_pass_filter_max_speed_usage(), 'f', 3));

    this->kb_switch_playback->setText(this->settings->get_keyboard_shortcut(KB_SWITCH_PLAYBACK));
    this->kb_load_track_on_deck->setText(this->settings->get_keyboard_shortcut(KB_LOAD_TRACK_ON_DECK));
    this->kb_play_begin_track_on_deck->setText(this->settings->get_keyboard_shortcut(KB_PLAY_BEGIN_TRACK_ON_DECK));
    this->kb_set_cue_point_on_deck->setText(this->settings->get_keyboard_shortcut(KB_SET_CUE_POINT_ON_DECK));
    this->kb_play_cue_point_on_deck->setText(this->settings->get_keyboard_shortcut(KB_PLAY_CUE_POINT_ON_DECK));
    this->kb_collapse_browse->setText(this->settings->get_keyboard_shortcut(KB_COLLAPSE_BROWSER));
    this->kb_load_track_on_sampler1->setText(this->settings->get_keyboard_shortcut(KB_LOAD_TRACK_ON_SAMPLER1));
    this->kb_load_track_on_sampler2->setText(this->settings->get_keyboard_shortcut(KB_LOAD_TRACK_ON_SAMPLER2));
    this->kb_load_track_on_sampler3->setText(this->settings->get_keyboard_shortcut(KB_LOAD_TRACK_ON_SAMPLER3));
    this->kb_load_track_on_sampler4->setText(this->settings->get_keyboard_shortcut(KB_LOAD_TRACK_ON_SAMPLER4));

    return this->exec();
}

ShortcutQLabel::ShortcutQLabel(QWidget *parent) : QLabel(parent)
{
    qDebug() << "ShortcutQLabel::ShortcutQLabel: create object...";

    this->capturing = false;

    qDebug() << "ShortcutQLabel::ShortcutQLabel: create object done.";

    return;
}

ShortcutQLabel::~ShortcutQLabel()
{
    qDebug() << "ShortcutQLabel::ShortcutQLabel: delete object...";

    qDebug() << "ShortcutQLabel::ShortcutQLabel: delete object done.";

    return;
}

void
ShortcutQLabel::start_capture()
{
    // Store current text to get it back if cancel.
    this->old_text = this->text();

    // Ask to press a key.
    this->setText(tr("Press a key or Esc to cancel"));

    // Capture keyboard.
    this->capturing = true;
    this->grabKeyboard();
    this->grabMouse();

    qDebug() << "ShortcutQLabel::start_capture: capturing keyboard... ";

    return;
}

void
ShortcutQLabel::cancel_capture()
{
    this->capturing = false;
    this->releaseKeyboard();
    this->releaseMouse();

    // Keep previous shortcut.
    this->setText(this->old_text);

    qDebug() << "ShortcutQLabel::cancel_capture: keyboard capture canceled";

    return;
}

void
ShortcutQLabel::finish_capture(int in_key)
{
    this->capturing = false;
    this->releaseKeyboard();
    this->releaseMouse();

    // Set new shortcut.
    this->setText(QKeySequence(in_key).toString(QKeySequence::NativeText));

    qDebug() << "ShortcutQLabel::cancel_capture: keyboard capture done with " << QKeySequence(in_key).toString(QKeySequence::NativeText);

    return;
}

void
ShortcutQLabel::mousePressEvent(QMouseEvent *in_mouse_event)

{
    qDebug() << "ShortcutQLabel::mousePressEvent: x=" << in_mouse_event->x() << "  capturing keyboard... ";

    if (this->capturing == false)
    {
        this->start_capture();
    }
    else
    {
        // Was capturing, user clicked some where, so cancel capture.
        this->cancel_capture();
    }
}


void
ShortcutQLabel::keyPressEvent(QKeyEvent *in_key_event)
{
    if (this->capturing == true)
    {
        int keyInt = in_key_event->key();
        Qt::Key key = static_cast<Qt::Key>(keyInt);

        if (key == Qt::Key_Escape)
        {
            // Cancel capture.
            this->cancel_capture();
        }
        else
        {
            if (key != Qt::Key_unknown &&
                key != Qt::Key_Control &&
                key != Qt::Key_Shift &&
                key != Qt::Key_Alt &&
                key != Qt::Key_Meta)
            {
                // check for a combination of user clicks
                Qt::KeyboardModifiers modifiers = in_key_event->modifiers();
                if(modifiers & Qt::ShiftModifier)
                    keyInt += Qt::SHIFT;
                if(modifiers & Qt::ControlModifier)
                    keyInt += Qt::CTRL;
                if(modifiers & Qt::AltModifier)
                    keyInt += Qt::ALT;
                if(modifiers & Qt::MetaModifier)
                    keyInt += Qt::META;

                qDebug() << "ShortcutQLabel::keyPressEvent: Keysequence:" << QKeySequence(keyInt).toString(QKeySequence::NativeText);

                // Stop capture and show key sequence.
                this->finish_capture(keyInt);
            }
        }
    }
}
