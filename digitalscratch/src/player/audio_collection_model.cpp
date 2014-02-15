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

#include <QtDebug>
#include <QStringList>
#include <QTextCodec>
#include <iostream>
#include <QPixmap>
#include <application_const.h>
#include <audio_collection_model.h>
#include <audio_track.h>
#include <utils.h>
#include <data_persistence.h>
#include <singleton.h>
#include <QtConcurrentRun>
#include <QtConcurrentMap>
#include <application_settings.h>
#include <QMimeData>

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
    if (this->parentItem != NULL)
    {
        return this->parentItem->childItems.indexOf(const_cast<Audio_collection_item*>(this));
    }

    return 0;
}

QString Audio_collection_item::get_full_path()
{
    return this->fullPath;
}

QString Audio_collection_item::get_file_hash()
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

void Audio_collection_item::read_from_db()
{
    // Init.
    Data_persistence *data_persist = &Singleton<Data_persistence>::get_instance();
    Audio_track      *at           = new Audio_track(44100); // could be hardcoded, not used for this purpose.

    // Get the hash of audio file.
    at->reset();
    at->set_hash(this->get_file_hash());

    // Get audio data from db and put it back to the item.
    if (data_persist->get_audio_track(at) == true)
    {
        // File found in DB, put data back to item.
        this->set_data(COLUMN_KEY, at->get_music_key());
    }

    // Cleanup.
    delete at;
}

void Audio_collection_item::compute_and_store_to_db()
{
    Application_settings *settings = &Singleton<Application_settings>::get_instance();

    // Check in application settings if we should analyze only files with missing data.
    if ((settings->get_audio_collection_full_refresh() == true) ||
        ((settings->get_audio_collection_full_refresh() == false) && (this->get_data(COLUMN_KEY) == "")))
    {
        // Calculate things (music key, bpm, etc...)
        this->calculate_audio_data();

        // Store audio collection to DB.
        this->store_to_db();
    }
}

void Audio_collection_item::calculate_audio_data()
{
    // Calculate data and put them back in current audio item.
    this->set_data(COLUMN_KEY, Utils::get_file_music_key(this->fullPath));
}

void Audio_collection_item::store_to_db()
{
    // Init.
    Data_persistence *data_persist = &Singleton<Data_persistence>::get_instance();
    Audio_track      *at           = new Audio_track(44100); // Could be hardcoded, not used for this purpose

    // Get a hash, set audio data to an Audio_track and persist it.
    at->reset();
    at->set_hash(this->get_file_hash());
    at->set_fullpath(this->get_full_path());
    at->set_music_key(this->get_data(COLUMN_KEY).toString());
    if (data_persist->store_audio_track(at) == false)
    {
        qWarning() << "Audio_collection_item::store_collection_to_db: can not store " << this->get_full_path() << " to DB";
    }

    // Cleanup.
    delete at;
}

Audio_collection_model::Audio_collection_model(QObject *in_parent) : QAbstractItemModel(in_parent)
{
    this->rootItem = NULL;
    this->create_header("", false);
    this->audio_item_list.clear();
    this->root_path = "";

    // Init thread tools.
    this->concurrent_future        = new QFuture<void>;
    this->concurrent_watcher_read  = new QFutureWatcher<void>;
    this->concurrent_watcher_store = new QFutureWatcher<void>;
}

Audio_collection_model::~Audio_collection_model()
{
    if (this->rootItem != NULL)
    {
        delete this->rootItem;
    }

    // Stop running threads.
    if (this->concurrent_watcher_store->isStarted() == true)
    {
        this->concurrent_watcher_store->cancel();
        this->concurrent_watcher_store->waitForFinished();
    }
    if (this->concurrent_watcher_read->isStarted() == true)
    {
        this->concurrent_watcher_read->cancel();
        this->concurrent_watcher_read->waitForFinished();
    }

    delete this->concurrent_future;
    delete this->concurrent_watcher_read;
    delete this->concurrent_watcher_store;
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
    rootData << tr("Track") << tr("Key");
    if (in_show_path == true)
    {
        rootData << tr("Path");
    }

    if (this->rootItem != NULL)
    {
        delete this->rootItem;
    }
    this->rootItem = new Audio_collection_item(rootData, "", in_path, false, 0);
}

QModelIndex Audio_collection_model::set_root_path(QString in_root_path)
{
    // Clean collection.
    this->beginResetModel();
    this->endResetModel();

    // Create root item which is the collection header.
    this->create_header(in_root_path, false);

    // Reset internal list of audio files (item pointers).
    this->audio_item_list.clear();

    // Fill the model.
    this->setup_model_data(in_root_path, this->rootItem);

    // Store root path.
    this->root_path = in_root_path;

    return this->get_root_index();
}

QString Audio_collection_model::get_root_path()
{
    return this->root_path;
}

QModelIndex Audio_collection_model::set_playlist(Playlist *in_playlist)
{
    // Clean collection.
    this->beginResetModel();
    this->endResetModel();

    // Create root item which is the collection header.
    this->create_header(in_playlist->get_basepath(), true);

    // Reset internal list of audio files (item pointers).
    this->audio_item_list.clear();

    // Fill the model.
    this->setup_model_data_from_tracklist(in_playlist->get_tracklist(), this->rootItem);

    return this->get_root_index();
}

QModelIndex Audio_collection_model::get_root_index()
{
    if (this->rootItem != NULL)
    {
        return createIndex(0, 0, this->rootItem);
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

struct Audio_collection_item_key_comparer
{
    bool operator()(const Audio_collection_item *in_item_a, const Audio_collection_item *in_item_b) const
    {
        QString key       = in_item_a->get_data(COLUMN_KEY).toString();
        QString other_key = in_item_b->get_data(COLUMN_KEY).toString();

        if ((key.size() >= 2) && (other_key.size() >= 2))
        {
            // First compare one or 2 first number digits. If there are same, Compare the last letter.
            int     key_digits = 0;
            QString key_letter = "";
            if (key.size() == 2)
            {
                // Like "2B".
                key_digits = key.left(1).toInt();
                key_letter = key.right(1);
            }
            if (key.size() == 3)
            {
                // Like "11A".
                key_digits = key.left(2).toInt();
                key_letter = key.right(1);
            }

            int     other_key_digits = 0;
            QString other_key_letter = "";
            if (other_key.size() == 2)
            {
                // Like "2B".
                other_key_digits = other_key.left(1).toInt();
                other_key_letter = other_key.right(1);
            }
            if (other_key.size() == 3)
            {
                // Like "11A".
                other_key_digits = other_key.left(2).toInt();
                other_key_letter = other_key.right(1);
            }

            if (key_digits != other_key_digits)
            {
                // Digits are different, compare on it.
                return key_digits < other_key_digits;
            }
            else
            {
                // Digits are same, compare letter.
                return key_letter < other_key_letter;
            }
        }
        else
        {
            return key < other_key;
        }
    }
};

struct Audio_collection_item_name_comparer
{
    bool operator()(const Audio_collection_item *in_item_a, const Audio_collection_item *in_item_b) const
    {
        QString name       = in_item_a->get_data(COLUMN_FILE_NAME).toString();
        QString other_name = in_item_b->get_data(COLUMN_FILE_NAME).toString();

        return name < other_name;
    }
};

struct Audio_collection_item_path_comparer
{
    bool operator()(const Audio_collection_item *in_item_a, const Audio_collection_item *in_item_b) const
    {
        QString path       = in_item_a->get_data(COLUMN_PATH).toString();
        QString other_path = in_item_b->get_data(COLUMN_PATH).toString();

        // Extract common part of both paths.
        QString common_path("");
        int     i = 1;
        bool    end_of_common = false;
        while ((i <= path.length()) && (i <= other_path.length()) && (end_of_common == false))
        {
            if (path.left(i) == other_path.left(i))
            {
                // Common part of 2 paths.
                common_path = path.left(i);
            }
            else
            {
                // End of common part.
                end_of_common = true;
            }
            i++;
        }

        // If common part is the full part for one of the path, then the full path is higher.
        // It results that file in dir are placed after files in subdirs.
        if (common_path.length() > 0)
        {
            if (common_path == path)
            {
                return false;
            }
            else if (common_path == other_path)
            {
                return true;
            }

        }

        // In all other cases we are doing a basic paths compare.
        return path < other_path;
    }
};

void Audio_collection_model::sort(int in_column, Qt::SortOrder in_order)
{
    // FIXME sort keys directory per directory (and not for the full list), actually sort childItems recursively
    // Then rebuild the full structure with parents and childs.
    // So for the moment, do not sort if there are item with childs.
    bool do_sort = true;
    foreach (Audio_collection_item *item, this->rootItem->childItems)
    {
        if (item->get_child_count() > 0)
        {
            do_sort = false;
        }
    }

    // Sort audio item list.
    if (do_sort == true)
    {
        if (in_column == COLUMN_KEY)
        {
            qSort(this->audio_item_list.begin(), this->audio_item_list.end(), Audio_collection_item_key_comparer());
        }
        else if (in_column == COLUMN_FILE_NAME)
        {
            qSort(this->audio_item_list.begin(), this->audio_item_list.end(), Audio_collection_item_name_comparer());
        }
        else if (in_column == COLUMN_PATH)
        {
            qSort(this->audio_item_list.begin(), this->audio_item_list.end(), Audio_collection_item_path_comparer());
        }

        // Clean and populate rows based on audio item list.
        this->beginRemoveRows(this->get_root_index(), 1, this->rowCount());
        this->endRemoveRows();
        this->rootItem->childItems.clear();
        if (in_order == Qt::AscendingOrder)
        {
            foreach (Audio_collection_item *item, this->audio_item_list)
            {
                this->rootItem->append_child(item);
            }
        }
        else
        {
            for (int i = (this->audio_item_list.count() - 1); i >= 0; i--)
            {
                this->rootItem->append_child(this->audio_item_list.at(i));
            }
        }
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
        return item->get_data(in_index.column());
    }
    else if ((in_role               == Qt::DecorationRole) &&
             (in_index.column()     == COLUMN_FILE_NAME))
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
    else if (in_role == Qt::BackgroundColorRole)
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
    if ((item != NULL) && (item->is_directory() == false))
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
    if (childItem != NULL)
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
    Audio_collection_item *parentItem = childItem->get_parent();

    if ((parentItem == this->rootItem) || (parentItem == NULL))
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
        line << displayed_path << "";

        // Add a child item.
        if (file_info.isDir() == false)
        {
            // It is a file, add the item.
            Audio_collection_item *file_item = new Audio_collection_item(line,
                                                                         Utils::get_file_hash(file_info.absoluteFilePath(), FILE_HASH_SIZE),
                                                                         file_info.absoluteFilePath(),
                                                                         false,
                                                                         in_item);
            in_item->append_child(file_item);

            // Add the item's reference to a list (useful for future parsing).
            this->audio_item_list << file_item;
        }
        else
        {
            // It is a directory, add the item and analyze file under it.
            Audio_collection_item *dir_item = new Audio_collection_item(line,
                                                                        "",
                                                                        file_info.absoluteFilePath(),
                                                                        true,
                                                                        in_item);
            in_item->append_child(dir_item);

            this->setup_model_data(file_info.absoluteFilePath(), dir_item);
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
            line << displayed_path << "" << file_info.absolutePath();

            // Add a child item.
            if (file_info.isDir() == false)
            {
                if (Utils::audio_file_extensions.contains(file_info.suffix(), Qt::CaseInsensitive) == true)
                {
                    // It is a file (mp3 or flac), add the item.
                    Audio_collection_item *file_item = new Audio_collection_item(line,
                                                                                 Utils::get_file_hash(file_info.absoluteFilePath(), FILE_HASH_SIZE),
                                                                                 file_info.absoluteFilePath(),
                                                                                 false,
                                                                                 in_item);
                    in_item->append_child(file_item);

                    // Add the item's reference to a list (useful for future parsing).
                    this->audio_item_list << file_item;
                }
            }
            else
            {
                // It is a directory, add the item and analyze file under it.
                Audio_collection_item *dir_item = new Audio_collection_item(line,
                                                                            "",
                                                                            file_info.absoluteFilePath(),
                                                                            true,
                                                                            in_item);
                in_item->append_child(dir_item);

                this->setup_model_data(file_info.absoluteFilePath(), dir_item);
            }
        }
    }
}

void Audio_collection_model::begin_db_change()
{
    // Start DB transaction.
    Data_persistence *data_persist = &Singleton<Data_persistence>::get_instance();
    data_persist->begin_transaction();
}

void Audio_collection_model::commit_db_change()
{
    // Commit DB transaction.
    Data_persistence *data_persist = &Singleton<Data_persistence>::get_instance();
    if (data_persist->commit_transaction() == false)
    {
        qWarning() << "Audio_collection_model::commit_db_change: can not commit DB transaction";
    }
}

void external_read_from_db(Audio_collection_item *&in_audio_item)
{
    // Only a wrapper to get data from DB for an audio item object.
    in_audio_item->read_from_db();
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
    // Only a wrapper to analyze and store data of an audio item object.
    in_audio_item->compute_and_store_to_db();
}

void Audio_collection_model::concurrent_analyse_audio_collection()
{
    // Just be sure DB was init.
    Data_persistence *data_persist = &Singleton<Data_persistence>::get_instance();
    if (data_persist->is_initialized == true)
    {
        // Do not do anything if we are still reading collection from DB.
        if ((this->concurrent_watcher_read->isRunning()  == false) &&
            (this->concurrent_watcher_store->isRunning() == false))
        {
            // Analyze item and store to DB (for the whole collection).
            QFuture<void> future = QtConcurrent::map(this->audio_item_list, &external_analyze_audio_collection);
            this->concurrent_watcher_store->setFuture(future);
        }
    }
}

void Audio_collection_model::stop_concurrent_analyse_audio_collection()
{
    if (this->concurrent_watcher_store->isRunning() == true)
    {
        this->concurrent_watcher_store->cancel();
        this->concurrent_watcher_store->waitForFinished();
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

QList<QModelIndex>
Audio_collection_model::set_next_keys(QString in_next_key,
                                      QString in_previous_key,
                                      QString in_next_major_key)
{
    // Init list of QModelIndex which contains next/previous/opposite key.
    QList<QModelIndex> dir_list;

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

            // Add parent QModelIndex (the dir) to the result list.
            dir_list.append(this->parent_from_item(*item));

        }
        else if (item->get_data(COLUMN_KEY) == in_next_major_key)
        {
            // The item is a next major or minor key.
            item->set_next_major_key(true);

            // Add parent QModelIndex (the dir) to the result list.
            dir_list.append(this->parent_from_item(*item));
        }
    }

    return dir_list;
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

