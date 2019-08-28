/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                     Digital Scratch Player Test                            */
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

#include <QTextCodec>
#include "audio_track_test.h"
#include "audio_file_decoding_process_test.h"
#include "utils_test.h"
#include "data_persistence_test.h"
#include "playlist_persistence_test.h"
#include "audio_device_access_rules_test.h"
#include "control_and_playback_process_test.h"

int main(int argc, char** argv)
{
    // Necessary to have an event loop needed by some tests.
    QCoreApplication app(argc, argv);

   // Logging settings.
   qSetMessagePattern("[%{type}] | %{category} | %{function}@%{line} | %{message}");
   QLoggingCategory::setFilterRules(QStringLiteral("*.debug=false\n \
                                                   *.warning=false\n \
                                                   *.critical=false\n"));

   int status = 0;
   {
      Audio_track_Test tc;
      status |= QTest::qExec(&tc, argc, argv);
   }
   {
      Audio_file_decoding_process_Test tc;
      status |= QTest::qExec(&tc, argc, argv);
   }
   {
      Utils_Test tc;
      status |= QTest::qExec(&tc, argc, argv);
   }
   {
      Data_persistence_Test tc;
      status |= QTest::qExec(&tc, argc, argv);
   }
   {
      Playlist_persistence_Test tc;
      status |= QTest::qExec(&tc, argc, argv);
   }
#ifdef ENABLE_TEST_DEVICE
   #if 0 // FIXME: not supported for the moment.
   {
      Audio_device_access_rules_Test tc;
      status |= QTest::qExec(&tc, argc, argv);
   }
   #endif
   {
      Control_and_playback_process_Test tc;
      status |= QTest::qExec(&tc, argc, argv);
   }
#endif

   return status;
}
