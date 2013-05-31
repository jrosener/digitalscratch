#include <QObject>
#include <QtTest>
#include <utils.h>
#include <application_const.h>

class Utils_Test : public QObject
{
    Q_OBJECT

public:
    Utils_Test();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testCaseGetFileHash();
    void testCaseGetFileHashCharge();
    void testCaseGetFileMusicKey();
    void testCaseGetNextMusicKeys();
};
