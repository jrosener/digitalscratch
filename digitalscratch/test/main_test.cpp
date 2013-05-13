#include <audio_track_test.h>
#include <audio_file_decoding_process_test.h>
#include <utils_test.h>
#include <data_persistence_test.h>
#include <keyfinder_test.h>

int main(int argc, char** argv)
{
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
   return status;
}
