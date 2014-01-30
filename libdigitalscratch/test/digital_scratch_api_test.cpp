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
    QVERIFY2(dscratch_create_turntable("",               FINAL_SCRATCH_VINYL, 44100, &turntable_id_1) != 0, "empty name");
    QVERIFY2(dscratch_create_turntable(NULL,             FINAL_SCRATCH_VINYL, 44100, &turntable_id_1) != 0, "null name");
    QVERIFY2(dscratch_create_turntable("left_turntable", "",                  44100, &turntable_id_1) != 0, "empty vinyl");
    QVERIFY2(dscratch_create_turntable("left_turntable", NULL,                44100, &turntable_id_1) != 0, "null vinyl");

    // Create a turntable with correct parameters.
    QVERIFY2(dscratch_create_turntable("left_turntable", FINAL_SCRATCH_VINYL, 44100, &turntable_id_1) == 0, "correct params 1");

    // Check turntable_id_1.
    QVERIFY2(turntable_id_1 != -1, "new turntable id 1");

    // Create again a turntable with correct parameters.
    QVERIFY2(dscratch_create_turntable("left_turntable", FINAL_SCRATCH_VINYL, 44100, &turntable_id_2) == 0, "correct params 2");

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
    QVERIFY2(dscratch_create_turntable("turntable0", FINAL_SCRATCH_VINYL, 44100, &id0) == 0, "create id 0");
    QVERIFY2(id0 == 0, "check id 0");

    // Delete turntable id0.                                (x)
    QVERIFY2(dscratch_delete_turntable(id0) == 0, "delete id 0");

    // Call dscratch_create_turntable() and check if id0=0. (0)
    QVERIFY2(dscratch_create_turntable("turntable0", FINAL_SCRATCH_VINYL, 44100, &id0) == 0, "create id 0 again");
    QVERIFY2(id0 == 0, "check id 0 again");

    // Call dscratch_create_turntable() and check if id1=1. (0,1)
    QVERIFY2(dscratch_create_turntable("turntable1", FINAL_SCRATCH_VINYL, 44100, &id1) == 0, "create id 1");
    QVERIFY2(id1 == 1, "check id 1");

    // Call dscratch_create_turntable() and check if id2=2. (0,1,2)
    QVERIFY2(dscratch_create_turntable("turntable2", FINAL_SCRATCH_VINYL, 44100, &id2) == 0, "create id 2");
    QVERIFY2(id2 == 2, "check id 2");

    // Delete turntable id0.                                (x,1,2)
    QVERIFY2(dscratch_delete_turntable(id0) == 0, "delete id 0 again");

    // Delete turntable id1.                                (x,x,2)
    QVERIFY2(dscratch_delete_turntable(id1) == 0, "delete id 1");

    // Call dscratch_create_turntable() and check if id0=0. (0,x,2)
    QVERIFY2(dscratch_create_turntable("turntable0", FINAL_SCRATCH_VINYL, 44100, &id0) == 0, "create id 0 last time");
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
 * Test dscratch_analyze_recorded_datas() and dscratch_get_playing_parameters() and validate speed.
 *
 * Test Description:
 *      - Create a turntable with default parameters.
 *      - Call dscratch_analyze_recorded_datas() continously on next part of
 *        timecode. In the mean time check a little bit the quality of returned
 *        playing parameters.
 */
void DigitalScratchApi_Test::testCase_dscratch_analyze_timecode_serato()
{
    int id = -1;

    // Create a turntable
    QVERIFY2(dscratch_create_turntable("turntable", SERATO_VINYL, 44100, &id) == 0, "create turntable");

    // Read text file containing timecode data.
    QStringList csv_data;
    QVERIFY2(l_read_text_file_to_string_list(TIMECODE_SERATO_33RPM_STOP_FAST, csv_data) == 0, "read CSV");

    // Provide several times next part of timecode to digital-scratch and get
    // playing parameters.
    vector<float> channel_1;
    vector<float> channel_2;
    bool          eof             = false;
    float         expected_speed  = 0.0;
    float         speed           = 0.0;
    float         volume          = 0.0;
    while (eof == false)
    {
        // Get a chunk of timecode data.
        eof = l_get_next_buffer_of_timecode(csv_data, channel_1, channel_2, expected_speed);

        if (eof == false)
        {
            // Check dscratch_analyze_recorded_datas()
            QVERIFY2(dscratch_analyze_recorded_datas(id, &channel_1[0], &channel_2[0], (int)channel_1.size()) == 0, "analyze data");

            // Check if digital-scratch was able to find playing parameters.
            if (expected_speed != -99.0)
            {
                QVERIFY2(dscratch_get_playing_parameters(id, &speed, &volume) == 0, "get playing parameters");

                //cout << "expected speed=" << expected_speed << endl;
                //cout << "speed=" << speed << "\t" << "volume=" << volume << endl;
                //cout << "diff speed=" << qAbs(speed - expected_speed) << endl;
                //cout << endl;

                // Speed diff should not be more than 0.01%.
                QVERIFY2(qAbs(speed - expected_speed) < 0.0001, "speed diff < 0.01%");

                // Volume should be < 1.0 only if speed < 0.90.
                if (qAbs(expected_speed) < 0.90)
                {
                    QVERIFY2(volume < 1.0, "volume not full");
                }
                else
                {
                    QVERIFY2(volume == 1.0, "full volume");
                }
            }
        }
    }

    // Cleanup.
    QVERIFY2(dscratch_delete_turntable(id) == 0, "cleanup turntable");
}

/**
 * Test dscratch_analyze_recorded_datas() and dscratch_get_playing_parameters.
 *
 * Test Description:
 *      - Create a turntable with default parameters.
 *      - Call dscratch_analyze_recorded_datas() continously on next part of
 *        timecode. In the mean time check a little bit the quality of returned
 *        playing parameters.
 */
void DigitalScratchApi_Test::testCase_dscratch_analyze_timecode_finalscratch()
{
    int id = -1;

    // Create a turntable
    QVERIFY2(dscratch_create_turntable("turntable", FINAL_SCRATCH_VINYL, 44100, &id) == 0, "create turntable");

    // Read text file containing timecode data.
    QStringList csv_data;
    QVERIFY2(l_read_text_file_to_string_list(TIMECODE_FS_33RPM_SPEED100, csv_data) == 0, "read CSV");

    // Provide several times next part of timecode to digital-scratch and get
    // playing parameters.
    vector<float> channel_1;
    vector<float> channel_2;
    bool          eof             = false;
    float         expected_speed  = 0.0;
    float         speed           = 0.0;
    float         volume          = 0.0;
    while (eof == false)
    {
        // Get a chunk of timecode data.
        eof = l_get_next_buffer_of_timecode(csv_data, channel_1, channel_2, expected_speed);

        if (eof == false)
        {
            // Check dscratch_analyze_recorded_datas()
            QVERIFY2(dscratch_analyze_recorded_datas(id, &channel_1[0], &channel_2[0], (int)channel_1.size()) == 0, "analyze data");

            // Check if digital-scratch was able to find playing parameters.
            if (expected_speed != -99.0)
            {
                QVERIFY2(dscratch_get_playing_parameters(id, &speed, &volume) == 0, "get playing parameters");

                cout << "expected speed=" << expected_speed << endl;
                cout << "speed=" << speed << "\t" << "volume=" << volume << endl;
                cout << "diff speed=" << qAbs(speed - expected_speed) << endl;
                cout << endl;

                // Speed diff should not be more than 0.01%.
                QVERIFY2(qAbs(speed - expected_speed) < 0.0001, "speed diff < 0.01%");
            }
        }
    }

    // Cleanup.
    QVERIFY2(dscratch_delete_turntable(id) == 0, "cleanup turntable");
}

/**
 * Test dscratch_analyze_recorded_datas_interleaved() and dscratch_get_playing_parameters().
 *
 * Test Description:
 *      - Create a turntable with default parameters.
 *      - Call dscratch_analyze_recorded_datas() continously on next part of
 *        timecode. In the mean time check a little bit the quality of returned
 *        playing parameters.
 */
void DigitalScratchApi_Test::testCase_dscratch_analyze_timecode_finalscratch_interleaved()
{
    int id = -1;

    // Create a turntable
    QVERIFY2(dscratch_create_turntable("turntable", FINAL_SCRATCH_VINYL, 44100, &id) == 0, "create turntable");

    // Read text file containing timecode data.
    QStringList csv_data;
    QVERIFY2(l_read_text_file_to_string_list(TIMECODE_FS_33RPM_SPEED100, csv_data) == 0, "read CSV");

    // Provide several times next part of timecode to digital-scratch and get
    // playing parameters.
    vector<float> channel_1;
    vector<float> channel_2;
    vector<float> tab_interleaved;
    bool          eof             = false;
    float         expected_speed  = 0.0;
    float         speed           = 0.0;
    float         volume          = 0.0;
    while (eof == false)
    {
        // Get a chunk of timecode data.
        eof = l_get_next_buffer_of_timecode(csv_data, channel_1, channel_2, expected_speed);

        if (eof == false)
        {
            // Prepare interleaved data.
            for (unsigned int i = 0; i < channel_1.size(); i++)
            {
                tab_interleaved.push_back(channel_1[i]);
                tab_interleaved.push_back(channel_2[i]);
                tab_interleaved.push_back(0.0);
                tab_interleaved.push_back(0.0);
            }

            // Check dscratch_analyze_recorded_datas()
            QVERIFY2(dscratch_analyze_recorded_datas_interleaved(id, 4, 0, 1, &tab_interleaved[0], (int)channel_1.size()) == 0, "analyze interleaved data");

            // Check if digital-scratch was able to find playing parameters.
            if (expected_speed != -99.0)
            {
                QVERIFY2(dscratch_get_playing_parameters(id, &speed, &volume) == 0, "get playing parameters");

//                cout << "expected speed=" << expected_speed << endl;
//                cout << "speed=" << speed << "\t" << "volume=" << volume << endl;
//                cout << "diff speed=" << qAbs(speed - expected_speed) << endl;
//                cout << endl;

                // Speed diff should not be more than 0.01%.
                QVERIFY2(qAbs(speed - expected_speed) < 0.0001, "speed diff < 0.01%");
            }

            // Cleanup.
            tab_interleaved.clear();
        }
    }

    // Cleanup.
    QVERIFY2(dscratch_delete_turntable(id) == 0, "cleanup turntable");
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
    QVERIFY2(dscratch_create_turntable("turntable", FINAL_SCRATCH_VINYL, 44100, &id) == 0, "create turntable");

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
    QVERIFY2(dscratch_create_turntable("turntable", FINAL_SCRATCH_VINYL, 44100, &id) == 0, "create turntable");

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
    QVERIFY2(dscratch_create_turntable("turntable", FINAL_SCRATCH_VINYL, 44100, &id) == 0, "create turntable");

    QVERIFY2(dscratch_get_vinyl_type(id, &vinyl) == 0,       "get name");
    QVERIFY2(QString(vinyl) == QString(FINAL_SCRATCH_VINYL), "check name");

    // Cleanup.
    if (vinyl != NULL)
    {
        free(vinyl);
    }
    QVERIFY2(dscratch_delete_turntable(id) == 0, "cleanup turntable");
}
