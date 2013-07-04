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

///**
// * Test Volume::set_value().
// *
// * Test Description:
// *      - Create a Volume object using an harcoded turntable name.
// *      - Check if set_value(-1.0) return false and if the value is not set.
// *      - Check if set_value(2.4) return true and if the value is set.
// */
//BOOST_AUTO_TEST_CASE (test_volume_set_value)
//{
//    Volume *vol = new Volume("turntable_name");

//    BOOST_CHECK_EQUAL(vol->get_turntable_name(), "turntable_name");
//    BOOST_CHECK_EQUAL(vol->set_value(-1.0), false);
//    BOOST_CHECK(vol->get_value() != -1.0);
//    BOOST_CHECK_EQUAL(vol->set_value(2.4), true);
//    BOOST_CHECK_CLOSE(vol->get_value(), (float)2.4, 0.1);

//    // Cleanup
//    delete vol;
//}

///**
// * Test Volume::set_turntable_name().
// *
// * Test Description:
// *      - Create a Volume object using an harcoded turntable name.
// *      - Check if set_turntable_name("") return false and if the name is not
// *        set.
// *      - Check if set_turntable_name("left_turntable") return true and if the
// *        name is set.
// */
//BOOST_AUTO_TEST_CASE (test_volume_set_turntable_name)
//{
//    Volume *vol = new Volume("turntable_name");

//    BOOST_CHECK_EQUAL(vol->get_turntable_name(), "turntable_name");
//    BOOST_CHECK_EQUAL(vol->set_turntable_name(""), false);
//    BOOST_CHECK(vol->get_turntable_name() != "");
//    BOOST_CHECK_EQUAL(vol->set_turntable_name("left_turntable"), true);
//    BOOST_CHECK_EQUAL(vol->get_turntable_name(), "left_turntable");

//    // Cleanup
//    delete vol;
//}
