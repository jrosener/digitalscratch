/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*------------------------------------------------------------------( gui.h )-*/
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

#include "gui/config_dialog.h"
#include "gui/waveform.h"
#include "app/application_settings.h"
#include "app/application_const.h"
#include "player/deck_playback_process.h"
#include "player/playback_parameters.h"
#include "player/control_and_playback_process.h"
#include "audiodev/sound_driver_access_rules.h"
#include "tracks/audio_track.h"
#include "tracks/audio_file_decoding_process.h"
#include "tracks/audio_collection_model.h"
#include "tracks/playlist.h"

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
       void mousePressEvent(QMouseEvent *event);
       void mouseReleaseEvent(QMouseEvent *event);

   public:
       SpeedQPushButton(const QString &title);
       virtual ~SpeedQPushButton();
       bool is_pressed() { return l_pressed; }
       void redraw();

   signals:
       void right_clicked();
};

class SpeedQLabel : public QLabel
{
   Q_OBJECT

   protected:
       void mouseReleaseEvent(QMouseEvent *event);

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
       void mousePressEvent(QMouseEvent *event);
       void enterEvent(QEvent *event);
       void leaveEvent(QEvent *event);
       void dragEnterEvent(QDragEnterEvent *event);
       void dropEvent(QDropEvent *event);

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
       QPushButton                  *thru_button;
       QLabel                       *key;
       Waveform                     *waveform;
       QHBoxLayout                  *remaining_time_layout;
       QLabel                       *rem_time_minus;
       QLabel                       *rem_time_min;
       QLabel                       *rem_time_sep1;
       QLabel                       *rem_time_sec;
       QLabel                       *rem_time_sep2;
       QLabel                       *rem_time_msec;
       QHBoxLayout                  *buttons_layout;
       QPushButton                  *timecode_button;
       QPushButton                  *manual_button;
       SpeedQLabel                  *speed;
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
       Deck(const QString &title, const QSharedPointer<Audio_track> &at);
       virtual ~Deck();
       void init_display();
       void set_key(const QString &key);

    public slots:
       void set_speed_mode_timecode();
       void set_speed_mode_manual();
};

class QSamplerContainerWidget : public QWidget
{
   Q_OBJECT

  protected:
       void dragEnterEvent(QDragEnterEvent *event);
       void dropEvent(QDropEvent *event);

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
       Sampler(const QString &title,
               const unsigned short int &nb_samplers);
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
       FileBrowserControlButtons(const unsigned short int &deck_index,
                                 const unsigned short int &nb_samplers);
       virtual ~FileBrowserControlButtons();
       void init_display();
       void hide_load_sample_buttons();
       void show_load_sample_buttons();
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
    unsigned short int                  selected_deck;

    // Samplers area.
    QWidget                            *samplers_container;
    QHBoxLayout                        *samplers_layout;
    QList<Sampler*>                     samplers;

    // File and folder browser area.
    QHBoxLayout                        *file_layout;
    QSplitter                          *browser_splitter;
    QGroupBox                          *file_browser_gbox;
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

    // Action menu.
    QPushButton                        *refresh_file_browser; // FIXME: rename into get_audio_keys
    QPushButton                        *show_hide_samplers_button;
    QPushButton                        *clear_tracklist_button;
    QPushButton                        *save_tracklist_button;

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
    QDialog                            *refresh_audio_collection_dialog;

    // Tracklist.
    QSharedPointer<Playlist>           tracklist;

    // Main processing elements.
    QList<QSharedPointer<Audio_track>>                         ats;
    QList<QList<QSharedPointer<Audio_track>>>                  at_samplers;
    unsigned short int                                         nb_samplers;
    QList<QSharedPointer<Audio_file_decoding_process>>         decs;
    QList<QList<QSharedPointer<Audio_file_decoding_process>>>  dec_samplers;
    QList<QSharedPointer<Playback_parameters>>                 params;
    QList<QSharedPointer<Timecode_control_process>>            tcode_controls;
    QList<QSharedPointer<Manual_control_process>>              manual_controls;
    QList<QSharedPointer<Deck_playback_process>>               playbacks;
    QSharedPointer<Sound_driver_access_rules>                  sound_card;
    QSharedPointer<Control_and_playback_process>               control_and_play;
    DSCRATCH_HANDLE                                           *dscratch_handles;
    Application_settings                                      *settings;

 public:
    Gui(QList<QSharedPointer<Audio_track>>                        &ats,
        QList<QList<QSharedPointer<Audio_track>>>                 &at_samplers,
        QList<QSharedPointer<Audio_file_decoding_process>>        &decs,
        QList<QList<QSharedPointer<Audio_file_decoding_process>>> &dec_samplers,
        QList<QSharedPointer<Playback_parameters>>                &params,
        QList<QSharedPointer<Timecode_control_process>>           &tcode_controls,
        QList<QSharedPointer<Manual_control_process>>             &manual_controls,
        QList<QSharedPointer<Deck_playback_process>>              &playbacks,
        QSharedPointer<Sound_driver_access_rules>                 &sound_card,
        QSharedPointer<Control_and_playback_process>              &control_and_playback);
    virtual ~Gui();

 private:
    bool create_main_window();
    void init_header_buttons();
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
    void init_bottom_status();
    void display_audio_file_collection();
    bool apply_main_window_style();
    bool set_file_browser_playlist_tracks(const Playlist &playlist);
    bool set_file_browser_base_path(const QString &path);
    bool set_folder_browser_base_path(const QString &path);
    bool set_file_browser_title(const QString &title);
    bool apply_application_settings();
    void highlight_deck_sampler_area(const unsigned short int &deck_index);
    void highlight_border_deck_sampler_area(const unsigned short int &deck_index, const bool &switch_on);
    unsigned short int get_selected_deck_index();
    void resize_file_browser_columns();
    void analyze_audio_collection(const bool &is_all_files);
    void set_help_shortcut_value();
    bool can_stop_control_and_playback();
    void hide_samplers();
    void show_samplers();
    void add_track_path_to_tracklist(const unsigned short int &deck_index);
    void write_tracklist();

 public slots:
    void force_close();

 private slots:
    bool show_config_window();
    void set_fullscreen();
    void show_help();
    void set_focus_search_bar();
    void press_enter_in_search_bar();
    void press_esc_in_search_bar();
    void file_search_string(const QString &text);
    bool show_about_window();
    bool show_refresh_audio_collection_dialog();
    void reject_refresh_audio_collection_dialog();
    void close_refresh_audio_collection_dialog();
    void accept_refresh_audio_collection_dialog_all_files();
    void accept_refresh_audio_collection_dialog_new_files();
    bool show_error_window(const QString &error_message);
    void select_and_run_audio_file_decoding_process(const unsigned short int &deck_index);
    void run_audio_file_decoding_process();
    void show_hide_samplers();
    void select_and_run_sample_decoding_process(const unsigned short int &deck_index,
                                                const unsigned short int &sampler_index);
    void run_sampler_decoding_process(const unsigned short int &sampler_index);
    void run_sampler_decoding_process(const unsigned short int &deck_index,
                                      const unsigned short int &sampler_index);
    void run_sampler_decoding_process_on_deck(const unsigned short int &deck_index,
                                              const unsigned short int &sampler_index);
    void set_sampler_text(const QString &text,
                          const unsigned short int &deck_index,
                          const unsigned short int &sampler_index);
    void sampler_button_play_clicked(const unsigned short int &deck_index,
                                     const unsigned short int &sampler_index);
    void sampler_button_stop_clicked(const unsigned short int &deck_index,
                                     const unsigned short int &sampler_index);
    void sampler_button_del_clicked(const unsigned short int &deck_index,
                                    const unsigned short int &sampler_index);
    void set_remaining_time(const unsigned int &remaining_time,
                            const unsigned short int &deck_index);
    void set_sampler_remaining_time(const unsigned int &remaining_time,
                                    const unsigned short int &deck_index,
                                    const unsigned short int &sampler_index);
    void set_sampler_state(const unsigned short int &deck_index,
                           const unsigned short int &sampler_index,
                           const bool &state);
    void jump_to_position(const float &position,
                          const unsigned short int &deck_index); // 0.0 < Position < 1.0
    void deck_go_to_begin();
    void go_to_begin(const unsigned short int &deck_index);
    void deck_set_cue_point(const unsigned short int &cue_point_number);
    void set_cue_point(const unsigned short int &deck_index,
                       const unsigned short int &cue_point_number);
    void deck_go_to_cue_point(const unsigned short int &cue_point_number);
    void go_to_cue_point(const unsigned short int &deck_index,
                         const unsigned short int &cue_point_number);
    void deck_del_cue_point(const unsigned short int &cue_point_number);
    void del_cue_point(const unsigned short int &deck_index,
                       const unsigned short int &cue_point_number);
    void switch_playback_selection();
    void select_playback(const unsigned short int &deck_index);
    void hover_playback(const unsigned short int &deck_index);
    void unhover_playback(const unsigned short int &deck_index);
    void on_file_browser_expand(QModelIndex);
    void on_file_browser_double_click(QModelIndex in_model_index);
    void sync_file_browser_to_audio_collection();
    void on_finished_analyze_audio_collection();
    void update_refresh_progress_value(const unsigned int &value);
    void select_and_show_next_keys(const unsigned short int &deck_index);
    void show_next_keys();
    void on_file_browser_header_click(const int &index);
    void on_progress_cancel_button_click();
    void start_control_and_playback();
    void stop_control_and_playback();
    void run_concurrent_read_collection_from_db();
    void update_speed_label(const float &speed, const unsigned short &deck_index);
    void speed_up_down(const float &speed_inc, const unsigned short int &deck_index);
    void speed_accel(const float &speed_inc, const unsigned short &deck_index);
    void speed_reset_to_100p(const unsigned short int &deck_index);
    void playback_thru(const unsigned short int &deck_index, const bool &on_off);
    void can_close();
    void show_save_tracklist_dialog();
    void show_clear_tracklist_dialog();
};
