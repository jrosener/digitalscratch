/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*-----------------------------------------------( playlist_persistence.cpp )-*/
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
/*         File persistence methods for a Playlist (m3u, pls,...)             */
/*                                                                            */
/*============================================================================*/


#include <iostream>
#include <QtDebug>
#include "playlist_persistence.h"

Playlist_persistence::Playlist_persistence()
{
    qDebug() << "Playlist_persistence::Playlist_persistence: create object...";

    qDebug() << "Playlist_persistence::Playlist_persistence: create object done";

    return;
}


Playlist_persistence::~Playlist_persistence()
{
    qDebug() << "Playlist_persistence::~Playlist_persistence: delete object...";

    qDebug() << "Playlist_persistence::~Playlist_persistence: delete object done.";

    return;
}
