/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*---------------------------------------------------( data_persistence.cpp )-*/
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
/*                    Class defining how to store audio data                  */
/*                                                                            */
/*============================================================================*/


#include <utils.h>
#include <application_const.h>
#include <QtDebug>
#include "data_persistence.h"
#include <QSqlError>
#include <QDesktopServices>
#include <QFileInfo>
#include <QDir>
#include <QSqlQuery>

Data_persistence::Data_persistence()
{
    qDebug() << "Data_persistence::Data_persistence: create object...";

    this->is_initialized = this->init_db();

    qDebug() << "Data_persistence::Data_persistence: create object done";

    return;
}

Data_persistence::~Data_persistence()
{
    qDebug() << "Audio_track::~Audio_track: delete object...";

    this->close_db();

    qDebug() << "Audio_track::~Audio_track: delete object done.";

    return;
}

bool Data_persistence::init_db()
{
    qDebug() << "Data_persistence::init_db...";

    // Init.
    bool result = true;

    // Create DB.
    this->db = QSqlDatabase::addDatabase("QSQLITE");
    QFileInfo path_info(QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "/digitalscratch.sqlite");
    this->db.setDatabaseName(path_info.absoluteFilePath());

    // Make sure path exists, if not create it.
    QDir dir;
    dir.mkpath(path_info.absolutePath());

    // Open DB.
    if (this->db.open() == false)
    {
        qWarning() << "Data_persistence::init_db: " << this->db.lastError().text();
        result = false;
    }

    // Create DB structure if needed.
    if (this->create_db_structure() == false)
    {
        qWarning() << "Data_persistence::init_db: creating base DB structure failed";
        result = false;
    }

    qDebug() << "Data_persistence::init_db done.";

    return result;
}

bool Data_persistence::create_db_structure()
{
    qDebug() << "Data_persistence::create_db_structure...";

    // Init.
    bool result = true;

    // Create DB structure
    if (this->db.isOpen() == true)
    {
        QSqlQuery query;

        // Enable foreign key support.
        result = query.exec("PRAGMA foreign_keys = ON");

        // Create TRACK table
        if (result == true)
        {
            result = query.exec("CREATE TABLE IF NOT EXISTS \"TRACK\" "
                                "(\"id_track\" INTEGER PRIMARY KEY  AUTOINCREMENT  NOT NULL  UNIQUE , "
                                "\"hash\" VARCHAR NOT NULL , "
                                "\"bpm\" INTEGER, "
                                "\"key\" VARCHAR, "
                                "\"key_tag\" VARCHAR, "
                                "\"path\" VARCHAR, "
                                "\"filename\" VARCHAR);");
        }

        // Create CUE_POINT table
        if (result == true)
        {
            result = query.exec("CREATE TABLE IF NOT EXISTS \"CUE_POINT\" "
                                "(\"id_cuepoint\" INTEGER PRIMARY KEY  AUTOINCREMENT  NOT NULL  UNIQUE , "
                                "\"id_track\" INTEGER  NOT NULL  , "
                                "\"position\" INTEGER NOT NULL , "
                                "FOREIGN KEY(id_track) REFERENCES TRACK(id_track));");
        }
    }
    else
    {
        // Db not open.
        result = false;
    }

    qDebug() << "Data_persistence::create_db_structure done.";

    return result;
}

void Data_persistence::close_db()
{
    qDebug() << "Data_persistence::close_db...";

    this->db.close();

    qDebug() << "Data_persistence::close_db done.";
}

bool Data_persistence::store_audio_track(Audio_track *in_at)
{
    qDebug() << "Data_persistence::store_audio_track...";

    // Check input parameter.
    if ((in_at == NULL) ||
       (in_at->get_hash().size() == 0))
    {
        return false;
    }

    // Insert or update main data of the audio track.
    if (this->db.isOpen() == true)
    {
        // Try to get audio track from Db.
        QSqlQuery query = this->db.exec("SELECT id_track FROM TRACK WHERE hash=\"" + in_at->get_hash() + "\"");
        if (query.lastError().isValid())
        {
            qWarning() << "SELECT track failed: " << query.lastError().text();
            //cout << "select failed: " << query.lastError().text().toStdString().c_str() << endl;
            return false;
        }
        if (query.next() == true) // Check if there is a record.
        {
            // An audio track with same hash already exists, update it.
            int existing_id = query.value(0).toInt();
            //cout << "existing_id=" << existing_id << endl;
            query = this->db.exec("UPDATE TRACK SET path = \"" + in_at->get_path() + "\" , "
                                  + "filename = \"" + in_at->get_filename() + "\" , "
                                  + "key = \"" + in_at->get_music_key() + "\" , "
                                  + "key_tag = \"" + in_at->get_music_key_tag() + "\" "
                                  + "WHERE id_track = " + QString::number(existing_id));
            if (query.lastError().isValid())
            {
                qWarning() << "UPDATE track failed: " << query.lastError().text();
                //cout << "update failed: " << query.lastError().text().toStdString().c_str() << endl;
                return false;
            }
        }
        else
        {
            // No existing audio track found, insert it in DB.
            query = this->db.exec("INSERT INTO TRACK (hash,path,filename,key,key_tag) VALUES (\"" + in_at->get_hash()
                                  + "\",\"" + in_at->get_path()
                                  + "\",\"" + in_at->get_filename()
                                  + "\",\"" + in_at->get_music_key()
                                  + "\",\"" + in_at->get_music_key_tag()
                                  + "\")");
            if (query.lastError().isValid())
            {
                qWarning() << "INSERT track failed: " << query.lastError().text();
                //cout << "insert failed: " << query.lastError().text().toStdString().c_str() << endl;
                return false;
            }
        }


    }
    else
    {
        // Db not open.
        qWarning() << "Can not store audio track: db not open";
        return false;
    }

    qDebug() << "Data_persistence::store_audio_track done.";

    return true;
}

bool Data_persistence::get_audio_track(Audio_track *in_at)
{
    qDebug() << "Data_persistence::get_audio_track...";

    // Check input parameter.
    if ((in_at == NULL) ||
        (in_at->get_hash().size() == 0))
    {
        qWarning() << "Can not get audio track: hash not specified.";
        return false;
    }

    // Search the audio track (based on its hash) in DB.
    QSqlQuery query = this->db.exec("SELECT key, key_tag, path, filename FROM TRACK WHERE hash=\"" + in_at->get_hash() + "\"");
    if (query.lastError().isValid())
    {
        qWarning() << "SELECT track failed: " << query.lastError().text();
        //cout << "select failed: " << query.lastError().text().toStdString().c_str() << endl;
        return false;
    }
    if (query.next() == true) // Check if there is a record.
    {
        // The audio track exists, fill the returned object.
        in_at->set_music_key(query.value(0).toString());
        in_at->set_music_key_tag(query.value(1).toString());
        in_at->set_fullpath(query.value(2).toString() + "/" + query.value(3).toString());
    }
    else
    {
        // Audio track not found.
        return false;
    }

    qDebug() << "Data_persistence::get_audio_track done.";

    return true;
}
