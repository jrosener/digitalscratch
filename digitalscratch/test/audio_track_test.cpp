#include <QString>
#include <QtTest>
#include <audio_track.h>

class Audio_track_Test : public QObject
{
    Q_OBJECT
    
public:
    Audio_track_Test();
    
private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testCaseCreate();
};

Audio_track_Test::Audio_track_Test()
{
}

void Audio_track_Test::initTestCase()
{
}

void Audio_track_Test::cleanupTestCase()
{
}

void Audio_track_Test::testCaseCreate()
{
    //QVERIFY2(true, "Failure");
    Audio_track *at = NULL;
    at = new Audio_track(15);
    delete at;
}

QTEST_APPLESS_MAIN(Audio_track_Test)

#include "audio_track_test.moc"
