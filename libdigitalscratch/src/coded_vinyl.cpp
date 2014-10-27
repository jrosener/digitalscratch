/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                                                                            */
/*                                                                            */
/*--------------------------------------------------------( coded_vinyl.cpp )-*/
/*                                                                            */
/*  Copyright (C) 2003-2014                                                   */
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

#include "log.h"
#include "dscratch_parameters.h"
#include "coded_vinyl.h"

Coded_vinyl::Coded_vinyl(unsigned int sample_rate)
{
    this->rpm                                = RPM_33;
    this->sample_rate                        = sample_rate;
    this->input_amplify_coeff                = DEFAULT_INPUT_AMPLIFY_COEFF;
    this->old_speed                          = 0.0;
    this->current_speed                      = 0.0;
    this->no_new_speed_found_counter         = 0;
    this->too_diff_new_speed_counter         = 0;
    this->last_signal_was_centered           = false;
    this->last_zero_cross_list_size          = 0;
    this->validating_turntable_started       = false;
    this->validating_changing_direction      = false;
    this->min_amplitude_for_normal_speed     = 0;

    this->set_reverse_direction(false);
}

Coded_vinyl::~Coded_vinyl()
{
}

void Coded_vinyl::calculate_sin_wave_area_size()
{
    this->sin_wave_area_size = (float)this->sample_rate / (2.0f * (float)this->get_sinusoidal_frequency());
}

void Coded_vinyl::add_sound_data(vector<float> &input_samples_1,
                                 vector<float> &input_samples_2)
{
    qCDebug(DSLIB_ANALYZEVINYL) << "Adding timecode data to internal buffers...";

    // Add samples at the end of previous not-used samples.
    copy(input_samples_1.begin(), input_samples_1.end(), back_inserter(this->samples_channel_1));
    copy(input_samples_2.begin(), input_samples_2.end(), back_inserter(this->samples_channel_2));
}

float Coded_vinyl::get_speed()
{
    qCDebug(DSLIB_ANALYZEVINYL) << "Searching new speed...";

    // First calculate the basic size of a sin wave.
    this->calculate_sin_wave_area_size();

    // Check if samples table are not too large.
    // If yes, it means we are not able to calculate speed, so return a null speed.
    if (this->samples_channel_1.size() > (this->sin_wave_area_size * MAX_SIN_WAV_AREA_FACTOR))
    {
        this->samples_channel_1.clear();
        this->samples_channel_2.clear();
        this->zero_cross_list_1.clear();
        this->zero_cross_list_2.clear();
        this->old_speed     = 0.0;
        this->current_speed = 0.0;
        return 0.0;
    }

    // Center the signal if the average amplitude is too low or too high.
    if ((this->old_speed != NO_NEW_SPEED_FOUND) &&
        (this->old_speed != 0.0) &&
        (this->last_zero_cross_list_size >= 6)) // Ensure the previous signal contains some full sinusoidal waves.
    {
        // Center the signal if necessary.
        this->center_signal(this->samples_channel_1);
        this->center_signal(this->samples_channel_2);
    }
    else
    {
        this->last_signal_was_centered = false;
    }

    // For both channels, get the list of zero crossing.
    this->fill_zero_cross_list(this->zero_cross_list_1, this->samples_channel_1);
    this->fill_zero_cross_list(this->zero_cross_list_2, this->samples_channel_2);
    this->last_zero_cross_list_size = this->zero_cross_list_1.size() + this->zero_cross_list_2.size();

    // Calculate speed for all area and make the average value.
    float speed = this->calculate_speed();

    // Validate speed when starting turntable.
    this->validate_and_ajust_speed_when_starting(speed);

    // Validate speed when changing direction.
    this->validate_and_ajust_speed_when_changing_direction(speed);

    // Change speed smoothly.
    this->smoothly_change_speed(speed);

    // Return a speed.
    this->store_and_return_speed(speed);

    // Keep non-used samples in internal buffers (for the next time).
    // Put -99 to complete undefined samples.
    this->keep_unused_samples();

    return speed;
}

void Coded_vinyl::validate_and_ajust_speed_when_starting(float &speed)
{
    if (speed != NO_NEW_SPEED_FOUND)
    {
        if (this->old_speed == 0.0f)
        {
            // We were stopped and would like to start.
            if (qAbs(speed) != 0)
            {
                if (this->validating_turntable_started == false)
                {
                    // We are starting the turntable, we have to validate the speed, so keep speed = 0 for the moment.
                    this->validating_turntable_started = true;
                    speed = 0.0f;
                }
                else
                {
                    // Reset validation (and implicitely accept the current speed).
                    this->validating_turntable_started = false;
                }
            }
            else
            {
                // Validation failed, starting was wrong, reset the validation.
                this->validating_turntable_started = false;
            }
        }
    }
}

void Coded_vinyl::validate_and_ajust_speed_when_changing_direction(float &speed)
{
    if (speed != NO_NEW_SPEED_FOUND)
    {
        if (this->old_speed * speed < 0) // Direction just changed.
        {
            if (this->validating_changing_direction == false)
            {
                // Validate that we are changing direction.
                this->validating_changing_direction = true;
                speed = speed * -1.0f;
            }
            else
            {
                // Reset validation (and implicitely accept the new direction.
                this->validating_changing_direction = false;
            }
        }
        else
        {
            // Validation failed, so we were not really changing direction, reset the validation.
            this->validating_changing_direction = false;
        }
    }
}

void Coded_vinyl::store_and_return_speed(float &speed)
{
    if (speed != NO_NEW_SPEED_FOUND) // A new speed was found, use and store it.
    {
        this->old_speed                  = speed;
        this->current_speed              = speed;
        this->no_new_speed_found_counter = 0;
    }
    else // No speed was found, use the old one (but after 3 times, switch to 0).
    {
        // After 3 times with no new speed, switch to 0.
        this->no_new_speed_found_counter++;
        if (this->no_new_speed_found_counter > MAX_NO_NEW_SPEED_FOUND)
        {
            speed               = 0.0;
            this->old_speed     = speed;
            this->current_speed = speed;
            this->no_new_speed_found_counter = 0;
        }
        else
        {
            speed = this->old_speed;
        }
    }
}

void Coded_vinyl::smoothly_change_speed(float &speed)
{
    if ((this->old_speed != NO_NEW_SPEED_FOUND) && (speed != NO_NEW_SPEED_FOUND))
    {
        float diff = speed - this->old_speed;
        if ((qAbs(diff) < 0.1) && (this->old_speed * speed > 0.0f)) // same sign and diff not more than 1%
        {
            // Only change speed by half of the diff with the previous speed.
            diff = diff / 2.0f;
            speed = speed + diff;
        }
    }
}

void Coded_vinyl::fill_zero_cross_list(vector< pair<bool, unsigned int> > &zero_cross_list, vector<float> &samples)
{
    qCDebug(DSLIB_ANALYZEVINYL) << "Fill list of zero crossing...";

    // Create the zero crossing line.
    vector<float> zero_crossing_line(samples.size(), 0.0f);

    // Iterate over 2 sample channels, get size of sinusoidal wave areas by catching samples crossing zero.
    float sample = 0.0;
    bool  wait_zero_crossing_up   = false;
    bool  wait_zero_crossing_down = false;
    for (unsigned int i = 0; i < samples.size(); i++)
    {
        // Get sample value.
        sample = samples[i];

        if (sample != UNKNOWN_SAMPLE_VALUE)
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

void Coded_vinyl::center_signal(vector<float> &samples)
{
    // Check if true amplitude is not close to 0.
    float amplitude = this->get_signal_amplitude(samples);
    this->last_signal_was_centered = false;
    if (qAbs(amplitude) > 0.25) // Only center signal if amplitude is more than 0.25 or -0.25
    {
        // Amplify the signal.
        for (unsigned int i = 0; i < samples.size(); i++)
        {
            if (samples[i] != UNKNOWN_SAMPLE_VALUE)
            {
                samples[i] -= amplitude;
                // Clip if necessary.
                if (samples[i] >= 1.0)
                {
                    samples[i] = 0.99f;
                }
                else if (samples[i] <= -1.0)
                {
                    samples[i] = -0.99f;
                }
            }
        }
        this->last_signal_was_centered = true;
    }
}

void Coded_vinyl::amplify_and_clip_signal(float symetric_amp, vector<float> &samples)
{
    // For each samples, if it is more than the symetrical limit, change it to the max value, else to the min value.
    for (unsigned int i = 0; i < samples.size(); i++)
    {
        if (samples[i] != UNKNOWN_SAMPLE_VALUE)
        {
            if (samples[i] >= symetric_amp)
            {
                samples[i] = 0.99f;
            }
            else
            {
                samples[i] = -0.99f;
            }
        }
    }  
}

float Coded_vinyl::calculate_speed()
{
    // Make a speed average of both sample tables.
    float speed       = 0.0;
    float tmp         = 0.0;
    int   div         = 0;
    bool  speed_found = false;

    tmp = this->calculate_average_speed_one_channel(this->zero_cross_list_1);
    if (tmp != NO_NEW_SPEED_FOUND)
    {
        speed = tmp;
        div++;
        speed_found = true;
    }

    tmp = this->calculate_average_speed_one_channel(this->zero_cross_list_2);
    if (tmp != NO_NEW_SPEED_FOUND)
    {
        speed += tmp;
        div++;
        speed_found = true;
    }
    if (div > 1)
    {
        speed = speed / (float)div;
    }

    // Validate speed against average signal amplitude.
    if (speed_found == true)
    {
        speed_found = this->validate_and_adjust_speed_against_amplitude(speed);
    }

    // Return speed if found and validated.
    if (speed_found == true)
    {
        // Calculate direction.
        short int direction = this->calculate_direction();
        return direction * speed;
    }
    else
    {
        return NO_NEW_SPEED_FOUND;
    }
}

float Coded_vinyl::calculate_average_speed_one_channel(vector< pair<bool, unsigned int> > &zero_cross_list)
{
    // We need at least one period, so 3 zero crossing to find a speed.
    if (zero_cross_list.size() < 3)
    {
        return NO_NEW_SPEED_FOUND;
    }

    // Iterate over ranges of zero crossing and calculate the speed.
    float speed = 0.0;
    int   div   = 0;
    for (unsigned int i = 0; i < zero_cross_list.size() - 1; i++)
    {
        unsigned int range = zero_cross_list[i+1].second - zero_cross_list[i].second;
        speed += (float)range;
        div++;
    }
    if (div >= 1)
    {
        speed = this->sin_wave_area_size / (speed / (float)div);
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

bool Coded_vinyl::validate_and_adjust_speed_against_amplitude(float &speed)
{
    // Get average signal amplitude.
    float amplitude = (this->get_signal_average_amplitude(this->samples_channel_1)
                      + this->get_signal_average_amplitude(this->samples_channel_2)) / 2.0f;
    
    // Validate "normal" speed (more than 0.30) against signal amplitude.
    if (qAbs(speed) > 0.30)
    {
        if (amplitude < this->get_min_amplitude_for_normal_speed())
        {
            // This speed looks wrong because the amplitude is too small.
            speed = NO_NEW_SPEED_FOUND;
            return false;
        }
    }
    else
    {
        // In any case, if amplitude is less than a min value, we consider there is no signal.
        if (amplitude < this->get_min_amplitude())
        {
            speed = 0.0;
        }
    }

    return true;
}

void Coded_vinyl::keep_unused_samples()
{
    if (this->last_signal_was_centered == false)
    {
        if ((this->zero_cross_list_1.size() >= 3) && (this->zero_cross_list_2.size() >= 3))
        {
            // At least one period was analyzed.
            this->remove_used_samples(this->zero_cross_list_1, this->samples_channel_1);
            this->remove_used_samples(this->zero_cross_list_2, this->samples_channel_2);
            this->align_samples();
        }
        // Else keep all samples for the next analyzis.
    }
    else
    {
        // In case the signal was centered, do not use old samples.
        this->samples_channel_1.clear();
        this->samples_channel_2.clear();
    }

    // Clear zero cross list.
    this->zero_cross_list_1.clear();
    this->zero_cross_list_2.clear();
}

void Coded_vinyl::remove_used_samples(vector< pair<bool, unsigned int> > &zero_cross_list, vector<float> &samples)
{
    // Keep the last zero crossing (under 3 zero crossing, i.e. a period, they haven't be used).
    if (zero_cross_list.size() >= 3)
    {
        zero_cross_list.erase(zero_cross_list.begin(), zero_cross_list.end() - 1);
    }

    // Keep last sample area (after the zero crossing).
    if ((zero_cross_list.size() > 0) &&
        (((int)zero_cross_list[0].second - 1) >= 0))
    {
        samples.erase(samples.begin(), samples.begin() + (zero_cross_list[0].second - 1));
    }
    else
    {
        // In any other cases, clean the sample table.
        samples.clear();
    }
}

void Coded_vinyl::align_samples()
{
    // Add -99 at the beginning of the smallest sample table (to get the same size for both channels).
    if (this->samples_channel_1.size() < this->samples_channel_2.size())
    {
        unsigned int diff = this->samples_channel_2.size() - this->samples_channel_1.size();
        vector<float> tmp(diff, UNKNOWN_SAMPLE_VALUE);
        this->samples_channel_1.insert(this->samples_channel_1.begin(), tmp.begin(), tmp.end());
    }
    else if (this->samples_channel_2.size() < this->samples_channel_1.size())
    {
        unsigned int diff = this->samples_channel_1.size() - this->samples_channel_2.size();
        vector<float> tmp(diff, UNKNOWN_SAMPLE_VALUE);
        this->samples_channel_2.insert(this->samples_channel_2.begin(), tmp.begin(), tmp.end());
    }
}

float Coded_vinyl::get_signal_average_amplitude(vector<float> &samples)
{
    // Get the average amplitude of the signal (only abosoute value)
    float amplitude  = 0.0;
    unsigned int div = 0;
    for (unsigned int i = 0; i < samples.size(); i++)
    {
        if (samples[i] != UNKNOWN_SAMPLE_VALUE)
        {
            amplitude += qAbs(samples[i]);
            div++;
        }
    }
    
    return amplitude / (float)div;
}

float Coded_vinyl::get_signal_amplitude(vector<float> &samples)
{
    // Get the amplitude of the signal (a true symetrical sinusoidal signal should return 0.0).
    float amplitude  = 0.0;
    unsigned int div = 0;
    for (unsigned int i = 0; i < samples.size(); i++)
    {
        if (samples[i] != UNKNOWN_SAMPLE_VALUE)
        {
            amplitude += samples[i];
            div++;
        }
    }
    
    return amplitude / (float)div;
}

float Coded_vinyl::get_volume()
{
    qCDebug(DSLIB_ANALYZEVINYL) << "Searching new volume...";

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
        qCCritical(DSLIB_ANALYZEVINYL) << "sample_rate cannot be <= 0";

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
        qCCritical(DSLIB_ANALYZEVINYL) << "input amplify coeff cannot be <= 0";

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

void Coded_vinyl::set_min_amplitude_for_normal_speed(float amplitude)
{
    this->min_amplitude_for_normal_speed = amplitude;
    return;
}

float Coded_vinyl::get_min_amplitude_for_normal_speed()
{
    return this->min_amplitude_for_normal_speed;
}

void Coded_vinyl::set_min_amplitude(float amplitude)
{
    this->min_amplitude = amplitude;
    return;
}

float Coded_vinyl::get_min_amplitude()
{
    return this->min_amplitude;
}
