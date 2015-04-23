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

    QVERIFY2(qFuzzyCompare(speed->get_value(), 0.0f)   == true, "get speed");
    QVERIFY2(speed->get_max_nb_no_new_speed_found()    == DEFAULT_MAX_NB_NO_NEW_SPEED_FOUND, "get max nb no new speed found");
    QVERIFY2(speed->get_max_nb_cycle_before_starting() == DEFAULT_MAX_NB_CYCLE_BEFORE_STARTING, "get max nb cycle before starting");

    // Cleanup
    delete speed;
}

/**
 * Test: 
 *   set_max_nb_no_new_speed_found()
 *   get_max_nb_no_new_speed_found()
 */
void Speed_Test::testCase_set_max_nb_no_new_speed_found()
{
    Speed *speed = new Speed();

    QVERIFY2(speed->set_max_nb_no_new_speed_found(-1) == false, "negative value");
    QVERIFY2(speed->get_max_nb_no_new_speed_found()   == DEFAULT_MAX_NB_NO_NEW_SPEED_FOUND, "get default value");
    QVERIFY2(speed->set_max_nb_no_new_speed_found(0)  == false, "zero value");
    QVERIFY2(speed->get_max_nb_no_new_speed_found()   == DEFAULT_MAX_NB_NO_NEW_SPEED_FOUND, "get default value again");
    QVERIFY2(speed->set_max_nb_no_new_speed_found(2)  == true, "correct value");
    QVERIFY2(speed->get_max_nb_no_new_speed_found()   == 2, "get correct value");

    // Cleanup
    delete speed;
}

/**
 * Test: 
 *   set_max_nb_cycle_before_starting()
 *   get_max_nb_cycle_before_starting()
 */
void Speed_Test::testCase_set_max_nb_cycle_before_starting()
{
   Speed *speed = new Speed();

   QVERIFY2(speed->set_max_nb_cycle_before_starting(-1) == false, "negative value");
   QVERIFY2(speed->get_max_nb_cycle_before_starting()   == DEFAULT_MAX_NB_CYCLE_BEFORE_STARTING, "get default value");
   QVERIFY2(speed->set_max_nb_cycle_before_starting(0)  == false, "zero value");
   QVERIFY2(speed->get_max_nb_cycle_before_starting()   == DEFAULT_MAX_NB_CYCLE_BEFORE_STARTING, "get default value again");
   QVERIFY2(speed->set_max_nb_cycle_before_starting(2)  == true, "correct value");
   QVERIFY2(speed->get_max_nb_cycle_before_starting()   == 2, "get correct value");

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
   QVERIFY2(speed->set_max_nb_no_new_speed_found(3) == true, "set max nb no new speed");

   QVERIFY2(speed->set_value(2.5) == true,                   "set correct speed");
   QVERIFY2(qFuzzyCompare(speed->get_value(), 2.5f) == true, "get correct speed");

   for (int i = 0; i < speed->get_max_nb_no_new_speed_found(); i++)
   {
       QVERIFY2(speed->set_value(NO_NEW_SPEED_FOUND) == true, "set no new speed");
       QVERIFY2(speed->get_value() == NO_NEW_SPEED_FOUND,     "get no new speed");
   }
   QVERIFY2(speed->set_value(NO_NEW_SPEED_FOUND) == true,    "set no new speed #4");
   QVERIFY2(qFuzzyCompare(speed->get_value(), 0.0f) == true, "speed = 0.0");

   for (int i = 0; i < speed->get_max_nb_no_new_speed_found()-1; i++)
   {
       QVERIFY2(speed->set_value(NO_NEW_SPEED_FOUND) == true, "set no new speed");
       QVERIFY2(speed->get_value() == NO_NEW_SPEED_FOUND,     "get no new speed");
   }
   QVERIFY2(speed->set_value(1.08f) == true,                  "set correct speed 2");
   QVERIFY2(qFuzzyCompare(speed->get_value(), 1.08f) == true, "get correct speed 2");

   for (int i = 0; i < speed->get_max_nb_no_new_speed_found()-1; i++)
   {
       QVERIFY2(speed->set_value(NO_NEW_SPEED_FOUND) == true, "set no new speed");
       QVERIFY2(speed->get_value() == NO_NEW_SPEED_FOUND,     "get no new speed");
   }
   QVERIFY2(speed->set_value(5.04f) == true,          "set correct speed 3");
   QVERIFY2(qFuzzyCompare(speed->get_value(), 5.04f), "get correct speed 3");

   // Cleanup
   delete speed;
}
