/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*-------------------------------------------------( playlist_persistence.h )-*/
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
/*         File persistence methods for a Playlist (m3u, pls,...)             */
/*                                                                            */
/*============================================================================*/

#pragma once

#include <QSharedPointer>
#include "tracks/playlist.h"

using namespace std;

class Playlist_persistence
{
  public:
    Playlist_persistence();
    virtual ~Playlist_persistence();

  public:
    bool read(QSharedPointer<Playlist> &io_playlist);
    bool read_m3u(QSharedPointer<Playlist> &io_playlist);
    bool write(QSharedPointer<Playlist> &playlist);
    bool read_pls(QSharedPointer<Playlist> &io_playlist);
};
