/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                     Digital Scratch Player Test                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
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
/*============================================================================*/

#include <QtTest>
#include <iostream>
#include "playlist_persistence_test.h"
#include "tracks/playlist_persistence.h"

#define DATA_DIR          "./test/data/"
#ifdef WIN32
    #define URI_PREFIX    "file:///"
#else
    #define URI_PREFIX    "file://"
#endif

#define PLAYLIST_M3U_FILE "playlist.m3u"
#define PLAYLIST_M3U      "#EXTM3U\n"\
                          "#EXTINF:2316,\n"\
                          "# full path\n"\
                          "<DATA_DIR>/track_1.mp3\n"\
                          "#EXTINF:2316, -\n"\
                          "# relative path\n"\
                          "track_2.mp3\n"\
                          "#EXTINF:2316, -\n"\
                          "# duplicate path\n"\
                          "track_2.mp3\n"\
                          "#EXTINF:2316, -\n"\
                          "# file does not exists\n"\
                          "<URI_PREFIX><DATA_DIR>/track_does_not_exists.mp3\n"\
                          "#EXTINF:2316, -\n"\
                          "# uri style path\n"\
                          "<URI_PREFIX><DATA_DIR>/track_%C3%A9%C3%A8%C3%A0%40%C3%B9%26_3.mp3\n"\
                          "#EXTINF:2316, -\n"\
                          "# special chars\n"\
                          "track_éèà@ù&_4.mp3"

#define PLAYLIST_PLS_FILE "playlist.pls"
#define PLAYLIST_PLS      "[playlist]\n"\
                          "\n"\
                          "NumberOfEntries=6\n"\
                          "File1=<DATA_DIR>/track_1.mp3\n"\
                          "Title1=track 1 title\n"\
                          "Length1=28\n"\
                          "\n"\
                          "File2=track_2.mp3\n"\
                          "\n"\
                          "File3=track_2.mp3\n"\
                          "\n"\
                          "File4=<URI_PREFIX><DATA_DIR>/track_does_not_exists.mp3\n"\
                          "\n"\
                          "File5=<URI_PREFIX><DATA_DIR>/track_%C3%A9%C3%A8%C3%A0%40%C3%B9%26_3.mp3\n"\
                          "\n"\
                          "File6=track_éèà@ù&_4.mp3"

Playlist_persistence_Test::Playlist_persistence_Test()
{

}

void Playlist_persistence_Test::initTestCase()
{
    // Create an m3u test playlist file.
    QString m3u(PLAYLIST_M3U);
    m3u.replace("<DATA_DIR>",   QDir(DATA_DIR).absolutePath());
    m3u.replace("<URI_PREFIX>", URI_PREFIX);
    QFile m3u_file(QDir(DATA_DIR).filePath(PLAYLIST_M3U_FILE));
    if (m3u_file.open(QIODevice::WriteOnly | QIODevice::Text) == true)
    {
         QTextStream m3u_stream(&m3u_file);
         m3u_stream << m3u.toUtf8();
         m3u_file.close();
    }

    // Create a pls test file.
    QString pls(PLAYLIST_PLS);
    pls.replace("<DATA_DIR>",   QDir(DATA_DIR).absolutePath());
    pls.replace("<URI_PREFIX>", URI_PREFIX);
    QFile pls_file(QDir(DATA_DIR).filePath(PLAYLIST_PLS_FILE));
    if (pls_file.open(QIODevice::WriteOnly | QIODevice::Text) == true)
    {
         QTextStream pls_stream(&pls_file);
         pls_stream << pls.toUtf8();
         pls_file.close();
    }
}

void Playlist_persistence_Test::cleanupTestCase()
{
}

void Playlist_persistence_Test::testCaseReadM3u()
{
    // Init playlist and persistence process.
    Playlist playlist("base_path", "playlist", ".m3u");
    Playlist_persistence playlist_persist;

    // Read playlist.
    QString playlist_fullfilename = QDir(DATA_DIR).filePath(PLAYLIST_M3U_FILE);
    QVERIFY2(playlist_persist.read_m3u(playlist_fullfilename, playlist) == true, "Read M3U");

    // Check tracklist.
    QStringList tracklist = playlist.get_tracklist();
    QVERIFY2(tracklist.count() == 4, "number of tracks");
    QVERIFY2(tracklist[0]      == QDir(DATA_DIR).absoluteFilePath("track_1.mp3"),        "name of track 1");
    QVERIFY2(tracklist[1]      == QDir(DATA_DIR).absoluteFilePath("track_2.mp3"),        "name of track 2");
    QVERIFY2(tracklist[2]      == QDir(DATA_DIR).absoluteFilePath("track_éèà@ù&_3.mp3"), "name of track 3");
    QVERIFY2(tracklist[3]      == QDir(DATA_DIR).absoluteFilePath("track_éèà@ù&_4.mp3"), "name of track 4");
}

void Playlist_persistence_Test::testCaseReadPls()
{
    // Init playlist and persistence process.
    Playlist playlist("base_path", "playlist", ".m3u");
    Playlist_persistence playlist_persist;

    // Read playlist.
    QString playlist_fullfilename = QDir(DATA_DIR).filePath(PLAYLIST_PLS_FILE);
    QVERIFY2(playlist_persist.read_pls(playlist_fullfilename, playlist) == true, "Read PLS");

    // Check tracklist.
    QStringList tracklist = playlist.get_tracklist();
    QVERIFY2(tracklist.count() == 4, "number of tracks");
    QVERIFY2(tracklist[0]      == QDir(DATA_DIR).absoluteFilePath("track_1.mp3"),        "name of track 1");
    QVERIFY2(tracklist[1]      == QDir(DATA_DIR).absoluteFilePath("track_2.mp3"),        "name of track 2");
    QVERIFY2(tracklist[2]      == QDir(DATA_DIR).absoluteFilePath("track_éèà@ù&_3.mp3"), "name of track 3");
    QVERIFY2(tracklist[3]      == QDir(DATA_DIR).absoluteFilePath("track_éèà@ù&_4.mp3"), "name of track 4");
}
