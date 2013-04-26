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

Audio_collection_item::Audio_collection_item(const QList<QVariant> &in_data, Audio_collection_item *in_parent)
{
    this->parentItem = in_parent;
    this->itemData   = in_data;
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

Audio_collection_model::Audio_collection_model(QObject *in_parent) : QAbstractItemModel(in_parent)
{
    this->rootItem = NULL;
    this->create_header();
}

Audio_collection_model::~Audio_collection_model()
{
    if (this->rootItem != NULL)
    {
        delete this->rootItem;
    }
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

    //return this->index(0, 0, this->rootItem);
    return createIndex(0, 0, this->rootItem);
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
    else if ((in_role == Qt::DecorationRole) &&
             (in_index.column() == 0))
    {
        QString file_or_dir_name((item->get_data(in_index.column()).toString()));
        if ((file_or_dir_name.endsWith(".mp3",  Qt::CaseInsensitive) == true) ||
            (file_or_dir_name.endsWith(".flac", Qt::CaseInsensitive) == true))
        {
            return (QPixmap(ICON_AUDIO_FILE)).scaledToWidth(16, Qt::SmoothTransformation);
        }
        else
        {
            return (QPixmap(ICON_FOLDER)).scaledToWidth(16, Qt::SmoothTransformation);
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
    // Create a Qdir based on input path.
    QStringList filters;
    filters << "*.mp3" << "*.flac";
    QDir dir(in_path);
    dir.setNameFilters(filters);
    dir.setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::DirsFirst);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8")); // FIXME: Should be probably removed in Qt5.

    // Iterate in directory.
    QFileInfoList file_info_list = dir.entryInfoList();
    foreach (QFileInfo file_info, file_info_list)
    {
        // Prepare data for the item.
        QString displayed_path(file_info.fileName());
        QList<QVariant> line;

        // Add a child item.
        if (file_info.isDir() == false)
        {
            // It is a file.
            line << displayed_path << "KEY";
            Audio_collection_item *file_item = new Audio_collection_item(line, in_item);
            in_item->append_child(file_item);
        }
        else
        {
            // It is a directory, analyze file under it.
            line << displayed_path << "";
            Audio_collection_item *dir_item = new Audio_collection_item(line, in_item);
            in_item->append_child(dir_item);

            this->setup_model_data(file_info.absoluteFilePath(), dir_item);
        }
    }
}
