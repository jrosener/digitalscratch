#include <QObject>
#include <QtTest>
#include <iostream>
using namespace std;

class DigitalScratchApi_Test : public QObject
{
    Q_OBJECT

public:
    DigitalScratchApi_Test();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testCase_dscratch_create_turntable_1();
    void testCase_dscratch_create_turntable_2();
    void testCase_dscratch_analyze_timecode_serato();
    void testCase_dscratch_analyze_timecode_finalscratch();
    void testCase_dscratch_analyze_timecode_finalscratch_interleaved();
    void testCase_dscratch_display_turntable();
    void testCase_dscratch_get_turntable_name();
    void testCase_dscratch_get_vinyl_type();
};
