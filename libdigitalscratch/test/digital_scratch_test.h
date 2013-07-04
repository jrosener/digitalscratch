#include <QObject>
#include <QtTest>
#include <iostream>
using namespace std;

class DigitalScratch_Test : public QObject
{
    Q_OBJECT

public:
    DigitalScratch_Test();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testCase_constructor();
};
