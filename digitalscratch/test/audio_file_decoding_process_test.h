#include <QObject>
#include <QtTest>
#include "app/application_const.h"

class Audio_file_decoding_process_Test : public QObject
{
    Q_OBJECT

public:
    Audio_file_decoding_process_Test();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testCaseCreate();
    void testCaseRun();
};
