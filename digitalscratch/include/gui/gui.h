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
#include <QProgressBar>
#include <QFileIconProvider>

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
#include "application_const.h"
#include "audio_collection_model.h"
#include "playlist.h"
#include "sound_capture_and_playback_process.h"

using namespace std;

#define XSTR(x) #x
#define STR(x) XSTR(x)

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

class TreeViewIconProvider : public QFileIconProvider
{
    public:
        TreeViewIconProvider();
        virtual ~TreeViewIconProvider();

    private:
        QIcon drive;
        QIcon folder;
        QIcon file;
        QIcon other;

    public:
        QIcon   icon(IconType type) const;
        QIcon   icon(const QFileInfo &info) const;
        QString type(const QFileInfo &info) const;
        void    set_icons(const QIcon &drive,
                          const QIcon &folder,
                          const QIcon &file,
                          const QIcon &other);
        void    set_default_icons();
};

class Gui : QObject
{
    Q_OBJECT

 private:
    PlaybackQGroupBox                  *deck1_gbox;
    PlaybackQGroupBox                  *deck2_gbox;
    PlaybackQGroupBox                  *sampler1_gbox;
    PlaybackQGroupBox                  *sampler2_gbox;
    QLabel                             *deck1_track_name;
    QLabel                             *deck2_track_name;
    Remaining_time                    **decks_remaining_time;
    QLabel                             *deck1_key;
    QLabel                             *deck2_key;
    Waveform                           *deck1_waveform;
    Waveform                           *deck2_waveform;
    Vertical_waveform                  *deck1_vertical_waveform;
    Vertical_waveform                  *deck2_vertical_waveform;
    QGraphicsView                      *deck1_view;
    QGraphicsView                      *deck2_view;
    QGraphicsScene                     *deck1_scene;
    QWidget                            *window;
    Config_dialog                      *config_dialog;
    QString                             window_style;
    QTreeView                          *file_browser;
    QTreeView                          *folder_browser;
    QDialog                            *about_dialog;
    QDialog                            *refresh_audio_collection_dialog;
    QDialog                            *error_dialog;
    Audio_track                        *at_1;
    Audio_track                        *at_2;
    Audio_track                       **at_1_samplers;
    Audio_track                       **at_2_samplers;
    unsigned short int                  nb_samplers;
    Audio_file_decoding_process        *dec_1;
    Audio_file_decoding_process        *dec_2;
    Audio_file_decoding_process       **dec_1_samplers;
    Audio_file_decoding_process       **dec_2_samplers;
    QPushButton                       **sampler1_buttons_play;
    QPushButton                       **sampler1_buttons_stop;
    QLabel                            **sampler1_trackname;
    QLabel                            **sampler1_remainingtime;
    QPushButton                       **sampler2_buttons_play;
    QPushButton                       **sampler2_buttons_stop;
    QLabel                            **sampler2_trackname;
    QLabel                            **sampler2_remainingtime;
    Playback_parameters                *params_1;
    Playback_parameters                *params_2;
    Audio_track_playback_process       *playback;
    unsigned short int                  nb_decks;
    Sound_card_access_rules            *sound_card;
    Sound_capture_and_playback_process *capture_and_play;
    QFileSystemModel                   *folder_system_model;
    Audio_collection_model             *file_system_model;
    TreeViewIconProvider               *treeview_icon_provider;
    int                                *dscratch_ids;
    Application_settings               *settings;
    QShortcut                          *shortcut_switch_playback;
    QShortcut                          *shortcut_collapse_browser;
    QShortcut                          *shortcut_load_audio_file;
    QShortcut                          *shortcut_go_to_begin;
    QShortcut                          *shortcut_get_next_audio_tracks;
    QShortcut                          *shortcut_set_cue_point1;
    QShortcut                          *shortcut_go_to_cue_point1;
    QShortcut                          *shortcut_load_sample_file_1;
    QShortcut                          *shortcut_load_sample_file_2;
    QShortcut                          *shortcut_load_sample_file_3;
    QShortcut                          *shortcut_load_sample_file_4;
    QShortcut                          *shortcut_show_next_keys;
    QShortcut                          *shortcut_fullscreen;
    QShortcut                          *shortcut_help;
    QGroupBox                          *help_groupbox;
    QGroupBox                          *file_browser_gbox;
    QPushButton                        *refresh_file_browser;
    QProgressBar                       *progress_bar;
    QLabel                             *progress_label;
    QPushButton                        *progress_cancel_button;
    QGroupBox                          *progress_groupbox;
    QPushButton                        *load_track_on_deck1_button;
    QPushButton                        *load_track_on_deck2_button;
    QPushButton                        *restart_on_deck1_button;
    QPushButton                        *restart_on_deck2_button;
    QPushButton                       **cue_set_on_deck1_buttons;
    QPushButton                       *cue_play_on_deck1_button1;
    QLabel                            **cue_point_deck1_labels;
    QPushButton                        *cue_set_on_deck2_button1;
    QPushButton                        *cue_play_on_deck2_button1;
    QLabel                             *cue_point_label1_deck2;
    QPushButton                        *show_next_key_from_deck1_button;
    QPushButton                        *show_next_key_from_deck2_button;
    QPushButton                        *load_sample1_1_button;
    QPushButton                        *load_sample2_1_button;
    QPushButton                        *load_sample1_2_button;
    QPushButton                        *load_sample2_2_button;
    QPushButton                        *load_sample1_3_button;
    QPushButton                        *load_sample2_3_button;
    QPushButton                        *load_sample1_4_button;
    QPushButton                        *load_sample2_4_button;
    QLabel                             *help_fullscreen_value;
    QLabel                             *help_help_value;
    QLabel                             *help_switch_deck_value;
    QLabel                             *help_load_deck_value;
    QLabel                             *help_next_track_value;
    QLabel                             *help_cue_value;
    QLabel                             *help_sample_value;
    QLabel                             *help_browse_value1;
    QLabel                             *help_browse_value2;
    QPushButton                        *start_capture_button;
    QPushButton                        *stop_capture_button;

 public:
    Gui(Audio_track                        *in_at_1,
        Audio_track                        *in_at_2,
        Audio_track                      ***in_at_samplers,
        unsigned short int                  in_nb_samplers,
        Audio_file_decoding_process        *in_dec_1,
        Audio_file_decoding_process        *in_dec_2,
        Audio_file_decoding_process      ***in_dec_samplers,
        Playback_parameters                *in_params_1,
        Playback_parameters                *in_params_2,
        Audio_track_playback_process       *in_playback,
        unsigned short int                  in_nb_decks,
        Sound_card_access_rules            *in_sound_card,
        Sound_capture_and_playback_process *in_capture_and_playback,
        int                                *in_dscratch_ids);
    virtual ~Gui();

 private:
    bool    create_main_window();
    bool    apply_main_window_style();
    bool    set_file_browser_playlist_tracks(Playlist *in_playlist);
    bool    set_file_browser_base_path(QString in_path);
    bool    set_folder_browser_base_path(QString in_path);
    bool    set_file_browser_title(QString in_title);
    bool    restart_sound_card(short unsigned int in_nb_channels);
    bool    apply_application_settings();
    void    run_sampler_decoding_process(unsigned short int in_deck_index,
                                         unsigned short int in_sampler_index);
    void    on_sampler_button_play_click(unsigned short int in_deck_index,
                                         unsigned short int in_sampler_index);
    void    on_sampler_button_stop_click(unsigned short int in_deck_index,
                                         unsigned short int in_sampler_index);
    void    highlight_deck_sampler_area(unsigned short int in_deck_index);
    void    highlight_border_deck_sampler_area(unsigned short int in_deck_index,
                                               bool               switch_on);
    void    resize_file_browser_columns();
    void    analyze_audio_collection(bool is_all_files);
    void    set_help_shortcut_value();
    bool    can_stop_capture_and_playback();

 private slots:
    bool show_config_window();
    void set_fullscreen();
    void show_help();
    bool show_about_window();
    void done_about_window();
    bool show_refresh_audio_collection_dialog();
    void reject_refresh_audio_collection_dialog();
    void close_refresh_audio_collection_dialog();
    void accept_refresh_audio_collection_dialog_all_files();
    void accept_refresh_audio_collection_dialog_new_files();
    bool show_error_window(QString in_error_message);
    void done_error_window();
    void select_and_run_audio_file_decoding_process_deck1();
    void select_and_run_audio_file_decoding_process_deck2();
    void run_audio_file_decoding_process();
    void select_and_run_sample1_decoding_process_deck1();
    void select_and_run_sample2_decoding_process_deck1();
    void select_and_run_sample3_decoding_process_deck1();
    void select_and_run_sample4_decoding_process_deck1();
    void select_and_run_sample1_decoding_process_deck2();
    void select_and_run_sample2_decoding_process_deck2();
    void select_and_run_sample3_decoding_process_deck2();
    void select_and_run_sample4_decoding_process_deck2();
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
    void deck1_go_to_begin();
    void deck2_go_to_begin();
    void deck_set_cue_point(int in_cue_point_number);
    void deck1_set_cue_point(int in_cue_point_number);
    void deck2_set_cue_point(int in_cue_point_number);
    void deck_go_to_cue_point();
    void deck1_go_to_cue_point();
    void deck2_go_to_cue_point();
    void switch_playback_selection();
    void select_playback_1();
    void select_playback_2();
    void hover_playback(int in_deck_index);
    void unhover_playback(int in_deck_index);
    void can_close();
    void on_file_browser_expand(QModelIndex);
    void on_file_browser_double_click(QModelIndex in_model_index);
    void sync_file_browser_to_audio_collection();
    void on_finished_analyze_audio_collection();
    void update_refresh_progress_value(int in_value);
    void select_and_show_next_keys_deck1();
    void select_and_show_next_keys_deck2();
    void show_next_keys();
    void on_file_browser_header_click(int in_index);
    void on_progress_cancel_button_click();
    void start_capture_and_playback();
    void stop_capture_and_playback();
};

#endif /* GUI_H_ */
