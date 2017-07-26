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
#include <timecoded_signal_process.h>
#include <timecoded_signal_process_test.h>

TimecodedSignalProcess_Test::TimecodedSignalProcess_Test()
{
}

void TimecodedSignalProcess_Test::initTestCase()
{
}

void TimecodedSignalProcess_Test::cleanupTestCase()
{
}

/**
 * Test:
 *    analyze_captured_timecoded_signal()
 */
void TimecodedSignalProcess_Test::testCase_run()
{
   // Create a timecoded signal processor.
   Timecoded_signal_process *sig_process = new Timecoded_signal_process(FINAL_SCRATCH, 44100);

   // Process timecoded samples.
   QVector<float> tab_1;
   QVector<float> tab_2;
   QVERIFY2(sig_process->run(tab_1, tab_2) == false, "empty tables");

   tab_1.push_back(10); tab_1.push_back(20);
   tab_2.push_back(10);
   QVERIFY2(sig_process->run(tab_1, tab_2) == false, "different table sizes");

   tab_1.clear();
   tab_2.clear();
   l_create_default_input_samples(tab_1, tab_2);
   QVERIFY2(tab_1.size() > 0, "table 1 not empty");
   QVERIFY2(tab_2.size() > 0, "table 2 not empty");
   QVERIFY2(sig_process->run(tab_1, tab_2) == true, "correct tables");

   // Cleanup.
   delete sig_process;
}
