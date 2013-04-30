/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*-----------------------------------------------( audio_collection_model.h )-*/
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
/*                    Class defining a data model for audio files             */
/*                                                                            */
/*============================================================================*/

#ifndef AUDIO_COLLECTION_MODEL_H_
#define AUDIO_COLLECTION_MODEL_H_

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QList>
#include <QDir>
#include <QFuture>
#include <QFutureWatcher>
#include <QPixmap>

using namespace std;

#define COLUMN_FILE_NAME 0
#define COLUMN_KEY       1

class Audio_collection_item
{
 private:
    QList<Audio_collection_item*>  childItems;
    QList<QVariant>                itemData;
    Audio_collection_item         *parentItem;
    QString                        fullPath;
    bool                           directoryFlag;

 public:
    Audio_collection_item(const QList<QVariant>       &in_data,
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

    bool                   is_directory();
};

class Audio_collection_model : public QAbstractItemModel
{
    Q_OBJECT

 public:
    QFutureWatcher<void>  *concurrent_watcher_read;
    QFutureWatcher<void>  *concurrent_watcher_store;

 private:
    Audio_collection_item *rootItem;
    QFuture<void>         *concurrent_future;
    QPixmap                audio_file_icon;
    QPixmap                directory_icon;

 public:
    Audio_collection_model(QObject *in_parent = 0);
    ~Audio_collection_model();

    QModelIndex   set_root_path(QString in_root_path);
    QModelIndex   get_root_index();

    QVariant      data(const QModelIndex &in_index, int in_role) const;
    Qt::ItemFlags flags(const QModelIndex &in_index) const;
    QVariant      headerData(int in_section, Qt::Orientation in_orientation, int in_role = Qt::DisplayRole) const;
    QModelIndex   index(int in_row, int in_column, const QModelIndex &in_parent = QModelIndex()) const;
    QModelIndex   parent(const QModelIndex &in_index) const;
    int           rowCount(const QModelIndex &in_parent = QModelIndex()) const;
    int           columnCount(const QModelIndex &in_parent = QModelIndex()) const;

    void concurrent_read_collection_from_db(); // Call read_collection_from_db() in separate thread.
    void read_collection_from_db(Audio_collection_item *in_parent_item = NULL); 

    void concurrent_analyse_audio_collection(); // Call analyze_audio_collection in separate thread.
    void analyze_audio_collection(); // Call calculate_audio_collection_data() and store_collection_to_db().
    void calculate_audio_collection_data(Audio_collection_item *in_parent_item = NULL); // Compute music key, etc...
    void store_collection_to_db(Audio_collection_item *in_parent_item = NULL);

    void set_icons(QPixmap &in_audio_file_icon,
                   QPixmap &in_directory_icon);

 private:
    void setup_model_data(QString in_path, Audio_collection_item *in_item);
    void create_header();
};

#endif /* AUDIO_COLLECTION_MODEL_H_ */
