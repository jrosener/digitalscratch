#include <QtTest>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>

using namespace std;

#include "test_utils.h"
#include <position.h>
#include <position_test.h>

Position_Test::Position_Test()
{
}

void Position_Test::initTestCase()
{
}

void Position_Test::cleanupTestCase()
{
}


/**
 * Test Position::Position().
 *
 * Test Description:
 *      - Create a Position object using an harcoded turntable name.
 *      - Check if turntable name was correctly stored by Position object.
 *      - Check if value is correctly initialized.
 */
void Position_Test::testCase_constructor()
{
    Position *pos = new Position("turntable_name");

    QVERIFY2(pos->get_turntable_name() == "turntable_name", "get name");
    QVERIFY2(qFuzzyCompare(pos->get_value(), 0.0f) == true, "get default position");

    // Cleanup
    delete pos;
}

/**
 * Test Position::set_value().
 *
 * Test Description:
 *      - Create a Position object using an harcoded turntable name.
 *      - Check if set_value(-1.0) return false and if the value is not set.
 *      - Check if set_value(NO_NEW_POSITION_FOUND) return true and if the
 *        value is set.
 *      - Check if set_value(2.4) return true and if the value is not set.
 *      - Check if set_value(3.3) return true and if the value is set.
 *      - Check if set_value(2.5) return true and if the value is not set.
 *      - Check if set_value(5.3) return false and if the value is not set.
 */
void Position_Test::testCase_set_value()
{
    Position *pos = new Position("turntable_name");

    QVERIFY2(pos->get_turntable_name() == "turntable_name", "get name");

    QVERIFY2(pos->set_value(-1.0) == false,                   "set negative value");
    QVERIFY2(qFuzzyCompare(pos->get_value(), -1.0f) == false, "negative value not set");

    QVERIFY2(pos->set_value(NO_NEW_POSITION_FOUND) == true, "set no new position");
    QVERIFY2(pos->get_value() == NO_NEW_POSITION_FOUND,     "get no new position");

    QVERIFY2(pos->set_value(2.4f) == true,                   "set correct value 1");
    QVERIFY2(qFuzzyCompare(pos->get_value(), 2.4f) == false, "check value 1 not stored");

    QVERIFY2(pos->set_value(3.3f) == true,          "set correct value 2");
    QVERIFY2(qFuzzyCompare(pos->get_value(), 3.3f), "check correct value 2");

    QVERIFY2(pos->set_value(2.5f) == true,                   "set correct value 3");
    QVERIFY2(qFuzzyCompare(pos->get_value(), 2.5f) == false, "check value 3 not stored");
    QVERIFY2(qFuzzyCompare(pos->get_value(), 3.3f) == true,  "check previous value");

    QVERIFY2(pos->set_value(5.3f) == false,                  "bad value");
    QVERIFY2(qFuzzyCompare(pos->get_value(), 5.3f) == false, "bad value not stored");
    QVERIFY2(qFuzzyCompare(pos->get_value(), 3.3f) == true,  "previous value still there");

    // Cleanup
    delete pos;
}

/**
 * Test Position::set_turntable_name().
 *
 * Test Description:
 *      - Create a Position object using an harcoded turntable name.
 *      - Check if set_turntable_name("") return false and if the name is not
 *        set.
 *      - Check if set_turntable_name("left_turntable") return true and if the
 *        name is set.
 */
void Position_Test::testCase_set_turntable_name()
{
    Position *pos = new Position("turntable_name");

    QVERIFY2(pos->get_turntable_name() == "turntable_name", "get name");
    
    QVERIFY2(pos->set_turntable_name("") == false, "set empty name");
    QVERIFY2(pos->get_turntable_name() != "",      "name not set");
    
    QVERIFY2(pos->set_turntable_name("left_turntable") == true, "set correct name");
    QVERIFY2(pos->get_turntable_name() == "left_turntable",     "check correct name");

    // Cleanup
    delete pos;
}
