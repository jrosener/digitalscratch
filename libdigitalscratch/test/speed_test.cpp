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
 * Test Speed:Speed().
 *
 * Test Description:
 *      - Create a Speed object using an harcoded turntable name.
 *      - Check if turntable name was correctly stored.
 *      - Check if value is correctly initialized.
 */
void Speed_Test::testCase_Constructor()
{
    Speed *speed = new Speed("turntable_name");

    QVERIFY2(speed->get_turntable_name() == "turntable_name", "get turntable name");
    QVERIFY2(qFuzzyCompare(speed->get_value(), 0.0f)   == true, "get speed");
    QVERIFY2(speed->get_max_nb_no_new_speed_found()    == DEFAULT_MAX_NB_NO_NEW_SPEED_FOUND, "get max nb no new speed found");
    QVERIFY2(speed->get_max_nb_cycle_before_starting() == DEFAULT_MAX_NB_CYCLE_BEFORE_STARTING, "get max nb cycle before starting");

    // Cleanup
    delete speed;
}

/**
 * Test Speed::bool set_max_nb_no_new_speed_found().
 *
 * Test Description:
 *      - Create a Speed object using an harcoded turntable name.
 *      - Check if set_max_nb_no_new_speed_found(-1) return false and if the
 *        value is not set.
 *      - Check if set_max_nb_no_new_speed_found(0) return false and if the
 *        value is not set.
 *      - Check if set_max_nb_no_new_speed_found(2) return true and if the
 *        value is not set.
 */
void Speed_Test::testCase_set_max_nb_no_new_speed_found()
{
    Speed *speed = new Speed("turntable_name");

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
* Test Speed::set_max_nb_cycle_before_starting().
*
* Test Description:
*      - Create a Speed object using an harcoded turntable name.
*      - Check if set_max_nb_cycle_before_starting(-1) return false and if the
*        value is not set.
*      - Check if set_max_nb_cycle_before_starting(0) return false and if the
*        value is not set.
*      - Check if set_max_nb_cycle_before_starting(2) return true and if the
*        value is not set.
*/
void Speed_Test::testCase_set_max_nb_cycle_before_starting()
{
   Speed *speed = new Speed("turntable_name");

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
* Test Speed::set_value().
*
* Test Description:
*      - Create a Speed object using an harcoded turntable name.
*      - Prepare max_nb_no_new_speed_found = 3.
*      - Check if set_value(2.5) return true and if the value is set.
*      - Check for 3 times that if we set_value(NO_NEW_SPEED_FOUND) then
*        NO_NEW_SPEED_FOUND is set.
*      - Check that if we set_value(NO_NEW_SPEED_FOUND) again, then speed = 0.
*      - Same check as before, but only 2 times and then a correct value, so
*        speed should be correctly set.
*      - Same check as previous one to check if internal counters are
*        correctly initialized.
*/
void Speed_Test::testCase_set_value()
{
   Speed *speed = new Speed("turntable_name");
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
   QVERIFY2(speed->set_value(1.08) == true,                   "set correct speed 2");
   QVERIFY2(qFuzzyCompare(speed->get_value(), 1.08f) == true, "get correct speed 2");

   for (int i = 0; i < speed->get_max_nb_no_new_speed_found()-1; i++)
   {
       QVERIFY2(speed->set_value(NO_NEW_SPEED_FOUND) == true, "set no new speed");
       QVERIFY2(speed->get_value() == NO_NEW_SPEED_FOUND,     "get no new speed");
   }
   QVERIFY2(speed->set_value(5.04) == true,           "set correct speed 3");
   QVERIFY2(qFuzzyCompare(speed->get_value(), 5.04f), "get correct speed 3");

   // Cleanup
   delete speed;
}
