/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*-------------------------------------------------------------( playlist.h )-*/
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
/*    Class defining a playlist (basically just a list of audio file path)    */
/*                                                                            */
/*============================================================================*/

#pragma once

#include <QObject>
#include <QString>
#include <QStringList>

#include "app/application_const.h"

using namespace std;

class Playlist
{
 private:
    QString     basepath;
    QString     name;
    QString     extension;
    QStringList tracklist;
    QList<unsigned short int>  tracklist_deck_map;

 public:
    Playlist(const QString &basepath, const QString &name, const QString &extension);
    virtual ~Playlist();

 public:
    QString     get_basepath() const;
    void        set_basepath(const QString &basepath);
    QString     get_name() const;
    void        set_name(const QString &name);
    QString     get_extension() const;
    void        set_extension(const QString &extension);
    QString     get_fullpath() const;
    QStringList get_tracklist() const;
    void        add_track_no_duplicate(const QString &filename);
    void        add_track_from_deck(const QString &filename, const unsigned short int &deck_index);
    void        clear();
};
