/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*---------------------------------------------------( data_persistence.cpp )-*/
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
    this->is_initialized = this->init_db();

    return;
}

Data_persistence::~Data_persistence()
{
    this->mutex.lock();
    if (this->db.isValid() == true)
    {
        this->db.close();
    }
    this->mutex.unlock();

    return;
}

bool Data_persistence::init_db()
{
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
        qCWarning(DS_DB) << "cannot open DB:" << this->db.lastError().text();
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
        qCWarning(DS_DB) << "creating base DB structure failed";
        return false;
    }

    this->mutex.unlock();

    return true;
}

bool Data_persistence::create_db_structure()
{
    // Init.
    bool result = true;

    // Create DB structure
    if (this->db.isOpen() == true)
    {
        QSqlQuery query;

        // Create TRACK table
        result = query.exec("CREATE TABLE IF NOT EXISTS \"TRACK\" "
                            "(\"id_track\" INTEGER PRIMARY KEY  AUTOINCREMENT  NOT NULL  UNIQUE , "
                            " \"hash\" VARCHAR NOT NULL , "
                            " \"bpm\" INTEGER, "
                            " \"key\" VARCHAR, "
                            " \"key_tag\" VARCHAR, "
                            " \"path\" VARCHAR, "
                            " \"filename\" VARCHAR);");

        // Create TRACK_CUE_POINT table
        if (result == true)
        {
            result = query.exec("CREATE TABLE IF NOT EXISTS \"TRACK_CUE_POINT\" "
                                "(\"id_cuepoint\" INTEGER PRIMARY KEY  AUTOINCREMENT  NOT NULL  UNIQUE , "
                                " \"id_track\" INTEGER  NOT NULL, "
                                " \"number\" INTEGER  NOT NULL, "
                                " \"position\" INTEGER NOT NULL, "
                                " FOREIGN KEY(id_track) REFERENCES TRACK(id_track));");
        }

        // Create TAG table
        if (result == true)
        {
            result = query.exec("CREATE TABLE IF NOT EXISTS \"TAG\" "
                                "(\"id_tag\" INTEGER PRIMARY KEY  AUTOINCREMENT  NOT NULL  UNIQUE , "
                                " \"name\" VARCHAR NOT NULL);");
        }

        // Create TRACK_TAG table
        if (result == true)
        {
            result = query.exec("CREATE TABLE IF NOT EXISTS \"TRACK_TAG\" "
                                "(\"id_track_tag\" INTEGER PRIMARY KEY  AUTOINCREMENT  NOT NULL  UNIQUE , "
                                " \"id_track\" INTEGER  NOT NULL, "
                                " \"id_tag\" INTEGER  NOT NULL  , "
                                " FOREIGN KEY(id_track) REFERENCES TRACK(id_track), "
                                " FOREIGN KEY(id_tag) REFERENCES TAG(id_tag));");
        }
    }
    else
    {
        // Db not open.
        qCWarning(DS_DB) << "can not create DB structure: db not open";
        result = false;
    }

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

bool Data_persistence::store_audio_track(QSharedPointer<Audio_track> &in_at)
{
    // Init result.
    bool result = true;

    // Check input parameter.
    if ((in_at.data() == nullptr) ||
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
            qCWarning(DS_DB) << "SELECT track failed: " << query.lastError().text();
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
                    qCWarning(DS_DB) << "UPDATE track failed: " << query.lastError().text();
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
                qCWarning(DS_DB) << "INSERT track failed: " << query.lastError().text();
                result = false;
            }
        }

        // Release the DB connection.
        this->mutex.unlock();
    }
    else
    {
        // Db not open.
        qCWarning(DS_DB) << "can not store audio track: db not open";
        result = false;
    }

    return result;
}

bool Data_persistence::get_audio_track(QSharedPointer<Audio_track> &io_at)
{
    // Init result.
    bool result = true;

    // Check input parameter.
    if ((io_at.data() == nullptr) ||
        (io_at->get_hash().size() == 0))
    {
        qCWarning(DS_DB) << "can not get audio track: hash not specified.";
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
            qCWarning(DS_DB) << "SELECT track failed: " << query.lastError().text();
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

    return result;
}

bool Data_persistence::store_cue_point(QSharedPointer<Audio_track> &in_at,
                                       unsigned int                 in_number,
                                       unsigned int                 in_position_msec)
{
    // Init result.
    bool result = true;

    // Check input parameter.
    if ((in_at.data() == nullptr) ||
        (in_at->get_hash().size() == 0) ||
        (in_number >= MAX_NB_CUE_POINTS) ||
        (in_position_msec > in_at->get_length()))
    {
        result = false;
    }

    // Insert or update cue point for the specified audio track (identified by the hash).
    if ((result == true) &&
        (this->is_initialized == true))
    {
        // Create audio track if not already in DB.
        if (this->store_audio_track(in_at) == false)
        {
            result = false;
        }
        else
        {
            // Ensure no other thread can access the DB connection.
            this->mutex.lock();

            // Get audio track id from Db.
            QSqlQuery query_at = this->db.exec("SELECT id_track FROM TRACK WHERE hash=\"" + in_at->get_hash() + "\"");
            if (query_at.lastError().isValid())
            {
                qCWarning(DS_DB) << "SELECT track failed: " << query_at.lastError().text();
                result = false;
            }
            else if (query_at.next() == true) // Check if there is a record.
            {
                // Audio track found, search for the cue point.
                QSqlQuery query_cuepoint = this->db.exec(
                          "SELECT id_cuepoint, position FROM TRACK_CUE_POINT WHERE id_track=\"" + query_at.value(0).toString() + "\" AND number=\"" + QString::number(in_number) + "\"");
                if (query_cuepoint.lastError().isValid())
                {
                    qCWarning(DS_DB) << "SELECT cue_point failed: " << query_cuepoint.lastError().text();
                    result = false;
                }
                else if (query_cuepoint.next() == true) // Check if there is a record.
                {
                    // The cue point already exists, update it if the position changed.
                    if (query_cuepoint.value(1) != in_position_msec)
                    {
                        int id_cuepoint = query_cuepoint.value(0).toInt();
                        query_cuepoint.prepare("UPDATE TRACK_CUE_POINT SET position = :position WHERE id_cuepoint = :id_cuepoint");
                        query_cuepoint.bindValue(":position", in_position_msec);
                        query_cuepoint.bindValue(":id_cuepoint", id_cuepoint);
                        query_cuepoint.exec();

                        if (query_cuepoint.lastError().isValid())
                        {
                            qCWarning(DS_DB) << "UPDATE cue point failed: " << query_cuepoint.lastError().text();
                            result = false;
                        }
                    }
                }
                else
                {
                    // No existing cue point found, insert it in DB.
                    query_cuepoint.prepare("INSERT INTO TRACK_CUE_POINT (id_track, number, position) "
                                  "VALUES (:id_track, :number, :position)");
                    query_cuepoint.bindValue(":id_track", query_at.value(0));
                    query_cuepoint.bindValue(":number",   in_number);
                    query_cuepoint.bindValue(":position", in_position_msec);
                    query_cuepoint.exec();

                    if (query_cuepoint.lastError().isValid())
                    {
                        qCWarning(DS_DB) << "INSERT cue point failed: " << query_cuepoint.lastError().text();
                        result = false;
                    }
                }
            }

            // Release the DB connection.
            this->mutex.unlock();
        }
    }
    else
    {
        // Db not open.
        qCWarning(DS_DB) << "can not store cue point: db not open";
        result = false;
    }

    return result;
}

bool Data_persistence::get_cue_point(QSharedPointer<Audio_track> &in_at,
                                     unsigned int                 in_number,
                                     unsigned int                &out_position_msec)
{
    // Init result.
    bool result = true;

    // Check input parameter.
    if ((in_at.data() == nullptr) ||
        (in_at->get_hash().size() == 0) ||
        (in_number >= MAX_NB_CUE_POINTS))
    {
        qCWarning(DS_DB) << "can not get cue point: wrong params.";
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
            qCWarning(DS_DB) << "SELECT track failed: " << query.lastError().text();
            result = false;
        }
        else if (query.next() == true) // Check if there is a record.
        {
            // The audio track exists, look for the specified cue point.
            QSqlQuery query_cue_point = this->db.exec(
                        "SELECT position FROM TRACK_CUE_POINT WHERE id_track=\"" + query.value(0).toString() + "\" AND number=\"" + QString::number(in_number) + "\"");
            if (query_cue_point.lastError().isValid())
            {
                qCWarning(DS_DB) << "SELECT cue point failed: " << query_cue_point.lastError().text();
                result = false;
            }
            else if (query_cue_point.next() == true) // Check if there is a record.
            {
                // The cue point exists, get position.
                out_position_msec = query_cue_point.value(0).toInt();
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

    return result;
}

bool Data_persistence::delete_cue_point(QSharedPointer<Audio_track> &in_at,
                                        unsigned int                 in_number)
{
    // Init result.
    bool result = true;

    // Check input parameter.
    if ((in_at.data() == nullptr) ||
        (in_at->get_hash().size() == 0) ||
        (in_number >= MAX_NB_CUE_POINTS))
    {
        qCWarning(DS_DB) << "can not delete cue point: wrong params.";
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
            // Can not select audio track in DB.
            qCWarning(DS_DB) << "SELECT track failed: " << query.lastError().text();
            result = false;
        }
        else if (query.next() == true) // Check if there is a record.
        {
            // The audio track exists, look for the specified cue point.
            QSqlQuery query_cue_point = this->db.exec(
                        "DELETE FROM TRACK_CUE_POINT WHERE id_track=\"" + query.value(0).toString() + "\" AND number=\"" + QString::number(in_number) + "\"");
            if (query_cue_point.lastError().isValid())
            {
                // Can not delete cue point.
                qCWarning(DS_DB) << "DELETE cue point failed: " << query_cue_point.lastError().text();
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

    return result;
}

bool Data_persistence::store_tag(const QString &in_name)
{
    // Init result.
    bool result = true;

    // Check input parameter.
    if (in_name == "")
    {
        result = false;
        qCWarning(DS_DB) << "Can not store an empty tag";
    }

    // Insert the tag name (prevent duplicate).
    if ((result == true) &&
        (this->is_initialized == true))
    {
        // Ensure no other thread can access the DB connection.
        this->mutex.lock();

        // Try to get tag from Db.
        QSqlQuery query = this->db.exec("SELECT id_tag, name FROM TAG WHERE name=\"" + in_name + "\"");
        if (query.lastError().isValid())
        {
            qCWarning(DS_DB) << "SELECT tag failed: " << query.lastError().text();
            result = false;
        }
        else if (query.next() == false)
        {
            // Tag not found, add it.
            query.prepare("INSERT INTO TAG (name) "
                          "VALUES (:name)");
            query.bindValue(":name", in_name);
            query.exec();

            if (query.lastError().isValid())
            {
                qCWarning(DS_DB) << "INSERT tag failed: " << query.lastError().text();
                result = false;
            }
        }

        // Release the DB connection.
        this->mutex.unlock();
    }
    else
    {
        // Db not open.
        qCWarning(DS_DB) << "can not store new tag: db not open";
    }

    return result;
}

bool Data_persistence::rename_tag(const QString &in_old_name, const QString &in_new_name)
{
    // Init result.
    bool result = true;

    // Check input parameters.
    if ((in_old_name == "") ||
        (in_new_name == ""))
    {
        result = false;
        qCWarning(DS_DB) << "Can rename an empty tag";
    }

    // Check if the tag exists and rename it.
    if ((result == true) &&
        (this->is_initialized == true))
    {
        // Ensure no other thread can access the DB connection.
        this->mutex.lock();

        // Try to get tag from Db.
        QSqlQuery query = this->db.exec("SELECT id_tag, name FROM TAG WHERE name=\"" + in_old_name + "\"");
        if (query.lastError().isValid())
        {
            qCWarning(DS_DB) << "SELECT tag failed: " << query.lastError().text();
            result = false;
        }
        else if (query.next() == true)
        {
            // Tag found.
            int existing_id = query.value(0).toInt();
            query.prepare("UPDATE TAG SET name = :name "
                          "WHERE id_tag = :id_tag");
            query.bindValue(":name", in_new_name);
            query.bindValue(":id_tag", QString::number(existing_id));
            query.exec();

            if (query.lastError().isValid())
            {
                qCWarning(DS_DB) << "UPDATE tag failed: " << query.lastError().text();
                result = false;
            }
        }
        else
        {
            // Tag not found, error.
            result = false;
            qCWarning(DS_DB) << "can not found the tag " << in_old_name;
        }

        // Release the DB connection.
        this->mutex.unlock();
    }
    else
    {
        // Db not open.
        qCWarning(DS_DB) << "can not rename tag: db not open";
    }

    return result;
}

bool Data_persistence::delete_tag(const QString &in_name)
{
    // Init result.
    bool result = true;

    // Check input parameter.
    if (in_name == "")
    {
        qCWarning(DS_DB) << "can not delete tag: empty tag name.";
        result = false;
    }

    // Search the tag.
    if ((result == true) &&
        (this->is_initialized == true))
    {
        // Ensure no other thread can access the DB connection.
        this->mutex.lock();

        QSqlQuery query = this->db.exec("SELECT id_tag FROM TAG WHERE name=\"" + in_name + "\"");
        if (query.lastError().isValid())
        {
            // Can not select tag in DB.
            qCWarning(DS_DB) << "SELECT tag failed: " << query.lastError().text();
            result = false;
        }
        else if (query.next() == true) // Check if there is a record.
        {
            // The tag exists, remove it.
            QSqlQuery query_tag = this->db.exec("DELETE FROM TAG WHERE id_tag=\"" + query.value(0).toString() + "\"");
            if (query_tag.lastError().isValid())
            {
                // Can not delete tag.
                qCWarning(DS_DB) << "DELETE tag failed: " << query_tag.lastError().text();
                result = false;
            }
            else
            {
                // Delete also all references to this tag for all tracks.
                QSqlQuery query_track_tag = this->db.exec("DELETE FROM TRACK_TAG "
                                                          "WHERE id_tag=\"" + query.value(0).toString() + "\"");
                if (query_track_tag.lastError().isValid())
                {
                    // Can not delete track/tag.
                    qCWarning(DS_DB) << "DELETE track/tag failed: " << query_track_tag.lastError().text();
                    result = false;
                }
            }
        }
        else
        {
            // Tag not found.
            qCWarning(DS_DB) << "can not delete tag: tag not found";
            result = false;
        }

        // Release the DB connection.
        this->mutex.unlock();
    }

    return result;
}

bool Data_persistence::get_full_tag_list(QStringList &out_tags)
{
    // Init result.
    bool result = true;

    // Get all tags.
    if ((result == true) &&
        (this->is_initialized == true))
    {
        // Ensure no other thread can access the DB connection.
        this->mutex.lock();

        QSqlQuery query = this->db.exec("SELECT name FROM TAG");
        if (query.lastError().isValid())
        {
            // Can not select tag in DB.
            qCWarning(DS_DB) << "SELECT tags failed: " << query.lastError().text();
            result = false;
        }
        else
        {
            while (query.next() == true)
            {
                out_tags.push_back(query.value(0).toString());
            }
        }

        // Release the DB connection.
        this->mutex.unlock();
    }

    return result;
}

bool Data_persistence::add_tag_to_track(QSharedPointer<Audio_track> &in_at,
                                        const QString               &in_tag_name)
{
    // Init result.
    bool result = true;

    // Check input parameter.
    if ((in_at.data() == nullptr) ||
        (in_at->get_hash().size() == 0) ||
        (in_tag_name == ""))
    {
        qCWarning(DS_DB) << "can not add tag to track: wrong params.";
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
            qCWarning(DS_DB) << "SELECT track failed: " << query.lastError().text();
            result = false;
        }
        else if (query.next() == true) // Check if there is a record.
        {
            // The audio track exists, let's search the tag.
            QSqlQuery query_tag = this->db.exec("SELECT id_tag FROM TAG WHERE name=\"" + in_tag_name + "\"");
            if (query_tag.lastError().isValid())
            {
                qCWarning(DS_DB) << "SELECT tag failed: " << query_tag.lastError().text();
                result = false;
            }
            else if (query_tag.next() == true) // Check if there is a record.
            {
                // The tag exists, let's do the association with the track (do not duplicate).
                result = this->store_track_tag(query.value(0).toString(),
                                               query_tag.value(0).toString());
            }
            else
            {
                // Tag not found.
                qCWarning(DS_DB) << "can not add tag to track: tag not found";
                result = false;
            }
        }
        else
        {
            // Audio track not found.
            qCWarning(DS_DB) << "can not add tag to track: track not found";
            result = false;
        }

        // Release the DB connection.
        this->mutex.unlock();
    }
    

    return result;
}

bool Data_persistence::store_track_tag(const QString &in_id_track,
                                       const QString &in_id_tag)
{
    // Init result.
    bool result = true;
    
    // Check input parameter.
    if ((in_id_track == "") ||
        (in_id_tag   == ""))
    {
        result = false;
        qCWarning(DS_DB) << "Bad input ids";
    }

    // Insert the tag/track association (prevent duplicate).
    if ((result == true) &&
        (this->is_initialized == true))
    {
        // Try to get tag/track association  from Db.
        QSqlQuery query = this->db.exec("SELECT id_track, id_tag FROM TRACK_TAG WHERE id_track=\"" + in_id_track + "\" AND id_tag=\"" + in_id_tag + "\"");
        if (query.lastError().isValid())
        {
            qCWarning(DS_DB) << "SELECT track/tag failed: " << query.lastError().text();
            result = false;
        }
        else if (query.next() == false)
        {
            // Track/tag not found, add it.
            query.prepare("INSERT INTO TRACK_TAG (id_track, id_tag) VALUES (:id_track, :id_tag)");
            query.bindValue(":id_track", in_id_track);
            query.bindValue(":id_tag", in_id_tag);
            query.exec();

            if (query.lastError().isValid())
            {
                qCWarning(DS_DB) << "INSERT track/tag failed: " << query.lastError().text();
                result = false;
            }
        }
    }
    else
    {
        // Db not open.
        qCWarning(DS_DB) << "can not store track/tag: db not open";
    }
    
    return result;
}

bool Data_persistence::get_tags_from_track(QSharedPointer<Audio_track> &in_at,
                                           QStringList                 &out_tags)
{
    // Init result.
    bool result = true;

    // Check input parameter.
    if ((in_at.data() == nullptr) ||
        (in_at->get_hash().size() == 0))
    {
        qCWarning(DS_DB) << "can not get tags from track: wrong params.";
        result = false;
    }

    // Get all tags.
    if ((result == true) &&
        (this->is_initialized == true))
    {
        // Ensure no other thread can access the DB connection.
        this->mutex.lock();

        QSqlQuery query = this->db.exec("SELECT name FROM TAG "
                                        "JOIN TRACK_TAG "
                                        "ON TAG.id_tag=TRACK_TAG.id_tag "
                                        "JOIN TRACK "
                                        "ON TRACK_TAG.id_track=TRACK.id_track "
                                        "WHERE TRACK.hash=\"" + in_at->get_hash() + "\"");
        if (query.lastError().isValid())
        {
            // Can not select tag list in DB.
            qCWarning(DS_DB) << "SELECT tags failed: " << query.lastError().text();
            result = false;
        }
        else
        {
            // Fill result string list.
            while (query.next() == true)
            {
                out_tags.push_back(query.value(0).toString());
            }
        }

        // Release the DB connection.
        this->mutex.unlock();
    }

    return result;
}


bool Data_persistence::rem_tag_from_track(QSharedPointer<Audio_track> &in_at,
                                          const QString               &in_tag_name)
{
    // Init result.
    bool result = true;

    // Check input parameter.
    if ((in_at.data() == nullptr) ||
        (in_at->get_hash().size() == 0) ||
        (in_tag_name == ""))
    {
        qCWarning(DS_DB) << "can not delete tag from track: wrong params.";
        result = false;
    }

    // Get all tags.
    if ((result == true) &&
        (this->is_initialized == true))
    {
        // Ensure no other thread can access the DB connection.
        this->mutex.lock();

        QSqlQuery query = this->db.exec("DELETE FROM TRACK_TAG "
                                        "WHERE id_track_tag IN "
                                        "(SELECT id_track_tag FROM TRACK_TAG "
                                        "JOIN TAG "
                                        "ON TRACK_TAG.id_tag=TAG.id_tag "
                                        "JOIN TRACK "
                                        "ON TRACK_TAG.id_track=TRACK.id_track "
                                        "WHERE TRACK.hash=\"" + in_at->get_hash() + "\" AND TAG.name=\"" + in_tag_name + "\")");
        if (query.lastError().isValid())
        {
            // Can not delete tag in DB.
            qCWarning(DS_DB) << "DELETE FROM TRACK_TAG failed: " << query.lastError().text();
            result = false;
        }

        // Release the DB connection.
        this->mutex.unlock();
    }

    return result;
}
