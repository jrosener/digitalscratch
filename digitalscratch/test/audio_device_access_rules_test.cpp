/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                     Digital Scratch Player Test                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/*                                                                            */
/*  Copyright (C) 2003-2020                                                   */
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

#include <QString>
#include <QtTest>
#include "audio_device_access_rules_test.h"
#include "audiodev/sound_card_control_rules.h"

Audio_device_access_rules_Test::Audio_device_access_rules_Test()
{
}

void Audio_device_access_rules_Test::initTestCase()
{
}

void Audio_device_access_rules_Test::cleanupTestCase()
{
}

void Audio_device_access_rules_Test::testCase_get_device_list()
{
    // Get sound card list.
    QList<QString> cards = Sound_card_control_rules::get_device_list();

    // There must be at least one device.
    QVERIFY2(cards.size() >= 1, "list sound card in the system");
}

