/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*------------------------------------------------------------------( gui.h )-*/
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
/*                Creates GUI for DigitalScratch player                       */
/*                                                                            */
/*============================================================================*/

#pragma once

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
#include <QSplitter>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSharedPointer>

#include "config_dialog.h"
#include "audio_track.h"
#include "audio_file_decoding_process.h"
#include "audio_track_playback_process.h"
#include "playback_parameters.h"
#include "sound_driver_access_rules.h"
#include "waveform.h"
#include "remaining_time.h"
#include "application_settings.h"
#include "application_const.h"
#include "audio_collection_model.h"
#include "playlist.h"
#include "sound_capture_and_playback_process.h"

using namespace std;

#define XSTR(x) #x
#define STR(x) XSTR(x)

class SpeedQPushButton : public QPushButton
{
   Q_OBJECT

   Q_PROPERTY(bool pressed
              READ is_pressed
              WRITE set_pressed
              STORED true)
   private:
       bool l_pressed;
       void set_pressed(bool is_pressed) { l_pressed = is_pressed; }

   protected:
       void mousePressEvent(QMouseEvent *in_mouse_event);
       void mouseReleaseEvent(QMouseEvent *in_mouse_event);

   public:
       SpeedQPushButton(const QString &title);
       virtual ~SpeedQPushButton();
       bool is_pressed() { return l_pressed; }
       void redraw();

   signals:
       void right_clicked();
};

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
       void dragEnterEvent(QDragEnterEvent *in_event);
       void dropEvent(QDropEvent *in_event);

   public:
       PlaybackQGroupBox(const QString &title);
       virtual ~PlaybackQGroupBox();
       bool is_selected() { return l_selected; }
       void redraw();

   signals:
       void selected();
       void hover();
       void unhover();
       void file_dropped();
};

class Deck : public PlaybackQGroupBox
{
   Q_OBJECT

   public:
       QSharedPointer<Audio_track>   at;
       QLabel                       *track_name;
       QLabel                       *key;
       Waveform                     *waveform;
       QHBoxLayout                  *remaining_time_layout;
       Remaining_time               *remaining_time;
       QHBoxLayout                  *buttons_layout;
       QPushButton                  *timecode_button;
       QPushButton                  *manual_button;
       bool                          speed_mode;
       QLabel                       *speed;
       SpeedQPushButton             *speed_up_button;
       SpeedQPushButton             *speed_down_button;
       SpeedQPushButton             *accel_up_button;
       SpeedQPushButton             *accel_down_button;
       QPushButton                  *restart_button;
       QPushButton                 **cue_set_buttons;
       QPushButton                 **cue_play_buttons;
       QPushButton                 **cue_del_buttons;
       QLabel                      **cue_point_labels;

   private:
       Application_settings *settings;

   public:
       Deck(const QString &title, QSharedPointer<Audio_track> &in_at);
       virtual ~Deck();
       void init_display();
       void set_key(const QString& in_key);

    public slots:
       void set_speed_mode_timecode();
       void set_speed_mode_manual();
};

class QSamplerContainerWidget : public QWidget
{
   Q_OBJECT

  protected:
       void dragEnterEvent(QDragEnterEvent *in_event);
       void dropEvent(QDropEvent *in_event);

   public:
       QSamplerContainerWidget();
       virtual ~QSamplerContainerWidget();

   signals:
       void file_dropped_in_sampler();
};

class Sampler : public PlaybackQGroupBox
{
   Q_OBJECT

   public:
       QList<QPushButton*>              buttons_play;
       QList<QPushButton*>              buttons_stop;
       QList<QPushButton*>              buttons_del;
       QList<QLabel*>                   tracknames;
       QList<QLabel*>                   remaining_times;
       QList<QSamplerContainerWidget*>  drop_areas;

   private:
       unsigned short int    nb_samplers;
       Application_settings *settings;

   public:
       Sampler(const QString      &in_title,
               unsigned short int  in_nb_samplers);
       virtual ~Sampler();
       void init_display();
};

class FileBrowserControlButtons : public QHBoxLayout
{
   Q_OBJECT

   public:
       QPushButton         *load_track_on_deck_button;
       QPushButton         *show_next_key_from_deck_button;
       QList<QPushButton*>  load_sample_buttons;

   public:
       FileBrowserControlButtons(unsigned short int in_deck_index,
                                 unsigned short int in_nb_samplers);
       virtual ~FileBrowserControlButtons();
       void init_display();
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

class Gui : public QObject
{
    Q_OBJECT

 private:
    // Header area.
    QPushButton                        *config_button;
    QPushButton                        *fullscreen_button;
    QPushButton                        *stop_capture_button;
    QPushButton                        *logo;
    QPushButton                        *start_capture_button;
    QPushButton                        *help_button;
    QPushButton                        *quit_button;
    QHBoxLayout                        *header_layout;
    QShortcut                          *shortcut_fullscreen;
    QShortcut                          *shortcut_help;

    // Decks area.
    QHBoxLayout                        *decks_layout;
    QShortcut                          *shortcut_go_to_begin;
    QShortcut                         **shortcut_set_cue_points;
    QShortcut                         **shortcut_go_to_cue_points;
    QShortcut                          *shortcut_switch_playback;
    QList<Deck*>                        decks;

    // Samplers area.
    QWidget                            *samplers_container;
    QHBoxLayout                        *samplers_layout;
    QPushButton                        *show_hide_samplers_button;
    QList<Sampler*>                     samplers;

    // File and folder browser area.
    QHBoxLayout                        *file_layout;
    QSplitter                          *browser_splitter;
    QGroupBox                          *file_browser_gbox;
    QPushButton                        *refresh_file_browser;
    QList<FileBrowserControlButtons*>   file_browser_control_buttons;
    QFutureWatcher<void>               *watcher_parse_directory;

    // Folder browser.
    TreeViewIconProvider               *treeview_icon_provider;
    QFileSystemModel                   *folder_system_model;
    QTreeView                          *folder_browser;

    // Track browser.
    Audio_collection_model             *file_system_model;
    QTreeView                          *file_browser;
    QShortcut                          *shortcut_collapse_browser;
    QShortcut                          *shortcut_load_audio_file;
    QList<QShortcut*>                   shortcut_load_samples;
    QShortcut                          *shortcut_show_next_keys;
    QShortcut                          *shortcut_file_search;
    QShortcut                          *shortcut_file_search_press_enter;
    QShortcut                          *shortcut_file_search_press_esc;

    // Track search bar.
    QLineEdit                          *file_search;
    bool                                search_from_begin;
    unsigned int                        file_browser_selected_index;
    QString                             last_search_string;

    // Bottom help area.
    QHBoxLayout                        *bottom_layout;
    QGroupBox                          *help_groupbox;
    QLabel                             *help_fullscreen_value;
    QLabel                             *help_help_value;
    QLabel                             *help_switch_deck_value;
    QLabel                             *help_load_deck_value;
    QLabel                             *help_next_track_value;
    QLabel                             *help_cue_value;
    QLabel                             *help_sample_value;
    QLabel                             *help_browse_value1;
    QLabel                             *help_browse_value2;

    // Status area (progress bar).
    QHBoxLayout                        *status_layout;
    QProgressBar                       *progress_bar;
    QLabel                             *progress_label;
    QPushButton                        *progress_cancel_button;
    QGroupBox                          *progress_groupbox;

    // Main window.
    QWidget                            *window;
    QString                             window_style;
    bool                                is_window_rendered;
    unsigned short int                  nb_decks;

    // Pop dialogs.
    Config_dialog                      *config_dialog;
    QDialog                            *about_dialog;
    QDialog                            *refresh_audio_collection_dialog;
    QDialog                            *error_dialog;

    // Main processing elements.
    QList<QSharedPointer<Audio_track>>                         ats;
    QList<QList<QSharedPointer<Audio_track>>>                  at_samplers;
    unsigned short int                                         nb_samplers;
    QList<QSharedPointer<Audio_file_decoding_process>>         decs;
    QList<QList<QSharedPointer<Audio_file_decoding_process>>>  dec_samplers;
    QList<QSharedPointer<Playback_parameters>>                 params;
    QList<QSharedPointer<Audio_track_playback_process>>        playbacks;
    QSharedPointer<Sound_driver_access_rules>                  sound_card;
    QSharedPointer<Sound_capture_and_playback_process>         capture_and_play;
    int                                                       *dscratch_ids;
    Application_settings                                      *settings;

 public:
    Gui(QList<QSharedPointer<Audio_track>>                        &in_ats,
        QList<QList<QSharedPointer<Audio_track>>>                 &in_at_samplers,
        QList<QSharedPointer<Audio_file_decoding_process>>        &in_decs,
        QList<QList<QSharedPointer<Audio_file_decoding_process>>> &in_dec_samplers,
        QList<QSharedPointer<Playback_parameters>>                &in_params,
        QList<QSharedPointer<Audio_track_playback_process>>       &in_playbacks,
        QSharedPointer<Sound_driver_access_rules>                 &in_sound_card,
        QSharedPointer<Sound_capture_and_playback_process>        &in_capture_and_playback,
        int                                                       *in_dscratch_ids);
    virtual ~Gui();

 private:
    bool create_main_window();
    void init_header_buttons();
    void clean_header_buttons();
    void connect_header_buttons();
    void init_decks_area();
    void clean_decks_area();
    void connect_decks_area();
    void init_samplers_area();
    void clean_samplers_area();
    void connect_samplers_area();
    void connect_decks_and_samplers_selection();
    void init_file_browser_area();
    void clean_file_browser_area();
    void connect_file_browser_area();
    void init_bottom_help();
    void clean_bottom_help();
    void init_bottom_status();
    void clean_bottom_status();
    void display_audio_file_collection();
    bool apply_main_window_style();
    bool set_file_browser_playlist_tracks(Playlist *in_playlist);
    bool set_file_browser_base_path(QString in_path);
    bool set_folder_browser_base_path(QString in_path);
    bool set_file_browser_title(QString in_title);
    bool restart_sound_card(short unsigned int in_nb_channels);
    bool apply_application_settings();
    void highlight_deck_sampler_area(unsigned short int in_deck_index);
    void highlight_border_deck_sampler_area(unsigned short int in_deck_index, bool switch_on);
    unsigned short int get_selected_deck_index();
    void resize_file_browser_columns();
    void analyze_audio_collection(bool is_all_files);
    void set_help_shortcut_value();
    bool can_stop_capture_and_playback();
    void hide_samplers();
    void show_samplers();

 public slots:
    void run_sampler_decoding_process(unsigned short int in_deck_index,
                                      unsigned short int in_sampler_index);

 private slots:
    bool show_config_window();
    void set_fullscreen();
    void show_help();
    void set_focus_search_bar();
    void press_enter_in_search_bar();
    void press_esc_in_search_bar();
    void file_search_string(QString in_text);
    bool show_about_window();
    void done_about_window();
    bool show_refresh_audio_collection_dialog();
    void reject_refresh_audio_collection_dialog();
    void close_refresh_audio_collection_dialog();
    void accept_refresh_audio_collection_dialog_all_files();
    void accept_refresh_audio_collection_dialog_new_files();
    bool show_error_window(QString in_error_message);
    void done_error_window();
    void select_and_run_audio_file_decoding_process(unsigned short int in_deck_index);
    void run_audio_file_decoding_process();
    void show_hide_samplers();
    void select_and_run_sample_decoding_process(unsigned short int in_deck_index,
                                                unsigned short int in_sampler_index);
    void run_sampler_decoding_process(unsigned short int in_sampler_index);
    void run_sampler_decoding_process_on_deck(unsigned short int in_deck_index,
                                              unsigned short int in_sampler_index);
    void set_sampler_text(QString in_text, unsigned short int in_deck_index, unsigned short int in_sampler_index);
    void on_sampler_button_play_click(unsigned short int in_deck_index,
                                      unsigned short int in_sampler_index);
    void on_sampler_button_stop_click(unsigned short int in_deck_index,
                                      unsigned short int in_sampler_index);
    void on_sampler_button_del_click(unsigned short int in_deck_index,
                                     unsigned short int in_sampler_index);
    void set_remaining_time(unsigned int in_remaining_time, int in_deck_index);
    void set_sampler_remaining_time(unsigned int in_remaining_time, int in_deck_index, int in_sampler_index);
    void set_sampler_state(int in_deck_index, int in_sampler_index, bool in_state);
    void jump_to_position(float in_position, unsigned short int in_deck_index); // 0.0 < Position < 1.0
    void deck_go_to_begin();
    void go_to_begin(unsigned short int in_deck_index);
    void deck_set_cue_point(int in_cue_point_number);
    void set_cue_point(unsigned short int in_deck_index, int in_cue_point_number);
    void deck_go_to_cue_point(int in_cue_point_number);
    void go_to_cue_point(unsigned short int in_deck_index, int in_cue_point_number);
    void deck_del_cue_point(int in_cue_point_number);
    void del_cue_point(unsigned short int in_deck_index, int in_cue_point_number);
    void switch_playback_selection();
    void select_playback(int in_deck_index);
    void hover_playback(int in_deck_index);
    void unhover_playback(int in_deck_index);
    void can_close();
    void on_file_browser_expand(QModelIndex);
    void on_file_browser_double_click(QModelIndex in_model_index);
    void sync_file_browser_to_audio_collection();
    void on_finished_analyze_audio_collection();
    void update_refresh_progress_value(int in_value);
    void select_and_show_next_keys(unsigned short int in_deck_index);
    void show_next_keys();
    void on_file_browser_header_click(int in_index);
    void on_progress_cancel_button_click();
    void start_control_and_playback();
    void stop_control_and_playback();
    void run_concurrent_read_collection_from_db();
    void update_speed_label(float in_speed, int in_deck_index);
    void speed_up_down(float in_speed_inc, int in_deck_index);
};
