/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                     libdigitalscratch tests                                */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/*                                                                            */
/*  Copyright (C) 2003-2020                                                   */
/*                Julien Rosener <julien.rosener@digital-scratch.org>         */
/*                                                                            */
/*----------------------------------------------------------------( License )-*/
/*                                                                            */
/*  This program is free software: you can redistribute it and/or modify      */
/*  it under the terms of the GNU General Public License as published by      */
/*  the Free Software Foundation, either version 3 of the License, or         */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  This package is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU General Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with this program. If not, see <http://www.gnu.org/licenses/>.      */
/*                                                                            */
/*============================================================================*/

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

void l_create_default_input_samples(QVector<float> &tb_1,
                                    QVector<float> &tb_2)
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
                                    QStringList   &string_list)
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

int l_get_next_buffer_of_timecode(QStringList    &csv_data,
                                  QVector<float> &channel_1,
                                  QVector<float> &channel_2,
                                  float          &expected_speed)
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

