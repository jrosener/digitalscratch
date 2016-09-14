#include <QString>
#include <QtTest>
#include "audio_device_access_rules_test.h"
#include "audiodev/sound_card_control_rules.h"

Audio_device_access_rules_Test::Audio_device_access_rules_Test()
{
}

void Audio_device_access_rules_Test::initTestCase()
{
}

void Audio_device_access_rules_Test::cleanupTestCase()
{
}

void Audio_device_access_rules_Test::testCase_get_device_list()
{
    // Get sound card list.
    QList<QString> cards = Sound_card_control_rules::get_device_list();
    
    // There must be at least one device.
    QVERIFY2(cards.size() >= 1, "list sound card in the system");
}

