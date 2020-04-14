/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                     libdigitalscratch tests                                */
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

#include <digital_scratch_test.h>
#include <timecoded_signal_process_test.h>

int main(int argc, char** argv)
{

   // Logging settings.
   qSetMessagePattern("[%{type}] | %{category} | %{function}@%{line} | %{message}");
   QLoggingCategory::setFilterRules(QStringLiteral("*.debug=false\n \
                                                    *.warning=false\n \
                                                    *.critical=false\n"));

   int status = 0;
   {
      DigitalScratch_Test tc;
      status |= QTest::qExec(&tc, argc, argv);
   }
   {
      TimecodedSignalProcess_Test tc;
      status |= QTest::qExec(&tc, argc, argv);
   }
   return status;
}
