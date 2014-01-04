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
    void testCase_dscratch_analyze_recorded_datas();
    void testCase_dscratch_analyze_recorded_datas_interleaved();
    void testCase_dscratch_set_extreme_min();
    void testCase_dscratch_display_turntable();
    void testCase_dscratch_get_turntable_name();
    void testCase_dscratch_get_vinyl_type();
    void testCase_dscratch_set_max_speed_diff();
};
