/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                                                                            */
/*                                                                            */
/*--------------------------------------------------------( coded_vinyl.cpp )-*/
/*                                                                            */
/*  Copyright (C) 2003-2013                                                   */
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
/*                Coded_vinyl class : define a coded vinyl disk               */
/*                                                                            */
/*============================================================================*/

#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <cmath>
#include <iterator>

using namespace std;

#include "include/dscratch_parameters.h"
#include "include/utils.h"
#include "include/coded_vinyl.h"

Coded_vinyl::Coded_vinyl(unsigned int sample_rate)
{
    Utils::trace_object_life(TRACE_PREFIX_CODED_VINYL,
                             "+ Creating Coded_vinyl object...");

    this->left_channel_extreme_list.reserve(200);
    this->right_channel_extreme_list.reserve(200);
    this->extreme_list_clean(LEFT_CHANNEL);
    this->extreme_list_clean(RIGHT_CHANNEL);

    this->reuse_sample_data               = false;
    this->current_speed                   = 0.0;
    this->direction                       = 1;
    this->nb_same_direction_change        = 0;
    this->direction_is_changing           = false;
    this->latest_available_detected_speed = 0;
    this->nb_buffer_to_wait               = 0;
    this->waiting_other_buffer            = false;
    this->latest_was_right                = false;
    this->old_volume                      = 0.0;

    this->rpm                                = RPM_33;
    this->extreme_min                        = (float)DEFAULT_EXTREME_MIN;
    this->max_buffer_coeff                   = DEFAULT_MAX_BUFFER_COEFF;
    this->nb_cycle_before_changing_direction = DEFAULT_NB_CYCLE_BEFORE_CHANGING_DIRECTION;
    this->coeff_right_dist_min_bit1_to_bit1  = (float)DEFAULT_COEFF_RIGHT_DIST_MIN_BIT1_TO_BIT1;
    this->coeff_left_dist_min_bit1_to_bit1   = (float)DEFAULT_COEFF_LEFT_DIST_MIN_BIT1_TO_BIT1;
    this->coeff_right_dist_max_bit0_to_bit0  = (float)DEFAULT_COEFF_RIGHT_DIST_MAX_BIT0_TO_BIT0;
    this->coeff_left_dist_max_bit0_to_bit0   = (float)DEFAULT_COEFF_LEFT_DIST_MAX_BIT0_TO_BIT0;
    this->progressive_volume_coeff           = (float)DEFAULT_PROGRESSIVE_VOLUME_COEFF;
    this->full_volume_amplitude              = (float)DEFAULT_FULL_VOLUME_AMPLITUDE;
    this->sample_rate                        = sample_rate;
    this->low_pass_filter_max_speed_usage    = (float)DEFAULT_LOW_PASS_FILTER_MAX_SPEED_USAGE;
    this->input_amplify_coeff                = DEFAULT_INPUT_AMPLIFY_COEFF;
    this->total_input_samples_1.reserve(512 * this->get_max_buffer_coeff());
    this->total_input_samples_2.reserve(512 * this->get_max_buffer_coeff());

    this->set_no_input_signal(true);
    this->set_reverse_direction(false);

    Utils::trace_object_life(TRACE_PREFIX_CODED_VINYL,
                             "+ Coded_vinyl object created");
}

Coded_vinyl::~Coded_vinyl()
{
    Utils::trace_object_life(TRACE_PREFIX_CODED_VINYL, "- Deleting Coded_vinyl object...");

    this->extreme_list_clean(LEFT_CHANNEL);
    this->extreme_list_clean(RIGHT_CHANNEL);
    this->bit_list.clear();
    this->delete_total_input_samples();

    Utils::trace_object_life(TRACE_PREFIX_CODED_VINYL, "- Coded_vinyl object deleted");
}

void Coded_vinyl::add_sound_data(vector<float> &input_samples_1,
                                         vector<float> &input_samples_2)
{
    // Prepare size of extreme lists. Increase size if needed.
    unsigned int max_size_extreme_lists = (input_samples_1.size()/10) * this->get_max_buffer_coeff();
    if (this->left_channel_extreme_list.capacity() < max_size_extreme_lists)
    {
        this->left_channel_extreme_list.reserve(max_size_extreme_lists);
        this->right_channel_extreme_list.reserve(max_size_extreme_lists);
    }

    //
    // are we able to detect new frequency ?
    // if latest detected speed is enough we can use these new input samples,
    // otherwise, we have to wait for other input samples and then analyze the
    // whole table of samples
    //

    // add this new buffer to the total buffer
    this->add_new_input_samples(input_samples_1, input_samples_2);

    float average = 0.0;
    this->get_average_amplitude(this->total_input_samples_1, &average);

    if (average < this->extreme_min)
    {
        this->set_no_input_signal(true);
        this->delete_total_input_samples();
        return;
    }


    //
    // apply low-pass filter on each samples buffers
    //


    // use low_pass filter to remove noise on input signal
    if (fabs(this->latest_available_detected_speed) < this->get_low_pass_filter_max_speed_usage())
    {
        // get cut frequency corresponding to average amplitude
        int cut_frequency_1 = 0;
        if (this->get_cut_frequency(&cut_frequency_1) == false)
        {
            Utils::trace_error(TRACE_PREFIX_CODED_VINYL,
                               "Can not get cut frequency for low-pass filtering");
        }
        int cut_frequency_2 = 0;
        if (this->get_cut_frequency(&cut_frequency_2) == false)
        {
            Utils::trace_error(TRACE_PREFIX_CODED_VINYL,
                               "Can not get cut frequency for low-pass filtering");
        }

        // call low_pass filter on each channel
        if (this->low_pass_filter(this->total_input_samples_1,
                                  cut_frequency_1) == false)
        {
            Utils::trace_error(TRACE_PREFIX_CODED_VINYL,
                               "Can not apply low-pass filter on recorded timecode");
        }
        if (this->low_pass_filter(this->total_input_samples_2,
                                  cut_frequency_2) == false)
        {
            Utils::trace_error(TRACE_PREFIX_CODED_VINYL,
                               "Can not apply low-pass filter on recorded timecode");
        }
    }

    //
    // fill extremes lists with datas from sample list
    //
    this->channels_extreme_list_fill(this->total_input_samples_1,
                                     this->total_input_samples_2);

    //
    // check signal (using extreme values) level
    //
    #ifdef ENABLE_CHECK_EXTREMES_LEVEL
        if (this->check_extremes_level() == false)
        {
            Utils::trace_error(TRACE_PREFIX_CODED_VINYL,
                               "Can not check signal level");
        }
        if (this->is_no_input_signal() == true)
        {
            this->extreme_list_clean(LEFT_CHANNEL);
            this->extreme_list_clean(RIGHT_CHANNEL);
            this->delete_total_input_samples();
            return;
        }
    #endif

    //
    // fill extremes position lists with datas from extremes lists
    //
    if (this->left_channel_extreme_list.size() > 1)
    {
        // security if bit_list is too large
        if (this->bit_list.size() > this->total_input_samples_1.size()/2)
        {
            this->bit_list.clear();
            this->latest_was_right = false;
        }
        // fill bit_list
        if (this->fill_bit_list() == false)
        {
            Utils::trace_position(TRACE_PREFIX_CODED_VINYL, "Can not fill bit list");
        }

        #ifdef TRACE_POSITION
            this->display_bit_list();
        #endif
    }

    //
    // delete total_input_samples because all datas were analyzed
    //
    this->delete_total_input_samples();
}

bool Coded_vinyl::check_extremes_level()
{
    int i          = 0;
    int counter    = 0;
    int comparator = 0;
    extreme_t *extreme      = NULL;
    extreme_t *next_extreme = NULL;

    if ((int)this->left_channel_extreme_list.size() <= 2)
    {
        this->set_no_input_signal(true);
        return true;
    }

    // For each extremes check the amplitude interval with next extreme.
    while (i <= ((int)this->left_channel_extreme_list.size() - 2))
    {
        if(this->extreme_list_get_element(LEFT_CHANNEL, i, &extreme) == false)
        {
            Utils::trace_error(TRACE_PREFIX_CODED_VINYL,
                               "Can not get extreme element, program fail");

            return false;
        }
        if(this->extreme_list_get_element(LEFT_CHANNEL, i+1, &next_extreme) == false)
        {
            Utils::trace_error(TRACE_PREFIX_CODED_VINYL,
                               "Can not get extreme element, program fail");

            return false;
        }
        if (fabs(extreme->value - next_extreme->value) < this->extreme_min)
        {
            // If interval beetween these 2 extremes is to small:
            // there is not enough signal to detect something, increment a counter.
            counter++;
        }
        // Go to next extreme in the list.
        i++;
    }

    // Analyze counter: if it is more than extreme_list_size/3 we will consider
    // that there is not enough signal for the complete extreme list.
    comparator = (int)(this->left_channel_extreme_list.size()/3);
    if (comparator == 0) comparator = 1;
    if (counter >= comparator)
    {
        this->set_no_input_signal(true);
    }
    else
    {
        this->set_no_input_signal(false);
    }

    return true;
}

void Coded_vinyl::delete_total_input_samples()
{
    this->total_input_samples_1.clear();
    this->total_input_samples_2.clear();

    return;
}

void Coded_vinyl::add_new_input_samples(vector<float> &input_samples_1,
                                        vector<float> &input_samples_2)
{
    // Check if total_input_samples tables are large enough.
    if (this->total_input_samples_1.capacity()
        < (input_samples_1.size() * this->get_max_buffer_coeff()))
    {
        this->total_input_samples_1.reserve(input_samples_1.size() * this->get_max_buffer_coeff());
        this->total_input_samples_2.reserve(input_samples_2.size() * this->get_max_buffer_coeff());
    }

    // Add new input samples at the end of total samples tables.
    copy(input_samples_1.begin(), input_samples_1.end(), back_inserter(this->total_input_samples_1));
    copy(input_samples_2.begin(), input_samples_2.end(), back_inserter(this->total_input_samples_2));

    return;
}

void Coded_vinyl::channels_extreme_list_fill(vector<float> &input_samples_1,
                                             vector<float> &input_samples_2)
{
    // remove old extremes from left and right lists
    this->remove_old_extremes_from_lists();

    // prepare new extreme list and fill it
    this->prepare_and_fill_extreme_lists(input_samples_1,
                                         input_samples_2);
}

void Coded_vinyl::remove_old_extremes_from_lists()
{
    extreme_t *last_extreme = NULL;
    extreme_t *before_last_extreme = NULL;

    //
    // Left channel
    //
    if (this->left_channel_extreme_list.size() > 1)
    {
        Utils::trace_analyze_extreme(TRACE_PREFIX_CODED_VINYL, "Removing old extremes on left channel extreme list...");

        if(this->extreme_list_get_element(LEFT_CHANNEL, this->left_channel_extreme_list.size() - 1, &last_extreme) == false)
        {
            Utils::trace_error(TRACE_PREFIX_CODED_VINYL, "Can not get extreme element, program fail");

            return;
        }
        if(this->extreme_list_get_element(LEFT_CHANNEL, this->left_channel_extreme_list.size() - 2, &before_last_extreme) == false)
        {
            Utils::trace_error(TRACE_PREFIX_CODED_VINYL, "Can not get extreme element, program fail");

            return;
        }
        if ((last_extreme->type == MAX_SEARCH || last_extreme->type == MIN_SEARCH)
            && (before_last_extreme->type == MAX || before_last_extreme->type == MIN))
        {
            // delete extreme (in extreme list) until before_last_extreme
            Utils::trace_analyze_extreme(TRACE_PREFIX_CODED_VINYL, "Cleaning left channel extreme list (except the 2 last elements)");

            this->extreme_list_multiple_pop_front(LEFT_CHANNEL, this->left_channel_extreme_list.size() - 2);
        }
        else
        {
            if (last_extreme->type == MAX || last_extreme->type == MIN)
            {
                // delete extreme (in extreme list) until last_extreme
                Utils::trace_analyze_extreme(TRACE_PREFIX_CODED_VINYL, "Cleaning left channel extreme list (except the latest element)");

                this->extreme_list_multiple_pop_front(LEFT_CHANNEL, this->left_channel_extreme_list.size() - 1);
            }
        }

        Utils::trace_analyze_extreme(TRACE_PREFIX_CODED_VINYL, "Old extremes on left channel extreme list removed");

        #ifdef TRACE_ANALYZE_EXTREME
            this->extreme_list_display(LEFT_CHANNEL);
        #endif
    }

    //
    // Right channel
    //
    last_extreme = NULL;
    before_last_extreme = NULL;

    if (this->right_channel_extreme_list.size() > 1)
    {
        Utils::trace_analyze_extreme(TRACE_PREFIX_CODED_VINYL, "Removing old extremes on right channel extreme list...");

        if(this->extreme_list_get_element(RIGHT_CHANNEL, this->right_channel_extreme_list.size() - 1, &last_extreme) == false)
        {
            Utils::trace_error(TRACE_PREFIX_CODED_VINYL, "Can not get extreme element, program fail");

            return;
        }
        if(this->extreme_list_get_element(RIGHT_CHANNEL, this->right_channel_extreme_list.size() - 2, &before_last_extreme) == false)
        {
            Utils::trace_error(TRACE_PREFIX_CODED_VINYL, "Can not get extreme element, program fail");

            return;
        }
        if ((last_extreme->type == MAX_SEARCH || last_extreme->type == MIN_SEARCH)
            && (before_last_extreme->type == MAX || before_last_extreme->type == MIN))
        {
            // delete extreme (in extreme list) until before_last_extreme
            Utils::trace_analyze_extreme(TRACE_PREFIX_CODED_VINYL, "Cleaning left channel extreme list (except the 2 last elements)");

            this->extreme_list_multiple_pop_front(RIGHT_CHANNEL, this->right_channel_extreme_list.size() - 2);
        }
        else
        {
            if (last_extreme->type == MAX || last_extreme->type == MIN)
            {
                // delete extreme (in extreme list) until last_extreme
                Utils::trace_analyze_extreme(TRACE_PREFIX_CODED_VINYL, "Cleaning left channel extreme list (except the latest element)");

                this->extreme_list_multiple_pop_front(RIGHT_CHANNEL, this->right_channel_extreme_list.size() - 1);
            }
        }

        Utils::trace_analyze_extreme(TRACE_PREFIX_CODED_VINYL, "Old extremes on right channel extreme list removed");

        #ifdef TRACE_ANALYZE_EXTREME
            this->extreme_list_display(RIGHT_CHANNEL);
        #endif
    }
}

void Coded_vinyl::prepare_and_fill_extreme_lists(vector<float> &input_samples_1,
                                                         vector<float> &input_samples_2)
{
    extreme_t *last_extreme = NULL;
    extreme_t *before_last_extreme = NULL;


    //
    // prepare new left extreme list
    //
    short int offset_left = 0;
    float last_extreme_value_left = 0.0;
    char state_left = UNDEFINE;  // this is the current extreme state

    if (this->extreme_list_get_size(LEFT_CHANNEL) == 1) // extreme type is X_SEARCH
    {
        if(this->extreme_list_get_element(LEFT_CHANNEL, this->extreme_list_get_size(LEFT_CHANNEL) - 1, &last_extreme) == false)
        {
            Utils::trace_error(TRACE_PREFIX_CODED_VINYL, "Can not get extreme element, program fail");

            return;
        }
        offset_left = 0;
        state_left = last_extreme->type;
        last_extreme_value_left = last_extreme->value;

        // delete last extreme
        this->extreme_list_clean(LEFT_CHANNEL);
    }
    if (this->extreme_list_get_size(LEFT_CHANNEL) == 2) // last extreme type is X_SEARCH and before there is a MIN or MAX
    {
        if(this->extreme_list_get_element(LEFT_CHANNEL, this->extreme_list_get_size(LEFT_CHANNEL) - 1, &last_extreme) == false)
        {
            Utils::trace_error(TRACE_PREFIX_CODED_VINYL, "Can not get extreme element, program fail");

            return;
        }
        if(this->extreme_list_get_element(LEFT_CHANNEL, this->extreme_list_get_size(LEFT_CHANNEL) - 2, &before_last_extreme) == false)
        {
            Utils::trace_error(TRACE_PREFIX_CODED_VINYL, "Can not get extreme element, program fail");

            return;
        }
        offset_left = last_extreme->index - before_last_extreme->index + 1;
        state_left = last_extreme->type;
        last_extreme_value_left = last_extreme->value;

        // change index of MIN or MAX extreme
        before_last_extreme->index = 0;

        // delete last extreme
        this->extreme_list_pop_back(LEFT_CHANNEL);
    }

    #ifdef TRACE_ANALYZE_EXTREME
        Utils::trace_analyze_extreme(TRACE_PREFIX_CODED_VINYL, "Left channel offset : " + Utils::to_string(offset_left));
    #endif


    //
    // prepare new right extreme list
    //
    short int offset_right = 0;
    float last_extreme_value_right = 0.0;
    char state_right = UNDEFINE;  // this is the current extreme state

    if (this->extreme_list_get_size(RIGHT_CHANNEL) == 1) // extreme type is X_SEARCH
    {
        if(this->extreme_list_get_element(RIGHT_CHANNEL, this->extreme_list_get_size(RIGHT_CHANNEL) - 1, &last_extreme) == false)
        {
            Utils::trace_error(TRACE_PREFIX_CODED_VINYL, "Can not get extreme element, program fail");

            return;
        }
        offset_right = 0;
        state_right = last_extreme->type;
        last_extreme_value_right = last_extreme->value;

        // delete last extreme
        this->extreme_list_clean(RIGHT_CHANNEL);
    }
    if (this->extreme_list_get_size(RIGHT_CHANNEL) == 2) // last extreme type is X_SEARCH and before there is a MIN or MAX
    {
        if(this->extreme_list_get_element(RIGHT_CHANNEL, this->extreme_list_get_size(RIGHT_CHANNEL) - 1, &last_extreme) == false)
        {
            Utils::trace_error(TRACE_PREFIX_CODED_VINYL, "Can not get extreme element, program fail");

            return;
        }
        if(this->extreme_list_get_element(RIGHT_CHANNEL, this->extreme_list_get_size(RIGHT_CHANNEL) - 2, &before_last_extreme) == false)
        {
            Utils::trace_error(TRACE_PREFIX_CODED_VINYL, "Can not get extreme element, program fail");

            return;
        }
        offset_right = last_extreme->index - before_last_extreme->index + 1;
        state_right = last_extreme->type;
        last_extreme_value_right = last_extreme->value;

        // change index of MIN or MAX extreme
        //before_last_extreme->index = 0;

        // delete last extreme
        this->extreme_list_pop_back(RIGHT_CHANNEL);
    }

    #ifdef TRACE_ANALYZE_EXTREME
        Utils::trace_analyze_extreme(TRACE_PREFIX_CODED_VINYL, "Right channel offset : " + Utils::to_string(offset_right));
    #endif



    //
    // fill extreme list
    //
    this->fill_extreme_list(LEFT_CHANNEL,
                            input_samples_1,
                            offset_left,
                            offset_right,
                            state_left,
                            last_extreme_value_left);
    this->fill_extreme_list(RIGHT_CHANNEL,
                            input_samples_2,
                            offset_right,
                            offset_left,
                            state_right,
                            last_extreme_value_right);
}

void Coded_vinyl::fill_extreme_list(char           left_or_right,
                                            vector<float> &input_samples,
                                            short int      offset_mine,
                                            short int      offset_other,
                                            char           state,
                                            float          last_extreme_value)
{
    float      min       = 0;
    float      max       = 0;
    short int  offset    = 0;
    int        nb_frames = input_samples.size();
    extreme_t *extreme   = NULL;

    // search the offset
    if (this->extreme_list_get_size(left_or_right) > 0)
    {
        if(this->extreme_list_get_element(left_or_right, 0, &extreme) == false)
        {
            Utils::trace_error(TRACE_PREFIX_CODED_VINYL, "Can not get extreme element, program fail");

            return;
        }
        if (offset_mine >= offset_other)
        {
            extreme->index = 0;
            offset = offset_mine;
        }
        else
        {
            extreme->index = offset_other - offset_mine;
            offset = offset_other;
        }
    }

    for (int i = 1; i < nb_frames; i++)
    {
        switch(state)
        {
            case UNDEFINE: // state = UNDEFINE
                if (input_samples[i] == input_samples[i-1])
                {
                    state = UNDEFINE;
                }
                if (input_samples[i] > input_samples[i-1])
                {
                    state = MAX;
                    max = input_samples[i];
                }
                if (input_samples[i] < input_samples[i-1])
                {
                    state = MIN;
                    min = input_samples[i];
                }
                break;

            case MAX: // state = MAX
                if (input_samples[i] >= max)
                {
                    max = input_samples[i];
                }
                else // input_samples_1[i] < max (new max found)
                {
                    // create new Extrem object (max type)
                    #ifdef TRACE_ANALYZE_EXTREME
                        Utils::trace_analyze_extreme(TRACE_PREFIX_CODED_VINYL, "New MAX found (index:" \
                                                     + Utils::to_string(i-1) + ",value:" \
                                                     + Utils::to_string(max) + ")");
                    #endif

                    extreme_t extreme;
                    extreme.index = offset+i-1;
                    extreme.value = max;
                    extreme.bit_value = BIT_UNDEFINED;
                    extreme.type = MAX;
                    this->extreme_list_push_back(left_or_right, &extreme);

                    state = MIN;
                    min = input_samples[i];
                }
                break;

            case MIN: // state = MIN
                if (input_samples[i] <= min)
                {
                    min = input_samples[i];
                }
                else // input_samples_1[i] > min (new min found)
                {
                    // create new Extrem object (min type)
                    #ifdef TRACE_ANALYZE_EXTREME
                        Utils::trace_analyze_extreme(TRACE_PREFIX_CODED_VINYL, "New MIN found (index:" \
                                                     + Utils::to_string(i-1) + ",value:" \
                                                     + Utils::to_string(min) + ")");
                    #endif

                    extreme_t extreme;
                    extreme.index = offset+i-1;
                    extreme.value = min;
                    extreme.bit_value = BIT_UNDEFINED;
                    extreme.type = MIN;
                    this->extreme_list_push_back(left_or_right, &extreme);

                    state = MAX;
                    max = input_samples[i];
                }
                break;

            case MAX_SEARCH: // state = MAX_SEARCH
                if (input_samples[i] >= last_extreme_value)
                {
                    max = input_samples[i];
                    state = MAX;
                }
                else // input_samples[i] < max (new max found)
                {
                    // create new extrem (max type)
                    #ifdef TRACE_ANALYZE_EXTREME
                        Utils::trace_analyze_extreme(TRACE_PREFIX_CODED_VINYL, "New MAX found (index:" \
                                                     + Utils::to_string("-x") + ",value:" \
                                                     + Utils::to_string(last_extreme_value) + ")");
                    #endif

                    extreme_t extreme;
                    extreme.index = offset;
                    extreme.value = last_extreme_value;
                    extreme.bit_value = BIT_UNDEFINED;
                    extreme.type = MAX;
                    this->extreme_list_push_back(left_or_right, &extreme);

                    state = MIN;
                    min = input_samples[i];
                }
                break;

            case MIN_SEARCH: // state = MIN_SEARCH
                if (input_samples[i] <= last_extreme_value)
                {
                    min = input_samples[i];
                    state = MIN;
                }
                else // input_samples[i] > min (new max found)
                {
                    // create new Extrem object (max type)
                    #ifdef TRACE_ANALYZE_EXTREME
                        Utils::trace_analyze_extreme(TRACE_PREFIX_CODED_VINYL, "New MIN found (index:" \
                                                     + Utils::to_string("-x") + ",value:" \
                                                     + Utils::to_string(last_extreme_value) + ")");
                    #endif

                    extreme_t extreme;
                    extreme.index = offset;
                    extreme.value = last_extreme_value;
                    extreme.bit_value = BIT_UNDEFINED;
                    extreme.type = MIN;
                    this->extreme_list_push_back(left_or_right, &extreme);

                    state = MAX;
                    max = input_samples[i];
                }
                break;

            default:
                Utils::trace_error(TRACE_PREFIX_CODED_VINYL, "No current extrem state");
                break;
        }

        // we are analyzing the last sample
        if (i == (nb_frames-1))
        {
            if (state == MAX)
            {
                // create new extreme (max_search type)
                #ifdef TRACE_ANALYZE_EXTREME
                    Utils::trace_analyze_extreme(TRACE_PREFIX_CODED_VINYL, "New MAX_SEARCH found (index:" \
                                                 + Utils::to_string(i) + ",value:" \
                                                 + Utils::to_string(max) + ")");
                #endif

                extreme_t extreme;
                extreme.index = offset + i;
                extreme.value = max;
                extreme.bit_value = BIT_UNDEFINED;
                extreme.type = MAX_SEARCH;
                this->extreme_list_push_back(left_or_right, &extreme);
            }
            if (state == MIN)
            {
                // create new Extrem object (min_search type)
                #ifdef TRACE_ANALYZE_EXTREME
                    Utils::trace_analyze_extreme(TRACE_PREFIX_CODED_VINYL, "New MIN_SEARCH found (index:" \
                                                 + Utils::to_string(i) + ",value:" \
                                                 + Utils::to_string(min) + ")");
                #endif

                extreme_t extreme;
                extreme.index = offset + i;
                extreme.value = min;
                extreme.bit_value = BIT_UNDEFINED;
                extreme.type = MIN_SEARCH;
                this->extreme_list_push_back(left_or_right, &extreme);
            }
        }
    }

    // display channel extreme list
    #ifdef TRACE_ANALYZE_EXTREME
        this->extreme_list_display(left_or_right);
    #endif
}

float Coded_vinyl::get_volume()
{
    Utils::trace_analyze_vinyl(TRACE_PREFIX_CODED_VINYL, "Searching new volume...");

    // TODO: remove this code and replace it by progressive algo implemented in player (based on current speed).
#if 0 // Seems to be too complex. return 1.0 and make a cut in player.
    // Get real volume value.
    float new_volume = this->get_real_volume();

    if ((new_volume != NO_NEW_VOLUME_FOUND)
        && (this->old_volume != NO_NEW_VOLUME_FOUND)
        && (new_volume != 0.0))
    {
        // Get volume using a progressive algorithm.
        new_volume = this->get_smoothed_volume(new_volume);
    }

    // Do not give volume under 0.01
    if (new_volume <= 0.01)
    {
        new_volume = 0.0;
    }

    // Store new volume value.
    this->old_volume = new_volume;

    // Return volume.
    return new_volume;
#else
    return 1.0;
#endif
}

float Coded_vinyl::get_real_volume()
{
    extreme_t* extreme = NULL;
    float      average = 0.0;
    float      volume  = 0.0;
    short int  nb      = 0;
    int        size    = this->extreme_list_get_size(RIGHT_CHANNEL);

    // Do not try to get volume if there are no data to analyze.
    if (size <= 0)
    {
        return 0.0;
    }

    // Get the average value of MAXs and MINs from right (arbitrary) channel.
    for (int i = 0; i < size; i++)
    {
        this->extreme_list_get_element(RIGHT_CHANNEL, i, &extreme);
        if (extreme->type == MIN || extreme->type == MAX)
        {
            average += fabs(extreme->value);
            nb++;
        }
    }

    // Return 0.0 if average equal 0.0 to avoid division per 0.
    if (average == 0.0)
    {
        return 0.0;
    }

    // Calculate the average value of all extremes.
    average = average / (float)nb;

    // If volume is more than 100%, cut it at 100%.
    volume = average / this->full_volume_amplitude;
    if (volume > 1.0)
    {
        volume = 1.0;
    }

    // Return volume.
    return volume;
}

float Coded_vinyl::get_smoothed_volume(float new_volume)
{
    float interval     = 0.0;
    float abs_interval = 0.0;

    // Get interval between old and new volume value.
    interval     = (new_volume - this->old_volume);
    abs_interval = fabs(interval);
    abs_interval *= 100.0;

    // Reduce interval according to its value.
    if (abs_interval >  0.0  && abs_interval < 1.0)  interval /= (float)(this->progressive_volume_coeff/2.0);
    if (abs_interval >= 1.0  && abs_interval < 2.0)  interval /= (float)(this->progressive_volume_coeff/1.0);
    if (abs_interval >= 2.0  && abs_interval < 5.0)  interval /= (float)(this->progressive_volume_coeff/3.0);
    if (abs_interval >= 5.0  && abs_interval < 10.0) interval /= (float)(this->progressive_volume_coeff/5.0);
    if (abs_interval >= 10.0 && abs_interval < 20.0) interval /= (float)(this->progressive_volume_coeff/10.0);
    if (abs_interval >= 20.0 && abs_interval < 30.0) interval /= (float)(this->progressive_volume_coeff/30.0);
    if (abs_interval >= 30.0 && abs_interval < 50.0) interval /= (float)(this->progressive_volume_coeff/50.0);

    // Get old volume plus reduced interval.
    new_volume = this->old_volume + interval;

    // Return new volume.
    return new_volume;
}

float Coded_vinyl::get_speed()
{
    Utils::trace_analyze_vinyl(TRACE_PREFIX_CODED_VINYL, "Searching new speed...");

    // display channel extreme list
    #ifdef TRACE_EXTREME_USED_FOR_DETECTING_SPEED
        this->extreme_list_display(RIGHT_CHANNEL);
        this->extreme_list_display(LEFT_CHANNEL);
    #endif

    // if there is no input signal, speed = 0
    if (this->is_no_input_signal() == true)
    {
        this->current_speed = 0.0;
        this->latest_available_detected_speed = 0.0;

        return this->current_speed;
    }

    float old_speed = this->current_speed;

    // search speed on each channel
    float speed_left  = this->get_independant_channel_speed(LEFT_CHANNEL);
    float speed_right = this->get_independant_channel_speed(RIGHT_CHANNEL);

    // direction undefined if no speed is detected
    if (speed_left == 0 && speed_right == 0)
    {
        this->direction = 0;
    }

    // get speed
    if (speed_left != NO_NEW_SPEED_FOUND && speed_right != NO_NEW_SPEED_FOUND
        && speed_left != 0 && speed_right != 0)
    {
        // get the average speed (between the 2 channels)
        this->current_speed = (float)((speed_left + speed_right) / 2.0);

        // get direction
        short int new_direction = this->get_direction();
        if (new_direction != 0) // a new direction was found
        {
            if (old_speed == NO_NEW_SPEED_FOUND)
            {
                Utils::trace_direction(TRACE_PREFIX_CODED_VINYL, "Vinyl is starting, use the direction found");

                this->direction = new_direction;
                this->current_speed *= (float)this->direction;
            }
            else
            {
                if (this->direction != new_direction) // direction is changing, don't change it for the moment
                {
                    this->nb_same_direction_change++;
                    if (this->direction_is_changing == true) // direction is changing
                    {
                        //this->nb_same_direction_change++;
                        if (this->nb_same_direction_change >= this->nb_cycle_before_changing_direction)
                        {
                            // ok, i'm sure the new direction is really a new direction, so change it
                            Utils::trace_direction(TRACE_PREFIX_CODED_VINYL, "Ok, direction has really changed");

                            this->direction_is_changing = false;
                            this->nb_same_direction_change = 0;
                            this->direction = new_direction;
                            this->current_speed *= (float)this->direction;
                        }
                        else
                        {
                            Utils::trace_direction(TRACE_PREFIX_CODED_VINYL, "Want to have more directions to be sure \
                                                                          there is a really direction change");

                            this->current_speed *= (float)this->direction;
                        }
                    }
                    else // now direction is changing
                    {
                        this->direction_is_changing = true;
                        //this->nb_same_direction_change++;
                        Utils::trace_direction(TRACE_PREFIX_CODED_VINYL, "Direction change, will test next directions");

                        this->current_speed *= (float)this->direction;
                    }
                }
                else // same direction as previous
                {
                    if (this->direction_is_changing == true) // we were testing a direction change
                    {
                        // it was a bad direction, so don't change direction
                        this->direction_is_changing = false;

                        Utils::trace_direction(TRACE_PREFIX_CODED_VINYL, "Previous found direction was bad, so don't change direction");
                    }
                    this->direction = new_direction;
                    this->current_speed *= (float)this->direction;
                }
            }
        }
        else // no new direction found, use the old direction
        {
            this->current_speed *= (float)this->direction;
        }
    }
    else
    {
        this->current_speed = NO_NEW_SPEED_FOUND;
    }

    #ifdef TRACE_ANALYZE_VINYL
        if (this->current_speed == NO_NEW_SPEED_FOUND)
        {
            Utils::trace_analyze_vinyl(TRACE_PREFIX_CODED_VINYL, "New speed : NO_NEW_SPEED_FOUND");
        }
        else
        {
            Utils::trace_analyze_vinyl(TRACE_PREFIX_CODED_VINYL, "New speed : " \
                                       + Utils::to_string(this->current_speed * 100.0) + "%");
        }
    #endif

#ifdef ENABLE_SPEED_ERRORS_WHEN_SLOW_SPEED
    if ((this->latest_available_detected_speed == 0
        || this->latest_available_detected_speed == NO_NEW_SPEED_FOUND)
        && (this->current_speed > 3.0))
    {
        this->current_speed = 0.0;
    }
#endif

    // Store speed to use in low_pass_filter()
    if (this->current_speed != NO_NEW_SPEED_FOUND)
    {
        this->latest_available_detected_speed = this->current_speed;
    }

    return this->current_speed;
}

float Coded_vinyl::get_independant_channel_speed(char left_or_right)
{
    float speed_outpout = 0.0;

    if (this->extreme_list_get_size(left_or_right) > 2)
    {
        //
        // Search all intervals between 2 extremes
        //

        extreme_t *current_extreme = NULL;
        extreme_t *before_current_extreme = NULL;
        int i = 1;
        int intervals_sum = 0;
        short int intervals_number = 0;

        this->extreme_list_get_element(left_or_right, i, &current_extreme);
        while (i < (this->extreme_list_get_size(left_or_right) - 1)
                && current_extreme->type != MIN_SEARCH && current_extreme->type != MAX_SEARCH)
        {
            this->extreme_list_get_element(left_or_right, i-1, &before_current_extreme);
            intervals_sum += (current_extreme->index - before_current_extreme->index);
            intervals_number++;

            i++;
            this->extreme_list_get_element(left_or_right, i, &current_extreme);
        }

        //
        // Calculate speed
        //

        float average_intervals = (float)intervals_sum / (float)intervals_number;

        #ifdef TRACE_OBJECT_LIFE
            Utils::trace_analyze_vinyl(TRACE_PREFIX_CODED_VINYL, "  Intervals sum : " \
                                       + Utils::to_string(intervals_sum) + ", intervals number : " \
                                       + Utils::to_string(intervals_number) + ", average : " \
                                       + Utils::to_string(average_intervals));
        #endif

        float theoric_average_nb_complete_sample = (float)this->get_sample_rate() / (float)(this->get_sinusoidal_frequency() * 2);
        speed_outpout = theoric_average_nb_complete_sample / average_intervals;

        #ifdef TRACE_ANALYZE_VINYL
            if (left_or_right == LEFT_CHANNEL)
            {
                Utils::trace_analyze_vinyl(TRACE_PREFIX_CODED_VINYL, "New right speed found : " \
                                           + Utils::to_string(speed_outpout * 100.0) + "%");
            }
            else
            {
                Utils::trace_analyze_vinyl(TRACE_PREFIX_CODED_VINYL, "New left speed found : " \
                                           + Utils::to_string(speed_outpout * 100.0) + "%");
            }
        #endif
    }
    else // not enough extremes value to calculate new speed value
    {
        speed_outpout = NO_NEW_SPEED_FOUND;

        #ifdef TRACE_ANALYZE_VINYL
            if (left_or_right == LEFT_CHANNEL)
            {
                Utils::trace_analyze_vinyl(TRACE_PREFIX_CODED_VINYL, "No new left speed found");
            }
            else
            {
                Utils::trace_analyze_vinyl(TRACE_PREFIX_CODED_VINYL, "No new right speed found");
            }
        #endif
    }

    return speed_outpout;
}

bool Coded_vinyl::is_enough_amplitude(char left_or_right)
{
    extreme_t *extreme = NULL;
    float      average = 0.0;
    short int  nb      = 0;

    //
    // BAD vinyl signal => impossible to determine a new speed value because there
    // is probably no signal input.
    //
    // Algorithm : search the MAX and MIN average and compare it to a reference
    //                value : extreme_min.
    //

    for (int i = 0; i < this->extreme_list_get_size(left_or_right); i++)
    {
        this->extreme_list_get_element(left_or_right, i, &extreme);
        if (extreme->type == MIN || extreme->type == MAX)
        {
            average += fabs(extreme->value);
            nb++;
        }
    }
    // calculate the average value of all extremes
    average = average / (float)nb;

    // compare the average value with the reference value
//cout << "average = " << average << endl;
    if (average < this->extreme_min)
    {
        #ifdef TRACE_ANALYZE_VINYL
            if (left_or_right == LEFT_CHANNEL)
            {
                Utils::trace_analyze_vinyl(TRACE_PREFIX_CODED_VINYL, "Bad signal, left speed = 0");
            }
            else
            {
                Utils::trace_analyze_vinyl(TRACE_PREFIX_CODED_VINYL, "Bad signal, right speed = 0");
            }
        #endif

        return false;
    }
    else
    {
        return true;
    }
}

short int Coded_vinyl::get_direction()
{
    extreme_t * first_extreme_right  = NULL;
    extreme_t * current_extreme_left = NULL;

    if (this->extreme_list_get_size(RIGHT_CHANNEL) > 0 && this->extreme_list_get_size(LEFT_CHANNEL) > 0 )
    {
        // get type of first extreme of right channel
        this->extreme_list_get_element(RIGHT_CHANNEL, 0, &first_extreme_right);

        // search the first extreme of left channel which has an index higher than
        // index of first extreme of right channel
        for (int i = 0; i < this->extreme_list_get_size(LEFT_CHANNEL); i++)
        {
            this->extreme_list_get_element(LEFT_CHANNEL, i, &current_extreme_left);
            if (current_extreme_left->index > first_extreme_right->index)
            {
                if (current_extreme_left->type == first_extreme_right->type)
                {
                    Utils::trace_direction(TRACE_PREFIX_CODED_VINYL, "New direction found : forward");

                    if (this->get_reverse_direction() == false)
                    {
                        return 1;
                    }
                    else
                    {
                        return -1;
                    }
                }
                else
                {
                    Utils::trace_direction(TRACE_PREFIX_CODED_VINYL, "New direction found : backward");

                    if (this->get_reverse_direction() == false)
                    {
                        return -1;
                    }
                    else
                    {
                        return 1;
                    }
                }
            }
        }
    }

    Utils::trace_direction(TRACE_PREFIX_CODED_VINYL, "Can't find new direction");

    return 0;
}

bool Coded_vinyl::set_reverse_direction(bool is_reverse_direction)
{
    this->is_reverse_direction = is_reverse_direction;

    return true;
}

bool Coded_vinyl::get_reverse_direction()
{
    return this->is_reverse_direction;
}

bool Coded_vinyl::get_bit_types()
{
    unsigned int i = 0;

    // find reference values use to set bit types
    if (this->set_bit_reference_values() == false)
    {
        Utils::trace_error(TRACE_PREFIX_CODED_VINYL, "Can not set bit references");

        return false;
    }

    // parse bit_list (that contains more than 32 amplitudes) and find all
    // corresponding bit types.
    while (i < this->bit_list.size() - 2)
    {
        // get the amplitude distance type beetween 2 extremes (right channel)
        this->get_type_of_2_bits(RIGHT_CHANNEL,
                                 this->bit_list[i].amplitude,
                                 this->bit_list[i+2].amplitude,
                                 &this->bit_list[i].value,
                                 &this->bit_list[i+2].value);

        // go to next pair of elements
        i = i + 2;
    }

    i = 1;
    while (i < this->bit_list.size() - 2)
    {
        // get the amplitude distance type beetween 2 extremes (left channel)
        this->get_type_of_2_bits(LEFT_CHANNEL,
                                 this->bit_list[i].amplitude,
                                 this->bit_list[i+2].amplitude,
                                 &this->bit_list[i].value,
                                 &this->bit_list[i+2].value);

        // go to next pair of elements
        i = i + 2;
    }

    return true;
}

bool Coded_vinyl::set_bit_reference_values()
{
    // Algorithm: Parse bit_list (only right channel, e.g. pair indexes) and
    //            select the higher interval between 2 consecutive amplitudes,
    //            it should be a pair "bit1 to bit1". Based on this value and by
    //            using hardcoded percentages set values for bit1-bit1 and
    //            bit0-bit0 for each channels.

    float highest_interval = 0.0;

    // Get higher interval form bit_list on right channel
    highest_interval = this->get_higher_interval_from_bit_list();
    if(highest_interval == 0.0)
    {
        Utils::trace_error(TRACE_PREFIX_CODED_VINYL, "Can not get higher amplitude \
                                                  interval for right channel");

        return false;
    }

    // Change bit reference values for left and right channel
    this->right_dist_min_bit1_to_bit1 =
        highest_interval * this->coeff_right_dist_min_bit1_to_bit1;
    this->right_dist_max_bit0_to_bit0 =
        highest_interval * this->coeff_right_dist_max_bit0_to_bit0;
    this->left_dist_min_bit1_to_bit1 =
        highest_interval * this->coeff_left_dist_min_bit1_to_bit1;
    this->left_dist_max_bit0_to_bit0 =
        highest_interval * this->coeff_left_dist_max_bit0_to_bit0;

    return true;
}

float Coded_vinyl::get_higher_interval_from_bit_list()
{
    unsigned int i                = 0;
    float        interval         = 0.0;
    float        highest_interval = 0.0;

    // Get higher interval form bit_list on right channel
    while (i < this->bit_list.size() - 2)
    {
        // Store amplitude interval for the current pair of elements if it is
        // the highest.
        interval = fabs(this->bit_list[i].amplitude - this->bit_list[i+2].amplitude);
        if (interval > highest_interval)
        {
            highest_interval = interval;
        }

        // Go to the next pair of elements.
        i = i + 2;
    }

    return highest_interval;
}

bool Coded_vinyl::get_type_of_2_bits(char  left_or_right,
                                             float amplitude_1,
                                             float amplitude_2,
                                             char *bit_type_1,
                                             char *bit_type_2)
{
    float distance = fabs(amplitude_1) + fabs(amplitude_2);
    float dist_bit_0_to_0_max = 0.0;
    float dist_bit_1_to_1_min = 0.0;

    if (left_or_right == RIGHT_CHANNEL)
    {
        dist_bit_0_to_0_max = this->right_dist_max_bit0_to_bit0;
        dist_bit_1_to_1_min = this->right_dist_min_bit1_to_bit1;
    }
    else
    {
        dist_bit_0_to_0_max = this->left_dist_max_bit0_to_bit0;
        dist_bit_1_to_1_min = this->left_dist_min_bit1_to_bit1;
    }

    // if distance is close to hardcoded value for distance between bit 0
    // and another bit 0, then return 0 (correspond to [dist 0-0])
    if (distance <= dist_bit_0_to_0_max)
    {
        if (*bit_type_1 == BIT_UNDEFINED) // first bit not defined
        {
            *bit_type_1 = BIT_0;
        }
        *bit_type_2 = BIT_0;

        return true;
    }

    // if distance is close to hardcoded value for distance between bit 1
    // and another bit 1, then return 3 (correspond to [dist 1-1])
    if (distance >= dist_bit_1_to_1_min)
    {
        if (*bit_type_1 == BIT_UNDEFINED) // first bit not defined
        {
            *bit_type_1 = BIT_1;
        }
        *bit_type_2 = BIT_1;

        return true;
    }

    // if distance is close to hardcoded value for distance between bit 0
    // and another bit 1, then return 1 (correspond to [dist 0-1]) or return 2 (correspond to [dist 1-0])
    if (*bit_type_1 == BIT_UNDEFINED) // first bit not defined
    {
        if (fabs(amplitude_1) <= fabs(amplitude_2))
        {
            *bit_type_1 = BIT_0;
            *bit_type_2 = BIT_1;
        }
        else
        {
            *bit_type_1 = BIT_1;
            *bit_type_2 = BIT_0;
        }
        return true;
    }
    else // first bit was defined, do not change it
    {
        if (*bit_type_1 == BIT_0)
        {
            *bit_type_2 = BIT_1;
        }
        else
        {
            *bit_type_2 = BIT_0;
        }
        return true;
    }

    return false;
}

bool Coded_vinyl::fill_bit_list()
{
    Utils::trace_position(TRACE_PREFIX_CODED_VINYL, "Fill bit list with new extreme bit values");

    //
    // if size of left channel extreme list is different than right channel
    // extreme list (+/- 1) we can not detect timecode, so clean bit_list
    //
    int diff_size = this->right_channel_extreme_list.size() - this->left_channel_extreme_list.size();
    if (diff_size != 0 && diff_size != -1 && diff_size != 1)
    {
        Utils::trace_position(TRACE_PREFIX_CODED_VINYL, "Left and right extreme list has not same size (+/-1), can not get timecode, clean bit list");

        this->bit_list.clear();
        this->latest_was_right = false;

        return false;
    }

    //
    // add new bit types at the end of bit list
    //
    unsigned int i = 0;
    unsigned int j = 0;

    // parse all extreme lists and set bit type for each extremes
    while ((i < this->right_channel_extreme_list.size()-2) || (i < this->left_channel_extreme_list.size()-2))
    {
        // get extreme element, find bit value and add it to bit_list
        for (j = 0; j < 2; j++)
        {
            if (this->latest_was_right == true)
            {
                this->add_value_to_bit_list_from_extreme_list(LEFT_CHANNEL, i);
            }
            else
            {
                this->add_value_to_bit_list_from_extreme_list(RIGHT_CHANNEL, i);
            }
        }

        // go to next element in extreme list
        i++;
    }

    return true;
}

void Coded_vinyl::add_value_to_bit_list_from_extreme_list(char left_or_right, int index)
{
    extreme_t *extreme = NULL;

    if (index < this->extreme_list_get_size(left_or_right)-2)
    {
        if (left_or_right == RIGHT_CHANNEL)
        {
            this->latest_was_right = true;
        }
        else
        {
            this->latest_was_right = false;
        }

        // Get extreme element
        this->extreme_list_get_element(left_or_right, index, &extreme);

        // Create a new bit_t element and fill it with amplitude value.
        bit_t bit;
        bit.amplitude = extreme->value;
        bit.value     = BIT_UNDEFINED;
        this->bit_list.push_back(bit);
    }
}

void Coded_vinyl::display_bit_list()
{
    unsigned int i;
    string string_to_display = "";

    for (i = 0; i < this->bit_list.size(); i++)
    {
        string_to_display += Utils::to_string(this->bit_list[i].amplitude);
        if (this->bit_list[i].value == BIT_1)
        {
            string_to_display += ": 1\n";
        }
        if (this->bit_list[i].value == BIT_0)
        {
            string_to_display += ": 0\n";
        }
    }

    Utils::trace_position(TRACE_PREFIX_CODED_VINYL, "bit_list : " + string_to_display);
}

bool Coded_vinyl::extreme_list_clean(char left_or_right)
{
    // exit if arg passed is not left or right channel
    if (left_or_right != LEFT_CHANNEL && left_or_right != RIGHT_CHANNEL)
    {
        #ifdef TRACE_ERROR
            string msg = "Can not clean extreme list, \
                bad call of Coded_vinyl::extreme_list_clean(" + Utils::to_string(left_or_right) +")";
            Utils::trace_error(TRACE_PREFIX_CODED_VINYL, msg);
        #endif
        return false;
    }

    // information trace
    #ifdef TRACE_ANALYZE_EXTREME
        if (left_or_right == LEFT_CHANNEL)
        {
            Utils::trace_analyze_extreme(TRACE_PREFIX_CODED_VINYL, "Cleaning left channel extreme list");
        }
        else
        {
            Utils::trace_analyze_extreme(TRACE_PREFIX_CODED_VINYL, "Cleaning right channel extreme list");
        }
    #endif

    // remove all element of the list
    if (left_or_right == LEFT_CHANNEL)
        this->left_channel_extreme_list.clear();
    else
        this->right_channel_extreme_list.clear();

    // information trace
    #ifdef TRACE_ANALYZE_EXTREME
        if (left_or_right == LEFT_CHANNEL)
        {
            Utils::trace_analyze_extreme(TRACE_PREFIX_CODED_VINYL, "Left channel extreme list cleaned");
        }
        else
        {
            Utils::trace_analyze_extreme(TRACE_PREFIX_CODED_VINYL, "Right channel extreme list cleaned");
        }
    #endif

    return true;
}

bool Coded_vinyl::extreme_list_push_back(char left_or_right, extreme_t *extreme)
{
    // exit if arg passed is not left or right channel
    if (left_or_right != LEFT_CHANNEL && left_or_right != RIGHT_CHANNEL)
    {
        #ifdef TRACE_ERROR
            string msg = "Can not push back on extreme list, \
                bad call of Coded_vinyl::extreme_list_push_back(" + Utils::to_string(left_or_right) +")";
            Utils::trace_error(TRACE_PREFIX_CODED_VINYL, msg);
        #endif
        return false;
    }

    if (left_or_right == LEFT_CHANNEL)
    {
        this->left_channel_extreme_list.push_back(*extreme);

        #ifdef TRACE_OBJECT_LIFE
            Utils::trace_analyze_extreme(TRACE_PREFIX_CODED_VINYL, "Left channel extreme list, push back :\t" \
                                         + this->get_extreme_properties(extreme));
        #endif
    }
    else
    {
        this->right_channel_extreme_list.push_back(*extreme);

        #ifdef TRACE_OBJECT_LIFE
            Utils::trace_analyze_extreme(TRACE_PREFIX_CODED_VINYL, "Right channel extreme list, push back :\t" \
                                         + this->get_extreme_properties(extreme));
        #endif
    }

    return true;
}

bool Coded_vinyl::extreme_list_pop_back(char left_or_right)
{
    // exit if arg passed is not left or right channel
    if (left_or_right != LEFT_CHANNEL && left_or_right != RIGHT_CHANNEL)
    {
        #ifdef TRACE_ERROR
            string msg = "Can not pop back on extreme list, \
                bad call of Coded_vinyl::extreme_list_pop_back(" + Utils::to_string(left_or_right) +")";
            Utils::trace_error(TRACE_PREFIX_CODED_VINYL, msg);
        #endif
        return false;
    }

    // pop back on extreme list
    if (left_or_right == LEFT_CHANNEL)
    {
        if (this->left_channel_extreme_list.size() < 1) // left channel extreme list is empty
        {
            Utils::trace_error(TRACE_PREFIX_CODED_VINYL, "Can't pop last element because left channel extreme list is empty");
        }
        else // left channel extreme is not empty
        {
            #ifdef TRACE_OBJECT_LIFE
                Utils::trace_analyze_extreme(TRACE_PREFIX_CODED_VINYL, "Left channel extreme list, pop back :\t" \
                        + this->get_extreme_properties(&this->left_channel_extreme_list[this->left_channel_extreme_list.size()-1]));
            #endif

            // remove the last pointer (on extreme struct) from left channel extreme list
            this->left_channel_extreme_list.pop_back();
        }
    }
    else
    {
        if (this->right_channel_extreme_list.size() < 1) // right channel extreme list is empty
        {
            Utils::trace_error(TRACE_PREFIX_CODED_VINYL, "Can't pop last element because right channel extreme list is empty");
        }
        else // right channel extreme is not empty
        {
            #ifdef TRACE_OBJECT_LIFE
                Utils::trace_analyze_extreme(TRACE_PREFIX_CODED_VINYL, "Right channel extreme list, pop back :\t" \
                        + this->get_extreme_properties(&this->right_channel_extreme_list[this->right_channel_extreme_list.size()-1]));
            #endif

            // remove the last pointer (on extreme struct) from right channel extreme list
            this->right_channel_extreme_list.pop_back();
        }
    }

    return true;
}

bool Coded_vinyl::extreme_list_pop_front(char left_or_right)
{
    // exit if arg passed is not left or right channel
    if (left_or_right != LEFT_CHANNEL && left_or_right != RIGHT_CHANNEL)
    {
        #ifdef TRACE_ERROR
            string msg = "Can not pop front on extreme list, \
                bad call of Coded_vinyl::extreme_list_pop_front(" + Utils::to_string(left_or_right) +")";
            Utils::trace_error(TRACE_PREFIX_CODED_VINYL, msg);
        #endif
        return false;
    }

    // iterator declaration
    vector<extreme_t>::iterator itr;

    // pop front on extreme list
    if (left_or_right == LEFT_CHANNEL)
    {
        if (this->left_channel_extreme_list.size() < 1) // left channel extreme list is empty
        {
            Utils::trace_error(TRACE_PREFIX_CODED_VINYL, "Can't pop first element because left channel extreme list is empty");
        }
        else // left channel extreme is not empty
        {
            #ifdef TRACE_OBJECT_LIFE
                Utils::trace_analyze_extreme(TRACE_PREFIX_CODED_VINYL, "Left channel extreme list, pop front :\t" \
                        + this->get_extreme_properties(&this->left_channel_extreme_list[0]));
            #endif

            // move iterator on first extreme
            itr = this->left_channel_extreme_list.begin();

            // remove the first extreme struct from left channel extreme list
            this->left_channel_extreme_list.erase(itr);
        }
    }
    else
    {
        if (this->right_channel_extreme_list.size() < 1) // right channel extreme list is empty
        {
            Utils::trace_error(TRACE_PREFIX_CODED_VINYL, "Can't pop first element because right channel extreme list is empty");
        }
        else // right channel extreme is not empty
        {
            #ifdef TRACE_OBJECT_LIFE
                Utils::trace_analyze_extreme(TRACE_PREFIX_CODED_VINYL, "Right channel extreme list, pop front :\t" \
                        + this->get_extreme_properties(&this->right_channel_extreme_list[0]));
            #endif

            /// move iterator on first extreme
            itr = this->right_channel_extreme_list.begin();

            // remove the first extreme struct from right channel extreme list
            this->right_channel_extreme_list.erase(itr);
        }
    }

    return true;
}

bool Coded_vinyl::extreme_list_multiple_pop_front(char left_or_right, short int nb_element)
{
    // exit if arg passed is not left or right channel
    if (left_or_right != LEFT_CHANNEL && left_or_right != RIGHT_CHANNEL)
    {
        #ifdef TRACE_ERROR
            string msg = "Can not multiple pop front on extreme list, \
                bad call of Coded_vinyl::extreme_list_multiple_pop_front(" + Utils::to_string(left_or_right) +")";
            Utils::trace_error(TRACE_PREFIX_CODED_VINYL, msg);
        #endif
        return false;
    }

    // iterator declaration
    vector<extreme_t>::iterator itr_begin;
    vector<extreme_t>::iterator itr_end;

    // multiple pop front on extreme list
    if (left_or_right == LEFT_CHANNEL)
    {
        if (this->left_channel_extreme_list.size() < 1) // left channel extreme list is empty
        {
            Utils::trace_error(TRACE_PREFIX_CODED_VINYL, "Can't pop the first " + Utils::to_string(nb_element) \
                                                     + " element(s) because left channel extreme list is empty");
        }
        else // left channel extreme is not empty
        {
            #ifdef TRACE_ANALYZE_EXTREME
                string msg1 = "";
                msg1 = "Left channel extreme list, multiple pop front :\n";
                for (int i = 0; i < nb_element; i++) msg1 += "#\t\t\t\t\t\t[" + Utils::to_string(i) + "] " \
                    + this->get_extreme_properties(&this->left_channel_extreme_list[i]) + "\n";
                Utils::trace_analyze_extreme(TRACE_PREFIX_CODED_VINYL, msg1);
            #endif

            // move iterator on first extreme
            itr_begin = this->left_channel_extreme_list.begin();
            // move end iterator on last extreme to remove
            itr_end = itr_begin + nb_element;

            // remove the first extreme struct from left channel extreme list
            this->left_channel_extreme_list.erase(itr_begin, itr_end);
        }
    }
    else
    {
        if (this->left_channel_extreme_list.size() < 1) // right channel extreme list is empty
        {
            Utils::trace_error(TRACE_PREFIX_CODED_VINYL, "Can't pop the first " + Utils::to_string(nb_element) \
                                                     + " element(s) because right channel extreme list is empty");
        }
        else // right channel extreme is not empty
        {
            #ifdef TRACE_ANALYZE_EXTREME
                string msg2 = "";
                msg2 = "Right channel extreme list, multiple pop front :\n";
                for (int i = 0; i < nb_element; i++) msg2 += "#\t\t\t\t\t\t[" + Utils::to_string(i) + "] " \
                    + this->get_extreme_properties(&this->right_channel_extreme_list[i]) + "\n";
                Utils::trace_analyze_extreme(TRACE_PREFIX_CODED_VINYL, msg2);
            #endif

            // move iterator on first extreme
            itr_begin = this->right_channel_extreme_list.begin();
            // move end iterator on last extreme to remove
            itr_end = itr_begin + nb_element;

            // remove the first extreme struct from left channel extreme list
            this->right_channel_extreme_list.erase(itr_begin, itr_end);
        }
    }

    return true;
}

bool Coded_vinyl::extreme_list_multiple_pop(char left_or_right, short int start_index, short int nb_element)
{
    // exit if arg passed is not left or right channel
    if (left_or_right != LEFT_CHANNEL && left_or_right != RIGHT_CHANNEL)
    {
        #ifdef TRACE_ERROR
            string msg = "Impossible to multiple pop on extreme list, \
                bad call of Coded_vinyl::extreme_list_multiple_pop(" + Utils::to_string(left_or_right) +")";
            Utils::trace_error(TRACE_PREFIX_CODED_VINYL, msg);
        #endif
        return false;
    }

    // iterator declaration
    vector<extreme_t>::iterator itr_begin;
    vector<extreme_t>::iterator itr_end;

    // multiple pop on extreme list
    if (left_or_right == LEFT_CHANNEL)
    {
        if (this->left_channel_extreme_list.size() < 1) // left channel extreme list is empty
        {
            Utils::trace_error(TRACE_PREFIX_CODED_VINYL, "Can't pop " + Utils::to_string(nb_element - start_index) \
                                                     + " element(s) because left channel extreme list is empty");
        }
        else // left channel extreme is not empty
        {
            #ifdef TRACE_ANALYZE_EXTREME
                string msg1 = "";
                msg1 = "Left channel extreme list, multiple pop :\n";
                for (int i = start_index; i < (start_index + nb_element); i++) msg1 += "#\t\t\t\t\t\t[" + Utils::to_string(i) + "] " \
                    + this->get_extreme_properties(&this->left_channel_extreme_list[i]) + "\n";
                Utils::trace_analyze_extreme(TRACE_PREFIX_CODED_VINYL, msg1);
            #endif

            // move iterator on first extreme to delete
            itr_begin = this->left_channel_extreme_list.begin();
            itr_begin += start_index;
            // move end iterator on last extreme to remove
            itr_end = itr_begin + nb_element;

            // remove the extreme struct from left channel extreme list
            this->left_channel_extreme_list.erase(itr_begin, itr_end);
        }
    }
    else
    {
        if (this->left_channel_extreme_list.size() < 1) // right channel extreme list is empty
        {
            Utils::trace_error(TRACE_PREFIX_CODED_VINYL, "Can't pop  " + Utils::to_string(nb_element - start_index) \
                                                     + " element(s) because right channel extreme list is empty");
        }
        else // right channel extreme is not empty
        {
            #ifdef TRACE_ANALYZE_EXTREME
                string msg1 = "";
                msg1 = "Right channel extreme list, multiple pop :\n";
                for (int i = start_index; i < (start_index + nb_element); i++)
                {
                    msg1 += "#\t\t\t\t\t\t[" + Utils::to_string(i) + "] " \
                         + this->get_extreme_properties(&this->right_channel_extreme_list[i]) + "\n";
                }
                Utils::trace_analyze_extreme(TRACE_PREFIX_CODED_VINYL, msg1);
            #endif

            // move iterator on first extreme to delete
            itr_begin = this->right_channel_extreme_list.begin();
            itr_begin += start_index;
            // move end iterator on last extreme to remove
            itr_end = itr_begin + nb_element;

            // remove the extreme struct from right channel extreme list
            this->right_channel_extreme_list.erase(itr_begin, itr_end);
        }
    }

    return true;
}

bool Coded_vinyl::extreme_list_pop_at(char left_or_right, short int index)
{
    // exit if arg passed is not left or right channel
    if (left_or_right != LEFT_CHANNEL && left_or_right != RIGHT_CHANNEL)
    {
        Utils::trace_error(TRACE_PREFIX_CODED_VINYL, "Impossible to multiple pop on extreme list, \
                bad call of Coded_vinyl::extreme_list_pop_at(" + Utils::to_string(left_or_right) +")");

        return false;
    }

    // iterator declaration
    vector<extreme_t>::iterator itr;

    string msg = "";

    // pop on extreme list
    if (left_or_right == LEFT_CHANNEL)
    {
        if (this->left_channel_extreme_list.size() < 1) // left channel extreme list is empty
        {
            Utils::trace_error(TRACE_PREFIX_CODED_VINYL, "Can not pop element because left channel extreme list is empty");
        }
        else // left channel extreme is not empty
        {
            #ifdef TRACE_ANALYZE_EXTREME
                msg = "";
                msg = "Left channel extreme list, pop at " + Utils::to_string(index) + " :\n";
                msg += "#\t\t\t\t\t\t[" + Utils::to_string(index) + "] " \
                    + this->get_extreme_properties(&this->left_channel_extreme_list[index]) + "\n";
                Utils::trace_analyze_extreme(TRACE_PREFIX_CODED_VINYL, msg);
            #endif

            // move iterator on first extreme to delete
            itr = this->left_channel_extreme_list.begin();
            itr += index;

            // remove the extreme struct from left channel extreme list
            this->left_channel_extreme_list.erase(itr);
        }
    }
    else
    {
        if (this->right_channel_extreme_list.size() < 1) // right channel extreme list is empty
        {
            Utils::trace_error(TRACE_PREFIX_CODED_VINYL, "Can not pop element because right channel extreme list is empty");
        }
        else // right channel extreme is not empty
        {
            #ifdef TRACE_ANALYZE_EXTREME
                msg = "";
                msg = "Right channel extreme list, pop at " + Utils::to_string(index) + " :\n";
                msg += "#\t\t\t\t\t\t[" + Utils::to_string(index) + "] " \
                    + this->get_extreme_properties(&this->right_channel_extreme_list[index]) + "\n";
                Utils::trace_analyze_extreme(TRACE_PREFIX_CODED_VINYL, msg);
            #endif

            // move iterator on first extreme to delete
            itr = this->right_channel_extreme_list.begin();
            itr += index;

            // remove the extreme struct from right channel extreme list
            this->right_channel_extreme_list.erase(itr);
        }
    }

    return true;
}

int Coded_vinyl::extreme_list_get_size(char left_or_right)
{
    if (left_or_right == LEFT_CHANNEL)
    {
        return this->left_channel_extreme_list.size();
    }
    else
    {
        return this->right_channel_extreme_list.size();
    }
}

bool Coded_vinyl::extreme_list_display(char left_or_right)
{
    // exit if arg passed is not left or right channel
    if (left_or_right != LEFT_CHANNEL && left_or_right != RIGHT_CHANNEL)
    {
        #ifdef TRACE_ERROR
            string msg = "Impossible to display extreme list, \
                bad call of Coded_vinyl::extreme_list_display(" + Utils::to_string(left_or_right) +")";
            Utils::trace_error(TRACE_PREFIX_CODED_VINYL, msg);
        #endif
        return false;
    }

    // display extreme list
    #if defined(TRACE_ANALYZE_EXTREME) || defined (TRACE_EXTREME_USED_FOR_DETECTING_SPEED)
        string display_string = "";

        if (left_or_right == LEFT_CHANNEL)
        {
            display_string += "Left extreme list (size : " + Utils::to_string(this->left_channel_extreme_list.size()) + ")\n";
        }
        else
        {
            display_string += "Right extreme list (size : " + Utils::to_string(this->right_channel_extreme_list.size()) + ")\n";
        }

        if (left_or_right == LEFT_CHANNEL)
        {
            for (unsigned int i = 0; i < this->left_channel_extreme_list.size(); i++)
            {
                display_string += "#\t\t\t\t\t\t[" + Utils::to_string(i) + "] ";
                display_string += this->get_extreme_properties(&this->left_channel_extreme_list[i]) + "\n";
            }
        }
        else
        {
            for (unsigned int i = 0; i < this->right_channel_extreme_list.size(); i++)
            {
                display_string += "#\t\t\t\t\t\t[" + Utils::to_string(i) + "] ";
                display_string += this->get_extreme_properties(&this->right_channel_extreme_list[i]) + "\n";
            }
        }

        display_string += "#\t\t\t\t\t\t|___";

        Utils::trace_analyze_extreme(TRACE_PREFIX_CODED_VINYL, display_string);

        Utils::trace_extreme_used_for_detecting_speed(TRACE_PREFIX_CODED_VINYL, display_string);
    #endif

    return true;
}

string Coded_vinyl::get_extreme_properties(extreme_t *extreme)
{
    string msg = "";
    msg += "index : " + Utils::to_string(extreme->index);
    msg += "\tvalue : " + Utils::to_string(extreme->value);
    msg += "\tbit_value : " + Utils::to_string(extreme->bit_value);
    if (extreme->type == 0) msg += "\ttype : UNDEFINE";
    if (extreme->type == 1) msg += "\ttype : MIN";
    if (extreme->type == 2) msg += "\ttype : MIN_SEARCH";
    if (extreme->type == 3) msg += "\ttype : MAX";
    if (extreme->type == 4) msg += "\ttype : MAX_SEARCH";

    return msg;
}

bool Coded_vinyl::extreme_list_get_element(char left_or_right, short int index, extreme_t **extreme_to_get)
{
#if 0 // Safe implementation but use a lot of CPU.
    // exit if arg passed is not left or right channel
    if (left_or_right != LEFT_CHANNEL && left_or_right != RIGHT_CHANNEL)
    {
        #ifdef TRACE_ERROR
            string msg = "Cannot get element of extreme list, \
                bad call of Coded_vinyl::extreme_list_get_element(" + Utils::to_string(left_or_right) +")";
            Utils::trace_error(TRACE_PREFIX_CODED_VINYL, msg);
        #endif

        return false;
    }

    if (left_or_right == LEFT_CHANNEL)
    {
        if ((unsigned int)index < this->left_channel_extreme_list.size())
        {
            //return this->left_channel_extreme_list[index];
            *extreme_to_get = &this->left_channel_extreme_list[index];
            return true;
        }
        else // index too big
        {
            Utils::trace_error(TRACE_PREFIX_CODED_VINYL, "Left channel extreme list : can't access element #" \
                                                     + Utils::to_string(index) + ", because there is only " \
                                                     + Utils::to_string(this->left_channel_extreme_list.size()) \
                                                     + " element(s) in the extreme list");

            return false;
        }
    }
    else
    {
        if ((unsigned int)index < this->right_channel_extreme_list.size())
        {
            //return this->right_channel_extreme_list[index];
            *extreme_to_get = &this->right_channel_extreme_list[index];
            return true;
        }
        else // index too big
        {
            Utils::trace_error(TRACE_PREFIX_CODED_VINYL, "Right channel extreme list : can't access element #" \
                                                     + Utils::to_string(index) + ", because there is only " \
                                                     + Utils::to_string(this->right_channel_extreme_list.size()) \
                                                     + " element(s) in the extreme list");

            return false;
        }
    }
#else
    if (left_or_right == LEFT_CHANNEL)
    {
        *extreme_to_get = &this->left_channel_extreme_list[index];
    }
    else
    {
        *extreme_to_get = &this->right_channel_extreme_list[index];
    }

    return true;
#endif
}

bool Coded_vinyl::low_pass_filter(vector<float> &samples,
                                            int cut_frequency)
{
    int   i       = 0;
    int   size    = samples.size();
    float coeff_a = 0.0;

    // size of samples table must be greater than 0
    if (size < 1)
    {
        Utils::trace_error(TRACE_PREFIX_CODED_VINYL, "Try to apply low-pass filter on an empty buffer");

        return false;
    }

    // get value of coefficient a
    coeff_a = (float)((1.0/this->get_sample_rate()) / ((1.0/this->get_sample_rate()) + (1.0/(2.0*3.14159*cut_frequency))));

    // get samples_output filtered values
    for (i = 1; i < size; i++)
    {
        samples[i] = coeff_a * samples[i] + (1-coeff_a) * samples[i-1];
    }

    // all is OK, return TRUE
    return true;
}

bool Coded_vinyl::get_average_amplitude(vector<float> &samples,
                                                float *average_amplitude)
{
    int i    = 0;
    int size = samples.size();

    // size of samples table must be greater than 0
    if (size < 1)
    {
        Utils::trace_error(TRACE_PREFIX_CODED_VINYL, "Try to get average amplitude on an empty buffer");

        return false;
    }

    for (i = 0; i < size; i++)
    {
        *average_amplitude += fabs(samples[i]);
    }
    *average_amplitude = *average_amplitude / size;

    return true;
}

bool Coded_vinyl::get_cut_frequency(int *cut_frequency)
{
    // previous frequency way
    if (this->latest_available_detected_speed != 0.0)
    {
        *cut_frequency = (int)(fabs(this->latest_available_detected_speed)*1200.0);
    }
    else
    {
        *cut_frequency = 100;
    }

    return true;
}

bool Coded_vinyl::check_signal_amplitude(vector<float> &samples)
{
    // size of samples table must be greater than 0
    if (samples.size() < 1)
    {
        Utils::trace_error(TRACE_PREFIX_CODED_VINYL, "Try to check signal amplitude on an empty buffer");

        return false;
    }

    float average_amplitude = 0.0;

    // get average amplitude of this signal buffer
    if (this->get_average_amplitude(samples, &average_amplitude) == false)
    {
        Utils::trace_error(TRACE_PREFIX_CODED_VINYL, "Can not get average amplitude");

        return false;
    }

    // compare average amplitude to a reference minimal acceptable sample value
    if (average_amplitude < this->extreme_min)
    {
        Utils::trace_analyze_vinyl(TRACE_PREFIX_CODED_VINYL, "No input signal");

        this->set_no_input_signal(true);
    }
    else
    {
        this->set_no_input_signal(false);
    }

    return true;
}

float Coded_vinyl::get_average_extreme()
{
    float average = 0.0;
    unsigned int i = 0;

    // Average extreme is 0.0 if extremes list are empty.
    if ((this->left_channel_extreme_list.size() <= 0)
        || (this->right_channel_extreme_list.size() <= 0))
    {
        return 0.0;
    }

    // Accumulate extreme values of left and right list.
    for (i = 0; i < this->left_channel_extreme_list.size(); i++)
    {
        average += fabs(this->left_channel_extreme_list[i].value);
    }
    for (i = 0; i < this->right_channel_extreme_list.size(); i++)
    {
        average += fabs(this->right_channel_extreme_list[i].value);
    }

    // Get average of every extreme values.
    average /= (this->left_channel_extreme_list.size() + this->right_channel_extreme_list.size());

    return average;
}

void Coded_vinyl::set_no_input_signal(bool signal)
{
    this->no_input_signal = signal;
}

bool Coded_vinyl::is_no_input_signal()
{
    return this->no_input_signal;
}

bool Coded_vinyl::set_extreme_min(float extreme_min)
{
    if (extreme_min <= 0.0)
    {
        Utils::trace_error(TRACE_PREFIX_CODED_VINYL, "extreme_min cannot be <= 0.0");

        return false;
    }

    this->extreme_min = extreme_min;

    return true;
}

float Coded_vinyl::get_extreme_min()
{
    return this->extreme_min;
}

bool Coded_vinyl::set_max_buffer_coeff(int max_buffer_coeff)
{
    if (max_buffer_coeff <= 0)
    {
        Utils::trace_error(TRACE_PREFIX_CODED_VINYL, "max_buffer_coeff cannot be <= 0");

        return false;
    }

    this->max_buffer_coeff = max_buffer_coeff;

    return true;
}

int Coded_vinyl::get_max_buffer_coeff()
{
    return this->max_buffer_coeff;
}

bool Coded_vinyl::set_nb_cycle_before_changing_direction(int nb_cycle_before_changing_direction)
{
    if (nb_cycle_before_changing_direction <= 0)
    {
        Utils::trace_error(TRACE_PREFIX_CODED_VINYL,
                           "nb_cycle_before_changing_direction cannot be <= 0");

        return false;
    }

    this->nb_cycle_before_changing_direction = nb_cycle_before_changing_direction;

    return true;
}

int Coded_vinyl::get_nb_cycle_before_changing_direction()
{
    return this->nb_cycle_before_changing_direction;
}

bool Coded_vinyl::set_coeff_right_dist_min_bit1_to_bit1(float coeff_right_dist_min_bit1_to_bit1)
{
    if (coeff_right_dist_min_bit1_to_bit1 <= 0.0)
    {
        Utils::trace_error(TRACE_PREFIX_CODED_VINYL,
                           "coeff_right_dist_min_bit1_to_bit1 cannot be <= 0.0");

        return false;
    }

    this->coeff_right_dist_min_bit1_to_bit1 = coeff_right_dist_min_bit1_to_bit1;

    return true;
}

float Coded_vinyl::get_coeff_right_dist_min_bit1_to_bit1()
{
    return this->coeff_right_dist_min_bit1_to_bit1;
}

bool Coded_vinyl::set_coeff_left_dist_min_bit1_to_bit1(float coeff_left_dist_min_bit1_to_bit1)
{
    if (coeff_left_dist_min_bit1_to_bit1 <= 0.0)
    {
        Utils::trace_error(TRACE_PREFIX_CODED_VINYL,
                           "coeff_left_dist_min_bit1_to_bit1 cannot be <= 0.0");

        return false;
    }

    this->coeff_left_dist_min_bit1_to_bit1 = coeff_left_dist_min_bit1_to_bit1;

    return true;
}

float Coded_vinyl::get_coeff_left_dist_min_bit1_to_bit1()
{
    return this->coeff_left_dist_min_bit1_to_bit1;
}

bool Coded_vinyl::set_coeff_right_dist_max_bit0_to_bit0(float coeff_right_dist_max_bit0_to_bit0)
{
    if (coeff_right_dist_max_bit0_to_bit0 <= 0.0)
    {
        Utils::trace_error(TRACE_PREFIX_CODED_VINYL,
                           "coeff_right_dist_max_bit0_to_bit0 cannot be <= 0.0");

        return false;
    }

    this->coeff_right_dist_max_bit0_to_bit0 = coeff_right_dist_max_bit0_to_bit0;

    return true;
}

float Coded_vinyl::get_coeff_right_dist_max_bit0_to_bit0()
{
    return this->coeff_right_dist_max_bit0_to_bit0;
}

bool Coded_vinyl::set_coeff_left_dist_max_bit0_to_bit0(float coeff_left_dist_max_bit0_to_bit0)
{
    if (coeff_left_dist_max_bit0_to_bit0 <= 0.0)
    {
        Utils::trace_error(TRACE_PREFIX_CODED_VINYL,
                           "coeff_left_dist_max_bit0_to_bit0 cannot be <= 0.0");

        return false;
    }

    this->coeff_left_dist_max_bit0_to_bit0 = coeff_left_dist_max_bit0_to_bit0;

    return true;
}

float Coded_vinyl::get_coeff_left_dist_max_bit0_to_bit0()
{
    return this->coeff_left_dist_max_bit0_to_bit0;
}

bool Coded_vinyl::set_progressive_volume_coeff(float progressive_volume_coeff)
{
    if (progressive_volume_coeff <= 0.0)
    {
        Utils::trace_error(TRACE_PREFIX_CODED_VINYL, "progressive_volume_coeff cannot be <= 0.0");

        return false;
    }

    this->progressive_volume_coeff = progressive_volume_coeff;

    return true;
}

float Coded_vinyl::get_progressive_volume_coeff()
{
    return this->progressive_volume_coeff;
}

bool Coded_vinyl::set_full_volume_amplitude(float full_volume_amplitude)
{
    if (full_volume_amplitude <= 0.0)
    {
        Utils::trace_error(TRACE_PREFIX_CODED_VINYL, "full_volume_amplitude cannot be <= 0.0");

        return false;
    }

    this->full_volume_amplitude = full_volume_amplitude;

    return true;
}

float Coded_vinyl::get_full_volume_amplitude()
{
    return this->full_volume_amplitude;
}

bool Coded_vinyl::set_sample_rate(int sample_rate)
{
    if (sample_rate <= 0)
    {
        Utils::trace_error(TRACE_PREFIX_CODED_VINYL,
                           "sample_rate cannot be <= 0");

        return false;
    }

    this->sample_rate = sample_rate;

    return true;
}

int Coded_vinyl::get_sample_rate()
{
    return this->sample_rate;
}

bool Coded_vinyl::set_low_pass_filter_max_speed_usage(float low_pass_filter_max_speed_usage)
{
    if (low_pass_filter_max_speed_usage < 0.0)
    {
        Utils::trace_error(TRACE_PREFIX_CODED_VINYL, "low_pass_filter_max_speed_usage cannot be < 0.0");

        return false;
    }

    this->low_pass_filter_max_speed_usage = low_pass_filter_max_speed_usage;

    return true;
}

float Coded_vinyl::get_low_pass_filter_max_speed_usage()
{
    return this->low_pass_filter_max_speed_usage;
}

bool Coded_vinyl::set_input_amplify_coeff(int coeff)
{
    if (coeff <= 0)
    {
        Utils::trace_error(TRACE_PREFIX_CODED_VINYL, "coeff cannot be <= 0");

        return false;
    }

    this->input_amplify_coeff = coeff;

    return true;
}

int Coded_vinyl::get_input_amplify_coeff()
{
    return this->input_amplify_coeff;
}

bool Coded_vinyl::set_rpm(unsigned short int rpm)
{
    this->rpm = rpm;

    return true;
}

unsigned short int Coded_vinyl::get_rpm()
{
    return this->rpm;
}
