/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*---------------------------------------------------( data_persistence.cpp )-*/
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
/*                    Class defining how to store audio data                  */
/*                                                                            */
/*============================================================================*/

#include <QtDebug>
#include <QSqlError>
#include <QFileInfo>
#include <QDir>
#include <QSqlQuery>
#include <QDateTime>
#include <QStandardPaths>

#include "utils.h"
#include "app/application_const.h"
#include "app/application_logging.h"
#include "tracks/data_persistence.h"

Data_persistence::Data_persistence() // FIXME: rename to Audio_track_persistence ?
{
    this->db = QSqlDatabase::addDatabase("QSQLITE");
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
    QFileInfo path_info(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/digitalscratch.sqlite");
    this->db.setDatabaseName(path_info.absoluteFilePath());

    // Make sure path exists, if not create it.
    QDir dir;
    dir.mkpath(path_info.absolutePath());

#ifndef ENABLE_TEST_MODE
    // Backup previous DB file if it exists.
    this->backup_db();
#endif

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

#ifndef ENABLE_TEST_MODE
void Data_persistence::backup_db()
{
    QFileInfo db_file(this->db.databaseName());
    QString db_dir(db_file.absolutePath());
    QString db_name(db_file.fileName());
    QString timestamp(QDateTime::currentDateTime().toString("yyyyMMdd-HH:mm:ss"));
    QString db_backup_dir(db_dir + QDir::separator() + "db_backup");
    QString db_backup_name(timestamp + "-" + db_name);

    if (db_file.exists() == true)
    {
        // Create a backup directory if it does not exists.
        QDir dir;
        dir.mkpath(db_backup_dir);

        if (dir.cd(db_backup_dir) == true)
        {
            // Backup the DB file: duplicate it and prefix the name with date/time.
            QFile::copy(db_file.absoluteFilePath(), db_backup_dir + QDir::separator() + db_backup_name);

            // Remove old backups.
            dir.setNameFilters(QStringList() << "*.sqlite");
            dir.setFilter(QDir::Files);
            if (dir.entryList().count() > 5)
            {
                dir.remove(dir.entryList().at(0));
            }
        }
    }

    return;
}
#endif

bool Data_persistence::restore_db(const QString &file_path)
{
    bool res = true;

    // Check that the file to restore exists.
    QFileInfo fi(file_path);
    QString abs_file_path = fi.absoluteFilePath();
    if (fi.exists() == true)
    {
        // Get file path of current DB.
        QFileInfo cur_db_file(this->db.databaseName());
        QString cur_db_file_path = cur_db_file.absoluteFilePath();

        // Backup current DB.
#ifndef ENABLE_TEST_MODE
        this->backup_db();
#endif

        // Close current DB.
        this->mutex.lock();
        if (this->db.isValid() == true)
        {
            this->db.close();
        }
        this->mutex.unlock();

        // Overwrite with restoration DB.
        if (QFile::exists(cur_db_file_path))
        {
            QFile::remove(cur_db_file_path);
        }
        if ((res = QFile::copy(abs_file_path, cur_db_file_path)) == false)
        {
            qCWarning(DS_DB) << "can not restore DB" << abs_file_path << "into" << cur_db_file_path;
        }

        // Init DB again.
        this->is_initialized = this->init_db();
    }
    else
    { // Restore file does not exists.
        qCWarning(DS_DB) << "can not restore DB, file does not exists:" << file_path;
        res = false;
    }

    return res;
}

bool
Data_persistence::export_db(const QString &dest_file_path)
{
    bool res = true;

    QFileInfo db_file(this->db.databaseName());
    QString abs_db_file = db_file.absoluteFilePath();

    if (db_file.exists() == true)
    {
        // Remove existing backup file.
        if (QFile::exists(dest_file_path))
        {
            QFile::remove(dest_file_path);
        }

        // Backup the DB file.
        if ((res = QFile::copy(abs_db_file, dest_file_path)) == false)
        {
            qCWarning(DS_DB) << "can not export DB" << abs_db_file << "into" << dest_file_path;
        }
    }
    else
    { // Current DB file does not exists.
        qCWarning(DS_DB) << "can not restore DB, file does not exists:" << abs_db_file;
        res = false;
    }

    return res;
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

        // Add an index on TRACK.hash which will be the main key to search a track.
        if (result == true)
        {
            result = query.exec("CREATE UNIQUE INDEX IF NOT EXISTS index_TRACK_hash on TRACK (hash);");
        }

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
                                " \"id_tag\" INTEGER  NOT NULL, "
                                " \"position\" INTEGER, "
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

bool Data_persistence::rollback_transaction()
{
    return this->db.rollback();
}

bool Data_persistence::store_audio_track(const QSharedPointer<Audio_track> &at)
{
    // Init result.
    bool result = true;

    // Check input parameter.
    if ((at.data() == nullptr) ||
       (at->get_hash().size() == 0))
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
        QSqlQuery query = this->db.exec("SELECT id_track, path, filename, key, key_tag FROM TRACK WHERE hash=\"" + at->get_hash() + "\"");
        if (query.lastError().isValid())
        {
            qCWarning(DS_DB) << "SELECT track failed: " << query.lastError().text();
            result = false;
        }
        else if (query.next() == true) // Check if there is a record.
        {
            // An audio track with same hash already exists, update it if at least one element changed.
            if ((query.value(1) != at->get_path()) ||
                (query.value(2) != at->get_filename()) ||
                (query.value(3) != at->get_music_key()) ||
                (query.value(4) != at->get_music_key_tag()))
            {
                int existing_id = query.value(0).toInt();
                query.prepare("UPDATE TRACK SET path = :path, filename = :filename, key = :key, key_tag = :key_tag "
                              "WHERE id_track = :id_track");
                query.bindValue(":path",     at->get_path());
                query.bindValue(":filename", at->get_filename());
                query.bindValue(":key",      at->get_music_key());
                query.bindValue(":key_tag",  at->get_music_key_tag());
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
            query.bindValue(":hash",     at->get_hash());
            query.bindValue(":path",     at->get_path());
            query.bindValue(":filename", at->get_filename());
            query.bindValue(":key",      at->get_music_key());
            query.bindValue(":key_tag",  at->get_music_key_tag());
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
            qCDebug(DS_DB) << "audio track not found in DB, hash: " << io_at->get_hash();
        }

        // Release the DB connection.
        this->mutex.unlock();

        // Get the list of tags associated to that track.
        QStringList tags;
        if (this->get_tags_from_track(io_at, tags) == true)
        {
            io_at->set_tags(tags);
        }
    }

    return result;
}

bool Data_persistence::store_cue_point(const QSharedPointer<Audio_track> &at,
                                       const unsigned int                &number,
                                       const unsigned int                &position_msec)
{
    // Init result.
    bool result = true;

    // Check input parameter.
    if ((at.data() == nullptr) ||
        (at->get_hash().size() == 0) ||
        (number >= MAX_NB_CUE_POINTS) ||
        (position_msec > at->get_length()))
    {
        result = false;
    }

    // Insert or update cue point for the specified audio track (identified by the hash).
    if ((result == true) &&
        (this->is_initialized == true))
    {
        // Create audio track if not already in DB.
        if (this->store_audio_track(at) == false)
        {
            result = false;
        }
        else
        {
            // Ensure no other thread can access the DB connection.
            this->mutex.lock();

            // Get audio track id from Db.
            QSqlQuery query_at = this->db.exec("SELECT id_track FROM TRACK WHERE hash=\"" + at->get_hash() + "\"");
            if (query_at.lastError().isValid())
            {
                qCWarning(DS_DB) << "SELECT track failed: " << query_at.lastError().text();
                result = false;
            }
            else if (query_at.next() == true) // Check if there is a record.
            {
                // Audio track found, search for the cue point.
                QSqlQuery query_cuepoint = this->db.exec(
                          "SELECT id_cuepoint, position FROM TRACK_CUE_POINT WHERE id_track=\"" + query_at.value(0).toString() + "\" AND number=\"" + QString::number(number) + "\"");
                if (query_cuepoint.lastError().isValid())
                {
                    qCWarning(DS_DB) << "SELECT cue_point failed: " << query_cuepoint.lastError().text();
                    result = false;
                }
                else if (query_cuepoint.next() == true) // Check if there is a record.
                {
                    // The cue point already exists, update it if the position changed.
                    if (query_cuepoint.value(1) != position_msec)
                    {
                        int id_cuepoint = query_cuepoint.value(0).toInt();
                        query_cuepoint.prepare("UPDATE TRACK_CUE_POINT SET position = :position WHERE id_cuepoint = :id_cuepoint");
                        query_cuepoint.bindValue(":position", position_msec);
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
                    query_cuepoint.bindValue(":number",   number);
                    query_cuepoint.bindValue(":position", position_msec);
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

bool Data_persistence::get_cue_point(const QSharedPointer<Audio_track> &at,
                                     const unsigned int                &number,
                                     unsigned int                      &out_position_msec)
{
    // Init result.
    bool result = true;

    // Check input parameter.
    if ((at.data() == nullptr) ||
        (at->get_hash().size() == 0) ||
        (number >= MAX_NB_CUE_POINTS))
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

        QSqlQuery query = this->db.exec("SELECT id_track FROM TRACK WHERE hash=\"" + at->get_hash() + "\"");
        if (query.lastError().isValid())
        {
            qCWarning(DS_DB) << "SELECT track failed: " << query.lastError().text();
            result = false;
        }
        else if (query.next() == true) // Check if there is a record.
        {
            // The audio track exists, look for the specified cue point.
            QSqlQuery query_cue_point = this->db.exec(
                        "SELECT position FROM TRACK_CUE_POINT WHERE id_track=\"" + query.value(0).toString() + "\" AND number=\"" + QString::number(number) + "\"");
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

bool Data_persistence::delete_cue_point(const QSharedPointer<Audio_track> &at,
                                        const unsigned int                &number)
{
    // Init result.
    bool result = true;

    // Check input parameter.
    if ((at.data() == nullptr) ||
        (at->get_hash().size() == 0) ||
        (number >= MAX_NB_CUE_POINTS))
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

        QSqlQuery query = this->db.exec("SELECT id_track FROM TRACK WHERE hash=\"" + at->get_hash() + "\"");
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
                        "DELETE FROM TRACK_CUE_POINT WHERE id_track=\"" + query.value(0).toString() + "\" AND number=\"" + QString::number(number) + "\"");
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

bool Data_persistence::store_tag(const QString &name)
{
    // Init result.
    bool result = true;

    // Check input parameter.
    if (name == "")
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
        QSqlQuery query = this->db.exec("SELECT id_tag, name FROM TAG WHERE name=\"" + name + "\"");
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
            query.bindValue(":name", name);
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

bool Data_persistence::rename_tag(const QString &old_name, const QString &new_name)
{
    // Init result.
    bool result = true;

    // Check input parameters.
    if ((old_name == "") ||
        (new_name == ""))
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
        QSqlQuery query = this->db.exec("SELECT id_tag, name FROM TAG WHERE name=\"" + old_name + "\"");
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
            query.bindValue(":name", new_name);
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
            qCWarning(DS_DB) << "can not found the tag " << old_name;
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

bool Data_persistence::delete_tag(const QString &name)
{
    // Init result.
    bool result = true;

    // Check input parameter.
    if (name == "")
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

        QString select_str = "SELECT id_tag FROM TAG WHERE name=\"" + name + "\"";
        QSqlQuery query = this->db.exec(select_str);
        if (query.lastError().isValid())
        {
            // Can not select tag in DB.
            qCWarning(DS_DB) << "SELECT tag failed (" << query.lastError().text() << ") "
                             << select_str;
            result = false;
        }
        else if (query.next() == true) // Check if there is a record.
        {
            // Delete all references to this tag for all tracks.
            QString del_trtag_str = "DELETE FROM TRACK_TAG "
                                "WHERE id_tag=\"" + query.value(0).toString() + "\"";
            QSqlQuery query_track_tag = this->db.exec(del_trtag_str);
            if (query_track_tag.lastError().isValid())
            {
                // Can not delete track/tag.
                qCWarning(DS_DB) << "DELETE track/tag failed (" << query_track_tag.lastError().text() << ") "
                                 << del_trtag_str;
                result = false;
            }
            else
            {
                // All references to the tag has been deleted.
                qCDebug(DS_DB) << "DELETE track/tag success: " << del_trtag_str;

                // Now remove the tag itself.
                QString del_tag_str = "DELETE FROM TAG WHERE id_tag=\"" + query.value(0).toString() + "\"";
                QSqlQuery query_tag = this->db.exec(del_tag_str);
                if (query_tag.lastError().isValid())
                {
                    // Can not delete tag.
                    qCWarning(DS_DB) << "DELETE tag failed (" << query_tag.lastError().text() << ") "
                                     << del_tag_str;
                    result = false;
                }
                else
                {
                    // The tag has been deleted.
                    qCDebug(DS_DB) << "DELETE tag success: " << del_tag_str;
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
    if (this->is_initialized == true)
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

bool Data_persistence::add_tag_to_track(const QSharedPointer<Audio_track> &at,
                                        const QString                     &tag_name)
{
    // Init result.
    bool result = true;

    // Check input parameter.
    if ((at.data() == nullptr) ||
        (at->get_hash().size() == 0) ||
        (tag_name == ""))
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

        QSqlQuery query = this->db.exec("SELECT id_track FROM TRACK WHERE hash=\"" + at->get_hash() + "\"");
        if (query.lastError().isValid())
        {
            qCWarning(DS_DB) << "SELECT track failed: " << query.lastError().text();
            result = false;
        }
        else if (query.next() == true) // Check if there is a record.
        {
            // The audio track exists, let's search the tag.
            QSqlQuery query_tag = this->db.exec("SELECT id_tag FROM TAG WHERE name=\"" + tag_name + "\"");
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

    // Reorganize position of tracks in tracklist of a tag.
    if (result == true)
    {
        result = this->reorganize_track_pos_in_tag_list();
    }

    return result;
}

bool Data_persistence::store_track_tag(const QString &id_track,
                                       const QString &id_tag)
{
    // Init result.
    bool result = true;

    // Check input parameter.
    if ((id_track == "") ||
        (id_tag   == ""))
    {
        result = false;
        qCWarning(DS_DB) << "Bad input ids";
    }

    // Insert the tag/track association (prevent duplicate).
    if ((result == true) &&
        (this->is_initialized == true))
    {
        // Try to get tag/track association  from Db.
        QSqlQuery query = this->db.exec("SELECT id_track, id_tag FROM TRACK_TAG WHERE id_track=\"" + id_track + "\" AND id_tag=\"" + id_tag + "\"");
        if (query.lastError().isValid())
        {
            qCWarning(DS_DB) << "SELECT track/tag failed: " << query.lastError().text();
            result = false;
        }
        else if (query.next() == false)
        {
            // Track/tag not found, add it.
            query.prepare("INSERT INTO TRACK_TAG (id_track, id_tag) VALUES (:id_track, :id_tag)");
            query.bindValue(":id_track", id_track);
            query.bindValue(":id_tag", id_tag);
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

bool Data_persistence::rem_tag_from_track(const QSharedPointer<Audio_track> &at,
                                          const QString                     &tag_name)
{
    // Init result.
    bool result = true;

    // Check input parameter.
    if ((at.data() == nullptr) ||
        (at->get_hash().size() == 0) ||
        (tag_name == ""))
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
                                        "WHERE TRACK.hash=\"" + at->get_hash() + "\" AND TAG.name=\"" + tag_name + "\")");
        if (query.lastError().isValid())
        {
            // Can not delete tag in DB.
            qCWarning(DS_DB) << "DELETE FROM TRACK_TAG failed: " << query.lastError().text();
            result = false;
        }

        // Release the DB connection.
        this->mutex.unlock();
    }

    // Reorganize position of tracks in tracklist of a tag.
    if (result == true)
    {
        result = this->reorganize_track_pos_in_tag_list();
    }

    return result;
}

bool Data_persistence::get_tags_from_track(const QSharedPointer<Audio_track> &at,
                                           QStringList                       &out_tags)
{
    // Init result.
    bool result = true;

    // Check input parameter.
    if ((at.data() == nullptr) ||
        (at->get_hash().size() == 0))
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
                                        "WHERE TRACK.hash=\"" + at->get_hash() + "\"");
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

bool Data_persistence::get_tracks_from_tag(const QString &tag_name,
                                           QStringList   &out_tracklist)
{
    // Init result.
    bool result = true;

    // Check input parameter.
    if (tag_name == "")
    {
        qCWarning(DS_DB) << "can not get track list: empty tag name.";
        result = false;
    }

    // Get all tracks tagged with the specified tag name.
    if ((result == true) &&
        (this->is_initialized == true))
    {
        // Ensure no other thread can access the DB connection.
        this->mutex.lock();

        QSqlQuery query = this->db.exec("SELECT path, filename FROM TRACK "
                                        "JOIN TRACK_TAG "
                                        "ON TRACK.id_track=TRACK_TAG.id_track "
                                        "JOIN TAG "
                                        "ON TRACK_TAG.id_tag=TAG.id_tag "
                                        "WHERE TAG.name=\"" + tag_name + "\" "
                                        "ORDER BY TRACK_TAG.position");
        if (query.lastError().isValid())
        {
            // Can not select track list in DB.
            qCWarning(DS_DB) << "SELECT tracks failed: " << query.lastError().text();
            result = false;
        }
        else
        {
            // Fill result string list.
            while (query.next() == true)
            {
                out_tracklist.push_back(query.value(0).toString() + "/" + query.value(1).toString());
            }
        }

        // Release the DB connection.
        this->mutex.unlock();
    }

    return result;
}

bool Data_persistence::reorganize_track_pos_in_tag_list() // TODO: do it only for a specified tag (much faster).
{
    // Init result.
    bool result = true;

    if (this->is_initialized == true)
    {
        // Ensure no other thread can access the DB connection.
        this->mutex.lock();

        // Get all tag ids.
        QSqlQuery query_tag_ids = this->db.exec("SELECT id_tag FROM TAG");
        if (query_tag_ids.lastError().isValid())
        {
            // Can not select tag in DB.
            qCWarning(DS_DB) << "SELECT tags failed: " << query_tag_ids.lastError().text();
            result = false;
        }
        else
        {
            // For each tag ids, get the list of track ids (position value in ascendant order).
            while (query_tag_ids.next() == true)
            {
                int pos = 0;
                QSqlQuery query_tracklist = this->db.exec("SELECT id_track_tag, position FROM TRACK_TAG "
                                                          "WHERE TRACK_TAG.id_tag=" + query_tag_ids.value(0).toString() + " "
                                                          "ORDER BY CASE WHEN position IS NULL THEN 1 ELSE 0 END, position");
                if (query_tracklist.lastError().isValid())
                {
                    // Can not select tag in DB.
                    qCWarning(DS_DB) << "SELECT tracklist failed: " << query_tracklist.lastError().text();
                    result = false;
                }
                else
                {
                    // For each tag_track association (which are ordered), overwrite the position starting from 0.
                    // tag_track associations with no position are append at the end.
                    QSqlQuery query_update_tracklist(this->db);
                    while (query_tracklist.next() == true)
                    {
                        query_update_tracklist.prepare("UPDATE TRACK_TAG SET position = :position "
                                                       "WHERE id_track_tag = :id_track_tag");
                        query_update_tracklist.bindValue(":position", QString::number(pos));
                        query_update_tracklist.bindValue(":id_track_tag", query_tracklist.value(0).toString());
                        query_update_tracklist.exec();

                        if (query_update_tracklist.lastError().isValid())
                        {
                            qCWarning(DS_DB) << "UPDATE track_tag failed: " << query_update_tracklist.lastError().text();
                            result = false;
                        }

                        pos++;
                    }
                }
            }
        }

        // Release the DB connection.
        this->mutex.unlock();
    }

    return result;
}

int Data_persistence::get_track_pos_in_tag_list(const QSharedPointer<Audio_track> &at,
                                                const QString &tag_name)
{
    int pos = -1;

    if (this->is_initialized == true)
    {
        // Ensure no other thread can access the DB connection.
        this->mutex.lock();

        QSqlQuery query = this->db.exec("SELECT TRACK_TAG.position FROM TRACK_TAG "
                                        "JOIN TRACK ON TRACK.id_track=TRACK_TAG.id_track "
                                        "JOIN TAG ON TAG.id_tag=TRACK_TAG.id_tag "
                                        "WHERE TRACK.hash=\"" + at->get_hash() + "\" AND TAG.name=\"" + tag_name + "\"");
        if (query.lastError().isValid())
        {
            // Can not select position in DB.
            qCWarning(DS_DB) << "SELECT position failed: " << query.lastError().text();
        }
        else
        {
            if (query.next() == true)
            {
                // Get position of track in the tracklist of the specified tag.
                pos = query.value(0).toInt();
            }
        }

        // Release the DB connection.
        this->mutex.unlock();
    }

    return pos;
}

bool Data_persistence::set_track_position_in_tag_list(const QString &tag_name,
                                                      const QSharedPointer<Audio_track> &at,
                                                      const int &position)
{
    // Init result.
    bool result = true;

    if (this->is_initialized == true)
    {
        // Ensure no other thread can access the DB connection.
        this->mutex.lock();

        QSqlQuery query_update_pos(this->db);
        query_update_pos.prepare("UPDATE TRACK_TAG SET position = :position "
                                 "WHERE id_track_tag = (SELECT TRACK_TAG.id_track_tag FROM TRACK_TAG "
                                                        "JOIN TRACK ON TRACK.id_track = TRACK_TAG.id_track "
                                                        "JOIN TAG ON TAG.id_tag = TRACK_TAG.id_tag "
                                                        "WHERE TRACK.hash = :hash AND TAG.name = :tag)");
        query_update_pos.bindValue(":position", QString::number(position));
        query_update_pos.bindValue(":hash", at->get_hash());
        query_update_pos.bindValue(":tag", tag_name);
        query_update_pos.exec();
        if (query_update_pos.lastError().isValid())
        {
            qCWarning(DS_DB) << "UPDATE position failed: " << query_update_pos.lastError().text();
            result = false;
        }

        // Release the DB connection.
        this->mutex.unlock();
    }

    return result;
}

bool Data_persistence::switch_track_positions_in_tag_list(const QString &tag_name,
                                                          const QSharedPointer<Audio_track> &at1,
                                                          const QSharedPointer<Audio_track> &at2)
{
    // Init result.
    bool result = true;

    // First ensure that both tracks are tagged with the tag.
    QStringList tags_1;
    this->get_tags_from_track(at1, tags_1);
    QStringList tags_2;
    this->get_tags_from_track(at2, tags_2);
    if ((tags_1.contains(tag_name) == false)
        || (tags_2.contains(tag_name) == false))
    {
        qCWarning(DS_DB) << "Track not tagged with " << tag_name;
        result = false;
    }
    else
    {
        // Get positions of tracks.
        int pos1 = this->get_track_pos_in_tag_list(at1, tag_name);
        int pos2 = this->get_track_pos_in_tag_list(at2, tag_name);

        // Update position of track 1 with the position of track 2 and vice versa.
        if ((this->set_track_position_in_tag_list(tag_name, at1, pos2) == false) ||
            (this->set_track_position_in_tag_list(tag_name, at2, pos1) == false))
        {
            qCWarning(DS_DB) << "Can not change position of tracks";
            result = false;
        }

    }

    return result;
}
