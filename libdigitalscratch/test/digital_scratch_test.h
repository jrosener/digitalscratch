#include <QObject>
#include <QtTest>
#include <iostream>
using namespace std;

class DigitalScratch_Test : public QObject
{
    Q_OBJECT

public:
    DigitalScratch_Test();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testCase_constructor();
    void testCase_set_max_nb_buffer();
    void testCase_set_max_nb_speed_for_stability();
    void testCase_set_max_slow_speed();
    void testCase_set_max_speed_diff();
    void testCase_analyze_recording_data_1();
    void testCase_analyze_recording_data_2();
    void testCase_analyze_recording_data_3();
};
