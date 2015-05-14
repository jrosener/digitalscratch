#include <QObject>
#include <QtTest>
#include <iostream>
#include <digital_scratch_api.h>

using namespace std;

class DigitalScratchApi_Test : public QObject
{
    Q_OBJECT
    
private:
    void l_dscratch_analyze_timecode(DSCRATCH_VINYLS vinyl_type, const char *txt_timecode_file);
    void l_dscratch_analyze_timecode_V2(DSCRATCH_VINYLS vinyl_type, const char *txt_timecode_file);

public:
    DigitalScratchApi_Test();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testCase_dscratch_create_turntable();
    void testCase_dscratch_analyze_timecode_serato_stop_fast();
    void testCase_dscratch_analyze_timecode_serato_stop_fast_V2();
    void testCase_dscratch_analyze_timecode_serato_noises();
    void testCase_dscratch_analyze_timecode_finalscratch();
    void testCase_dscratch_display_turntable();
    void testCase_dscratch_get_vinyl_type();
};
