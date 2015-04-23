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
 * Test: 
 *   get_playing_parameters()
 *   get_max_nb_no_new_speed_found()
 *   get_max_nb_cycle_before_starting()
 */
void Controller_Test::testCaseConstructor()
{
    Controller *ctrl = new Controller();
    float speed  = 0.0;
    float volume = 0.0;

    QVERIFY2(ctrl->get_playing_parameters(&speed, &volume) == false, "playing parameters");
    QVERIFY2(speed    == 0.0, "speed");
    QVERIFY2(volume   == 0.0, "volume");
    QVERIFY2(ctrl->get_max_nb_no_new_speed_found()    == 1, "max nb no new speed found");
    QVERIFY2(ctrl->get_max_nb_cycle_before_starting() == 1, "max nb cycle before starting");

    // Cleanup
    delete ctrl;
}
