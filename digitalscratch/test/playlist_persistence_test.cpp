#include <QtTest>
#include "playlist_persistence_test.h"
#include "playlist_persistence.h"
#include <iostream>

#define DATA_DIR          "./test/data/"
#define DATA_PLAYLIST_M3U "playlist.m3u"

Playlist_persistence_Test::Playlist_persistence_Test()
{

}

void Playlist_persistence_Test::initTestCase()
{
    this->playlist         = new Playlist("playlist");
    this->playlist_persist = new Playlist_persistence();
}

void Playlist_persistence_Test::cleanupTestCase()
{
    delete this->playlist;
    delete this->playlist_persist;
}

void Playlist_persistence_Test::testCaseReadM3u()
{
    // Read playlist.
    QString playlist_fullfilename = QDir(DATA_DIR).filePath(DATA_PLAYLIST_M3U);
    QVERIFY2(this->playlist_persist->read_m3u(playlist_fullfilename, this->playlist) == true, "Read M3U");

    // Check tracklist.
    QList<QString> tracklist = this->playlist->get_tracklist();
    QVERIFY2(tracklist.count() == 3, "number of track");
    QVERIFY2(tracklist[0]      == QDir(DATA_DIR).absoluteFilePath("track_1.mp3"),        "name of track 1");
    QVERIFY2(tracklist[1]      == QDir(DATA_DIR).absoluteFilePath("track_2.mp3"),        "name of track 2");
    QVERIFY2(tracklist[2]      == QDir(DATA_DIR).absoluteFilePath("track_éèà@ù&_3.mp3"), "name of track 3");
}
