#include <controller_test.h>
#include <digital_scratch_api_test.h>
#include <speed_test.h>
#include <position_test.h>
#include <digital_scratch_test.h>
#include <volume_test.h>

int main(int argc, char** argv)
{
   int status = 0;
   {
      Controller_Test tc;
      status |= QTest::qExec(&tc, argc, argv);
   }
   {
      DigitalScratchApi_Test tc;
      status |= QTest::qExec(&tc, argc, argv);
   }
   {
      Speed_Test tc;
      status |= QTest::qExec(&tc, argc, argv);
   }
   {
      Position_Test tc;
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
