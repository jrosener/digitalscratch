#include <QObject>
#include <QtTest>

class Audio_track_Test : public QObject
{
    Q_OBJECT

public:
    Audio_track_Test();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testCaseCreate();
    void testCaseFillSamples();
};
