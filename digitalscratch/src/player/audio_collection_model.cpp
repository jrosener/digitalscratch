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

Audio_collection_item::Audio_collection_item(const QList<QVariant> &in_data,
                                             QString                in_file_hash,
                                             QString                in_full_path,
                                             bool                   in_is_directory,
                                             Audio_collection_item *in_parent)
{
    this->parentItem    = in_parent;
    this->fileHash      = in_file_hash;
    this->itemData      = in_data;
    this->fullPath      = in_full_path;
    this->directoryFlag = in_is_directory;
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

Audio_collection_model::Audio_collection_model(QObject *in_parent) : QAbstractItemModel(in_parent)
{
    this->rootItem = NULL;
    this->create_header();
    this->nb_audio_file_items = 0;

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

void Audio_collection_model::create_header()
{
    // Create root item which is the collection header.
    QList<QVariant> rootData;
    rootData << tr("Track") << tr("Key");
    if (this->rootItem != NULL)
    {
        delete this->rootItem;
    }
    this->rootItem = new Audio_collection_item(rootData);
}

QModelIndex Audio_collection_model::set_root_path(QString in_root_path)
{
    // Clean collection.
    this->beginResetModel();
    this->endResetModel();

    // Create root item which is the collection header.
    this->create_header();

    // Fill the model.
    this->setup_model_data(in_root_path, this->rootItem);

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

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
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

    if (parentItem == this->rootItem)
    {
        return QModelIndex();
    }

    return createIndex(parentItem->get_row(), 0, parentItem);
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
    // Reset nb file items.
    this->nb_audio_file_items = 0;

    // Create a Qdir based on input path.
    QStringList filters;
    filters << "*.mp3" << "*.flac";
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
            this->nb_audio_file_items++;
            Audio_collection_item *file_item = new Audio_collection_item(line,
                                                                         Utils::get_file_hash(file_info.absoluteFilePath(), FILE_HASH_SIZE),
                                                                         file_info.absoluteFilePath(),
                                                                         false,
                                                                         in_item);
            in_item->append_child(file_item);
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

void Audio_collection_model::concurrent_read_collection_from_db()
{
    // Run read_collection_from_db() in a separate thread.
    if ((this->concurrent_watcher_read->isRunning()  == false) &&
        (this->concurrent_watcher_store->isRunning() == false))
    {
        *this->concurrent_future = QtConcurrent::run(this, &Audio_collection_model::read_collection_from_db, this->rootItem);
        this->concurrent_watcher_read->setFuture(*this->concurrent_future);
    }
}

void Audio_collection_model::read_collection_from_db(Audio_collection_item *in_parent_item)
{
    // Init.
    Data_persistence *data_persist = &Singleton<Data_persistence>::get_instance();
    Audio_track      *at           = new Audio_track();

    // Get the root (parent) audio item.
    Audio_collection_item *parent_item;
    if (in_parent_item == NULL)
    {
        // No parent specified, take the root of the collection.
        parent_item = this->rootItem;
    }
    else
    {
        parent_item = in_parent_item;
    }

    // Iterate over child items.
    for (int i = 0; i < parent_item->get_child_count(); i++)
    {
       Audio_collection_item *child_item = parent_item->get_child(i);
       if (child_item->is_directory() == true)
       {
           // Child is a directory, call recursively this method.
           this->read_collection_from_db(child_item);
       }
       else
       {
           // Child is an audio file, get a hash, get audio data from db and put it back to the item.
           at->reset();
           at->set_hash(child_item->get_file_hash());
           if (data_persist->get_audio_track(at) == true)
           {
               // File found in DB, put data back to item.
               child_item->set_data(COLUMN_KEY, at->get_music_key());
           }
       }
    }

    // Cleanup.
    delete at;
}

void Audio_collection_model::concurrent_analyse_audio_collection()
{
    // Run store_collection_to_db() in a separate thread.
    if ((this->concurrent_watcher_read->isRunning()  == false) &&
        (this->concurrent_watcher_store->isRunning() == false))
    {
        *this->concurrent_future = QtConcurrent::run(this, &Audio_collection_model::analyze_audio_collection);
        this->concurrent_watcher_store->setFuture(*this->concurrent_future);
    }
}

void Audio_collection_model::analyze_audio_collection()
{
    // Calculate things on audio collection (music key, etc...)
    this->calculate_audio_collection_data(this->rootItem);

    // Store audio collection to DB.
    this->store_collection_to_db(this->rootItem);
}

void Audio_collection_model::calculate_audio_collection_data(Audio_collection_item *in_parent_item)
{
    // Get the root (parent) audio item.
    Audio_collection_item *parent_item;
    if (in_parent_item == NULL)
    {
        // No parent specified, take the root of the collection.
        parent_item = this->rootItem;
    }
    else
    {
        parent_item = in_parent_item;
    }

    // Iterate over child items.
    for (int i = 0; i < parent_item->get_child_count(); i++)
    {
       Audio_collection_item *child_item = parent_item->get_child(i);
       if (child_item->is_directory() == true)
       {
           // Child is a directory, call recursively this method.
           this->calculate_audio_collection_data(child_item);
       }
       else
       {
           // Child is an audio file, calculate data and put them back in audio item.
           child_item->set_data(COLUMN_KEY, "A1"); // TODO: calculate audio data.
       }
    }
}

void Audio_collection_model::store_collection_to_db(Audio_collection_item *in_parent_item)
{
    // Init.
    Data_persistence *data_persist = &Singleton<Data_persistence>::get_instance();
    Audio_track      *at           = new Audio_track();

    // Get the root (parent) audio item.
    Audio_collection_item *parent_item;
    if (in_parent_item == NULL)
    {
        // No parent specified, take the root of the collection.
        parent_item = this->rootItem;
    }
    else
    {
        parent_item = in_parent_item;
    }

    // Iterate over child items.
    for (int i = 0; i < parent_item->get_child_count(); i++)
    {
       Audio_collection_item *child_item = parent_item->get_child(i);
       if (child_item->is_directory() == true)
       {
           // Child is a directory, call recursively this method.
           this->store_collection_to_db(child_item);
       }
       else
       {
           // Child is an audio file, get a hash, set audio data and persist them.
           at->reset();
           at->set_hash(child_item->get_file_hash());
           at->set_fullpath(child_item->get_full_path());
           at->set_music_key(child_item->get_data(COLUMN_KEY).toString());
           if (data_persist->store_audio_track(at) == false)
           {
               qWarning() << "Audio_collection_model::store_collection_to_db: can not store " << child_item->get_full_path() << " to DB";
           }
       }
    }

    // Cleanup.
    delete at;
}
