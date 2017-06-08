/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*-----------------------------------------------( audio_collection_model.h )-*/
/*                                                                            */
/*  Copyright (C) 2003-2017                                                   */
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
/*                    Class defining a data model for audio files             */
/*                                                                            */
/*============================================================================*/

#pragma once

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QList>
#include <QDir>
#include <QFuture>
#include <QFutureWatcher>
#include <QPixmap>
#include <QList>
#include <QSharedPointer>

#include "tracks/playlist.h"

using namespace std;

#define COLUMN_FILE_NAME 0
#define COLUMN_KEY       1
#define COLUMN_PATH      2

class Audio_collection_item
{
 public:
    QList<Audio_collection_item*>  childItems;
 private:
    QList<QVariant>                itemData;
    Audio_collection_item         *parentItem;
    QString                        fullPath;
    bool                           directoryFlag;
    QString                        fileHash;
    bool                           next_key;
    bool                           next_major_key;

 public:
    Audio_collection_item(const QList<QVariant>       &in_data,
                                QString                in_file_hash    = "",
                                QString                in_full_path    = "",
                                bool                   in_is_directory = false,
                                Audio_collection_item *in_parent       = 0);
    ~Audio_collection_item();

    void                   append_child(Audio_collection_item *in_item);
    Audio_collection_item *get_child(int in_row);
    int                    get_child_count() const;
    Audio_collection_item *get_parent();

    int                    get_row() const;
    int                    get_column_count() const;
    QVariant               get_data(int in_column) const;
    void                   set_data(int in_column, QVariant in_data);
    QString                get_full_path();
    QString                get_file_hash();

    void                   read_from_db();
    void                   compute_and_store_to_db();

    bool                   is_directory();

    bool                   is_a_next_key();
    bool                   is_a_next_major_key();
    void                   set_next_key(bool is_a_next_key);
    void                   set_next_major_key(bool is_a_next_major_key);

 private:
    void calculate_audio_data();     // Compute music key, bpm, etc...
    void store_to_db();              // Persist to DB.
};

class Audio_collection_model : public QAbstractItemModel
{
 public:
    QSharedPointer<QFutureWatcher<void>>  concurrent_watcher_read;
    QSharedPointer<QFutureWatcher<void>>  concurrent_watcher_store;

 private:
    Audio_collection_item         *rootItem;
    QSharedPointer<QFuture<void>>  concurrent_future;
    QPixmap                        audio_file_icon;
    QPixmap                        directory_icon;
    QList<Audio_collection_item*>  audio_item_list;
    QString                        root_path;

 public:
    explicit Audio_collection_model(QObject *in_parent = 0);
    ~Audio_collection_model();

    QModelIndex   set_root_path(QString in_root_path);
    QString       get_root_path();
    QModelIndex   set_playlist(const Playlist &playlist);
    QModelIndex   get_root_index();

    QVariant      data(const QModelIndex &in_index, int in_role) const;
    Qt::ItemFlags flags(const QModelIndex &in_index) const;
    QVariant      headerData(int in_section, Qt::Orientation in_orientation, int in_role = Qt::DisplayRole) const;
    QModelIndex   index(int in_row, int in_column, const QModelIndex &in_parent = QModelIndex()) const;
    QModelIndex   parent(const QModelIndex &in_index) const;
    QModelIndex   parent_from_item(Audio_collection_item &in_item) const;
    int           rowCount(const QModelIndex &in_parent = QModelIndex()) const;
    int           columnCount(const QModelIndex &in_parent = QModelIndex()) const;
    void          sort(int in_column, Qt::SortOrder in_order);
    QStringList   mimeTypes() const;
    QMimeData    *mimeData(const QModelIndexList &in_indexes) const;

    void concurrent_read_collection_from_db();                  // Call Audio_collection_item::read_from_db() on all collection in separate threads.
    void stop_concurrent_read_collection_from_db();             // Stop concurrent_read_collection_from_db().
    void concurrent_analyse_audio_collection();                 // Call Audio_collection_item::compute_and_store_to_db() on all collection in separate threads.
    void stop_concurrent_analyse_audio_collection();            // Stop concurrent_analyse_audio_collection().
    int  get_nb_items();                                        // Get number of files.
    int  get_nb_new_items();                                    // Get number of new files (i.e. files with missing data such as music key).
    QList<QModelIndex> set_next_keys(QString in_next_key,       // Set flags for previous/next keys (for all items).
                                     QString in_previous_key,
                                     QString in_next_major_key);

    void set_icons(QPixmap in_audio_file_icon,
                   QPixmap in_directory_icon);
    QModelIndexList search(QString in_text);
    void clear();

 private:
    void setup_model_data(QString in_path, Audio_collection_item *in_item);
    void setup_model_data_from_tracklist(QStringList in_tracklist, Audio_collection_item *in_item);
    void create_header(QString in_path, bool in_show_path);
};
