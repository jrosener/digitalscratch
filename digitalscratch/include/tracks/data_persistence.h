/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*-----------------------------------------------------( data_persistence.h )-*/
/*                                                                            */
/*  Copyright (C) 2003-2020                                                   */
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

#pragma once

#include <iostream>
#include <QObject>
#include <QSqlDatabase>
#include <QMutex>
#include <QSharedPointer>

#include "tracks/audio_track.h"

using namespace std;

class Data_persistence
{
 public:
    Data_persistence();
    virtual ~Data_persistence();

 public:
    bool is_initialized;

 private:
    QString main_db_name;
    QString db_path;
    QMutex  mutex;

 public:
    bool restore_db(const QString &file_path);
    bool export_db(const QString &dest_file_path);
    bool rollback_transaction();

    bool store_audio_track(const QSharedPointer<Audio_track> &at);        // Insert (or update if exists) an audio track in DB.
    bool get_audio_track(QSharedPointer<Audio_track> &io_at);             // Get and fill the audio track specified by io_at->get_hash().

    bool store_cue_point(const QSharedPointer<Audio_track>   &at,         // Insert (or update) a cue point in DB.
                         const unsigned int                  &number,
                         const unsigned int                  &position_msec);
    bool get_cue_point(const QSharedPointer<Audio_track>     &at,         // Get the in_number cue point of an audio track.
                       const unsigned int                    &number,
                       unsigned int                          &out_position_msec);
    bool delete_cue_point(const QSharedPointer<Audio_track>  &at,         // Delete the in_number cue point of an audio track.
                          const unsigned int                 &number);

    bool store_tag(const QString &name);                                   // Insert a new tag.
    bool rename_tag(const QString &old_name,                               // Rename a tag.
                    const QString &new_name);
    bool delete_tag(const QString &name);                                  // Delete a tag (and remove it from all tracks).
    bool get_full_tag_list(QStringList &out_tags);                         // Get a list of existing tags.

    bool add_tag_to_track(const QSharedPointer<Audio_track> &at,           // Associate a tag to a track.
                          const QString                     &tag_name);
    bool rem_tag_from_track(const QSharedPointer<Audio_track> &at,         // Delete association between a track and a tag.
                            const QString                     &tag_name);
    bool get_tags_from_track(const QSharedPointer<Audio_track> &at,        // Get the list of tags for the specified track.
                             QStringList                       &out_tags);
    bool get_tracks_from_tag(const QString                     &tag_name,  // Get the list of tracks for the specified tag.
                             QStringList                       &out_tracklist);
    bool switch_track_positions_in_tag_list(const QString &tag_name,                // In the tracklist of a specified tag,
                                            const QSharedPointer<Audio_track> &at1, // Switch position of 2 tracks.
                                            const QSharedPointer<Audio_track> &at2);

 private:
    bool init_db();
    void get_db_connection(QSqlDatabase &db);
    bool create_db_structure();
    bool store_track_tag(const QString &id_track,
                         const QString &id_tag);
    bool reorganize_track_pos_in_tag_list();                               // If track/tag association has no position in the track list
                                                                           // of the tag, then put position to the end of the list.
                                                                           // Reorganize also position values to have a contiguous list.
    int get_track_pos_in_tag_list(const QSharedPointer<Audio_track> &at1,
                                  const QString &tag_name);
    bool set_track_position_in_tag_list(const QString &tag_name,
                                        const QSharedPointer<Audio_track> &at,
                                        const int &position);
#ifndef ENABLE_TEST_MODE
    void backup_db();
#endif
};
