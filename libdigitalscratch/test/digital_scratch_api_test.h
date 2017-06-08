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

#include <QObject>
#include <QtTest>
#include <iostream>
#include <digital_scratch_api.h>

using namespace std;

class DigitalScratchApi_Test : public QObject
{
    Q_OBJECT

private:
    void l_dscratch_analyze_timecode(dscratch_vinyls_t vinyl_type, const char *txt_timecode_file);

public:
    DigitalScratchApi_Test();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testCase_dscratch_create_turntable();
    void testCase_dscratch_analyze_timecode_serato_stop_fast();
    void testCase_dscratch_analyze_timecode_serato_noises();
    void testCase_dscratch_display_turntable();
    void testCase_dscratch_get_vinyl_type();
};
