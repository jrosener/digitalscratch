/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------( playlist.h )-*/
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
/*    Class defining a playlist (basically just a list of audio file path)    */
/*                                                                            */
/*============================================================================*/

#ifndef PLAYLIST_H_
#define PLAYLIST_H_

#include <QObject>
#include <QString>
#include <application_const.h>
using namespace std;

class Playlist : public QObject
{
    Q_OBJECT

 private:
    QString        name;
    QList<QString> tracklist;

 public:
    Playlist(const QString &in_name);
    virtual ~Playlist();

 public:
    QString        get_name();
    QList<QString> get_tracklist();
    void           add_track(const QString &in_filename);
};

#endif /* PLAYLIST_H_ */
