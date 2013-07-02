#include <QtTest>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>

#include "test_utils.h"
#include <digital_scratch_api.h>
#include <digital_scratch_api_test.h>

DigitalScratchApi_Test::DigitalScratchApi_Test()
{
}

void DigitalScratchApi_Test::initTestCase()
{
}

void DigitalScratchApi_Test::cleanupTestCase()
{
}

/**
 * Test #1 of dscratch_create_turntable().
 *
 * Test Description:
 *      - Call dscratch_create_turntable() with an empty name, it must fail.
 *      - Call dscratch_create_turntable() with name=NULL, it must fail.
 *      - Call dscratch_create_turntable() with an empty vinyl, it must fail.
 *      - Call dscratch_create_turntable() with vinyl=NULL, it must fail.
 *      - Call dscratch_create_turntable() with correct params, it must pass.
 *      - Check id, it must have changed.
 *      - Call dscratch_create_turntable() again, and check that id is different
 *        than first one.
 *      - Delete both turntables.
 */
void DigitalScratchApi_Test::testCase_dscratch_create_turntable_1()
{
    int turntable_id_1 = -1;
    int turntable_id_2 = -1;

    // Try to create a turntable with wrong parameters.
    QVERIFY2(dscratch_create_turntable("",               FINAL_SCRATCH_VINYL, &turntable_id_1) != 0, "empty name");
    QVERIFY2(dscratch_create_turntable(NULL,             FINAL_SCRATCH_VINYL, &turntable_id_1) != 0, "null name");
    QVERIFY2(dscratch_create_turntable("left_turntable", "",                  &turntable_id_1) != 0, "empty vinyl");
    QVERIFY2(dscratch_create_turntable("left_turntable", NULL,                &turntable_id_1) != 0, "null vinyl");

    // Create a turntable with correct parameters.
    QVERIFY2(dscratch_create_turntable("left_turntable", FINAL_SCRATCH_VINYL, &turntable_id_1) == 0, "correct params 1");

    // Check turntable_id_1.
    QVERIFY2(turntable_id_1 != -1, "new turntable id 1");

    // Create again a turntable with correct parameters.
    QVERIFY2(dscratch_create_turntable("left_turntable", FINAL_SCRATCH_VINYL, &turntable_id_2) == 0, "correct params 2");

    // Check turntable_id_2.
    QVERIFY2(turntable_id_2 != -1, "new turntable id 2");

    // Check turntable_id_2 against turntable_id_1.
    QVERIFY2(turntable_id_2 != turntable_id_1, "different turntable ids");

    // Cleanup.
    QVERIFY2(dscratch_delete_turntable(turntable_id_1) == 0, "clean turntable 1");
    QVERIFY2(dscratch_delete_turntable(turntable_id_2) == 0, "clean turntable 2");
}

/**
 * Test #2 of dscratch_create_turntable().
 *
 * Test Description:
 *      - Note: All calls of dscratch_create_turntable() are with correct params, so
 *        it must pass.
 *      - Call dscratch_create_turntable() and check if id0=0. (0)
 *      - Delete turntable id0.                                (x)
 *      - Call dscratch_create_turntable() and check if id0=0. (0)
 *      - Call dscratch_create_turntable() and check if id1=1. (0,1)
 *      - Call dscratch_create_turntable() and check if id2=2. (0,1,2)
 *      - Delete turntable id0.                                (x,1,2)
 *      - Delete turntable id1.                                (x,x,2)
 *      - Call dscratch_create_turntable() and check if id0=0. (0,x,2)
 *      - Delete turntable id0.                                (x,x,2)
 *      - Delete turntable id1, should fail.                   (x,x,2)
 *      - Delete turntable id2.                                (x,x,x)
 */
void DigitalScratchApi_Test::testCase_dscratch_create_turntable_2()
{
    int id0 = -1;
    int id1 = -1;
    int id2 = -1;

    // Call dscratch_create_turntable() and check if id0=0. (0)
    QVERIFY2(dscratch_create_turntable("turntable0", FINAL_SCRATCH_VINYL, &id0) == 0, "create id 0");
    QVERIFY2(id0 == 0, "check id 0");

    // Delete turntable id0.                                (x)
    QVERIFY2(dscratch_delete_turntable(id0) == 0, "delete id 0");

    // Call dscratch_create_turntable() and check if id0=0. (0)
    QVERIFY2(dscratch_create_turntable("turntable0", FINAL_SCRATCH_VINYL, &id0) == 0, "create id 0 again");
    QVERIFY2(id0 == 0, "check id 0 again");

    // Call dscratch_create_turntable() and check if id1=1. (0,1)
    QVERIFY2(dscratch_create_turntable("turntable1", FINAL_SCRATCH_VINYL, &id1) == 0, "create id 1");
    QVERIFY2(id1 == 1, "check id 1");

    // Call dscratch_create_turntable() and check if id2=2. (0,1,2)
    QVERIFY2(dscratch_create_turntable("turntable2", FINAL_SCRATCH_VINYL, &id2) == 0, "create id 2");
    QVERIFY2(id2 == 2, "check id 2");

    // Delete turntable id0.                                (x,1,2)
    QVERIFY2(dscratch_delete_turntable(id0) == 0, "delete id 0 again");

    // Delete turntable id1.                                (x,x,2)
    QVERIFY2(dscratch_delete_turntable(id1) == 0, "delete id 1");

    // Call dscratch_create_turntable() and check if id0=0. (0,x,2)
    QVERIFY2(dscratch_create_turntable("turntable0", FINAL_SCRATCH_VINYL, &id0) == 0, "create id 0 last time");
    QVERIFY2(id0 == 0, "check id 0 last time");


    // Cleanup.

    // Delete turntable id0.                                (x,x,2)
    QVERIFY2(dscratch_delete_turntable(id0) == 0, "cleanup id 0");

    // Delete turntable id1, should fail.                   (x,x,2)
    QVERIFY2(dscratch_delete_turntable(id1) != 0, "cleanup id 1");

    // Delete turntable id2.                                (x,x,x)
    QVERIFY2(dscratch_delete_turntable(id2) == 0, "cleanup id 2");
}

/**
 * Test dscratch_analyze_recorded_datas() and dscratch_get_playing_parameters.
 *
 * Test Description:
 *      - Create a turntable with default parameters.
 *      - Enable position detection.
 *      - Call dscratch_analyze_recorded_datas() continously on next part of
 *        timecode. In the mean time check a little bit the quality of returned
 *        playing parameters.
 */
void DigitalScratchApi_Test::testCase_dscratch_analyze_recorded_datas()
{
    int id = -1;

    // Create a turntable
    QVERIFY2(dscratch_create_turntable("turntable", FINAL_SCRATCH_VINYL, &id) == 0, "create turntable");

    // Enable position detection.
    QVERIFY2(dscratch_set_position_detection(id, 1) == 0, "position detection");

    // Prepare table of samples.
    vector<float> full_tab_1;
    vector<float> full_tab_2;
    vector<int>   tab_index;
    QVERIFY2(l_read_input_samples_from_file(TIMECODE_FILE_1,
                                            full_tab_1,
                                            full_tab_2,
                                            tab_index) == 0, "read input samples");
    QVERIFY2(full_tab_1.size() > 0, "check input samples");

    // Provide several times next part of timecode to digital-scratch and get
    // playing parameters.
    vector<float> tab_1;
    vector<float> tab_2;
    int           start_index     = 0;
    float         speed           = 0.0;
    float         volume          = 0.0;
    float         position        = 0.0;
    bool          are_params_found = false;

    for (unsigned int i = 0; i < tab_index.size(); i++)
    {
        // Check dscratch_analyze_recorded_datas()
        l_select_samples(full_tab_1,
                         full_tab_2,
                         start_index,
                         tab_index[i] - 1,
                         tab_1,
                         tab_2);
        QVERIFY2(dscratch_analyze_recorded_datas(id, &tab_1[0], &tab_2[0], (int)tab_1.size()) == 0,
                                                 QString("analyze data " + QString::number(i)).toStdString().c_str());
        start_index = tab_index[i];

        // Check if digital-scratch was able to find playing parameters.
        are_params_found = dscratch_get_playing_parameters(id,
                                                           &speed,
                                                           &volume,
                                                           &position);
        if (are_params_found == true)
        {
//            cout << "\nspeed=" << speed
//                 << "\t" << "volume=" << volume
//                 << "\t" << "position=" << position
//                 << endl;

            // Speed diff should not be more than 2%.
            QVERIFY2(qAbs(speed - 1.0f) < 0.02, "speed diff < 2%");
        }
    }

    // Cleanup.
    QVERIFY2(dscratch_delete_turntable(id) == 0, "cleanup turntable");
}

/**
 * Test dscratch_analyze_recorded_datas_interleaved() and dscratch_get_playing_parameters.
 *
 * Test Description:
 *      - Same as test_digital_scratch_api_dscratch_analyze_recorded_datas but
 *        by providing interleaved input datas.
 */
void DigitalScratchApi_Test::testCase_dscratch_analyze_recorded_datas_interleaved()
{
    int id = -1;

    // Create a turntable
    QVERIFY2(dscratch_create_turntable("turntable", FINAL_SCRATCH_VINYL, &id) == 0, "create turntable");

    // Enable position detection.
    QVERIFY2(dscratch_set_position_detection(id, 1) == 0, "position detection");

    // Prepare table of samples.
    vector<float> full_tab_1;
    vector<float> full_tab_2;
    vector<int>   tab_index;
    QVERIFY2(l_read_input_samples_from_file(TIMECODE_FILE_1,
                                            full_tab_1,
                                            full_tab_2,
                                            tab_index) == 0, "read input samples");
    QVERIFY2(full_tab_1.size() > 0, "check input samples");

    // Provide several times next part of timecode to digital-scratch and get
    // playing parameters.
    vector<float> tab_1;
    vector<float> tab_2;
    vector<float> tab_interleaved;
    int           start_index     = 0;
    float         speed           = 0.0;
    float         volume          = 0.0;
    float         position        = 0.0;
    bool          are_params_found = false;

    for (unsigned int i = 0; i < tab_index.size(); i++)
    {
        // Prepare a set of samples.
        l_select_samples(full_tab_1,
                         full_tab_2,
                         start_index,
                         tab_index[i] - 1,
                         tab_1,
                         tab_2);
        for (unsigned int j = 0; j < tab_1.size(); j++)
        {
            tab_interleaved.push_back(tab_1[j]);
            tab_interleaved.push_back(tab_2[j]);
            tab_interleaved.push_back(0.0);
            tab_interleaved.push_back(0.0);
        }

        // Check dscratch_analyze_recorded_datas_interleaved()
        QVERIFY2(dscratch_analyze_recorded_datas_interleaved(id, 4, 0, 1, &tab_interleaved[0], (int)tab_1.size()) == 0,
                 QString("analyze interleaved data " + QString::number(i)).toStdString().c_str());
        start_index = tab_index[i];

        // Check if digital-scratch was able to find playing parameters.
        are_params_found = dscratch_get_playing_parameters(id,
                                                           &speed,
                                                           &volume,
                                                           &position);
        if (are_params_found == true)
        {
            /*cout << "\nspeed=" << speed
                 << "\t" << "volume=" << volume
                 << "\t" << "position=" << position
                 << endl;*/

            // Speed diff should not be more than 2%.
            QVERIFY2(qAbs(speed - 1.0f) < 0.02, "speed diff < 2%");
        }

        // Clean tables of input samples.
        tab_1.clear();
        tab_2.clear();
        tab_interleaved.clear();
    }

    // Cleanup.
    QVERIFY2(dscratch_delete_turntable(id) == 0, "cleanup turntable");
}

/**
 * Test dscratch_get_extreme_min() and dscratch_set_extreme_min().
 *
 * Test Description:
 *      - Create a turntable.
 *      - Get current value.
 *      - Try to set wrong value.
 *      - Set the same value+1.
 *      - Check if the result is value+1.
 */
void DigitalScratchApi_Test::testCase_dscratch_set_extreme_min()
{
    int   id  = -1;
    float val = 0.0;
    float inc = 1.0;

    // Create a turntable
    QVERIFY2(dscratch_create_turntable("turntable", FINAL_SCRATCH_VINYL, &id) == 0, "create turntable");

    // Get default value.
    val = dscratch_get_extreme_min(id);

    // Try to set a negative value.
    QVERIFY2(dscratch_set_extreme_min(id, -val) == 1,   "set negative value");
    QVERIFY2(dscratch_get_extreme_min(id)       == val, "get negative value");

    // Set value + 1
    QVERIFY2(dscratch_set_extreme_min(id, val+inc)                == 0,    "set correct value");
    QVERIFY2(qFuzzyCompare(dscratch_get_extreme_min(id), val+inc) == true, "get correct value");

    // Cleanup.
    QVERIFY2(dscratch_delete_turntable(id) == 0, "cleanup turntable");
}

/**
 * Test dscratch_get_version().
 *
 * Test Description:
 *      - Ask version of digital-scratch and check it.
 */
void DigitalScratchApi_Test::testCase_dscratch_get_version()
{
    QVERIFY2(QString(dscratch_get_version()) == QString("1.0.0"), "current version");
}

/**
 * Test dscratch_display_turntable().
 *
 * Test Description:
 *      - Create a turntable with basic parameters.
 *      - Display information about the turntable (to be checked manually).
 */
void DigitalScratchApi_Test::testCase_dscratch_display_turntable()
{
    int id = -1;

    // Create a turntable
    QVERIFY2(dscratch_create_turntable("turntable", FINAL_SCRATCH_VINYL, &id) == 0, "create turntable");

#if 0 // Enable if you want to check result manually.
    // Display informations about the turntable.
    QVERIFY2(dscratch_display_turntable(id) == 0, "display and check manually");
#endif

    // Cleanup.
    QVERIFY2(dscratch_delete_turntable(id) == 0, "cleanup turntable");
}

/**
 * Test dscratch_get_turntable_name().
 *
 * Test Description:
 *      - Create a turntable with basic parameters.
 *      - Get turntable name and check it.
 */
void DigitalScratchApi_Test::testCase_dscratch_get_turntable_name()
{
    char *name = NULL;
    int   id   = -1;

    // Create a turntable
    QVERIFY2(dscratch_create_turntable("turntable", FINAL_SCRATCH_VINYL, &id) == 0, "create turntable");

    // Check turntable name.
    QVERIFY2(dscratch_get_turntable_name(id, &name) == 0, "get name");
    QVERIFY2(QString(name) == QString("turntable"),       "check name");

    // Cleanup.
    if (name != NULL)
    {
        free(name);
    }
    QVERIFY2(dscratch_delete_turntable(id) == 0, "cleanup turntable");
}

/**
 * Test dscratch_get_vinyl_type().
 *
 * Test Description:
 *      - Create a turntable with basic parameters.
 *      - Check if vinyl type is final scratch.
 */
void DigitalScratchApi_Test::testCase_dscratch_get_vinyl_type()
{
    char *vinyl = NULL;
    int   id    = -1;

    // Create a turntable
    QVERIFY2(dscratch_create_turntable("turntable", FINAL_SCRATCH_VINYL, &id) == 0, "create turntable");

    QVERIFY2(dscratch_get_vinyl_type(id, &vinyl) == 0,       "get name");
    QVERIFY2(QString(vinyl) == QString(FINAL_SCRATCH_VINYL), "check name");

    // Cleanup.
    if (vinyl != NULL)
    {
        free(vinyl);
    }
    QVERIFY2(dscratch_delete_turntable(id) == 0, "cleanup turntable");
}

/**
 * Test dscratch_set_max_speed_diff() and dscratch_get_max_speed_diff().
 *
 * Test Description:
 *      - Create a turntable.
 *      - Get current value.
 *      - Try to set wrong value.
 *      - Set the same value+1.
 *      - Check if the result is value+1.
 */
void DigitalScratchApi_Test::testCase_dscratch_set_max_speed_diff()
{
    int   id  = -1;
    float val = 0.0;
    float inc = 1.0;

    // Create a turntable
    QVERIFY2(dscratch_create_turntable("turntable", FINAL_SCRATCH_VINYL, &id) == 0, "create turntable");

    // Get default value.
    val = dscratch_get_max_speed_diff(id);

    // Try to set a negative value.
    QVERIFY2(dscratch_set_max_speed_diff(id, -val) == 1, "set negative max speed diff");
    QVERIFY2(qFuzzyCompare(dscratch_get_max_speed_diff(id), val) == true, "get default max speed diff");

    // Set value + 1
    QVERIFY2(dscratch_set_max_speed_diff(id, val+inc) == 0, "set correct max speed diff");
    QVERIFY2(qFuzzyCompare(dscratch_get_max_speed_diff(id), val+inc) == true, "get correct max speed diff");

    // Cleanup.
    QVERIFY2(dscratch_delete_turntable(id) == 0, "cleanup turntable");
}
