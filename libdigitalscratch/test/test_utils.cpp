#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextStream>
using namespace std;

#include "test_utils.h"

void l_create_default_input_samples(vector<float> &tb_1,
                                    vector<float> &tb_2)
{
    // Reserve enough memory for tb_1 and tb_2.
    tb_1.reserve(5);
    tb_2.reserve(5);

    // Fill tables with default values.
    tb_1.push_back(0.0f); tb_2.push_back(0.5f);
    tb_1.push_back(0.1f); tb_2.push_back(0.6f);
    tb_1.push_back(0.2f); tb_2.push_back(0.7f);
    tb_1.push_back(0.3f); tb_2.push_back(0.8f);
    tb_1.push_back(0.4f); tb_2.push_back(0.9f);

    return;
}

int l_read_text_file_to_string_list(const QString &file_name,
                                    QStringList &string_list)
{
    QFile text_file(file_name);
    if (text_file.open(QIODevice::ReadOnly | QIODevice::Text) == true)
    {
        QTextStream text_stream(&text_file);
        while(true)
        {
            QString line = text_stream.readLine();
            if (line.isNull())
            {
                break;
            }
            else
            {
                // Do not take comments.
                if (line.startsWith('#') == false)
                {
                    string_list.append(line);
                }
            }
        }
    }
    else
    {
        return -1;
    }

    return 0;
}

int l_get_next_buffer_of_timecode(QStringList   &csv_data,
                                  vector<float> &channel_1,
                                  vector<float> &channel_2,
                                  float         &expected_speed)
{
    // Init.
    channel_1.clear();
    channel_2.clear();
    expected_speed = -99; // speed not found.

    if (csv_data.size() == 0)
    {
        return true; // EOF
    }

    QString line = csv_data.at(0);
    csv_data.removeAt(0);
    if (csv_data.size() == 0)
    {
        return true; // EOF
    }

    // Parse until beginning of a buffer.
    while (line.startsWith("buffer_size;speed") == false)
    {
        line = csv_data.at(0);
        csv_data.removeAt(0);
        if (csv_data.size() == 0)
        {
            return true; // EOF
        }
    }

    // Get line for buffer size and expected speed.
    line = csv_data.at(0);
    csv_data.removeAt(0);
    if (csv_data.size() == 0)
    {
        return true; // EOF
    }
    QStringList buffer_and_speed = line.split(';');

    // Remove "channel1;channel2"
    line = csv_data.at(0);
    csv_data.removeAt(0);
    if (csv_data.size() == 0)
    {
        return true; // EOF
    }

    // Get buffer size.
    int buffer_size = buffer_and_speed.at(0).toInt();

    // Get data.
    if (csv_data.size() >= buffer_size)
    {
        for (int i = 0; i < buffer_size; i++)
        {
            line = csv_data.at(0);
            csv_data.removeAt(0);
            channel_1.push_back(line.split(';').at(0).toFloat());
            channel_2.push_back(line.split(';').at(1).toFloat());
        }
    }
    else
    {
        return true; // EOF
    }

    // Get expected speed.
    if (buffer_and_speed.at(1) != "TODO")
    {
        expected_speed = buffer_and_speed.at(1).toFloat();
    }

    return false;
}

int l_read_input_samples_from_file(const char    *file_name,
                                   vector<float> &tb_1,
                                   vector<float> &tb_2,
                                   vector<int>   &tb_index)
{
    int    tb_size       = 0;
    int    tb_index_size = 0;
    string line;

    // Open file.
    std::ifstream file(file_name);
    if (file.is_open() != true)
    {
        cout << "Cannot open " << file_name << endl;
        return -1;
    }

    // Count number of line.
    while (std::getline(file, line))
    {
        // Ignore line describing buffer size.
        if (line.find_first_of('>') == string::npos) // Not a line with buffer size.
        {
            tb_size++; // Line with pair of values.
        }
        else
        {
            tb_index_size++; // Line with buffer size.
        }
    }

    if (tb_size <= 0)
    {
        cout << "Cannot read " << file_name << endl;
        return -1;
    }

    // Close file. TODO: file should be opened only one time !
    file.close();

    // Open file.
    std::ifstream file_1(file_name);
    if (file_1.is_open() != true)
    {
        cout << "Cannot open " << file_name << endl;
        return -1;
    }

    // Reserve memory for tb_1, tb_2 and tb_index.
    tb_1.reserve(tb_size);
    tb_2.reserve(tb_size);
    tb_index.reserve(tb_index_size);

    // Fill tables with samples read from file.
    int   i  = 0;
    int   j  = 0;
    float d1 = 0.0;
    float d2 = 0.0;
    int   id = 0;
    char *p_end;
    while (std::getline(file_1, line))
    {
        if (line.find_first_of('>') == string::npos) // Not a line with buffer size.
        {
            d1 = (float)strtod (line.c_str(), &p_end);
            d2 = (float)strtod (p_end, NULL);
            tb_1.push_back(d1);
            tb_2.push_back(d2);
            i++;
        }
        else // Line with buffer size.
        {
            id += atoi(line.c_str()+2); // Avoid 2 first char ("> ").
            tb_index.push_back(id);
            j++;
        }
    }

    // Close file.
    file_1.close();

    return 0;
}

void l_select_samples(vector<float> &full_tab_1,
                      vector<float> &full_tab_2,
                      int            start_index,
                      int            end_index,
                      vector<float> &tb_1,
                      vector<float> &tb_2)
{
    // Get tab_size.
    int tb_size = (end_index - start_index) + 1;

    // Clean tb_1 and tb_2.
    tb_1.clear();
    tb_2.clear();

    // Reserve memory for tb_1 and tb_2.
    tb_1.reserve(tb_size);
    tb_2.reserve(tb_size);

    // Copy elements of full tables in tb_1 and tb_2.
    for (int i = 0; i < tb_size; i++)
    {
        tb_1.push_back(full_tab_1[start_index + i]);
        tb_2.push_back(full_tab_2[start_index + i]);
    }

    return;
}
