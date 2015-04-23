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
 * Test: 
 *   Volume()
 */
void Volume_Test::testCase_constructor()
{
    Volume *vol = new Volume();

    QVERIFY2(qFuzzyCompare(vol->get_value(), 0.0f) == true, "value");

    // Cleanup
    delete vol;
}

/**
 * Test: 
 *   set_value()
 *   get_value()
 */
void Volume_Test::testCase_set_value()
{
   Volume *vol = new Volume();

   QVERIFY2(vol->set_value(-1.0f) == false,                  "set negative value");
   QVERIFY2(qFuzzyCompare(vol->get_value(), -1.0f) == false, "get default value");
   QVERIFY2(vol->set_value(2.4f) == true,                    "set correct value");
   QVERIFY2(qFuzzyCompare(vol->get_value(), 2.4f) == true,   "check correct value");

   // Cleanup
   delete vol;
}
