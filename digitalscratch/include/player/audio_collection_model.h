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

 public:
    Audio_collection_item(const QList<QVariant> &in_data, QString in_full_path = "", Audio_collection_item *in_parent = 0);
    ~Audio_collection_item();

    void                   append_child(Audio_collection_item *in_item);
    Audio_collection_item *get_child(int in_row);
    int                    get_child_count() const;

    int      get_column_count() const;
    QVariant get_data(int in_column) const;

    int                    get_row() const;
    Audio_collection_item *get_parent();

    QString                get_full_path();
};

class Audio_collection_model : public QAbstractItemModel
{
    Q_OBJECT

 private:
    Audio_collection_item *rootItem;

 public:
    Audio_collection_model(QObject *in_parent = 0);
    ~Audio_collection_model();

    QModelIndex   set_root_path(QString in_root_path);

    QVariant      data(const QModelIndex &in_index, int in_role) const;
    Qt::ItemFlags flags(const QModelIndex &in_index) const;
    QVariant      headerData(int in_section, Qt::Orientation in_orientation, int in_role = Qt::DisplayRole) const;
    QModelIndex   index(int in_row, int in_column, const QModelIndex &in_parent = QModelIndex()) const;
    QModelIndex   parent(const QModelIndex &in_index) const;
    int           rowCount(const QModelIndex &in_parent = QModelIndex()) const;
    int           columnCount(const QModelIndex &in_parent = QModelIndex()) const;

    QModelIndex read_from_db(); // TODO : iterate over file items, get a hash, get audio data from db and put it to the item
    QModelIndex store_to_db();  // TODO : iterate over file items, calculate or get data (key, key_tag, etc..), get a hash and update db.

 private:
    void setup_model_data(QString in_path, Audio_collection_item *in_item);
    void create_header();
};

#endif /* AUDIO_COLLECTION_MODEL_H_ */
