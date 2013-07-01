 #include <controller_test.h>
 #include <digital_scratch_api_test.h>

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
   
   return status;
}
