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
void DigitalScratch_Test::testCase_analyze_recording_data()
{
   // Create Digital_scratch.
   Digital_scratch *dscratch = new Digital_scratch(FINAL_SCRATCH, 44100);

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
