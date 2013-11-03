/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*-----------------------------------------------------( data_persistence.h )-*/
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

#ifndef DATA_PERSISTENCE_H
#define DATA_PERSISTENCE_H

#include <iostream>
#include <QObject>
#include <QSqlDatabase>
#include <audio_track.h>
#include <QMutex>

using namespace std;

class Data_persistence : public QObject
{
    Q_OBJECT

 public:
    Data_persistence();
    virtual ~Data_persistence();

 public:
    bool is_initialized;

 private:
    QSqlDatabase db;
    QMutex       mutex;

 public:
    bool begin_transaction();
    bool commit_transaction();
    bool rollback_transaction();

    bool store_audio_track(Audio_track *in_at); // Insert (or update if exists) an audio track in DB.
    bool get_audio_track(Audio_track *io_at);   // Get and fill the audio track specified by io_at->get_hash().

    bool store_cue_point(Audio_track *in_at, unsigned int in_number, unsigned int in_position);

 private:
    bool init_db();
    bool create_db_structure();
};

#endif // DATA_PERSISTENCE_H
