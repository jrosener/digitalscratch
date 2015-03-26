#include <QObject>
#include <QtTest>

class Control_and_playback_process_Test : public QObject
{
    Q_OBJECT

 public:
    Control_and_playback_process_Test();

 private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testCaseRunWithJack_1deck();
    void testCaseRunWithJack_2decks();
};
