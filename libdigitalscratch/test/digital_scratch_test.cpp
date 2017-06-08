/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                     libdigitalscratch tests                                */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/*                                                                            */
/*  Copyright (C) 2003-2017                                                   */
/*                Julien Rosener <julien.rosener@digital-scratch.org>         */
/*                                                                            */
/*----------------------------------------------------------------( License )-*/
/*                                                                            */
/*  This program is free software: you can redistribute it and/or modify      */
/*  it under the terms of the GNU General Public License as published by      */
/*  the Free Software Foundation, either version 3 of the License, or         */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  This package is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU General Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with this program. If not, see <http://www.gnu.org/licenses/>.      */
/*                                                                            */
/*============================================================================*/

#include <QtTest>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <QVector>

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
 * Test:
 *    analyze_captured_timecoded_signal()
 */
void DigitalScratch_Test::testCase_analyze_recording_data()
{
   // Create Digital_scratch.
   Digital_scratch *dscratch = new Digital_scratch(FINAL_SCRATCH, 44100);

   // Check analyze_recording_data()
   QVector<float> tab_1;
   QVector<float> tab_2;
   QVERIFY2(dscratch->analyze_captured_timecoded_signal(tab_1, tab_2) == false, "empty tables");

   tab_1.push_back(10); tab_1.push_back(20);
   tab_2.push_back(10);
   QVERIFY2(dscratch->analyze_captured_timecoded_signal(tab_1, tab_2) == false, "different table sizes");

   tab_1.clear();
   tab_2.clear();
   l_create_default_input_samples(tab_1, tab_2);
   QVERIFY2(tab_1.size() > 0, "table 1 not empty");
   QVERIFY2(tab_2.size() > 0, "table 2 not empty");
   QVERIFY2(dscratch->analyze_captured_timecoded_signal(tab_1, tab_2) == true, "correct tables");

   // Cleanup.
   delete dscratch;
}
