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

    qDebug() << "Data_persistence::create_db_structure done.";

    return result;
}

void Data_persistence::close_db()
{
    qDebug() << "Data_persistence::close_db...";

    this->db.close();

    qDebug() << "Data_persistence::close_db done.";
}

