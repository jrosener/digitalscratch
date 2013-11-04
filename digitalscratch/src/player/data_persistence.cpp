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
#include <QFileInfo>
#include <QDir>
#include <QSqlQuery>
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  #include <QDesktopServices>
#else
  #include <QStandardPaths>
#endif

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

    this->mutex.lock();
    if (this->db.isValid() == true)
    {
        this->db.close();
    }
    this->mutex.unlock();

    qDebug() << "Audio_track::~Audio_track: delete object done.";

    return;
}

bool Data_persistence::init_db()
{
    qDebug() << "Data_persistence::init_db...";

    // Create DB.
    this->mutex.lock();
    this->db = QSqlDatabase::addDatabase("QSQLITE");
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    QFileInfo path_info(QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "/digitalscratch.sqlite");
#else
    QFileInfo path_info(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/digitalscratch.sqlite");
#endif
    this->db.setDatabaseName(path_info.absoluteFilePath());

    // Make sure path exists, if not create it.
    QDir dir;
    dir.mkpath(path_info.absolutePath());

    // Open DB.
    if (this->db.open() == false)
    {
        qWarning() << "Data_persistence::init_db: " << this->db.lastError().text();
        return false;
    }
    else
    {
        // Enable foreign key support.
        QSqlQuery query;
        if (query.exec("PRAGMA foreign_keys = ON") == false)
        {
            return false;
        }

        // Disable wait on write (on hdd).
        if (query.exec("PRAGMA synchronous = OFF") == false)
        {
            return false;
        }
    }    

    // Create DB structure if needed.
    if (this->create_db_structure() == false)
    {
        qWarning() << "Data_persistence::init_db: creating base DB structure failed";
        return false;
    }

    this->mutex.unlock();

    qDebug() << "Data_persistence::init_db done.";

    return true;
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

        // Create TRACK table
        result = query.exec("CREATE TABLE IF NOT EXISTS \"TRACK\" "
                            "(\"id_track\" INTEGER PRIMARY KEY  AUTOINCREMENT  NOT NULL  UNIQUE , "
                            "\"hash\" VARCHAR NOT NULL , "
                            "\"bpm\" INTEGER, "
                            "\"key\" VARCHAR, "
                            "\"key_tag\" VARCHAR, "
                            "\"path\" VARCHAR, "
                            "\"filename\" VARCHAR);");

        // Create CUE_POINT table
        if (result == true)
        {
            result = query.exec("CREATE TABLE IF NOT EXISTS \"CUE_POINT\" "
                                "(\"id_cuepoint\" INTEGER PRIMARY KEY  AUTOINCREMENT  NOT NULL  UNIQUE , "
                                "\"id_track\" INTEGER  NOT NULL  , "
                                "\"number\" INTEGER  NOT NULL  , "
                                "\"position\" INTEGER NOT NULL , "
                                "FOREIGN KEY(id_track) REFERENCES TRACK(id_track));");
        }
    }
    else
    {
        // Db not open.
        qWarning() << "Can not create DB structure: db not open";
        result = false;
    }

    qDebug() << "Data_persistence::create_db_structure done.";

    return result;
}

bool Data_persistence::begin_transaction()
{
    return this->db.transaction();
}

bool Data_persistence::commit_transaction()
{
    return this->db.commit();
}

bool Data_persistence::rollback_transaction()
{
    return this->db.rollback();
}

bool Data_persistence::store_audio_track(Audio_track *in_at)
{
    // Init result.
    bool result = true;

    qDebug() << "Data_persistence::store_audio_track...";

    // Check input parameter.
    if ((in_at == NULL) ||
       (in_at->get_hash().size() == 0))
    {
        result = false;
    }

    // Insert or update main data of the audio track (identified by the hash).
    if ((result == true) &&
        (this->is_initialized == true))
    {
        // Ensure no other thread can access the DB connection.
        this->mutex.lock();

        // Try to get audio track from Db.
        QSqlQuery query = this->db.exec("SELECT id_track, path, filename, key, key_tag FROM TRACK WHERE hash=\"" + in_at->get_hash() + "\"");
        if (query.lastError().isValid())
        {
            qWarning() << "SELECT track failed: " << query.lastError().text();
            result = false;
        }
        else if (query.next() == true) // Check if there is a record.
        {
            // An audio track with same hash already exists, update it if at least one element changed.
            if ((query.value(1) != in_at->get_path()) ||
                (query.value(2) != in_at->get_filename()) ||
                (query.value(3) != in_at->get_music_key()) ||
                (query.value(4) != in_at->get_music_key_tag()))
            {
                int existing_id = query.value(0).toInt();
                query.prepare("UPDATE TRACK SET path = :path, filename = :filename, key = :key, key_tag = :key_tag "
                              "WHERE id_track = :id_track");
                query.bindValue(":path",     in_at->get_path());
                query.bindValue(":filename", in_at->get_filename());
                query.bindValue(":key",      in_at->get_music_key());
                query.bindValue(":key_tag",  in_at->get_music_key_tag());
                query.bindValue(":id_track", QString::number(existing_id));
                query.exec();

                if (query.lastError().isValid())
                {
                    qWarning() << "UPDATE track failed: " << query.lastError().text();
                    result = false;
                }
            }
        }
        else
        {
            // No existing audio track found, insert it in DB.
            query.prepare("INSERT INTO TRACK (hash, path, filename, key, key_tag) "
                          "VALUES (:hash, :path, :filename, :key, :key_tag)");
            query.bindValue(":hash",     in_at->get_hash());
            query.bindValue(":path",     in_at->get_path());
            query.bindValue(":filename", in_at->get_filename());
            query.bindValue(":key",      in_at->get_music_key());
            query.bindValue(":key_tag",  in_at->get_music_key_tag());
            query.exec();

            if (query.lastError().isValid())
            {
                qWarning() << "INSERT track failed: " << query.lastError().text();
                result = false;
            }
        }

        // Release the DB connection.
        this->mutex.unlock();
    }
    else
    {
        // Db not open.
        qWarning() << "Can not store audio track: db not open";
        result = false;
    }

    qDebug() << "Data_persistence::store_audio_track done.";

    return result;
}

bool Data_persistence::get_audio_track(Audio_track *io_at)
{
    // Init result.
    bool result = true;

    qDebug() << "Data_persistence::get_audio_track...";

    // Check input parameter.
    if ((io_at == NULL) ||
        (io_at->get_hash().size() == 0))
    {
        qWarning() << "Can not get audio track: hash not specified.";
        result = false;
    }

    // Search the audio track (based on its hash) in DB.
    if ((result == true) &&
        (this->is_initialized == true))
    {
        // Ensure no other thread can access the DB connection.
        this->mutex.lock();

        QSqlQuery query = this->db.exec("SELECT key, key_tag, path, filename FROM TRACK WHERE hash=\"" + io_at->get_hash() + "\"");
        if (query.lastError().isValid())
        {
            qWarning() << "SELECT track failed: " << query.lastError().text();
            result = false;
        }
        else if (query.next() == true) // Check if there is a record.
        {
            // The audio track exists, fill the returned object.
            io_at->set_music_key(query.value(0).toString());
            io_at->set_music_key_tag(query.value(1).toString());
            io_at->set_fullpath(query.value(2).toString() + "/" + query.value(3).toString());
        }
        else
        {
            // Audio track not found.
            result = false;
        }

        // Release the DB connection.
        this->mutex.unlock();
    }

    qDebug() << "Data_persistence::get_audio_track done.";

    return result;
}

bool Data_persistence::store_cue_point(Audio_track *in_at, unsigned int in_number, unsigned int in_position)
{
    // Init result.
    bool result = true;

    qDebug() << "Data_persistence::store_cue_point...";

    // Check input parameter.
    if ((in_at == NULL) ||
        (in_at->get_hash().size() == 0) ||
        (in_number == 0) ||
        (in_number > MAX_NB_CUE_POINTS) ||
        (in_position > (MAX_MINUTES_TRACK * NB_SAMPLES_PER_MIN)))
    {
        result = false;
    }

    // Insert or update cue point for the specified audio track (identified by the hash).
    if ((result == true) &&
        (this->is_initialized == true))
    {
        // Ensure no other thread can access the DB connection.
        this->mutex.lock();

        // Try to get audio track from Db.
        QSqlQuery query_at = this->db.exec("SELECT id_track FROM TRACK WHERE hash=\"" + in_at->get_hash() + "\"");
        if (query_at.lastError().isValid())
        {
            qWarning() << "SELECT track failed: " << query_at.lastError().text();
            result = false;
        }
        else if (query_at.next() == true) // Check if there is a record.
        {
            // Audio track found, search for the cue point.
            QSqlQuery query_cuepoint = this->db.exec(
                      "SELECT id_cuepoint, position FROM CUE_POINT WHERE id_track=\"" + query_at.value(0).toString() + "\" AND number=\"" + QString::number(in_number) + "\"");
            if (query_cuepoint.lastError().isValid())
            {
                qWarning() << "SELECT cue_point failed: " << query_cuepoint.lastError().text();
                result = false;
            }
            else if (query_cuepoint.next() == true) // Check if there is a record.
            {
                // The cue point already exists, update it if the position changed.
                if (query_cuepoint.value(1) != in_position)
                {
                    int id_cuepoint = query_cuepoint.value(0).toInt();
                    query_cuepoint.prepare("UPDATE CUE_POINT SET position = :position WHERE id_cuepoint = :id_cuepoint");
                    query_cuepoint.bindValue(":position", in_position);
                    query_cuepoint.bindValue(":id_cuepoint", id_cuepoint);
                    query_cuepoint.exec();

                    if (query_cuepoint.lastError().isValid())
                    {
                        qWarning() << "UPDATE cue point failed: " << query_cuepoint.lastError().text();
                        result = false;
                    }
                }
            }
            else
            {
                // No existing cue point found, insert it in DB.
                query_cuepoint.prepare("INSERT INTO CUE_POINT (id_track, number, position) "
                              "VALUES (:id_track, :number, :position)");
                query_cuepoint.bindValue(":id_track", query_at.value(0));
                query_cuepoint.bindValue(":number",   in_number);
                query_cuepoint.bindValue(":position", in_position);
                query_cuepoint.exec();

                if (query_cuepoint.lastError().isValid())
                {
                    qWarning() << "INSERT cue point failed: " << query_cuepoint.lastError().text();
                    result = false;
                }
            }
        }

        // Release the DB connection.
        this->mutex.unlock();
    }
    else
    {
        // Db not open.
        qWarning() << "Can not store cue point: db not open";
        result = false;
    }

    qDebug() << "Data_persistence::store_cue_point done.";

    return result;
}

bool Data_persistence::get_cue_point(Audio_track *in_at, unsigned int in_number, unsigned int &out_position)
{
    // Init result.
    bool result = true;

    qDebug() << "Data_persistence::get_cue_point...";

    // Check input parameter.
    if ((in_at == NULL) ||
        (in_at->get_hash().size() == 0) ||
        (in_number == 0) ||
        (in_number > MAX_NB_CUE_POINTS))
    {
        qWarning() << "Can not get cue point: wrong params.";
        result = false;
    }

    // Search the audio track (based on its hash) in DB.
    if ((result == true) &&
        (this->is_initialized == true))
    {
        // Ensure no other thread can access the DB connection.
        this->mutex.lock();

        QSqlQuery query = this->db.exec("SELECT id_track FROM TRACK WHERE hash=\"" + in_at->get_hash() + "\"");
        if (query.lastError().isValid())
        {
            qWarning() << "SELECT track failed: " << query.lastError().text();
            result = false;
        }
        else if (query.next() == true) // Check if there is a record.
        {
            // The audio track exists, look for the specified cue point.
            QSqlQuery query_cue_point = this->db.exec(
                        "SELECT position FROM CUE_POINT WHERE id_track=\"" + query.value(0).toString() + "\" AND number=\"" + QString::number(in_number) + "\"");
            if (query_cue_point.lastError().isValid())
            {
                qWarning() << "SELECT cue point failed: " << query_cue_point.lastError().text();
                result = false;
            }
            else if (query_cue_point.next() == true) // Check if there is a record.
            {
                // The cue point exists, get position.
                out_position = query_cue_point.value(0).toInt();
            }
            else
            {
                // Cue point not found.
                result = false;
            }
        }
        else
        {
            // Audio track not found.
            result = false;
        }

        // Release the DB connection.
        this->mutex.unlock();
    }

    qDebug() << "Data_persistence::get_cue_point done.";

    return result;
}
