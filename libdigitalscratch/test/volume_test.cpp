#include <QtTest>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>

using namespace std;

#include "test_utils.h"
#include <volume.h>
#include <volume_test.h>

Volume_Test::Volume_Test()
{
}

void Volume_Test::initTestCase()
{
}

void Volume_Test::cleanupTestCase()
{
}

/**
 * Test Volume::Volume().
 *
 * Test Description:
 *      - Create a Volume object using an harcoded turntable name.
 *      - Check if turntable name was correctly stored by Volume object.
 *      - Check if value is correctly initialized.
 */
void Volume_Test::testCase_constructor()
{
    Volume *vol = new Volume("turntable_name");

    QVERIFY2(vol->get_turntable_name() == "turntable_name", "name");
    QVERIFY2(qFuzzyCompare(vol->get_value(), 0.0f) == true, "value");

    // Cleanup
    delete vol;
}

/**
* Test Volume::set_value().
*
* Test Description:
*      - Create a Volume object using an harcoded turntable name.
*      - Check if set_value(-1.0) return false and if the value is not set.
*      - Check if set_value(2.4) return true and if the value is set.
*/
void Volume_Test::testCase_set_value()
{
   Volume *vol = new Volume("turntable_name");

   QVERIFY2(vol->get_turntable_name() == "turntable_name",   "get default name");
   QVERIFY2(vol->set_value(-1.0) == false,                   "set negative value");
   QVERIFY2(qFuzzyCompare(vol->get_value(), -1.0f) == false, "get default value");
   QVERIFY2(vol->set_value(2.4) == true,                     "set correct value");
   QVERIFY2(qFuzzyCompare(vol->get_value(), 2.4f) == true,   "check correct value");

   // Cleanup
   delete vol;
}

/**
* Test Volume::set_turntable_name().
*
* Test Description:
*      - Create a Volume object using an harcoded turntable name.
*      - Check if set_turntable_name("") return false and if the name is not
*        set.
*      - Check if set_turntable_name("left_turntable") return true and if the
*        name is set.
*/
void Volume_Test::testCase_set_turntable_name()
{
   Volume *vol = new Volume("turntable_name");

   QVERIFY2(vol->get_turntable_name() == "turntable_name",     "get default name");
   QVERIFY2(vol->set_turntable_name("") == false,              "set empty name");
   QVERIFY2(vol->get_turntable_name() != "",                   "get default name again");
   QVERIFY2(vol->set_turntable_name("left_turntable") == true, "set correct name");
   QVERIFY2(vol->get_turntable_name() == "left_turntable",     "get correct name");

   // Cleanup
   delete vol;
}
