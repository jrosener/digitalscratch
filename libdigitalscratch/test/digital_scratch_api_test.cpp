#include <QtTest>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>

#include "test_utils.h"
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

/** Test:
 *    dscratch_create_turntable() 
 *    dscratch_delete_turntable()
 */
void DigitalScratchApi_Test::testCase_dscratch_create_turntable()
{
    DSCRATCH_HANDLE handle_1 = nullptr;
    DSCRATCH_HANDLE handle_2 = nullptr;

    // Create a turntable with bad parameters.
    QVERIFY2(dscratch_create_turntable(FINAL_SCRATCH, 44100, nullptr) == DSCRATCH_ERROR, "bad handle");

    // Create a turntable with correct parameters.
    QVERIFY2(dscratch_create_turntable(FINAL_SCRATCH, 44100, &handle_1) == DSCRATCH_SUCCESS, "correct params 1");

    // Check handle_1.
    QVERIFY2(handle_1 != nullptr, "new turntable 1");

    // Create again a turntable with correct parameters.
    QVERIFY2(dscratch_create_turntable(FINAL_SCRATCH, 44100, &handle_2) == DSCRATCH_SUCCESS, "correct params 2");

    // Check handle_2.
    QVERIFY2(handle_2 != nullptr, "new turntable 2");

    // Check handle_2 against handle_1.
    QVERIFY2(handle_2 != handle_1, "different turntable ids");

    // Cleanup.
    QVERIFY2(dscratch_delete_turntable(handle_1) == DSCRATCH_SUCCESS, "clean turntable 1");
    QVERIFY2(dscratch_delete_turntable(handle_2) == DSCRATCH_SUCCESS, "clean turntable 2");
}

void DigitalScratchApi_Test::l_dscratch_analyze_timecode(DSCRATCH_VINYLS vinyl_type, const char *txt_timecode_file)
{
    DSCRATCH_HANDLE handle = nullptr;

    // Create a turntable
    QVERIFY2(dscratch_create_turntable(vinyl_type, 44100, &handle) == DSCRATCH_SUCCESS, "create turntable");

    // Read text file containing timecode data.
    QStringList csv_data;
    QVERIFY2(l_read_text_file_to_string_list(txt_timecode_file, csv_data) == 0, "read CSV");

    // Provide several times next part of timecode to digital-scratch and get
    // playing parameters.
    QVector<float> channel_1;
    QVector<float> channel_2;
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
            QVERIFY2(dscratch_analyze_recorded_datas(handle, &channel_1[0], &channel_2[0], (int)channel_1.size()) == DSCRATCH_SUCCESS, "analyze data");

            // Check if digital-scratch was able to find playing parameters.
            if (expected_speed != -99.0)
            {
                QVERIFY2(dscratch_get_playing_parameters(handle, &speed, &volume) == DSCRATCH_SUCCESS, "get playing parameters");

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
    QVERIFY2(dscratch_delete_turntable(handle) == DSCRATCH_SUCCESS, "cleanup turntable");
}

/** Test:
 *    dscratch_analyze_recorded_datas()
 *    dscratch_get_playing_parameters()
 */
void DigitalScratchApi_Test::testCase_dscratch_analyze_timecode_serato_stop_fast()
{
    l_dscratch_analyze_timecode(SERATO, TIMECODE_SERATO_33RPM_STOP_FAST);
}

void DigitalScratchApi_Test::testCase_dscratch_analyze_timecode_serato_noises()
{
    l_dscratch_analyze_timecode(SERATO, TIMECODE_SERATO_33RPM_NOISES);
}

void DigitalScratchApi_Test::testCase_dscratch_analyze_timecode_finalscratch()
{
    l_dscratch_analyze_timecode(FINAL_SCRATCH, TIMECODE_FS_33RPM_SPEED100);
}

/**
 * Test: 
 *   dscratch_display_turntable()
 */
void DigitalScratchApi_Test::testCase_dscratch_display_turntable()
{
    DSCRATCH_HANDLE handle = nullptr;

    // Create a turntable
    QVERIFY2(dscratch_create_turntable(FINAL_SCRATCH, 44100, &handle) == DSCRATCH_SUCCESS, "create turntable");

#if 0 // Enable if you want to check result manually.
    // Display informations about the turntable.
    QVERIFY2(dscratch_display_turntable(handle) == DSCRATCH_SUCCESS, "display and check manually");
#endif

    // Cleanup.
    QVERIFY2(dscratch_delete_turntable(handle) == DSCRATCH_SUCCESS, "cleanup turntable");
}

/**
 * Test: 
 *   dscratch_get_turntable_vinyl_type()
 *   dscratch_get_vinyl_name_from_type()
 */
void DigitalScratchApi_Test::testCase_dscratch_get_vinyl_type()
{
    DSCRATCH_VINYLS vinyl;
    DSCRATCH_HANDLE handle = nullptr;

    // Create a turntable
    QVERIFY2(dscratch_create_turntable(FINAL_SCRATCH, 44100, &handle) == DSCRATCH_SUCCESS, "create turntable");

    QVERIFY2(dscratch_get_turntable_vinyl_type(handle, &vinyl) == DSCRATCH_SUCCESS, "get type");
    QVERIFY2(QString(dscratch_get_vinyl_name_from_type(vinyl)) == "final scratch standard 2.0", "check name");

    // Cleanup.
    QVERIFY2(dscratch_delete_turntable(handle) == DSCRATCH_SUCCESS, "cleanup turntable");
}
