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
   {
      Audio_device_access_rules_Test tc;
      status |= QTest::qExec(&tc, argc, argv);
   }
   {
      Control_and_playback_process_Test tc;
      status |= QTest::qExec(&tc, argc, argv);
   }
#endif

   return status;
}
