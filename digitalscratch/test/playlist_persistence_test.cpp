#include <QtTest>
#include "playlist_persistence_test.h"
#include "playlist_persistence.h"
#include <iostream>

Playlist_persistence_Test::Playlist_persistence_Test()
{

}

void Playlist_persistence_Test::initTestCase()
{
    this->playlist         = new Playlist("electro");
    this->playlist_persist = new Playlist_persistence();
}

void Playlist_persistence_Test::cleanupTestCase()
{
    delete this->playlist;
    delete this->playlist_persist;
}

void Playlist_persistence_Test::testCaseReadM3u()
{
    QVERIFY2(this->playlist_persist->read_m3u(QString("electro.m3u"), this->playlist) == true, "Read M3U");

    QList<QString> tracklist = this->playlist->get_tracklist();
    QVERIFY2(tracklist[0] == "track1", "name of track 1");
    QVERIFY2(tracklist[1] == "track2", "name of track 2");
}
