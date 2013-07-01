#include <QObject>
#include <QtTest>
#include <iostream>
using namespace std;

class DigitalScratchApi_Test : public QObject
{
    Q_OBJECT

public:
    DigitalScratchApi_Test();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testCase_dscratch_create_turntable_1();
};
