/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                                    Tests                                   */
/*                                                                            */
/*                                                                            */
/*---------------------------------------------------------[ test_utils.cpp ]-*/
/*                                                                            */
/*  Copyright (C) 2003-2008                                                   */
/*                Julien Rosener <julien.rosener@digital-scratch.org>         */
/*                                                                            */
/*----------------------------------------------------------------( License )-*/
/*                                                                            */
/*  This program is free software; you can redistribute it and/or modify      */
/*  it under the terms of the GNU General Public License as published by      */
/*  the Free Software Foundation; either version 2 of the License, or         */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              */
/*  GNU General Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with this program; if not, write to the Free Software               */
/*  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA */
/*                                                                            */
/*------------------------------------------------------------( Description )-*/
/*                                                                            */
/*                        Digital-scratch Tests                               */
/*               Utility that can be used in every tests.                     */
/*                                                                            */
/*============================================================================*/

#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
using namespace std;

#include "test_utils.h"

void l_create_default_input_samples(vector<float> &tb_1,
                                    vector<float> &tb_2)
{
    // Reserve enough memory for tb_1 and tb_2.
    tb_1.reserve(5);
    tb_2.reserve(5);

    // Fill tables with default values.
    tb_1.push_back(0.0); tb_2.push_back(0.5);
    tb_1.push_back(0.1); tb_2.push_back(0.6);
    tb_1.push_back(0.2); tb_2.push_back(0.7);
    tb_1.push_back(0.3); tb_2.push_back(0.8);
    tb_1.push_back(0.4); tb_2.push_back(0.9);

    return;
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
            d1 = strtod (line.c_str(), &p_end);
            d2 = strtod (p_end, NULL);
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
