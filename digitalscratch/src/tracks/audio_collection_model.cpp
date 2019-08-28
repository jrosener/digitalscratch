/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*---------------------------------------------( audio_collection_model.cpp )-*/
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

#include <QtDebug>
#include <QStringList>
#include <QTextCodec>
#include <iostream>
#include <QPixmap>
#include <QtConcurrentMap>
#include <QMimeData>
#include <QCoreApplication>

#include "app/application_settings.h"
#include "app/application_const.h"
#include "app/application_logging.h"
#include "tracks/audio_collection_model.h"
#include "tracks/audio_track.h"
#include "tracks/data_persistence.h"
#include "utils.h"
#include "singleton.h"

Audio_collection_item::Audio_collection_item(const QList<QVariant> &in_data,
                                             QString                in_file_hash,
                                             QString                in_full_path,
                                             bool                   in_is_directory,
                                             Audio_collection_item *in_parent)
{
    this->parentItem     = in_parent;
    this->fileHash       = in_file_hash;
    this->itemData       = in_data;
    this->fullPath       = in_full_path;
    this->directoryFlag  = in_is_directory;
    this->next_key       = false;
    this->next_major_key = false;
}

Audio_collection_item::~Audio_collection_item()
{
    qDeleteAll(this->childItems);
}

void Audio_collection_item::append_child(Audio_collection_item *in_item)
{
    this->childItems.append(in_item);
}

Audio_collection_item *Audio_collection_item::get_child(int in_row)
{
    return this->childItems.value(in_row);
}

int Audio_collection_item::get_child_count() const
{
    return this->childItems.count();
}

int Audio_collection_item::get_column_count() const
{
    return this->itemData.count();
}

QVariant Audio_collection_item::get_data(int in_column) const
{
    return this->itemData.value(in_column);
}

void Audio_collection_item::set_data(int in_column, QVariant in_data)
{
    if (in_column < this->itemData.size())
    {
        this->itemData.replace(in_column, in_data);
    }
}

Audio_collection_item *Audio_collection_item::get_parent()
{
    return this->parentItem;
}

int Audio_collection_item::get_row() const
{
    if (this->parentItem != nullptr)
    {
        return this->parentItem->childItems.indexOf(const_cast<Audio_collection_item*>(this));
    }

    return 0;
}

QString Audio_collection_item::get_full_path()
{
    return this->fullPath;
}

QString Audio_collection_item::get_file_hash() const
{
    return this->fileHash;
}

bool Audio_collection_item::is_directory()
{
    return this->directoryFlag;
}

bool Audio_collection_item::is_a_next_key()
{
    return this->next_key;
}

bool Audio_collection_item::is_a_next_major_key()
{
    return this->next_major_key;
}

void Audio_collection_item::set_next_key(bool is_a_next_key)
{
    this->next_key = is_a_next_key;
}

void Audio_collection_item::set_next_major_key(bool is_a_next_major_key)
{
    this->next_major_key = is_a_next_major_key;
}

bool Audio_collection_item::read_from_db()
{
    // Init.
    Data_persistence *data_persist = &Singleton<Data_persistence>::get_instance();
    QSharedPointer<Audio_track> at(new Audio_track(44100));

    // Get the hash of audio file.
    at->reset();
    at->set_hash(this->get_file_hash());

    // Get audio data from db and put it back to the item.
    if (data_persist->get_audio_track(at) == true)
    {
        // File found in DB, put data back to item.
        this->set_data(COLUMN_KEY, at->get_music_key());
        this->set_data(COLUMN_TAGS, at->get_tags());
    }
    else
    {
        qCDebug(DS_FILE) << "audio track not found in DB: " << this->get_full_path();
        return false;
    }

    return true;
}

void Audio_collection_item::compute_audio_characteristics()
{
    Application_settings *settings = &Singleton<Application_settings>::get_instance();

    // Check in application settings if we should analyze only files with missing data.
    if ((settings->get_audio_collection_full_refresh() == true) ||
        ((settings->get_audio_collection_full_refresh() == false) && (this->get_data(COLUMN_KEY) == "")))
    {
        // Calculate music key.
        this->calculate_music_key();
    }
}

void Audio_collection_item::calculate_music_key()
{
    // Calculate data and put them back in current audio item.
    this->set_data(COLUMN_KEY, Utils::get_file_music_key(this->fullPath));
}

void Audio_collection_item::set_tag_list(const QStringList &tags)
{
    this->set_data(COLUMN_TAGS, tags);
}

void Audio_collection_item::store_to_db()
{
    // Init.
    Data_persistence *data_persist = &Singleton<Data_persistence>::get_instance();
    QSharedPointer<Audio_track> at(new Audio_track(44100));

    // Get a hash, set audio data to an Audio_track and persist it.
    at->reset();
    at->set_hash(this->get_file_hash());
    at->set_fullpath(this->get_full_path());
    at->set_music_key(this->get_data(COLUMN_KEY).toString());
    if (data_persist->store_audio_track(at) == false)
    {
        qCWarning(DS_DB) << "can not store" << this->get_full_path() << "to DB";
    }
}

Audio_collection_model::Audio_collection_model(QObject *in_parent) : QAbstractItemModel(in_parent)
{
    this->rootItem = nullptr;
    this->create_header("", false);
    this->audio_item_list.clear();
    this->root_path = "";

    // Init thread tools.
    this->concurrent_future = QSharedPointer<QFuture<void>>(new QFuture<void>);
    this->concurrent_watcher_read  = QSharedPointer<QFutureWatcher<void>>(new QFutureWatcher<void>);
    this->concurrent_watcher_analyze = QSharedPointer<QFutureWatcher<void>>(new QFutureWatcher<void>);
}

Audio_collection_model::~Audio_collection_model()
{
    if (this->rootItem != nullptr)
    {
        delete this->rootItem;
    }

    // Stop running threads.
    if (this->concurrent_watcher_analyze->isStarted() == true)
    {
        this->concurrent_watcher_analyze->cancel();
        this->concurrent_watcher_analyze->waitForFinished();
    }
    if (this->concurrent_watcher_read->isStarted() == true)
    {
        this->concurrent_watcher_read->cancel();
        this->concurrent_watcher_read->waitForFinished();
    }
}

void Audio_collection_model::set_icons(QPixmap in_audio_file_icon,
                                       QPixmap in_directory_icon)
{
    this->audio_file_icon = in_audio_file_icon;
    this->directory_icon  = in_directory_icon;
}

void Audio_collection_model::create_header(QString in_path, bool in_show_path)
{
    // Create root item which is the collection header.
    QList<QVariant> rootData;
    rootData << tr("Track") << tr("Key") << tr("Tags");
    if (in_show_path == true)
    {
        rootData << tr("Path");
    }

    if (this->rootItem != nullptr)
    {
        delete this->rootItem;
    }

    this->rootItem = new Audio_collection_item(rootData, "", in_path, false, 0);
}

QModelIndex Audio_collection_model::set_root_path(QString in_root_path)
{
    // Start cleaning collection.
    this->beginResetModel();

    // Create root item which is the collection header.
    this->create_header(in_root_path, true);

    // Reset internal list of audio files (item pointers).
    this->audio_item_list.clear();

    // Fill the model.
    this->setup_model_data(in_root_path, this->rootItem);

    // Store root path.
    this->root_path = in_root_path;

    // Model has been updated.
    this->endResetModel();

    return this->get_root_index();
}

QString Audio_collection_model::get_root_path()
{
    return this->root_path;
}

QModelIndex Audio_collection_model::set_playlist(const Playlist &playlist)
{
    // Start cleaning collection.
    this->beginResetModel();

    // Create root item which is the collection header.
    this->create_header(playlist.get_basepath(), true);

    // Reset internal list of audio files (item pointers).
    this->audio_item_list.clear();

    // Fill the model.
    this->setup_model_data_from_tracklist(playlist.get_tracklist(), this->rootItem);

    // Model has been updated.
    this->endResetModel();

    return this->get_root_index();
}

QModelIndex Audio_collection_model::get_root_index()
{
    if (this->rootItem != nullptr)
    {
        return this->createIndex(0, 0, this->rootItem);
    }
    else
    {
        return QModelIndex();
    }
}

int Audio_collection_model::columnCount(const QModelIndex &in_parent) const
{
    if (in_parent.isValid() == true)
    {
        return static_cast<Audio_collection_item*>(in_parent.internalPointer())->get_column_count();
    }
    else
    {
        return rootItem->get_column_count();
    }
}

QStringList
Audio_collection_model::mimeTypes() const
{
    // Export plain text when dragging out of the model.
    QStringList types;
    types << "application/vnd.text.list";

    return types;
}

QMimeData
*Audio_collection_model::mimeData(const QModelIndexList &in_indexes) const
{
    // Encode data that is dragged out.
    QMimeData *mime_data = new QMimeData();
    QByteArray encoded_dragged_data;
    QDataStream encoded_dragged_stream(&encoded_dragged_data, QIODevice::WriteOnly);

    foreach (const QModelIndex &index, in_indexes)
    {
        if (index.isValid())
        {
            // Put full path of file in encoded dragged stream.
            Audio_collection_item *item = static_cast<Audio_collection_item*>(index.internalPointer());
            QString text = item->get_full_path();
            encoded_dragged_stream << text;
        }
    }

    mime_data->setData("application/vnd.text.list", encoded_dragged_data);

    return mime_data;
}

QVariant Audio_collection_model::data(const QModelIndex &in_index, int in_role) const
{
    if (in_index.isValid() == false)
    {
        return QVariant();
    }

    Audio_collection_item *item = static_cast<Audio_collection_item*>(in_index.internalPointer());

    if (in_role == Qt::DisplayRole)
    {
        if (in_index.column() == COLUMN_TAGS)
        {
            // Show list of tags like: [tag1] [tag2] ...
            QStringList tags = item->get_data(in_index.column()).toStringList();
            QString tags_str = "";
            foreach (const QString &str, tags)
            {
                if (str.isEmpty() == false)
                {
                    tags_str += "[" + str + "] ";
                }
            }
            return tags_str;
        }
        else
        {
            return item->get_data(in_index.column());
        }
    }
    else if ((in_role == Qt::DecorationRole)
          && (in_index.column() == COLUMN_FILE_NAME))
    {
        if (item->is_directory() == false)
        {
            return this->audio_file_icon;
        }
        else
        {
            return this->directory_icon;
        }
    }
    else if (in_role == Qt::BackgroundRole)
    {
        if (item->is_a_next_major_key() == true)
        {
            return QColor(255, 153, 0); // orange
        }
        else if (item->is_a_next_key() == true)
        {
            return QColor(0, 153, 0); // green
        }
        else
        {
            return QVariant();
        }
    }
    else if ((in_role == Qt::ForegroundRole)
          && (in_index.column() == COLUMN_TAGS))
    {
        if ((item->is_a_next_major_key() == true)
         || (item->is_a_next_key() == true))
        {
            return QColor(0, 0, 0); // black
        }
        else
        {
            return QColor(255, 153, 0); // orange
        }
    }
    else
    {
        return QVariant();
    }
}

Qt::ItemFlags Audio_collection_model::flags(const QModelIndex &in_index) const
{
    if (in_index.isValid() == false)
    {
        return 0;
    }

    // Get item.
    Audio_collection_item *item;
    item = static_cast<Audio_collection_item*>(in_index.internalPointer());
    if ((item != nullptr) && (item->is_directory() == false))
    {
        // Only a file can be dragged to a deck.
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;
    }
    else
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
}

QVariant Audio_collection_model::headerData(int in_section, Qt::Orientation in_orientation, int in_role) const
{
    if (in_orientation == Qt::Horizontal && in_role == Qt::DisplayRole)
    {
        return this->rootItem->get_data(in_section);
    }

    return QVariant();
}

QModelIndex Audio_collection_model::index(int in_row, int in_column, const QModelIndex &in_parent) const
{
    if (hasIndex(in_row, in_column, in_parent) == false)
    {
        return QModelIndex();
    }

    Audio_collection_item *parentItem;

    if (in_parent.isValid() == false)
    {
        parentItem = this->rootItem;
    }
    else
    {
        parentItem = static_cast<Audio_collection_item*>(in_parent.internalPointer());
    }

    Audio_collection_item *childItem = parentItem->get_child(in_row);
    if (childItem != nullptr)
    {
        return createIndex(in_row, in_column, childItem);
    }
    else
    {
        return QModelIndex();
    }
}

QModelIndex Audio_collection_model::parent(const QModelIndex &in_index) const
{
    if (in_index.isValid() == false)
    {
        return QModelIndex();
    }

    Audio_collection_item *childItem  = static_cast<Audio_collection_item*>(in_index.internalPointer());
    if (childItem == nullptr)
    {
        return QModelIndex();
    }

    Audio_collection_item *parentItem = childItem->get_parent();

    if ((parentItem == this->rootItem) || (parentItem == nullptr))
    {
        return QModelIndex();
    }

    return createIndex(parentItem->get_row(), 0, parentItem);
}

QModelIndex Audio_collection_model::parent_from_item(Audio_collection_item &in_item) const
{
    Audio_collection_item *parent_item = in_item.get_parent();

    if (parent_item == this->rootItem)
    {
        return QModelIndex();
    }

    return createIndex(parent_item->get_row(), 0, parent_item);
}

int Audio_collection_model::rowCount(const QModelIndex &in_parent) const
{
    Audio_collection_item *parentItem;
    if (in_parent.column() > 0)
    {
        return 0;
    }

    if (in_parent.isValid() == false)
    {
        parentItem = this->rootItem;
    }
    else
    {
        parentItem = static_cast<Audio_collection_item*>(in_parent.internalPointer());
    }

    return parentItem->get_child_count();
}

void Audio_collection_model::setup_model_data(QString in_path, Audio_collection_item *in_item)
{
    // Create a Qdir based on input path.
    QStringList filters;
    for (int i = 0; i < Utils::audio_file_extensions.size(); i++)
    {
        filters << (QString("*.") + Utils::audio_file_extensions.at(i));
    }
    QDir dir(in_path);
    dir.setNameFilters(filters);
    dir.setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::DirsFirst);

    // Iterate in directory.
    QFileInfoList file_info_list = dir.entryInfoList();
    foreach (QFileInfo file_info, file_info_list)
    {
        // Prepare data to show for the item.
        QString displayed_path(file_info.fileName());
        QList<QVariant> line;
        line << displayed_path;
        line << ""; // music key
        line << ""; // tag list
        line << file_info.absolutePath(); // absolute path

        // Add a child item.
        if (file_info.isDir() == false)
        {
            // It is a file, add the item.
            Audio_collection_item *file_item = new Audio_collection_item(line,
                                                                         Utils::get_file_hash(file_info.absoluteFilePath()),
                                                                         file_info.absoluteFilePath(),
                                                                         false,
                                                                         in_item);
            in_item->append_child(file_item);

            // Add the item's reference to a list (useful for future parsing).
            this->audio_item_list << file_item;
        }
        else
        {
            // It is a directory, analyze file under it.
            this->setup_model_data(file_info.absoluteFilePath(), in_item);
        }
    }
}

void Audio_collection_model::setup_model_data_from_tracklist(QStringList in_tracklist, Audio_collection_item *in_item)
{
    // Iterate over tracklist.
    for (int i = 0; i < in_tracklist.size(); i++)
    {
        QFileInfo file_info(in_tracklist.at(i));
        if (file_info.exists() == true)
        {
            // Prepare data to show for the item.
            QString displayed_path(file_info.fileName());
            QList<QVariant> line;
            line << displayed_path;
            line << ""; // music key
            line << ""; // tag list
            line << file_info.absolutePath(); // absolute path

            // Add a child item.
            if (file_info.isDir() == false)
            {
                if (Utils::audio_file_extensions.contains(file_info.suffix(), Qt::CaseInsensitive) == true)
                {
                    // It is a file (mp3 or flac), add the item.
                    Audio_collection_item *file_item = new Audio_collection_item(line,
                                                                                 Utils::get_file_hash(file_info.absoluteFilePath()),
                                                                                 file_info.absoluteFilePath(),
                                                                                 false,
                                                                                 in_item);
                    in_item->append_child(file_item);

                    // Add the item's reference to a list (useful for future parsing).
                    this->audio_item_list << file_item;
                }
            }
        }
    }
}

void external_read_from_db(Audio_collection_item *&in_audio_item)
{
    // Only a wrapper to get data from DB for an audio item object.
    if (in_audio_item->read_from_db() == false)
    {
        // Not found: store the audio track to DB.
        in_audio_item->store_to_db();
    }
}

void Audio_collection_model::concurrent_read_collection_from_db()
{
    // Just be sure DB was init.
    Data_persistence *data_persist = &Singleton<Data_persistence>::get_instance();
    if (data_persist->is_initialized == true)
    {
        // Read audio item from database for the whole collection.
        QFuture<void> future = QtConcurrent::map(this->audio_item_list, &external_read_from_db);
        this->concurrent_watcher_read->setFuture(future);
    }
}

void Audio_collection_model::stop_concurrent_read_collection_from_db()
{
    if (this->concurrent_watcher_read->isRunning() == true)
    {
        this->concurrent_watcher_read->cancel();
        this->concurrent_watcher_read->waitForFinished();
    }
}

void external_analyze_audio_collection(Audio_collection_item *&in_audio_item)
{
    // Only a wrapper to analyze data of an audio item object.
    in_audio_item->compute_audio_characteristics();
}

void Audio_collection_model::concurrent_analyse_audio_collection()
{
    // Just be sure DB was init.
    Data_persistence *data_persist = &Singleton<Data_persistence>::get_instance();
    if (data_persist->is_initialized == true)
    {
        // Do not do anything if we are still reading collection from DB.
        if ((this->concurrent_watcher_read->isRunning()  == false) &&
            (this->concurrent_watcher_analyze->isRunning() == false))
        {
            // Analyze item and store to DB (for the whole collection).
            QFuture<void> future = QtConcurrent::map(this->audio_item_list, &external_analyze_audio_collection);
            this->concurrent_watcher_analyze->setFuture(future);
        }
    }
}

void Audio_collection_model::write_collection_to_db()
{
    foreach (Audio_collection_item *item, this->audio_item_list)
    {
        item->store_to_db();
    }
}

void Audio_collection_model::concurrent_analyse_audio_selection(QList<Audio_collection_item *> &items)
{
    // Just be sure DB was init.
    Data_persistence *data_persist = &Singleton<Data_persistence>::get_instance();
    if (data_persist->is_initialized == true)
    {
        // Do not do anything if we are still reading collection from DB.
        if ((this->concurrent_watcher_read->isRunning()  == false) &&
            (this->concurrent_watcher_analyze->isRunning() == false))
        {
            // Analyze item and store to DB (for the whole collection).
            QFuture<void> future = QtConcurrent::map(items, &external_analyze_audio_collection);
            this->concurrent_watcher_analyze->setFuture(future);
        }
    }
}

void Audio_collection_model::stop_concurrent_analyse_audio_collection()
{
    if (this->concurrent_watcher_analyze->isRunning() == true)
    {
        this->concurrent_watcher_analyze->cancel();
        this->concurrent_watcher_analyze->waitForFinished();
    }
}

int Audio_collection_model::get_nb_items()
{
    return this->audio_item_list.length();
}

int Audio_collection_model::get_nb_new_items()
{
    int nb_items = 0;
    foreach (Audio_collection_item *item, this->audio_item_list)
    {
        if (item->get_data(COLUMN_KEY) == "")
        {
            // This file does not have been analyzed, let's consider it as a new one.
            nb_items++;
        }
    }

    return nb_items;
}

void
Audio_collection_model::set_next_keys(QString in_next_key,
                                      QString in_previous_key,
                                      QString in_next_major_key)
{
    // Iterate over all items and set flags to true if they are of next/previous/major keys.
    foreach (Audio_collection_item *item, this->audio_item_list)
    {
        // Reset.
        item->set_next_key(false);
        item->set_next_major_key(false);

        if ((item->get_data(COLUMN_KEY) == in_next_key) ||
            (item->get_data(COLUMN_KEY) == in_previous_key))
        {
            // The item is a next or previous key.
            item->set_next_key(true);
        }
        else if (item->get_data(COLUMN_KEY) == in_next_major_key)
        {
            // The item is a next major or minor key.
            item->set_next_major_key(true);
        }
    }

    return;
}

QModelIndexList
Audio_collection_model::search(QString in_text)
{
    // Get elements that matches with in_text.
    QModelIndexList items = this->match(this->index(0, 0),
                                        Qt::DisplayRole,
                                        QVariant::fromValue(in_text),
                                        -1,
                                        Qt::MatchFlags(Qt::MatchStartsWith | Qt::MatchWrap));

    return items;
}

void
Audio_collection_model::clear()
{
    if (this->rootItem != nullptr)
    {
        this->beginRemoveRows(this->get_root_index(), 0, this->rowCount());
        this->endRemoveRows();
        this->rootItem->childItems.clear();
        qDeleteAll(this->audio_item_list);
        this->audio_item_list.clear();
    }
}
