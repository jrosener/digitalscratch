#include <QObject>
#include <QtTest>

class Sound_capture_and_playback_process_Test : public QObject
{
    Q_OBJECT

 public:
    Sound_capture_and_playback_process_Test();

 private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testCaseRunWithJack();
};
