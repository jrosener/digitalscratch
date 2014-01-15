#include <QtTest>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>

using namespace std;

#include "test_utils.h"
#include <digital_scratch.h>
#include <digital_scratch_test.h>

DigitalScratch_Test::DigitalScratch_Test()
{
}

void DigitalScratch_Test::initTestCase()
{
}

void DigitalScratch_Test::cleanupTestCase()
{
}


/**
 * Test Digital_scratch::Digital_scratch().
 *
 * Test Description:
 *      - Create a Digital_scratch object with valid parameters.
 *      - Check initialization of all public parameters.
 */
void DigitalScratch_Test::testCase_constructor()
{
    // Create Digital_scratch.
    Digital_scratch *dscratch = new Digital_scratch("left_turntable",
                                                    FINAL_SCRATCH_VINYL,
                                                    44100);

    // Check initilization of all public parameters.
    QVERIFY2(qFuzzyCompare(dscratch->get_max_speed_diff(), DEFAULT_MAX_SPEED_DIFF) == true,    "max speed diff");
    QVERIFY2(qFuzzyCompare(dscratch->get_max_slow_speed(), DEFAULT_MAX_SLOW_SPEED) == true,    "max slow speed");
    QVERIFY2(dscratch->get_max_nb_buffer() == DEFAULT_MAX_NB_BUFFER,                           "max nb buffer");
    QVERIFY2(dscratch->get_max_nb_speed_for_stability() == DEFAULT_MAX_NB_SPEED_FOR_STABILITY, "max nb speed for stability");

    // Cleanup.
    delete dscratch;
}

/**
* Test Digital_scratch::set_max_nb_buffer().
*
* Test Description:
*      - Create a Digital_scratch object with valid parameters.
*      - Check if set_max_nb_buffer(0) return false and if the value is not
*        set.
*      - Check if set_max_nb_buffer(-99) return false and if the value is not
*        set.
*      - Check if set_max_nb_buffer(10) return true and if the value is set.
*/
void DigitalScratch_Test::testCase_set_max_nb_buffer()
{
   // Create Digital_scratch.
   Digital_scratch *dscratch = new Digital_scratch("left_turntable",
                                                   FINAL_SCRATCH_VINYL,
                                                   44100);

   // Check set_max_nb_buffer().
   QVERIFY2(dscratch->set_max_nb_buffer(0)   == false,                 "set zero value");
   QVERIFY2(dscratch->get_max_nb_buffer()    == DEFAULT_MAX_NB_BUFFER, "get default value");
   QVERIFY2(dscratch->set_max_nb_buffer(-99) == false,                 "set negative value");
   QVERIFY2(dscratch->get_max_nb_buffer()    == DEFAULT_MAX_NB_BUFFER, "get default value again");
   QVERIFY2(dscratch->set_max_nb_buffer(10)  == true,                  "set correct value");
   QVERIFY2(dscratch->get_max_nb_buffer()    == 10,                    "get correct value");

   // Cleanup.
   delete dscratch;
}

/**
* Test Digital_scratch::set_max_nb_speed_for_stability().
*
* Test Description:
*      - Create a Digital_scratch object with valid parameters.
*      - Check if set_max_nb_speed_for_stability(0) return false and if the
*        value is not set.
*      - Check if set_max_nb_speed_for_stability(-99) return false and if the
*        value is not set.
*      - Check if set_max_nb_speed_for_stability(10) return true and if the
*        value is set.
*      - Check again if set_max_nb_speed_for_stability(8) return true and if
*        the value is set (check if memory free/allocation are working well).
*/
void DigitalScratch_Test::testCase_set_max_nb_speed_for_stability()
{
   // Create Digital_scratch.
   Digital_scratch *dscratch = new Digital_scratch("left_turntable",
                                                   FINAL_SCRATCH_VINYL,
                                                   44100);

   // Check set_max_nb_speed_for_stability().
   QVERIFY2(dscratch->set_max_nb_speed_for_stability(0)   == false,                              "set zero value");
   QVERIFY2(dscratch->get_max_nb_speed_for_stability()    == DEFAULT_MAX_NB_SPEED_FOR_STABILITY, "get default value");
   QVERIFY2(dscratch->set_max_nb_speed_for_stability(-99) == false,                              "set negative value");
   QVERIFY2(dscratch->get_max_nb_speed_for_stability()    == DEFAULT_MAX_NB_SPEED_FOR_STABILITY, "get default value again");
   QVERIFY2(dscratch->set_max_nb_speed_for_stability(10)  == true,                               "set correct value 1");
   QVERIFY2(dscratch->get_max_nb_speed_for_stability()    == 10,                                 "get correct value 1");
   QVERIFY2(dscratch->set_max_nb_speed_for_stability(8)   == true,                               "set correct value 2");
   QVERIFY2(dscratch->get_max_nb_speed_for_stability()    == 8,                                  "get correct value 2");

   // Cleanup.
   delete dscratch;
}

/**
* Test Digital_scratch::set_max_slow_speed().
*
* Test Description:
*      - Create a Digital_scratch object with valid parameters.
*      - Check if set_max_slow_speed(0.0) return false and if the value is not
*        set.
*      - Check if set_max_slow_speed(-99.0) return false and if the value is
*        not set.
*      - Check if set_max_slow_speed(10.2) return true and if the value is set.
*/
void DigitalScratch_Test::testCase_set_max_slow_speed()
{
   // Create Digital_scratch.
   Digital_scratch *dscratch = new Digital_scratch("left_turntable",
                                                   FINAL_SCRATCH_VINYL,
                                                   44100);

   // Check set_max_slow_speed()
   QVERIFY2(dscratch->set_max_slow_speed(0)                      == false,                  "set zero value");
   QVERIFY2(dscratch->get_max_slow_speed()                       == DEFAULT_MAX_SLOW_SPEED, "get default value");
   QVERIFY2(dscratch->set_max_slow_speed(-99)                    == false,                  "set negative value");
   QVERIFY2(dscratch->get_max_slow_speed()                       == DEFAULT_MAX_SLOW_SPEED, "get default value again");
   QVERIFY2(dscratch->set_max_slow_speed(10.2f)                  == true,                   "set correct value");
   QVERIFY2(qFuzzyCompare(dscratch->get_max_slow_speed(), 10.2f) == true,                   "get correct value");

   // Cleanup.
   delete dscratch;
}

/**
* Test Digital_scratch::set_max_speed_diff().
*
* Test Description:
*      - Create a Digital_scratch object with valid parameters.
*      - Check if set_max_speed_diff(0.0) return false and if the value is not
*        set.
*      - Check if set_max_speed_diff(-99.0) return false and if the value is
*        not set.
*      - Check if set_max_speed_diff(10.2) return true and if the value is set.
*/
void DigitalScratch_Test::testCase_set_max_speed_diff()
{
   // Create Digital_scratch.
   Digital_scratch *dscratch = new Digital_scratch("left_turntable",
                                                   FINAL_SCRATCH_VINYL,
                                                   44100);

   // Check set_max_speed_diff()
   QVERIFY2(dscratch->set_max_speed_diff(0) == false,                                      "set zero value");
   QVERIFY2(qFuzzyCompare(dscratch->get_max_speed_diff(), DEFAULT_MAX_SPEED_DIFF) == true, "get default value");
   QVERIFY2(dscratch->set_max_speed_diff(-99) == false,                                    "set negative value");
   QVERIFY2(qFuzzyCompare(dscratch->get_max_speed_diff(), DEFAULT_MAX_SPEED_DIFF) == true, "get default value again");
   QVERIFY2(dscratch->set_max_speed_diff(10.2f) == true,                                   "set correct value");
   QVERIFY2(qFuzzyCompare(dscratch->get_max_speed_diff(), 10.2f) == true,                  "get correct value");

   // Cleanup.
   delete dscratch;
}

/**
* Test Digital_scratch::analyze_recording_data().
* This test is not dedicated to test the quality of playing parameters values
* but only the behavior of analyze_recording_datas() against input paramaters.
*
* Test Description:
*      - Create a Digital_scratch object with valid parameters.
*      - Check if analyze_recording_datas() with empty size return false.
*      - Check if analyze_recording_datas() with tabs with different sizes
         return false.
*      - Check if analyze_recording_datas(tab_1, tab_2, 5) return true (tab_1
*        and tab_2 well defined and nb_frames=nb_elem(tab_1 or tab_2).
*/
void DigitalScratch_Test::testCase_analyze_recording_data_1()
{
   // Create Digital_scratch.
   Digital_scratch *dscratch = new Digital_scratch("left_turntable",
                                                   FINAL_SCRATCH_VINYL,
                                                   44100);

   // Check analyze_recording_data()
   vector<float> tab_1;
   vector<float> tab_2;
   QVERIFY2(dscratch->analyze_recording_data(tab_1, tab_2) == false, "empty tables");

   tab_1.push_back(10); tab_1.push_back(20);
   tab_2.push_back(10);
   QVERIFY2(dscratch->analyze_recording_data(tab_1, tab_2) == false, "different table sizes");

   tab_1.clear();
   tab_2.clear();
   l_create_default_input_samples(tab_1, tab_2);
   QVERIFY2(tab_1.size() > 0, "table 1 not empty");
   QVERIFY2(tab_2.size() > 0, "table 2 not empty");
   QVERIFY2(dscratch->analyze_recording_data(tab_1, tab_2) == true, "correct tables");

   // Cleanup.
   delete dscratch;
}

/**
* Test Digital_scratch::analyze_recording_data().
* This test is not dedicated to test the quality of playing parameters values
* but only the behavior of analyze_recording_datas() against input paramaters.
*
* Test Description:
*      - Create a Digital_scratch object with valid parameters.
*      - Get buffers of samples from a file.
*      - Check if analyze_recording_datas() return true.
*      - Check if get_playing_parameters() return true (it will mean that
*        playing parameters are found).
*/
void DigitalScratch_Test::testCase_analyze_recording_data_2()
{
   // Create Digital_scratch.
   Digital_scratch *dscratch = new Digital_scratch("left_turntable",
                                                   FINAL_SCRATCH_VINYL,
                                                   44100);

   // Prepare table of samples.
   vector<float> full_tab_1;
   vector<float> full_tab_2;
   vector<int>   tab_index;
   QVERIFY2(l_read_input_samples_from_file(TIMECODE_FS_33RPM_SPEED100,
                                           full_tab_1,
                                           full_tab_2,
                                           tab_index) == 0, "read input files");
   QVERIFY2(full_tab_1.size() > 0, "check table not empty");

   // Check analyze_recording_data()
   vector<float> tab_1;
   vector<float> tab_2;
   l_select_samples(full_tab_1,
                    full_tab_2,
                    0,
                    tab_index[0],
                    tab_1,
                    tab_2);
   QVERIFY2(dscratch->analyze_recording_data(tab_1, tab_2) == true, "ananlyze data");

   // Check if digital-scratch was able to find playing parameters.
   float speed    = 0.0;
   float volume   = 0.0;
   float position = 0.0;
   QVERIFY2(dscratch->get_playing_parameters(&speed,
                                             &volume,
                                             &position) == true, "get playing params");

//cout << "speed=" << speed << "\t" << "volume=" << volume << "\t" << "position=" << position << endl;

   // Cleanup.
   delete dscratch;
}

/**
* Test Digital_scratch::analyze_recording_data().
* This test is not dedicated to test the quality of playing parameters values
* but only the behavior of analyze_recording_datas() against input paramaters.
*
* Test Description:
*      - Create a Digital_scratch object with valid parameters.
*      - Get buffers of samples from a file.
*      - Check if analyze_recording_datas() return true.
*      - Check if get_playing_parameters() return true (it will mean that
*        playing parameters are found).
*      - Do same calls again with next buffers of samples.
*/
void DigitalScratch_Test::testCase_analyze_recording_data_3()
{
   // Create Digital_scratch.
   Digital_scratch *dscratch = new Digital_scratch("left_turntable",
                                                   FINAL_SCRATCH_VINYL,
                                                   44100);

   // Enable detection of position.
   QVERIFY2(dscratch->enable_position_detection(true) == true, "enable position detection");
   QVERIFY2(dscratch->get_position_detection_state()  == true, "get position detection");

   // Prepare table of samples.
   vector<float> full_tab_1;
   vector<float> full_tab_2;
   vector<int>   tab_index;
   QVERIFY2(l_read_input_samples_from_file(TIMECODE_FS_33RPM_SPEED100,
                                           full_tab_1,
                                           full_tab_2,
                                           tab_index) == 0, "read input samples");
   QVERIFY2(full_tab_1.size() > 0, "check inpujt sample table not empty");

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
       // Check analyze_recording_data()
       l_select_samples(full_tab_1,
                        full_tab_2,
                        start_index,
                        tab_index[i] - 1,
                        tab_1,
                        tab_2);
       QVERIFY2(dscratch->analyze_recording_data(tab_1, tab_2) == true, "analyze input data");
       start_index = tab_index[i];

       // Check if digital-scratch was able to find playing parameters.
       are_params_found = dscratch->get_playing_parameters(&speed,
                                                           &volume,
                                                           &position);
       if (are_params_found == true)
       {
           /*cout << "\nspeed=" << speed
                << "\t" << "volume=" << volume
                << "\t" << "position=" << position
                << endl;*/

           // Speed diff should not be more than 4%.
           QVERIFY2(qAbs(speed - 1.0f) < 0.04, "speed diff < 4%");
       }
   }

   // Cleanup.
   delete dscratch;
}
