#ifndef TEST_UTILS_H_
#define TEST_UTILS_H_

#include <vector>
using std::vector;

// Timecode file name.
#define TIMECODE_FILE_1 "test/data/timecode_1.txt" /** sp=1.0 dir=1 pos=? buf=225x50 */

/**
 * This function create 2 tables of float with 5 parameters.
 */
void l_create_default_input_samples(vector<float> &tb_1,
                                    vector<float> &tb_2);

/**
 * This function create 2 tables of float read from a specified file, syntax
 * of the text file must be:
 *      > 225           # ><space>buffer_size
 *      0.1234 0.5678   # sample_left<space>sample_right
 *      0.6597 0.2156
 *      0.3647 0.3265
 *      ...
 *      > 225
 *      0.6545 0.3564
 *      0.3651 0.3545
 *      ...
 *
 * @return tb_1 will contain all values corresponding to first column
 * @return tb_2 will contain all values corresponding to second column
 * @return tb_index will contain the indexes corresponding to buffer_size
 *
 * @return 0 if all is OK.
 *
 */

int l_read_input_samples_from_file(const char    *file_name,
                                   vector<float> &tb_1,
                                   vector<float> &tb_2,
                                   vector<int>   &tb_index);

/**
 * This function create 2 tables of float containing an extract of 2 provided
 * full tables.
 *
 * @return tab_1 will contain all values corresponding to first table between
 *         start_index and end_index
 * @return tab_2 will contain all values corresponding to second table between
 *         start_index and end_index
 *
 */
void l_select_samples(vector<float> &full_tab_1,
                      vector<float> &full_tab_2,
                      int            start_index,
                      int            end_index,
                      vector<float> &tb_1,
                      vector<float> &tb_2);

#endif /*TEST_UTILS_H_*/
