/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*-----------------------------------------------------------( playlist.cpp )-*/
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
/*    Class defining a playlist (basically just a list of audio file path)    */
/*                                                                            */
/*============================================================================*/


#include <iostream>
#include <QtDebug>

#include "tracks/playlist.h"

Playlist::Playlist(const QString &basepath, const QString &name)
{
    this->basepath = basepath;
    this->name     = name;
    this->tracklist.clear();

    return;
}


Playlist::~Playlist()
{
    return;
}

QString Playlist::get_basepath() const
{
    return this->basepath;
}

QString Playlist::get_name() const
{
    return this->name;
}

QStringList Playlist::get_tracklist() const
{
    return this->tracklist;
}

void Playlist::add_track(const QString &filename)
{
    this->tracklist.append(filename);
    this->tracklist.removeDuplicates();
}
