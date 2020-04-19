/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*-----------------------------------------------------------( playlist.cpp )-*/
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
/*    Class defining a playlist (basically just a list of audio file path)    */
/*                                                                            */
/*============================================================================*/


#include <iostream>
#include <QtDebug>
#include <QDir>

#include "tracks/playlist.h"

Playlist::Playlist(const QString &basepath,
                   const QString &name,
                   const QString &extension)
{
    this->basepath = basepath;
    this->name = name;
    this->set_extension(extension);
    this->tracklist.clear();

    return;
}

Playlist::Playlist(const QString &path)
{
    QFileInfo file_info(path);

    this->basepath = file_info.absolutePath();
    this->name = file_info.baseName();
    this->set_extension(file_info.suffix());
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

void Playlist::set_basepath(const QString &basepath)
{
    this->basepath = basepath;
}

QString Playlist::get_name() const
{
    return this->name;
}

void Playlist::set_name(const QString &name)
{
    this->name = name;
}

QString Playlist::get_extension() const
{
    return this->extension;
}

void Playlist::set_extension(const QString &extension)
{
    if (extension.startsWith(".") == false)
    {
        this->extension = "." + extension;
    }
    else
    {
        this->extension = extension;
    }
}

QString Playlist::get_fullpath() const
{
    if ((this->basepath != "") && (this->get_name() != ""))
    {
        return this->basepath
                + QDir::separator()
                + this->get_name()
                + this->get_extension();
    }
    else
    {
        return "";
    }
}

QStringList Playlist::get_tracklist() const
{
    return this->tracklist;
}

void Playlist::add_track_no_duplicate(const QString &filename)
{
    this->tracklist.append(filename);
    this->tracklist.removeDuplicates();
}

void Playlist::add_track(const QString &filename)
{
    this->tracklist.append(filename);
}

void Playlist::set_tracks(const QStringList &filepaths)
{
    this->tracklist.clear();
    this->tracklist.append(filepaths);
}

void Playlist::add_track_from_deck(const QString &filename, const unsigned short int &deck_index)
{
    if (this->tracklist_deck_map.size() == 0)
    {
        // First element, add it.
        this->tracklist_deck_map.push_back(deck_index);
        this->tracklist.append(filename);
    }
    else
    {
        // Next element, replace the last one if coming from the same deck, otherwise add it.
        if (this->tracklist_deck_map.last() == deck_index)
        {
            this->tracklist[this->tracklist.size() - 1] = filename;
        }
        else
        {
            this->tracklist_deck_map.push_back(deck_index);
            this->tracklist.append(filename);
        }
    }
}

void Playlist::rem_track(const QString &filename)
{
    this->tracklist.removeOne(filename);
}

void Playlist::clear()
{
    this->tracklist_deck_map.clear();
    this->tracklist.clear();
}
