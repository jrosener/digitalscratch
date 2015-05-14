#include <QtTest>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>

using namespace std;

#include "test_utils.h"
#include <speed.h>
#include <speed_test.h>

Speed_Test::Speed_Test()
{
}

void Speed_Test::initTestCase()
{
}

void Speed_Test::cleanupTestCase()
{
}

/**
 * Test: 
 *   Speed()
 */
void Speed_Test::testCase_Constructor()
{
    Speed *speed = new Speed();

    QVERIFY2(qFuzzyCompare(speed->get_value(), 0.0f) == true, "get speed");

    // Cleanup
    delete speed;
}

/**
 * Test: 
 *   set_value()
 *   get_value()
 */
void Speed_Test::testCase_set_value()
{
   Speed *speed = new Speed();

   QVERIFY2(speed->set_value(2.5) == true,                   "set correct speed");
   QVERIFY2(qFuzzyCompare(speed->get_value(), 2.5f) == true, "get correct speed");

   // Cleanup
   delete speed;
}
