#include <QTextCodec>
#include <audio_track_test.h>
#include <audio_file_decoding_process_test.h>
#include <utils_test.h>
#include <data_persistence_test.h>
#include <playlist_persistence_test.h>
#include <audio_device_access_rules_test.h>
#include <sound_capture_and_playback_process_test.h>

int main(int argc, char** argv)
{
    // Necessary to have an event loop needed by some tests.
    QCoreApplication app(argc, argv);

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
   QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
   QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
   QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
#endif

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
      Sound_capture_and_playback_process_Test tc;
      status |= QTest::qExec(&tc, argc, argv);
   }
#endif

   // Wait until threads are done (necessary because no QApplication is created/deleted).
  // QThreadPool::globalInstance()->waitForDone();

   return status;
}
