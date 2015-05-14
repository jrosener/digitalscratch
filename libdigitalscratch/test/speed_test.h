#include <QObject>
#include <QtTest>
#include <iostream>
using namespace std;

class Speed_Test : public QObject
{
    Q_OBJECT

public:
    Speed_Test();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testCase_Constructor();
    void testCase_set_value();
};
