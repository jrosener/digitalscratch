/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*-----------------------------------------------------------( playlist.cpp )-*/
/*                                                                            */
/*  Copyright (C) 2003-2014                                                   */
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
#include "playlist.h"

Playlist::Playlist(const QString &in_basepath, const QString &in_name)
{
    this->basepath = in_basepath;
    this->name     = in_name;
    this->tracklist.clear();

    return;
}


Playlist::~Playlist()
{
    return;
}

QString Playlist::get_basepath()
{
    return this->basepath;
}

QString Playlist::get_name()
{
    return this->name;
}

QStringList Playlist::get_tracklist()
{
    return this->tracklist;
}

void Playlist::add_track(const QString &in_filename)
{
    this->tracklist.append(in_filename);
    this->tracklist.removeDuplicates();
}
