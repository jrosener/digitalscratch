/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------( digital_scratch.cpp )-*/
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
/*        Digital_scratch class : define a Digital_scratch controller         */
/*                                                                            */
/*============================================================================*/

#include <iostream>
#include <cstdio>
#include <string>
#include <iterator>

using namespace std;

#include "include/dscratch_parameters.h"
#include "include/utils.h"
#include "include/digital_scratch_api.h"
#include "include/digital_scratch.h"

Digital_scratch::Digital_scratch(string controller_name,
                                 string coded_vinyl_type) : Controller(controller_name)
{
    Utils::trace_object_life(TRACE_PREFIX_DIGITALSCRATCH,
                             "+ Creating Digital_scratch object...");

    // Init.
    this->init(coded_vinyl_type);


    Utils::trace_object_life(TRACE_PREFIX_DIGITALSCRATCH,
                             "+ Digital_scratch object created");
}

bool Digital_scratch::init(string coded_vinyl_type)
{
    // Internal parameters.
    this->is_position_detection_enabled = 0;

    this->speeds_for_stability = NULL;

    this->old_speed = 0.0;
    this->nb_speed  = 0;

    this->nb_buffer               = 0;
    this->speed_state             = UNSTABLE_SPEED;
    this->is_waiting_other_buffer = false;
    this->set_playing_parameters_ready(false);

    // Set default value for public parameters.
    this->set_max_speed_diff(DEFAULT_MAX_SPEED_DIFF);
    this->set_max_slow_speed(DEFAULT_MAX_SLOW_SPEED);
    this->set_max_nb_buffer(DEFAULT_MAX_NB_BUFFER);
    this->set_max_nb_speed_for_stability(DEFAULT_MAX_NB_SPEED_FOR_STABILITY);

    this->total_input_samples_1.reserve(512 * this->get_max_nb_buffer());
    this->total_input_samples_2.reserve(512 * this->get_max_nb_buffer());

    this->vinyl = NULL;
    if (coded_vinyl_type == FINAL_SCRATCH_VINYL)
    {
        this->vinyl = new Final_scratch_vinyl();
    }
    else if (coded_vinyl_type == SERATO_VINYL)
    {
       this->vinyl = new Serato_vinyl();
    }
    else if (coded_vinyl_type == MIXVIBES_VINYL)
    {
       this->vinyl = new Mixvibes_vinyl();
    }
    else
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCH, "Cannot create Digital_scratch object using NULL vinyl.");
        return false;
    }

    return true;
}

Digital_scratch::~Digital_scratch()
{
    Utils::trace_object_life(TRACE_PREFIX_DIGITALSCRATCH,
                             "- Deleting Digital_scratch object...");

    // Cleanup.
    this->clean();

    Utils::trace_object_life(TRACE_PREFIX_DIGITALSCRATCH,
                             "- Digital_scratch object deleted");
}

void Digital_scratch::clean()
{
    if (this->vinyl != NULL)
    {
        delete this->vinyl;
    }
    if (this->speeds_for_stability != NULL)
    {
        delete [] this->speeds_for_stability;
    }
    this->delete_total_input_samples();
}

bool Digital_scratch::analyze_recording_data(vector<float> &input_samples_1,
                                             vector<float> &input_samples_2)
{
    if ((input_samples_1.size() == 0)
       || (input_samples_1.size() != input_samples_2.size()))
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCH,
                    "Wrong input samples table sizes");
                return false;
    }

    // The goal of this method is to analyze input datas, calculate playing
    // parameters and then say if yes or no we are ready to use these
    // values to control the player.
    this->set_playing_parameters_ready(false);

    switch(this->speed_state)
    {
        case UNSTABLE_SPEED:
//cout << "UNSTABLE" << endl;
            if (this->analyze_recording_data_unstable_speed(input_samples_1,
                                                            input_samples_2) == false)
            {
                Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCH,
                    "Cannot analyze recording data in case of unstable speed.");
                return false;
            }
            break;

        case STABLE_SPEED:
//cout << "STABLE" << endl;
            if (this->analyze_recording_data_stable_speed(input_samples_1,
                                                          input_samples_2) == false)
            {
                Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCH,
                    "Cannot analyze recording data in case of stable speed.");
                return false;
            }
            break;

        case SLOW_SPEED:
//cout << "SLOW" << endl;
            if (this->analyze_recording_data_slow_speed(input_samples_1,
                                                        input_samples_2) == false)
            {
                Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCH,
                    "Cannot analyze recording data in case of stable speed.");
                return false;
            }
            break;
    }
//cout << "buffer size = " << this->total_input_samples_length << endl;
    return true;
}

Coded_vinyl* Digital_scratch::get_coded_vinyl()
{
    return this->vinyl;
}

bool Digital_scratch::change_coded_vinyl(string coded_vinyl_type)
{
    // First clean all in Digital_scratch object.
    this->clean();

    // Then create a new coded vinyl.
    return this->init(coded_vinyl_type);
}

bool Digital_scratch::set_max_speed_diff(float diff)
{
    if (diff <= 0.0)
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCH,
                           "max_speed_diff can not be <= 0.0");
        return false;
    }

    this->max_speed_diff = diff;

    return true;
}

float Digital_scratch::get_max_speed_diff()
{
    return this->max_speed_diff;
}

bool Digital_scratch::set_max_slow_speed(float slow_speed)
{
    if (slow_speed <= 0.0)
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCH,
                           "max_slow_speed can not be <= 0.0");
        return false;
    }

    this->max_slow_speed = slow_speed;

    return true;
}

float Digital_scratch::get_max_slow_speed()
{
    return this->max_slow_speed;
}

bool Digital_scratch::set_max_nb_buffer(int nb)
{
    if (nb <= 0)
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCH,
                           "max_nb_buffer can not be <= 0.0");
        return false;
    }

    this->max_nb_buffer = nb;

    return true;
}

int Digital_scratch::get_max_nb_buffer()
{
    return this->max_nb_buffer;
}

bool Digital_scratch::set_max_nb_speed_for_stability(int nb)
{
    if (nb <= 0)
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCH,
                           "max_nb_speed_for_stability can not be <= 0.0");
        return false;
    }

    // Delete speeds_for_stability table if it already exists.
    if (this->speeds_for_stability != NULL)
    {
        delete [] this->speeds_for_stability;
    }

    // Create table of speeds.
    this->speeds_for_stability        = new float [nb];
    this->speeds_for_stability_length = 0;

    // Store max_nb_speed_for_stability.
    this->max_nb_speed_for_stability = nb;

    return true;
}

int Digital_scratch::get_max_nb_speed_for_stability()
{
    return this->max_nb_speed_for_stability;
}

bool Digital_scratch::enable_position_detection(bool is_enabled)
{
    this->is_position_detection_enabled = is_enabled;

    return true;
}

bool Digital_scratch::get_position_detection_state()
{
    return this->is_position_detection_enabled;
}

void Digital_scratch::calculate_speed()
{
    this->speed->set_value(this->vinyl->get_speed());
}

void Digital_scratch::calculate_average_speed()
{
    float average_speed = 0.0;

    // Get average speed from speeds_for_stability table.
    if (this->speeds_for_stability_length > 0)
    {
        for (int i = 0; i < this->speeds_for_stability_length; i++)
        {
            average_speed += this->speeds_for_stability[i];
        }
        average_speed /= this->speeds_for_stability_length;
    }

    this->speed->set_value(average_speed);
}

float Digital_scratch::get_speed()
{
    return this->speed->get_value();
}

void Digital_scratch::calculate_position()
{
    this->position->set_value(this->vinyl->get_position());
}

float Digital_scratch::get_position()
{
    return this->position->get_value();
}

void Digital_scratch::calculate_volume()
{
    this->volume->set_value(this->vinyl->get_volume());
}

float Digital_scratch::get_volume()
{
    return this->volume->get_value();
}

bool Digital_scratch::store_speed_for_stability()
{
    // Copy speed value in a table used to calculate speed stability.
    if (this->speeds_for_stability_length < this->get_max_nb_speed_for_stability())
    {
        this->speeds_for_stability_length++;
        this->speeds_for_stability[this->speeds_for_stability_length-1]
            = this->get_speed();
    }
    else
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCH,
            "Cannot access to element of speed table used for stability");
        return false;
    }

    return true;
}

bool Digital_scratch::is_speed_stable()
{
    float interval         = 0.0;
    float highest_interval = 0.0;
    int   i                = 0;
    int   nb_speed_zero    = 0;

    if (this->speeds_for_stability_length > 1)
    {
        // Special case: if all speeds are equal to 0 then speed is not stable.
        // Special case: if one of the speed is NO_NEW_SPEED_FOUND then speed
        //               is not stable.
        for (i = 0; i < this->speeds_for_stability_length; i++)
        {
            if (this->speeds_for_stability[i] == NO_NEW_SPEED_FOUND)
            {
                return false;
            }
            if (this->speeds_for_stability[i] == 0)
            {
                nb_speed_zero++;
            }
        }
        if (nb_speed_zero == this->speeds_for_stability_length)
        {
            return false;
        }

        // Select the highest speed interval (between elements of
        // speeds_for_stability table).
        for (i = 0; i < this->speeds_for_stability_length-1; i++)
        {
            interval = Utils::abs(this->speeds_for_stability[i]
                                  - this->speeds_for_stability[i+1]);
            if (interval > highest_interval)
            {
                highest_interval = interval;
            }
        }
    }
    else
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCH,
            "Cannot say if speed is stable because there is no speed to compute");
        return false;
    }

    // Compare this interval to max acceptable difference of speed.
    if (highest_interval > this->get_max_speed_diff())
    {
        return false;
    }

    return true;
}

bool Digital_scratch::is_speed_slow()
{
    if ((this->get_speed() != NO_NEW_SPEED_FOUND)
        && (Utils::abs(this->get_speed()) < this->get_max_slow_speed()))
    {
        return true;
    }

    return false;
}

bool Digital_scratch::add_new_input_samples(vector<float> &input_samples_1,
                                            vector<float> &input_samples_2)
{
    // Check if total_input_samples tables are large enough.
    if (this->total_input_samples_1.capacity()
        < (input_samples_1.size() * this->get_max_nb_buffer()))
    {
        this->total_input_samples_1.reserve(input_samples_1.size() * this->get_max_nb_buffer());
        this->total_input_samples_2.reserve(input_samples_2.size() * this->get_max_nb_buffer());
    }

    // Add new input samples at the end of total samples tables.
    copy(input_samples_1.begin(), input_samples_1.end(), back_inserter(this->total_input_samples_1));
    copy(input_samples_2.begin(), input_samples_2.end(), back_inserter(this->total_input_samples_2));

    return true;
}

bool Digital_scratch::delete_total_input_samples()
{
    this->total_input_samples_1.clear();
    this->total_input_samples_2.clear();

    return true;
}

bool Digital_scratch::is_old_speed_close_to_current()
{
    // Check if we can work with old and new speeds.
    if (this->old_speed == NO_NEW_SPEED_FOUND)
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCH,
            "Cannot say if current and new speeds are closed because old speed is not defined.");
        return false;
    }
    if (this->get_speed() == NO_NEW_SPEED_FOUND)
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCH,
            "Cannot say if current and new speeds are closed because current speed is not defined.");
        return false;
    }

    // Compare interval between current and old speed to an harcoded value.
    if(Utils::abs(this->get_speed() - this->old_speed) > this->get_max_speed_diff())
    {
        // Old speed is not closed to current speed.
        return false;
    }

    // Current and new speeds are closed.
    return true;
}

void Digital_scratch::calculate_position_and_volume()
{
    if (this->get_speed() != NO_NEW_SPEED_FOUND) // a new speed is found
    {
        if (this->is_position_detection_enabled == 1)
        {
            // Try to get position.
            this->calculate_position();
        }

        // Try to get volume.
        this->calculate_volume();
    }

}

bool Digital_scratch::analyze_recording_data_unstable_speed(vector<float> &input_samples_1,
                                                            vector<float> &input_samples_2)
{
    // Provide data to coded vinyl.
    this->vinyl->add_sound_data(input_samples_1,
                                input_samples_2);

    // Get speed from coded vinyl.
    this->calculate_speed();

    // Store speed in a table used to find if speed is stable or not.
    if (this->store_speed_for_stability() == false)
    {
        Utils::trace_error(TRACE_PREFIX_DIGITALSCRATCH,
            "Cannot store speed in speed table used for stability.");
        this->nb_speed = 0;
        this->speeds_for_stability_length = 0;

        return false;
    }
    this->nb_speed++;

    // Should we wait for other speed to find stability ?
    if (this->nb_speed < this->get_max_nb_speed_for_stability())
    {
        // Drive player with current speed. We are still in UNSTABLE_SPEED.
        this->speed_state = UNSTABLE_SPEED;
        this->calculate_position_and_volume();
        this->set_playing_parameters_ready(true);
    }
    else
    {
        // There are enough speeds to check if speed is stable or not.
        this->nb_speed = 0;

        // First of all, try to find if speed is slow.
        if (this->is_speed_slow() == true)
        {
            // Drive player with average speed. Let's change to SLOW_SPEED.
            this->calculate_average_speed();
            this->speed_state = SLOW_SPEED;
            this->calculate_position_and_volume();
            this->set_playing_parameters_ready(true);
        }
        else
        {
            // Try to find if speeds are stable.
            if (this->is_speed_stable() == true)
            {
                // Let's drive player with average speed. Now we are in STABLE_SPEED.
                this->calculate_average_speed();
                this->speed_state = STABLE_SPEED;
                this->calculate_position_and_volume();
                this->set_playing_parameters_ready(true);
            }
            else
            {
                // Drive player with current speed. We are still in UNSTABLE_SPEED.
                this->speed_state = UNSTABLE_SPEED;
                this->calculate_position_and_volume();
                this->set_playing_parameters_ready(true);
            }
        }

        // Clean speed table used for stability.
        this->speeds_for_stability_length = 0;
    }

    return true;
}

bool Digital_scratch::analyze_recording_data_stable_speed(vector<float> &input_samples_1,
                                                          vector<float> &input_samples_2)
{
    // Store old speed that will be used to known if new speed is close to old
    // one.
    this->old_speed = this->get_speed();

    // Accumulate buffers in total_input_buffer table.
    this->add_new_input_samples(input_samples_1,
                                input_samples_2);
    this->nb_buffer++;

    // Should we wait for other buffers ?
    if (this->nb_buffer < this->get_max_nb_buffer())
    {
        // Let's continue with SLOW_SPEED
        this->speed_state = STABLE_SPEED;
    }
    else
    {
        // We have accumulated enough buffers.
        this->nb_buffer = 0;

        // Provide datas to coded vinyl.
        this->vinyl->add_sound_data(this->total_input_samples_1,
                                    this->total_input_samples_2);

        // Clean total_input_samples tables.
        this->delete_total_input_samples();

        // Get speed from coded vinyl.
        this->calculate_speed();

        // Drive player.
        this->calculate_position_and_volume();
        this->set_playing_parameters_ready(true);

        // Find if speed is slow.
        if (this->is_speed_slow() == true)
        {
            this->speed_state = SLOW_SPEED;
        }
        else
        {
            if (this->is_old_speed_close_to_current() == true)
            {
                // New speed is almost the same as old one.
                this->speed_state = STABLE_SPEED;
            }
            else
            {
                this->speed_state = UNSTABLE_SPEED;
            }
        }
    }

    return true;
}

bool Digital_scratch::analyze_recording_data_slow_speed(vector<float> &input_samples_1,
                                                        vector<float> &input_samples_2)
{
    // Get number of buffer to wait.
    if (this->get_speed() != NO_NEW_SPEED_FOUND)
    {
        if (this->is_waiting_other_buffer == false)
        {
            this->is_waiting_other_buffer = true;
            this->max_nb_buffer = (int)(this->get_max_nb_buffer() / (1 + this->get_speed()));
        }
    }
    else
    {
        this->max_nb_buffer = this->get_max_nb_buffer();
    }

    // Accumulate buffers in total_input_buffer table.
    this->add_new_input_samples(input_samples_1,
                                input_samples_2);
    this->nb_buffer++;

    // Should we wait for other buffers ?
    if (this->nb_buffer < max_nb_buffer)
    {
        // Let's continue with STABLE_SPEED
        this->speed_state = SLOW_SPEED;
    }
    else
    {
        // We have accumulated enough buffers.
        this->nb_buffer = 0;
        this->is_waiting_other_buffer = false;

        // Provide datas to coded vinyl.
        this->vinyl->add_sound_data(this->total_input_samples_1,
                                    this->total_input_samples_2);

        // Clean total_input_samples tables.
        this->delete_total_input_samples();

        // Get speed from coded vinyl.
        this->calculate_speed();

        // Drive player.
        this->calculate_position_and_volume();
        this->set_playing_parameters_ready(true);

        // Find if speed is slow.
        if (this->is_speed_slow() == true)
        {
            this->speed_state = SLOW_SPEED;
        }
        else
        {
            this->speed_state = UNSTABLE_SPEED;
        }
    }

    return true;
}
