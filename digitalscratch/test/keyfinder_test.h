#include <QObject>
#include <QtTest>
#include <application_const.h>
#include <keyfinder_api.h>

class Keyfinder_Test : public QObject
{
    Q_OBJECT

public:
    Keyfinder_Test();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testCaseGetKey();
};
