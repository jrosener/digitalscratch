/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*------------------------------------------------------------------( gui.h )-*/
/*                                                                            */
/*  Copyright (C) 2003-2012                                                   */
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

#ifndef GUI_H_
#define GUI_H_

#include <QLabel>
#include <QGroupBox>
#include <QTreeView>
#include <QDirModel>
#include <QGraphicsView>
#include <QString>
#include <QFileSystemModel>
#include <QShortcut>
#include "config_dialog.h"
#include "audio_track.h"
#include "audio_file_decoding_process.h"
#include "audio_track_playback_process.h"
#include "playback_parameters.h"
#include "sound_card_access_rules.h"
#include "waveform.h"
#include "vertical_waveform.h"
#include "remaining_time.h"
#include "application_settings.h"
#include <application_const.h>
#include <audio_collection_model.h>

using namespace std;

#define XSTR(x) #x
#define STR(x) XSTR(x)

#define GUI_STYLE_DEFAULT_CSS ""
#define GUI_STYLE_DARK_CSS "*\
                           {\
                               color:            #f2f2ed;\
                               background-color: black;\
                               font:             8pt;\
                           }\
                           \
                           QLabel\
                           {\
                               background: transparent;\
                           }\
                           \
                           QPushButton#Sampler_buttons\
                           {\
                               color:            #000000;\
                               border:           0px;\
                               background-color: transparent;\
                           }\
                           \
                           QPushButton\
                           {\
                               color:            #bfbbbf;\
                               border:           2px solid gray;\
                               border-radius:    5px;\
                               padding:          5px;\
                               background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #737373, stop: 1 black);\
                           }\
                           QPushButton:hover\
                           {\
                               color:            #bfbbbf;\
                               background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 black, stop: 1 #737373);\
                           }\
                           QPushButton:pressed\
                           {\
                               color:            black;\
                               background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #737373, stop: 1 #b2b2b2);\
                           }\
                           \
                           QPushButton#Logo\
                           {\
                               border:           0px;\
                               background-color: transparent;\
                           }\
                           QPushButton#Logo:hover\
                           {\
                               border:           1px solid orange;\
                               border-radius:    13px;\
                           }\
                           QPushButton#Logo:pressed\
                           {\
                               background-color: lightGray;\
                           }\
                           \
                           QPushButton#Configuration_button\
                           {\
                               color:            black;\
                               background-color: gray;\
                               padding:          5px;\
                               border:           none;\
                               border-radius:    0px;\
                               image:            url(:/pixmaps/dark-settings.png);\
                               image-position:   left;\
                               height:           25px;\
                               min-width:        100px;\
                           }\
                           QPushButton#Configuration_button:hover\
                           {\
                               background-color: transparent;\
                               border:           1px solid orange;\
                               color:            #f2f2ed;\
                               image:            url(:/pixmaps/dark-settings_white.png);\
                           }\
                           QPushButton#Configuration_button:pressed\
                           {\
                               color:            black;\
                               background-color: lightGray;\
                               image:            url(:/pixmaps/dark-settings.png);\
                           }\
                           \
                           QPushButton#Fullscreen_button\
                           {\
                               color:            black;\
                               background-color: gray;\
                               padding:          5px;\
                               border:           none;\
                               border-radius:    0px;\
                               image:            url(:/pixmaps/dark-fullscreen.png);\
                               image-position:   left;\
                               height:           25px;\
                               min-width:        100px;\
                           }\
                           QPushButton#Fullscreen_button:hover\
                           {\
                               background-color: transparent;\
                               border:           1px solid orange;\
                               color:            #f2f2ed;\
                               image:            url(:/pixmaps/dark-fullscreen_white.png);\
                           }\
                           QPushButton#Fullscreen_button:pressed\
                           {\
                               color:            black;\
                               background-color: lightGray;\
                               image:            url(:/pixmaps/dark-fullscreen.png);\
                           }\
                           \
                           QPushButton#Help_button\
                           {\
                               color:            black;\
                               background-color: gray;\
                               padding:          5px;\
                               border:           none;\
                               border-radius:    0px;\
                               image:            url(:/pixmaps/dark-help.png);\
                               image-position:   left;\
                               height:           25px;\
                               min-width:        100px;\
                           }\
                           QPushButton#Help_button:hover\
                           {\
                               background-color: transparent;\
                               border:           1px solid orange;\
                               color:            #f2f2ed;\
                               image:            url(:/pixmaps/dark-help_white.png);\
                           }\
                           QPushButton#Help_button:pressed\
                           {\
                               color:            black;\
                               background-color: lightGray;\
                               image:            url(:/pixmaps/dark-help.png);\
                           }\
                           \
                           QPushButton#Quit_button\
                           {\
                               color:            black;\
                               background-color: gray;\
                               padding:          5px;\
                               border:           none;\
                               border-radius:    0px;\
                               image:            url(:/pixmaps/dark-exit.png);\
                               image-position:   left;\
                               height:           25px;\
                               min-width:        100px;\
                           }\
                           QPushButton#Quit_button:hover\
                           {\
                               background-color: transparent;\
                               border:           1px solid orange;\
                               color:            #f2f2ed;\
                               image:            url(:/pixmaps/dark-exit_white.png);\
                           }\
                           QPushButton#Quit_button:pressed\
                           {\
                               color:            black;\
                               background-color: lightGray;\
                               image:            url(:/pixmaps/dark-exit.png);\
                           }\
                           \
                           QGroupBox\
                           {\
                               border:        2px solid gray;\
                               border-radius: 5px;\
                               border-color:  #a6a2a6;\
                               margin-top:    1ex;\
                               font-weight:   bold;\
                               background:    transparent;\
                           }\
                           PlaybackQGroupBox#DeckGBox[selected=\"true\"]\
                           {\
                               background:    qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #595959, stop: 1 #333333);\
                           }\
                           PlaybackQGroupBox#SamplerGBox[selected=\"true\"]\
                           {\
                               background:    qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #333333, stop: 1 #262626);\
                           }\
                           PlaybackQGroupBox[selected=\"false\"]\
                           {\
                               background:    transparent;\
                           }\
                           PlaybackQGroupBox#DeckGBox[hover=\"true\"]\
                           {\
                               border:    2px solid orange;\
                           }\
                           PlaybackQGroupBox#SamplerGBox[hover=\"true\"]\
                           {\
                               border:    2px solid orange;\
                           }\
                           PlaybackQGroupBox[hover=\"false\"]\
                           {\
                               border:    2px solid gray;\
                           }\
                           \
                           QGroupBox::title\
                           {\
                               color:               #bfbbbf;\
                               subcontrol-origin:   margin;\
                               subcontrol-position: top center;\
                               padding:             0 3px;\
                           }\
                           \
                           QTreeView\
                           {\
                               border:        none;\
                               padding:       2px;\
                           }\
                           QTreeView::item {\
                               background-color: black;\
                           }\
                           QTreeView::item:selected{\
                               background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6ea1f1, stop: 1 #567dbc);\
                           }\
                           QTreeView::branch {\
                               background-color: black;\
                           }\
                           QTreeView::branch:has-siblings:!adjoins-item {\
                               border-image: url(:/pixmaps/dark-branch_vline.png) 0;\
                           }\
                           QTreeView::branch:has-children:!has-siblings:closed,\
                           QTreeView::branch:closed:has-children:has-siblings {\
                               image: url(:/pixmaps/dark-branch_closed.png);\
                           }\
                           QTreeView::branch:open:has-children:!has-siblings,\
                           QTreeView::branch:open:has-children:has-siblings  {\
                               image: url(:/pixmaps/dark-branch_open.png);\
                           }\
                           QHeaderView::section {\
                               background-color: gray;\
                               color:            black;\
                               padding-left:     4px;\
                               border:           1px solid #6c6c6c;\
                           }\
                           \
                           QLabel#TrackName\
                           {\
                               font:        10pt;\
                               font-weight: bold;\
                           }\
                           QLabel#RemainingTime\
                           {\
                               font:        14pt;\
                               font-weight: bold;\
                           }\
                           QLabel#RemainingTimeMsec\
                           {\
                               font:       10pt;\
                           }\
                           \
                           QTabBar::tab\
                           {\
                               background:              black;\
                               border:                  2px solid gray;\
                               padding:                 5px; \
                               border-top-left-radius:  1px;\
                               border-top-right-radius: 1px;\
                           }\
                           QTabBar::tab:selected\
                           {\
                               background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #737373, stop: 1 black);\
                           }\
                           QTabWidget::pane\
                           {\
                               border: 2px solid gray;\
                           }\
                           QComboBox\
                           {\
                               border:        1px solid gray;\
                               border-radius: 1px;\
                               padding:       2px;\
                           }\
                           QLineEdit\
                           {\
                               border:        1px solid gray;\
                               border-radius: 1px;\
                           }\
                           ShortcutQLabel\
                           {\
                               padding:       2px;\
                               border:        1px solid gray;\
                               border-radius: 1px;\
                           }\
                           ShortcutQLabel:hover\
                           {\
                               border: 2px solid gray;\
                           }\
                           QSlider::groove:horizontal {\
                               border:     1px solid gray;\
                               height:     1px;\
                               background: black;\
                           }\
                           QSlider::handle:horizontal {\
                               background:    qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #737373, stop: 1 black);\
                               border:        1px solid gray;\
                               width:         10px;\
                               margin:        -9px 0;\
                               border-radius: 2px;\
                           }\
                           \
                           QGroupBox#Help\
                           {\
                               border:           none;\
                               border-radius:    0px;\
                               background-color: gray;\
                           }\
                           QLabel#Help\
                           {\
                               color: black;\
                           }\
                           \
                           QPushButton#Sampler_buttons\
                           {\
                               background-color: gray;\
                               border:           none;\
                               border-radius:    0px;\
                               height:           24px;\
                               width:            24px;\
                           }\
                           QPushButton#Sampler_buttons:hover\
                           {\
                               background-color: lightGray;\
                               border:           1px solid orange;\
                           }\
                           QPushButton#Sampler_buttons:checked\
                           {\
                               background-color: gray;\
                               border:           1px solid orange;\
                           }\
                          "

class PlaybackQGroupBox : public QGroupBox
{
   Q_OBJECT

   Q_PROPERTY(bool selected
              READ is_selected
              WRITE set_selected
              STORED true)
   private:
       bool l_selected;
       void set_selected(bool is_selected) { l_selected = is_selected; }

   protected:
       void mousePressEvent(QMouseEvent *in_mouse_event);
       void enterEvent(QEvent *in_event);
       void leaveEvent(QEvent *in_event);

   public:
       PlaybackQGroupBox(const QString &title);
       virtual ~PlaybackQGroupBox();
       bool is_selected() { return l_selected; }
       void redraw();

   signals:
       void selected();
       void hover();
       void unhover();
};

class Gui : QObject
{
    Q_OBJECT

 private:
    PlaybackQGroupBox             *deck1_gbox;
    PlaybackQGroupBox             *deck2_gbox;
    PlaybackQGroupBox             *sampler1_gbox;
    PlaybackQGroupBox             *sampler2_gbox;
    QLabel                        *deck1_track_name;
    QLabel                        *deck2_track_name;
    Remaining_time               **decks_remaining_time;
    Waveform                      *deck1_waveform;
    Waveform                      *deck2_waveform;
    Vertical_waveform             *deck1_vertical_waveform;
    Vertical_waveform             *deck2_vertical_waveform;
    QGraphicsView                 *deck1_view;
    QGraphicsView                 *deck2_view;
    QGraphicsScene                *deck1_scene;
    QWidget                       *window;
    Config_dialog                 *config_dialog;
    QString                        window_style;
    QTreeView                     *file_browser;
    QDialog                       *about_dialog;
    QDialog                       *error_dialog;
    Audio_track                   *at_1;
    Audio_track                   *at_2;
    Audio_track                  **at_1_samplers;
    Audio_track                  **at_2_samplers;
    unsigned short int             nb_samplers;
    Audio_file_decoding_process   *dec_1;
    Audio_file_decoding_process   *dec_2;
    Audio_file_decoding_process  **dec_1_samplers;
    Audio_file_decoding_process  **dec_2_samplers;
    QPushButton                  **sampler1_buttons_play;
    QPushButton                  **sampler1_buttons_stop;
    QLabel                       **sampler1_trackname;
    QLabel                       **sampler1_remainingtime;
    QPushButton                  **sampler2_buttons_play;
    QPushButton                  **sampler2_buttons_stop;
    QLabel                       **sampler2_trackname;
    QLabel                       **sampler2_remainingtime;
    Playback_parameters           *params_1;
    Playback_parameters           *params_2;
    Audio_track_playback_process  *playback;
    unsigned short int             nb_decks;
    Sound_card_access_rules       *sound_card;
    Audio_collection_model        *file_system_model;
    int                           *dscratch_ids;
    Application_settings          *settings;
    QShortcut                     *shortcut_switch_playback;
    QShortcut                     *shortcut_collapse_browser;
    QShortcut                     *shortcut_load_audio_file;
    QShortcut                     *shortcut_go_to_begin;
    QShortcut                     *shortcut_set_cue_point;
    QShortcut                     *shortcut_go_to_cue_point;
    QShortcut                     *shortcut_load_sample_file_1;
    QShortcut                     *shortcut_load_sample_file_2;
    QShortcut                     *shortcut_load_sample_file_3;
    QShortcut                     *shortcut_load_sample_file_4;
    QShortcut                     *shortcut_fullscreen;
    QShortcut                     *shortcut_help;
    QGroupBox                     *help_groupbox;
    QGroupBox                     *file_browser_gbox;

 public:
    Gui(Application_settings           *in_settings,
        Audio_track                    *in_at_1,
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
        int                            *in_dscratch_ids);
    virtual ~Gui();

 private:
    bool create_main_window();
    bool apply_main_window_style();
    bool set_file_browser_base_path(QString in_path);
    bool set_file_browser_title();
    bool restart_sound_card(short unsigned int in_nb_channels);
    bool apply_application_settings();
    void run_sampler_decoding_process(unsigned short int in_deck_index,
                                      unsigned short int in_sampler_index);
    void on_sampler_button_play_click(unsigned short int in_deck_index,
                                      unsigned short int in_sampler_index);
    void on_sampler_button_stop_click(unsigned short int in_deck_index,
                                      unsigned short int in_sampler_index);
    void highlight_deck_sampler_area(unsigned short int in_deck_index);
    void highlight_border_deck_sampler_area(unsigned short int in_deck_index,
                                            bool               switch_on);
    void resize_file_browser_columns();

 private slots:
    bool show_config_window();
    void set_fullscreen();
    void show_help();
    bool show_about_window();
    void done_about_window();
    bool show_error_window(QString in_error_message);
    void done_error_window();
    void run_audio_file_decoding_process();
    void run_sample_1_decoding_process();
    void run_sample_2_decoding_process();
    void run_sample_3_decoding_process();
    void run_sample_4_decoding_process();
    void set_sampler_1_1_text(QString in_text);
    void set_sampler_1_2_text(QString in_text);
    void set_sampler_1_3_text(QString in_text);
    void set_sampler_1_4_text(QString in_text);
    void set_sampler_2_1_text(QString in_text);
    void set_sampler_2_2_text(QString in_text);
    void set_sampler_2_3_text(QString in_text);
    void set_sampler_2_4_text(QString in_text);
    void on_sampler_button_1_1_play_click();
    void on_sampler_button_1_2_play_click();
    void on_sampler_button_1_3_play_click();
    void on_sampler_button_1_4_play_click();
    void on_sampler_button_2_1_play_click();
    void on_sampler_button_2_2_play_click();
    void on_sampler_button_2_3_play_click();
    void on_sampler_button_2_4_play_click();
    void on_sampler_button_1_1_stop_click();
    void on_sampler_button_1_2_stop_click();
    void on_sampler_button_1_3_stop_click();
    void on_sampler_button_1_4_stop_click();
    void on_sampler_button_2_1_stop_click();
    void on_sampler_button_2_2_stop_click();
    void on_sampler_button_2_3_stop_click();
    void on_sampler_button_2_4_stop_click();
    void set_remaining_time(unsigned int in_remaining_time, int in_deck_index);
    void set_sampler_remaining_time(unsigned int in_remaining_time, int in_deck_index, int in_sampler_index);
    void set_sampler_state(int in_deck_index, int in_sampler_index, bool in_state);
    void deck1_jump_to_position(float in_position); // 0.0 < Position < 1.0
    void deck2_jump_to_position(float in_position); // 0.0 < Position < 1.0
    void deck_go_to_begin();
    void deck_set_cue_point();
    void deck_go_to_cue_point();
    void switch_playback_selection();
    void select_playback_1();
    void select_playback_2();
    void hover_playback(int in_deck_index);
    void unhover_playback(int in_deck_index);
    void can_close();
    void on_file_browser_expand_collapse(QModelIndex);
    void sync_file_browser_to_audio_collection();
    void on_finished_analyze_audio_collection();
};

#endif /* GUI_H_ */
