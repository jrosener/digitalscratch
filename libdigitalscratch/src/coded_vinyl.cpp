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
#include <QtGlobal>

using namespace std;

#include "include/dscratch_parameters.h"
#include "include/utils.h"
#include "include/coded_vinyl.h"

Coded_vinyl::Coded_vinyl(unsigned int sample_rate)
{
    Utils::trace_object_life(TRACE_PREFIX_CODED_VINYL,
                             "+ Creating Coded_vinyl object...");

    this->rpm                                = RPM_33;
    this->sample_rate                        = sample_rate;
    this->input_amplify_coeff                = DEFAULT_INPUT_AMPLIFY_COEFF;
    this->old_speed                          = 0.0;
    this->current_speed                      = 0.0;

    this->set_reverse_direction(false);

    Utils::trace_object_life(TRACE_PREFIX_CODED_VINYL,
                             "+ Coded_vinyl object created");
}

Coded_vinyl::~Coded_vinyl()
{
    Utils::trace_object_life(TRACE_PREFIX_CODED_VINYL, "- Deleting Coded_vinyl object...");


    Utils::trace_object_life(TRACE_PREFIX_CODED_VINYL, "- Coded_vinyl object deleted");
}

void Coded_vinyl::calculate_sin_wave_area_size()
{
    this->sin_wave_area_size = (float)this->sample_rate / (2.0f * (float)this->get_sinusoidal_frequency());
}

void Coded_vinyl::add_sound_data(vector<float> &input_samples_1,
                                 vector<float> &input_samples_2)
{
    Utils::trace_analyze_vinyl(TRACE_PREFIX_CODED_VINYL, "Adding timecode data to internal buffers...");

    // Add samples at the end of previous not-used samples.
    copy(input_samples_1.begin(), input_samples_1.end(), back_inserter(this->samples_channel_1));
    copy(input_samples_2.begin(), input_samples_2.end(), back_inserter(this->samples_channel_2));
}

float Coded_vinyl::get_speed()
{
    Utils::trace_analyze_vinyl(TRACE_PREFIX_CODED_VINYL, "Searching new speed...");

    this->calculate_sin_wave_area_size();

//    cout << "samples_channel_1.size()=" << this->samples_channel_1.size() << endl;

    // Check if samples table are not too large.
    // If yes, it means we are not able to calculate speed, so return a null speed.
    if (this->samples_channel_1.size() > (this->sin_wave_area_size * DEFAULT_MAX_SIN_WAV_AREA_FACTOR))
    {
        this->samples_channel_1.clear();
        this->samples_channel_2.clear();
        this->zero_cross_list_1.clear();
        this->zero_cross_list_2.clear();
        this->old_speed     = 0.0;
        this->current_speed = 0.0;
        return 0.0;
    }

    // For both channels, get the list of zero crossing.
    this->fill_zero_cross_list(this->zero_cross_list_1, this->samples_channel_1);
    this->fill_zero_cross_list(this->zero_cross_list_2, this->samples_channel_2);

//    cout << "zero_cross_list_1:" << endl;
//    for (unsigned int i = 0; i < this->zero_cross_list_1.size(); i++)
//    {
//        cout << this->zero_cross_list_1[i].first << ":" << this->zero_cross_list_1[i].second << endl;
//    }
//    cout << "zero_cross_list_2:" << endl;
//    for (unsigned int i = 0; i < this->zero_cross_list_2.size(); i++)
//    {
//        cout << this->zero_cross_list_2[i].first << ":" << this->zero_cross_list_2[i].second << endl;
//    }

    // Calculate speed for all area and make the average value.
    float speed = this->calculate_speed();
//    cout << "calculated speed = " << speed << endl;
    if (speed != NO_NEW_SPEED_FOUND) // A new speed was found, use and store it.
    {
        this->old_speed     = speed;
        this->current_speed = speed;
    }
    else // No speed was found, use the old one.
    {
        speed = this->old_speed;
    }
//    cout << "returned speed = " << speed << endl;

    // Keep non-used samples in internal buffers (for the next time).
    // Put -99 to complete undefined samples.
    this->keep_unused_samples();

    return speed;
}

void Coded_vinyl::fill_zero_cross_list(vector< pair<bool, unsigned int> > &zero_cross_list, vector<float> &samples)
{
    Utils::trace_analyze_vinyl(TRACE_PREFIX_CODED_VINYL, "Fill list of zero crossing...");

    // Create the zero crossing line.
    vector<float> zero_crossing_line(samples.size(), 0.0f);

    // Create the zero crossing line based on peaks.
//    this->calculate_zero_crossing_line(zero_crossing_line, samples);

    // Iterate over 2 sample channels, get size of sinusoidal wave areas by catching samples crossing zero.
    float sample = 0.0;
    bool  wait_zero_crossing_up   = false;
    bool  wait_zero_crossing_down = false;
    for (unsigned int i = 0; i < samples.size(); i++)
    {
        // Get sample value.
        sample = samples[i];

        if (sample != -99.0)
        {
            // First sample.
            if ((wait_zero_crossing_up == false) && (wait_zero_crossing_down == false))
            {
                if (sample < zero_crossing_line[i])
                {
                    // Now we are waiting to cross the zero up.
                    wait_zero_crossing_up   = true;
                    wait_zero_crossing_down = false;
                }
                else if (sample > zero_crossing_line[i])
                {
                    // Now we are waiting to cross the zero down.
                    wait_zero_crossing_up   = false;
                    wait_zero_crossing_down = true;
                }
            }
            else // Other samples.
            {
                if (sample < zero_crossing_line[i])
                {
                    if (wait_zero_crossing_down == true)
                    {
                        // We were waiting to cross the zero down. So store this index as a zero crosser.
                        zero_cross_list.push_back(make_pair(false, i));
                        // Now we are waiting to cross the zero up.
                        wait_zero_crossing_up   = true;
                        wait_zero_crossing_down = false;
                    }
                }
                else if (sample > zero_crossing_line[i])
                {
                    if (wait_zero_crossing_up == true)
                    {
                        // We were waiting to cross the zero up. So store this index as a zero crosser.
                        zero_cross_list.push_back(make_pair(true, i));
                        // Now we are waiting to cross the zero down.
                        wait_zero_crossing_up   = false;
                        wait_zero_crossing_down = true;
                    }
                }
            }
        }
    }
}

void Coded_vinyl::calculate_zero_crossing_line(vector<float> &zero_crossing_line, vector<float> &samples)
{
    // Get list of peak indexes.
    vector< pair<unsigned int, float> > peaks; // pair(table_index, sample_value)
    bool wait_for_up   = false;
    bool wait_for_down = false;
    for (unsigned int i = 0; i < samples.size()-1; i++)
    {
        // Diff with the next sample.
        float diff = samples[i+1] - samples[i];

        if (diff > 0.0) // We are going up, waiting for the down hill.
        {
            if (wait_for_up == true) // Found the hill going up, store it.
            {
                peaks.push_back(make_pair(i, samples[i]));
            }
            wait_for_down = true;
            wait_for_up   = false;
        }
        else if (diff < 0.0) // We are going down, waiting for the up hill
        {
            if (wait_for_down == true) // Found the hill going down, store it.
            {
                peaks.push_back(make_pair(i, samples[i]));
            }
            wait_for_up   = true;
            wait_for_down = false;
        }
        else
        {
            wait_for_up   = false;
            wait_for_down = false;
        }
    }
//    cout << "table of peaks:" << endl;
//    for (unsigned int i = 0; i < peaks.size(); i++)
//    {
//        cout << peaks[i].first << ":" << peaks[i].second << endl;
//    }

    // Create the zero crossing line based on peaks.
    if (peaks.size() >= 2)
    {
        fill(zero_crossing_line.begin(), zero_crossing_line.end(), -99.0f);
        int unknowns_index_start = 0;
        int unknowns_index_end   = 0;
        for (unsigned int i = 0; i < peaks.size() - 1; i++)
        {
            // Calculate index and values in the middle of 2 peaks.
            int index = qRound((float)peaks[i].first + ((float)(peaks[i+1].first - peaks[i].first) / 2.0));
            float value = qMax(peaks[i].second, peaks[i+1].second) - ((float)(qAbs(peaks[i].second) + qAbs(peaks[i+1].second)) / 2.0);

            // Add the value at the right place.
            zero_crossing_line[index] = value;
            unknowns_index_end = index;

            // Interpolate unknown values.
            this->interpolate_unknown_values(zero_crossing_line, unknowns_index_start, unknowns_index_end);

            // Next one.
            unknowns_index_start = index;
        }
        // Last part has to be interpolated as well.
        if (zero_crossing_line[zero_crossing_line.size() - 1] == -99)
        {
            zero_crossing_line[zero_crossing_line.size() - 1] = 0.0;
            unknowns_index_end = zero_crossing_line.size() - 1;
        }
        this->interpolate_unknown_values(zero_crossing_line, unknowns_index_start, unknowns_index_end);
    }
//    cout << "nb;samples;zero crossing line:" << endl;
//    for (unsigned int i = 0; i < samples.size(); i++)
//    {
//        cout << i << ";" << samples[i] << ";" << zero_crossing_line[i] << endl;
//    }
}

void Coded_vinyl::interpolate_unknown_values(vector<float> &table, int unknowns_index_start, int unknowns_index_end)
{
    // Check if the first element is not defined.
    if (table[unknowns_index_start] == -99.0)
    {
        table[unknowns_index_start] = 0.0;
    }

    // Get the value of the start and the end of the unknown area.
    float unknowns_value_start  = table[unknowns_index_start];
    float unknowns_value_end    = table[unknowns_index_end];

    // Calculate coeffs for affine function.
    float a_coeff = (unknowns_value_end - unknowns_value_start) / (float)(unknowns_index_end - unknowns_index_start);
    float b_coeff = unknowns_value_start - (a_coeff * (float)unknowns_index_start);

    // Apply value = a * index + b.
    for (int j = unknowns_index_start; j < unknowns_index_end; j++)
    {
        table[j] = a_coeff * j + b_coeff;
    }
}

float Coded_vinyl::calculate_speed()
{
    // Make a speed average of both sample tables.
    float speed       = 0.0;
    float tmp         = 0.0;
    int   div         = 0;
    bool  speed_found = false;
//    cout << "ranges for channel 1:" << endl;
    tmp = this->calculate_average_speed_one_channel(this->zero_cross_list_1);
    if (tmp != NO_NEW_SPEED_FOUND)
    {
        //cout << "speed channel 1: " << tmp << endl;
        speed = tmp;
        div++;
        speed_found = true;
    }
//    cout << "ranges for channel 2:" << endl;
    tmp = this->calculate_average_speed_one_channel(this->zero_cross_list_2);
    if (tmp != NO_NEW_SPEED_FOUND)
    {
        //cout << "speed channel 2: " << tmp << endl;
        speed += tmp;
        div++;
        speed_found = true;
    }
    if (div > 1)
    {
        speed = speed / (float)div;
    }

    // Validate speed againt average signal amplitude.
    if (speed_found == true)
    {
        speed_found = this->validate_speed_against_amplitude(speed);
    }

    // Return speed if found and validated.
    if (speed_found == false)
    {
        return NO_NEW_SPEED_FOUND;
    }
    else
    {
        // Calculate direction.
        short int direction = this->calculate_direction();

        return direction * speed;
    }
}

float Coded_vinyl::calculate_average_speed_one_channel(vector< pair<bool, unsigned int> > &zero_cross_list)
{
    // We need at least one range, so 2 zero crossing to find a speed.
    if (zero_cross_list.size() < 2)
    {
        return NO_NEW_SPEED_FOUND;
    }

    // Iterate over ranges of zero crossing an calculate the speed.
    float speed = 0.0;
    int   div   = 0;
    for (unsigned int i = 0; i < zero_cross_list.size() - 1; i++)
    {
        unsigned int range = zero_cross_list[i+1].second - zero_cross_list[i].second;
//        cout << range << endl;
        speed += this->sin_wave_area_size / (float)range;
        div++;
    }
    if (div > 1)
    {
        speed = speed / (float)div;
    }

    return speed;
}

short int Coded_vinyl::calculate_direction()
{
    // Init.
    short int nb_direction_forward  = 0;
    short int nb_direction_backward = 0;

    // Only check direction if we have at least one zero crossing on each channels.
    if ((this->zero_cross_list_1.size() >= 1) && (this->zero_cross_list_2.size() >= 1))
    {
        // Compare side by side zero crossings of both channel and get direction.
        unsigned int i = 0;
        while ((i < this->zero_cross_list_1.size()) && (i < this->zero_cross_list_2.size()))
        {
            // Get current zero crossing.
            pair<bool, unsigned int> zero_cross_1 = this->zero_cross_list_1[i];
            pair<bool, unsigned int> zero_cross_2 = this->zero_cross_list_2[i];

            // Determine direction for this zero crossing.
            if (zero_cross_1.second < zero_cross_2.second) // first zero cross is on channel 1
            {
                if (zero_cross_1.first == true) // zero cross 1 going up.
                {
                    if (zero_cross_2.first == true) // zero cross 2 going up.
                    {
                        nb_direction_backward++;
                    }
                    else // zero cross 2 going down.
                    {
                        nb_direction_forward++;
                    }
                }
                else // zero cross 1 going down.
                {
                    if (zero_cross_2.first == true) // zero cross 2 going up.
                    {
                        nb_direction_forward++;
                    }
                    else
                    {
                        nb_direction_backward++;
                    }
                }
            }
            else if (zero_cross_1.second > zero_cross_2.second) // first zero cross is on channel 2
            {
                if (zero_cross_2.first == true) // zero cross 2 going up.
                {
                    if (zero_cross_1.first == true) // zero cross 1 going up.
                    {
                        nb_direction_forward++;
                    }
                    else // zero cross 1 going down.
                    {
                        nb_direction_backward++;
                    }
                }
                else // zero cross 2 going down.
                {
                    if (zero_cross_1.first == true) // zero cross 2 going up.
                    {
                        nb_direction_backward++;
                    }
                    else
                    {
                        nb_direction_forward++;
                    }
                }
            }

            // Next zero crossing.
            i++;
        }
    }

    // Final direction will be the one which is better represented.
    if (nb_direction_forward >= nb_direction_backward)
    {
        // Going forward.
        return 1;
    }
    else
    {
        // Going backward.
        return -1;
    }
}

bool Coded_vinyl::validate_speed_against_amplitude(float speed)
{
    // Get average signal amplitude.
    float amplitude  = 0.0;
    unsigned int div = 0;
    for (unsigned int i = 0; i < this->samples_channel_1.size(); i++)
    {
        if (this->samples_channel_1[i] != -99)
        {
            amplitude += qAbs(this->samples_channel_1[i]);
            div++;
        }
    }
    for (unsigned int j = 0; j < this->samples_channel_2.size(); j++)
    {
        if (this->samples_channel_2[j] != -99)
        {
            amplitude += qAbs(this->samples_channel_2[j]);
            div++;
        }
    }
    amplitude = amplitude / (float)div;

    // Validate "normal" speed (more than 0.30) against signal amplitude.
    if (qAbs(speed) > 0.30)
    {
        if (amplitude < this->get_min_amplitude_for_normal_speed())
        {
            // This speed looks wrong because the amplitude is too small.
            cout << "speed=" << speed << "\tamplitude=" << amplitude
                 << "\t=> not enough signal for this speed (min=" << this->get_min_amplitude_for_normal_speed() << ")."<< endl;
            return false;
        }
    }

    cout << "speed=" << speed << "\tamplitude=" << amplitude << endl;

    return true;
}

void Coded_vinyl::keep_unused_samples()
{
    this->remove_used_samples(this->zero_cross_list_1, this->samples_channel_1);
    this->remove_used_samples(this->zero_cross_list_2, this->samples_channel_2);
    this->align_samples();
}

void Coded_vinyl::remove_used_samples(vector< pair<bool, unsigned int> > &zero_cross_list, vector<float> &samples)
{
    // Keep the last zero crossing.
    if (zero_cross_list.size() >= 2)
    {
        zero_cross_list.erase(zero_cross_list.begin(), zero_cross_list.end() - 1);
    }

    // Keep last sample area (after the zero crossing).
    if (zero_cross_list.size() == 1)
    {
        samples.erase(samples.begin(), samples.begin() + zero_cross_list[0].second);

        // Update the index of the first zero crossing.
        zero_cross_list[0].second = 0;
    }
}

void Coded_vinyl::align_samples()
{
    // Add -99 at the beginning of the smallest sample table (to get the same size for both channels).
    if (this->samples_channel_1.size() < this->samples_channel_2.size())
    {
        unsigned int diff = this->samples_channel_2.size() - this->samples_channel_1.size();
        vector<float> tmp(diff, -99.0);
        this->samples_channel_1.insert(this->samples_channel_1.begin(), tmp.begin(), tmp.end());

        // Update the index of the first zero crossing.
        if (this->zero_cross_list_1.size() == 1)
        {
            this->zero_cross_list_1[0].second = diff;
        }
    }
    else if (this->samples_channel_2.size() < this->samples_channel_1.size())
    {
        unsigned int diff = this->samples_channel_1.size() - this->samples_channel_2.size();
        vector<float> tmp(diff, -99.0);
        this->samples_channel_2.insert(this->samples_channel_2.begin(), tmp.begin(), tmp.end());

        // Update the index of the first zero crossing.
        if (this->zero_cross_list_2.size() == 1)
        {
            this->zero_cross_list_2[0].second = diff;
        }
    }
}

float Coded_vinyl::get_volume()
{
    Utils::trace_analyze_vinyl(TRACE_PREFIX_CODED_VINYL, "Searching new volume...");

    if (fabs(this->current_speed) < SPEED_FOR_VOLUME_CUT)
    {
        return fabs(this->current_speed) / SPEED_FOR_VOLUME_CUT;
    }
    else
    {
        return 1.0;
    }
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
