#ifndef TEST_UTILS_H_
#define TEST_UTILS_H_

#include <QVector>

// Timecode file name.
#define TIMECODE_FS_33RPM_SPEED100      "test/data/finalscratch_-_33rpm_0pitch.txt"
#define TIMECODE_SERATO_33RPM_STOP_FAST "test/data/serato_perf_-_33rpm_0pitch_-_stopping_fast.txt"
#define TIMECODE_SERATO_33RPM_NOISES    "test/data/serato_perf_-_33rpm_0pitch_-_noises.txt"

/**
 * This function create 2 tables of float with 5 parameters.
 */
void l_create_default_input_samples(QVector<float> &tb_1,
                                    QVector<float> &tb_2);

/**
 * @brief l_read_text_file_to_string_list read a file line by line and put result in string_list.
 * @param file_name is the file to read.
 * @param string_list is the list which will contain the lines of the file.
 * @return 0 if everything is OK.
 */
int l_read_text_file_to_string_list(const QString &file_name,
                                    QStringList   &string_list);

/**
 * @brief l_get_next_buffer_of_timecode Extract 2 tables of timecode data from the first buffer (and remove it from csv_data).
 * @param csv_data
 * @param channel_1
 * @param channel_2
 * @param expected_speed
 * @return true if we reached the end of the csv_data.
 */
int l_get_next_buffer_of_timecode(QStringList    &csv_data,
                                  QVector<float> &channel_1,
                                  QVector<float> &channel_2,
                                  float          &expected_speed);

#endif /*TEST_UTILS_H_*/
