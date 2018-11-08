/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*-----------------------------------------------( playlist_persistence.cpp )-*/
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
/*         File persistence methods for a Playlist (m3u, pls,...)             */
/*                                                                            */
/*============================================================================*/


#include <iostream>
#include <QtDebug>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTextCodec>
#include <QUrl>
#include <QDesktopServices>

#include "tracks/playlist_persistence.h"
#include "app/application_logging.h"

Playlist_persistence::Playlist_persistence()
{
    return;
}


Playlist_persistence::~Playlist_persistence()
{
    return;
}

bool Playlist_persistence::read_m3u(const QString &file_name, Playlist &io_playlist)
{
    // Check parameters.
    if (QFile::exists(file_name) == false)
    {
        return false;
    }

    // Get path of playlist file.
    QFileInfo file_info(file_name);
    QString   path(file_info.absolutePath());

    // Populate list of audio track.
    QFile file(file_name);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text) == true)
    {
        // Set current path to the one from the playlist file. Needed for relative track file path.
        QString old_path = QDir::currentPath();
        QDir::setCurrent(path);

        // Iterate over lines in playlist file.
        QTextStream stream(&file);
        while (stream.atEnd() == false)
        {
            // Get line.
            QString line = stream.readLine();

            // Skip comment lines.
            if (line.startsWith("#") == false)
            {
                // Check if file exists.
                QFileInfo line_info(line);
                if (line_info.exists() == true)
                {
                    // Add track path to playlist object.
                    io_playlist.add_track_no_duplicate(line_info.absoluteFilePath());
                }
                else
                {
                    // File does not exist, maybe because path is a URI.
                    line = QUrl::fromUserInput(line).toLocalFile();
                    if (line.length() > 0)
                    {
                        // Try to check again if it exists.
                        QFileInfo uri_info(line);
                        if (uri_info.exists() == true)
                        {
                            // Add track path to playlist object.
                            io_playlist.add_track_no_duplicate(uri_info.absoluteFilePath());
                        }
                    }
                }
            }
        }

        // Put back the current path;
        QDir::setCurrent(old_path);
    }
    file.close();

    return true;
}

bool Playlist_persistence::write(QSharedPointer<Playlist> &playlist)
{
    // Open the playlist file.
    QFile file(playlist->get_fullpath());

    if (file.open(QIODevice::WriteOnly | QIODevice::Text) == false)
    {
        qCWarning(DS_FILE) << "can not open tracklist file " << playlist->get_fullpath();
        return false;
    }

    // Map it with a text stream.
    QTextStream stream(&file);

    // Write each line of the tracklist.
    foreach (QString str, playlist->get_tracklist())
    {
        stream << "file://" << str << endl;
    }

    // Done.
    file.close();

    qCDebug(DS_FILE) << "tracklist written into " << playlist->get_fullpath();
    return true;
}

bool Playlist_persistence::read_pls(const QString &file_name, Playlist &io_playlist)
{
    // Check parameters.
    if (QFile::exists(file_name) == false)
    {
        return false;
    }

    // Get path of playlist file.
    QFileInfo file_info(file_name);
    QString   path(file_info.absolutePath());

    // Populate list of audio track.
    QFile file(file_name);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text) == true)
    {
        // Set current path to the one from the playlist file. Needed for relative track file path.
        QString old_path = QDir::currentPath();
        QDir::setCurrent(path);

        // Iterate over lines in playlist file.
        QTextStream stream(&file);
        while (stream.atEnd() == false)
        {
            // Get line.
            QString line = stream.readLine();

            // Get "FileX=" lines.
            if (line.contains(QRegExp("^File\\w*=")) == true)
            {
                line = line.split("=")[1];

                // Check if file exists.
                QFileInfo line_info(line);
                if (line_info.exists() == true)
                {
                    // Add track path to playlist object.
                    io_playlist.add_track_no_duplicate(line_info.absoluteFilePath());
                }
                else
                {
                    // File does not exist, maybe because path is a URI.
                    line = QUrl::fromUserInput(line).toLocalFile();
                    if (line.length() > 0)
                    {
                        // Try to check again if it exists.
                        QFileInfo uri_info(line);
                        if (uri_info.exists() == true)
                        {
                            // Add track path to playlist object.
                            io_playlist.add_track_no_duplicate(uri_info.absoluteFilePath());
                        }
                    }
                }
            }
        }

        // Put back the current path;
        QDir::setCurrent(old_path);
    }
    file.close();

    return true;
}
