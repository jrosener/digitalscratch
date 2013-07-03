#include <QObject>
#include <QtTest>
#include <iostream>
using namespace std;

class Position_Test : public QObject
{
    Q_OBJECT

public:
    Position_Test();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testCase_constructor();
    void testCase_set_value();
    void testCase_set_turntable_name();
};
