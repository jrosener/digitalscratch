/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                                                                            */
/*                                                                            */
/*------------------------------------------------( final_scratch_vinyl.cpp )-*/
/*                                                                            */
/*  Copyright (C) 2003-2012                                                   */
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
/*------------------------------------------------------------( Description )-*/
/*                                                                            */
/*  Final_scratch_vinyl class : define a Stanton FinalScratch timecoded vinyl */
/*                                                                            */
/*============================================================================*/

#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

#include "include/dscratch_parameters.h"
#include "include/utils.h"
#include "include/coded_vinyl.h"
#include "include/final_scratch_vinyl.h"
#include "include/timecode.h"

Final_scratch_vinyl::Final_scratch_vinyl() : Coded_vinyl()
{
    Utils::trace_object_life(TRACE_PREFIX_FSVINYL,
                             "+ Creating Final_scratch_vinyl object...");

    Utils::trace_object_life(TRACE_PREFIX_FSVINYL,
                             "+ Final_scratch_vinyl object created");
}

Final_scratch_vinyl::~Final_scratch_vinyl()
{
    Utils::trace_object_life(TRACE_PREFIX_FSVINYL,
                             "- Deleting Final_scratch_vinyl object...");

    Utils::trace_object_life(TRACE_PREFIX_FSVINYL,
                             "- Final_scratch_vinyl object deleted");
}

int Final_scratch_vinyl::get_timecode_start_sequence_position()
{
    // search a sequence of 1 1 1 0 on right channel bit list (all pair index
    // value of bit_list)
    unsigned int i = 0;

    for (i = 0; i < this->bit_list.size() - 6; i = i+2)
    {
        if (this->bit_list[i].value == START_SEQ_BIT1 \
            && this->bit_list[i+2].value == START_SEQ_BIT2 \
            && this->bit_list[i+4].value == START_SEQ_BIT3 \
            && this->bit_list[i+6].value == START_SEQ_BIT4)
        {
            // start sequence found
            #ifdef TRACE_POSITION
                Utils::trace_position(TRACE_PREFIX_FSVINYL, "Start sequence found on index " \
                                      + Utils::to_string(i));
            #endif

            return i;
        }
    }

    return -1;
}

unsigned int Final_scratch_vinyl::get_timecode()
{
    // bit_list combine bits from right and left channel, the timecode is
    // 16 bits from right channel and 16 bits from left channel
    unsigned int i = 0;
    int j = 31;
    unsigned int timecode = 0;

    for (i = 0; i < 32; i = i + 2)
    {
        if (this->bit_list[i].value == BIT_1)
        {
            timecode = timecode | (1 << j);
        }
        j--;
    }

    for (i = 1; i < 32; i = i + 2)
    {
        if (this->bit_list[i].value == BIT_1)
        {
            timecode = timecode | (1 << j);
        }
        j--;
    }

    return timecode;
}

float Final_scratch_vinyl::get_position_from_timecode_value(unsigned int timecode)
{
    // get the corresponding value in list of timecode (hard coded)
    unsigned int timecode_table_size = 271744; // size = 135872 x 2
    unsigned int int_position  = 0;
    float        time_position = 0;

    for (unsigned int i = 0; i < timecode_table_size; i = i + 2)
    {
        if (timecode == timecode_table[i])
        {
            int_position = timecode_table[i+1];
            time_position = (float)int_position / 48000.0 - 25.0;

            return time_position;
        }
    }

    return 0.0;
}

float Final_scratch_vinyl::get_position()
{
    Utils::trace_position(TRACE_PREFIX_FSVINYL, "Searching new position...");
#if 0 // TODO: check this position detection implementation.
    vector<bit_t>::iterator itr1;
    vector<bit_t>::iterator itr2;
    vector<float> positions;

    // if bit_list size is less than 32 (a complete timecode) we can not get
    // timecode value for the moment
    while (this->bit_list.size() >= 32)
    {
        #ifdef TRACE_POSITION
            Utils::trace_position(TRACE_PREFIX_FSVINYL, "bit_list size = " \
                                  + Utils::to_string(this->bit_list.size()) \
                                  + " (>= 32, so we can search timecode)");
        #endif

        // determine bit types corresponding to amplitude
        if (this->get_bit_types() == false)
        {
            Utils::trace_error(TRACE_PREFIX_FSVINYL, "Can not find bit types");

            return NO_NEW_POSITION_FOUND;
        }

        // search start sequence on right channel (pair positions in bit_list)
        int start_sequence_position = 0;
        if ((start_sequence_position = this->get_timecode_start_sequence_position()) == -1)
        {
#if 0
            // start sequence not found, remove all bit from bit_list exept 5
            // latest elements (maybe it is a beginning of a new start sequence)

            itr1 = this->bit_list.begin();
            if (this->latest_was_right == true)
            {
                Utils::trace_position(TRACE_PREFIX_FSVINYL, "No start sequence found, remove all elements except 5 latest");

                itr2 = this->bit_list.end()-5;
            }
            else
            {
                Utils::trace_position(TRACE_PREFIX_FSVINYL, "No start sequence found, remove all elements except 6 latest");

                itr2 = this->bit_list.end()-6;
            }
            this->bit_list.erase(itr1, itr2);
#else
this->bit_list.clear();
this->latest_was_right = false;
#endif
        }
        else
        {
            // we found a start sequence, remove first unusable elements

            #ifdef TRACE_POSITION
                Utils::trace_position(TRACE_PREFIX_FSVINYL, "Start sequence found in position " \
                                      + Utils::to_string(start_sequence_position));
                Utils::trace_position(TRACE_PREFIX_FSVINYL, "Remove firsts unusable elements (before start sequence)");
            #endif

            if (start_sequence_position > 0)
            {
                itr1 = this->bit_list.begin();
                itr2 = itr1 + start_sequence_position;
                this->bit_list.erase(itr1, itr2);
            }

            // if bit_list size is at least 32 then we can search timecode on the 32 firsts elements

            if (this->bit_list.size() >= 32)
            {
                #ifdef TRACE_POSITION
                    Utils::trace_position(TRACE_PREFIX_FSVINYL, "There are " \
                                          + Utils::to_string(this->bit_list.size()) \
                                          + " element in bit_list, we can search timecode");
                #endif

                unsigned int timecode = 0;
                timecode = this->get_timecode();

                // remove 32 firsts element used for detecting timecode
                itr1 = this->bit_list.begin();
                itr2 = itr1 + 32;
                this->bit_list.erase(itr1, itr2);

                if (timecode != 0)
                {
                    // we found a working timecode, get the corresponding position

                    #ifdef TRACE_POSITION
                        Utils::trace_position(TRACE_PREFIX_FSVINYL, "Timecode found (" \
                                              + Utils::to_string(timecode) \
                                              + "), get corresponding position");
                    #endif

                    float position = this->get_position_from_timecode_value(timecode);
                    if (position != 0.0)
                    {
                        // add this position to positions list
                        positions.push_back(position);
                        #ifdef TRACE_POSITION
                            Utils::trace_position(TRACE_PREFIX_FSVINYL, "New position found : " + Utils::to_string(position));
                        #endif
                    }
                    else
                    {
                        Utils::trace_position(TRACE_PREFIX_FSVINYL, "No position found");
                    }
                }
                else
                {
                    // we did not found a working timecode

                    Utils::trace_position(TRACE_PREFIX_FSVINYL, "Timecode not found");

                    return NO_NEW_POSITION_FOUND;
                }
            }
            else
            {
                #ifdef TRACE_POSITION
                    Utils::trace_position(TRACE_PREFIX_FSVINYL, "There are " \
                                          + Utils::to_string(this->bit_list.size()) \
                                          + " element in bit_list, it is not enough to search a timecode");
                #endif

                return NO_NEW_POSITION_FOUND;
            }
        }
    }

    // if we found some positions return only the latest
    // TODO: write an algorithm that check if the latest position is the
    // best (compare with other positions or make an average)
    if (positions.empty() == false)
    {
        return positions[positions.size()-1];
    }
#else
    // no position found
    return NO_NEW_POSITION_FOUND;
#endif
}

int Final_scratch_vinyl::get_sinusoidal_frequency()
{
    return FINAL_SCRATCH_SINUSOIDAL_FREQ;
}
