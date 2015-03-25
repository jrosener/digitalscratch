#include <QObject>
#include <QtTest>
#include "app/application_const.h"

class Audio_device_access_rules_Test : public QObject
{
    Q_OBJECT

public:
    Audio_device_access_rules_Test();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testCase_get_device_list();
};
