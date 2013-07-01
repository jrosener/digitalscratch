#include <QString>
#include <QtTest>
#include <controller_test.h>
#include <controller.h>
#include <iostream>

Controller_Test::Controller_Test()
{
}

void Controller_Test::initTestCase()
{
}

void Controller_Test::cleanupTestCase()
{
}

/**
 * Test Controller::Controller().
 *
 * Test Description:
 *      - Create a Controller object using a valid name.
 *      - Check if controller name was correctly stored.
 *      - Check if speed, position and volume are correctly initialized.
 */
void Controller_Test::testCaseConstructor()
{
    Controller *ctrl = new Controller("controller_name");
    float speed    = 0.0;
    float volume   = 0.0;
    float position = 0.0;

    QVERIFY2(ctrl->get_name() == "controller_name", "name");
    QVERIFY2(ctrl->get_playing_parameters(&speed, &volume, &position) == false, "playing parameters");
    QVERIFY2(speed    == 0.0, "speed");
    QVERIFY2(volume   == 0.0, "volume");
    QVERIFY2(position == 0.0, "position");
    QVERIFY2(ctrl->get_max_nb_no_new_speed_found()    == 1, "max nb no new speed found");
    QVERIFY2(ctrl->get_max_nb_cycle_before_starting() == 1, "max nb cycle before starting");

    // Cleanup
    delete ctrl;
}

/**
 * Test Controller::set_name().
 *
 * Test Description:
 *      - Create a Controller object using an valid name.
 *      - Check if the name was correctly stored.
 *      - Put an empty name using set_name(""), check if it return false and
 *        if the name is not set.
 */
void Controller_Test::testControllerSetName()
{
    Controller *ctrl = new Controller("controller_name");

    // Default name.
    QVERIFY2(ctrl->get_name() == "controller_name", "get default name");

    // Wrong name.
    QVERIFY2(ctrl->set_name("") == false, "set wrong name");
    QVERIFY2(ctrl->get_name() == "controller_name", "get default name again");
    
    // New name.
    QVERIFY2(ctrl->set_name("test_name") == true, "set correct name");
    QVERIFY2(ctrl->get_name() == "test_name", "get new name");

    // Cleanup
    delete ctrl;
}
