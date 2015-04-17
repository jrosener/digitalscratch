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
    DSCRATCH_HANDLE handle_1 = -1;
    DSCRATCH_HANDLE handle_2 = -1;

    // Try to create a turntable with wrong parameters.
    QVERIFY2(dscratch_create_turntable("",               FINAL_SCRATCH_VINYL, 44100, &handle_1) != 0, "empty name");
    QVERIFY2(dscratch_create_turntable(NULL,             FINAL_SCRATCH_VINYL, 44100, &handle_1) != 0, "null name");
    QVERIFY2(dscratch_create_turntable("left_turntable", "",                  44100, &handle_1) != 0, "empty vinyl");
    QVERIFY2(dscratch_create_turntable("left_turntable", NULL,                44100, &handle_1) != 0, "null vinyl");

    // Create a turntable with correct parameters.
    QVERIFY2(dscratch_create_turntable("left_turntable", FINAL_SCRATCH_VINYL, 44100, &handle_1) == 0, "correct params 1");

    // Check handle_1.
    QVERIFY2(handle_1 != -1, "new turntable id 1");

    // Create again a turntable with correct parameters.
    QVERIFY2(dscratch_create_turntable("left_turntable", FINAL_SCRATCH_VINYL, 44100, &handle_2) == 0, "correct params 2");

    // Check handle_2.
    QVERIFY2(handle_2 != -1, "new turntable id 2");

    // Check handle_2 against handle_1.
    QVERIFY2(handle_2 != handle_1, "different turntable ids");

    // Cleanup.
    QVERIFY2(dscratch_delete_turntable(handle_1) == 0, "clean turntable 1");
    QVERIFY2(dscratch_delete_turntable(handle_2) == 0, "clean turntable 2");
}

/**
 * Test #2 of dscratch_create_turntable().
 *
 * Test Description:
 *      - Note: All calls of dscratch_create_turntable() are with correct params, so
 *        it must pass.
 *      - Call dscratch_create_turntable() and check if handle0=0. (0)
 *      - Delete turntable handle0.                                (x)
 *      - Call dscratch_create_turntable() and check if handle0=0. (0)
 *      - Call dscratch_create_turntable() and check if handle1=1. (0,1)
 *      - Call dscratch_create_turntable() and check if handle2=2. (0,1,2)
 *      - Delete turntable handle0.                                (x,1,2)
 *      - Delete turntable handle1.                                (x,x,2)
 *      - Call dscratch_create_turntable() and check if handle0=0. (0,x,2)
 *      - Delete turntable handle0.                                (x,x,2)
 *      - Delete turntable handle1, should fail.                   (x,x,2)
 *      - Delete turntable handle2.                                (x,x,x)
 */
void DigitalScratchApi_Test::testCase_dscratch_create_turntable_2()
{
    DSCRATCH_HANDLE handle0 = -1;
    DSCRATCH_HANDLE handle1 = -1;
    DSCRATCH_HANDLE handle2 = -1;

    // Call dscratch_create_turntable() and check if handle0=0. (0)
    QVERIFY2(dscratch_create_turntable("turntable0", FINAL_SCRATCH_VINYL, 44100, &handle0) == 0, "create handle0");
    QVERIFY2(handle0 == 0, "check handle0");

    // Delete turntable handle0.                                (x)
    QVERIFY2(dscratch_delete_turntable(handle0) == 0, "delete handle0");

    // Call dscratch_create_turntable() and check if handle0=0. (0)
    QVERIFY2(dscratch_create_turntable("turntable0", FINAL_SCRATCH_VINYL, 44100, &handle0) == 0, "create handleO again");
    QVERIFY2(handle0 == 0, "check handle0 again");

    // Call dscratch_create_turntable() and check if handle1=1. (0,1)
    QVERIFY2(dscratch_create_turntable("turntable1", FINAL_SCRATCH_VINYL, 44100, &handle1) == 0, "create handle1");
    QVERIFY2(handle1 == 1, "check handle1");

    // Call dscratch_create_turntable() and check if handle2=2. (0,1,2)
    QVERIFY2(dscratch_create_turntable("turntable2", FINAL_SCRATCH_VINYL, 44100, &handle2) == 0, "create hande2");
    QVERIFY2(handle2 == 2, "check handle2");

    // Delete turntable handle0.                                (x,1,2)
    QVERIFY2(dscratch_delete_turntable(handle0) == 0, "delete handle0 again");

    // Delete turntable handle1.                                (x,x,2)
    QVERIFY2(dscratch_delete_turntable(handle1) == 0, "delete handle1");

    // Call dscratch_create_turntable() and check if handle0=0. (0,x,2)
    QVERIFY2(dscratch_create_turntable("turntable0", FINAL_SCRATCH_VINYL, 44100, &handle0) == 0, "create handle0 last time");
    QVERIFY2(handle0 == 0, "check handle0 last time");


    // Cleanup.

    // Delete turntable handle0.                                (x,x,2)
    QVERIFY2(dscratch_delete_turntable(handle0) == 0, "cleanup handle0");

    // Delete turntable handle1, should fail.                   (x,x,2)
    QVERIFY2(dscratch_delete_turntable(handle1) != 0, "cleanup handle1");

    // Delete turntable handle2.                                (x,x,x)
    QVERIFY2(dscratch_delete_turntable(handle2) == 0, "cleanup handle2");
}

void DigitalScratchApi_Test::l_dscratch_analyze_timecode(const char *vinyl_type, const char *txt_timecode_file)
{
    DSCRATCH_HANDLE handle = -1;

    // Create a turntable
    QVERIFY2(dscratch_create_turntable("turntable", vinyl_type, 44100, &handle) == 0, "create turntable");

    // Read text file containing timecode data.
    QStringList csv_data;
    QVERIFY2(l_read_text_file_to_string_list(txt_timecode_file, csv_data) == 0, "read CSV");

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
            QVERIFY2(dscratch_analyze_recorded_datas(handle, &channel_1[0], &channel_2[0], (int)channel_1.size()) == 0, "analyze data");

            // Check if digital-scratch was able to find playing parameters.
            if (expected_speed != -99.0)
            {
                QVERIFY2(dscratch_get_playing_parameters(handle, &speed, &volume) == 0, "get playing parameters");

                // cout << "expected speed=" << expected_speed << endl;
                // cout << "speed=" << speed << "\t" << "volume=" << volume << endl;
                // cout << "diff speed=" << qAbs(speed - expected_speed) << endl;
                // cout << endl;

                // Speed diff should not be more than 0.01%.
                QVERIFY2(qAbs(speed - expected_speed) < 0.0001, qPrintable("expected speed = " + QString::number(expected_speed) + ", speed = " + QString::number(speed)));

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
    QVERIFY2(dscratch_delete_turntable(handle) == 0, "cleanup turntable");
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
void DigitalScratchApi_Test::testCase_dscratch_analyze_timecode_serato_stop_fast()
{
    l_dscratch_analyze_timecode(SERATO_VINYL, TIMECODE_SERATO_33RPM_STOP_FAST);
}
void DigitalScratchApi_Test::testCase_dscratch_analyze_timecode_serato_noises()
{
    l_dscratch_analyze_timecode(SERATO_VINYL, TIMECODE_SERATO_33RPM_NOISES);
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
    l_dscratch_analyze_timecode(FINAL_SCRATCH_VINYL, TIMECODE_FS_33RPM_SPEED100);
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
    DSCRATCH_HANDLE handle = -1;

    // Create a turntable
    QVERIFY2(dscratch_create_turntable("turntable", FINAL_SCRATCH_VINYL, 44100, &handle) == 0, "create turntable");

#if 0 // Enable if you want to check result manually.
    // Display informations about the turntable.
    QVERIFY2(dscratch_display_turntable(handle) == 0, "display and check manually");
#endif

    // Cleanup.
    QVERIFY2(dscratch_delete_turntable(handle) == 0, "cleanup turntable");
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
    DSCRATCH_HANDLE handle = -1;

    // Create a turntable
    QVERIFY2(dscratch_create_turntable("turntable", FINAL_SCRATCH_VINYL, 44100, &handle) == 0, "create turntable");

    // Check turntable name.
    QVERIFY2(dscratch_get_turntable_name(handle, &name) == 0, "get name");
    QVERIFY2(QString(name) == QString("turntable"),       "check name");

    // Cleanup.
    if (name != NULL)
    {
        free(name);
    }
    QVERIFY2(dscratch_delete_turntable(handle) == 0, "cleanup turntable");
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
    DSCRATCH_HANDLE handle = -1;

    // Create a turntable
    QVERIFY2(dscratch_create_turntable("turntable", FINAL_SCRATCH_VINYL, 44100, &handle) == 0, "create turntable");

    QVERIFY2(dscratch_get_vinyl_type(handle, &vinyl) == 0,       "get name");
    QVERIFY2(QString(vinyl) == QString(FINAL_SCRATCH_VINYL), "check name");

    // Cleanup.
    if (vinyl != NULL)
    {
        free(vinyl);
    }
    QVERIFY2(dscratch_delete_turntable(handle) == 0, "cleanup turntable");
}
