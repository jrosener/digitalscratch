#include <QObject>
#include <QtTest>
#include <iostream>
using namespace std;

class Controller_Test : public QObject
{
    Q_OBJECT

public:
    Controller_Test();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testCaseConstructor();
};
