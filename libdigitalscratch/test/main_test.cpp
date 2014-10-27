#include <controller_test.h>
#include <digital_scratch_api_test.h>
#include <speed_test.h>
#include <digital_scratch_test.h>
#include <volume_test.h>

int main(int argc, char** argv)
{

   // Logging settings.
   qSetMessagePattern("[%{type}] | %{category} | %{function}@%{line} | %{message}");
   QLoggingCategory::setFilterRules(QStringLiteral("*.debug=false\n \
                                                    *.warning=false\n \
                                                    *.critical=false\n"));

   int status = 0;
   {
      DigitalScratchApi_Test tc;
      status |= QTest::qExec(&tc, argc, argv);
   }
   {
      Controller_Test tc;
      status |= QTest::qExec(&tc, argc, argv);
   }
   {
      Speed_Test tc;
      status |= QTest::qExec(&tc, argc, argv);
   }
   {
      DigitalScratch_Test tc;
      status |= QTest::qExec(&tc, argc, argv);
   }
   {
      Volume_Test tc;
      status |= QTest::qExec(&tc, argc, argv);
   }

   return status;
}
