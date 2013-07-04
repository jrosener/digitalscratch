/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                                                                            */
/*                                                                            */
/*------------------------------------------------------( digital_scratch.h )-*/
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

#ifndef _DIGITAL_SCRATCH_H_
#define _DIGITAL_SCRATCH_H_

#include <string>
#include "dscratch_parameters.h"
#include "controller.h"
#include "coded_vinyl.h"
#include "final_scratch_vinyl.h"
#include "serato_vinyl.h"
#include "mixvibes_vinyl.h"

#ifdef DISPLAY_CODE_LINE
    #define TRACE_PREFIX_DIGITALSCRATCH "[Digital_scratch:" \
                                         + Utils::to_string(__LINE__) \
                                         + "]\t\t" + this->get_name() \
                                         + " : "
#else
    #define TRACE_PREFIX_DIGITALSCRATCH "[Digital_scratch]\t\t" \
                                        + this->get_name() \
                                        + " : "
#endif

// Speed states
#define UNSTABLE_SPEED 0
#define STABLE_SPEED   1
#define SLOW_SPEED     2

// Default values
#define DEFAULT_MAX_SPEED_DIFF             0.05f
#define DEFAULT_MAX_SLOW_SPEED             0.5f
#define DEFAULT_MAX_NB_BUFFER              5
#define DEFAULT_MAX_NB_SPEED_FOR_STABILITY 3

/**
 * Define a Digital_scratch class.\n
 * Base class : Controller\n
 * It implement a Controller class.
 * @author Julien Rosener
 */
class Digital_scratch : public Controller
{
    /* Attributes */
    private:
        /**
         * Coded vinyl object pointer.
         */
        Coded_vinyl *vinyl;

        int is_position_detection_enabled;
        float player_track_length;
        float player_current_position;
        int confirm_position_jump;

        float *speeds_for_stability;
        int speeds_for_stability_length;

        /**
         * Table containing the concatenation of input sample buffers 1
         */
        vector<float> total_input_samples_1;

        /**
         * Table containing the concatenation of input sample buffers 2
         */
        vector<float> total_input_samples_2;

        float old_speed;

        int nb_speed;
        int nb_buffer;
        int speed_state;
        bool is_waiting_other_buffer;

        int max_nb_speed_for_stability;
        int max_nb_buffer;
        float max_speed_diff;
        float max_slow_speed;

    /* Constructor / Destructor */
    public:
        /**
         * Constructor.
         * @param controller_name is the name of this DigitalScratch controller.
         * @param timecoded_vinyl is the Coded_vinyl object used with
         *        Digital_scratch (e.g. Final_scratch_vinyl)
         */
        Digital_scratch(string controller_name,
                        string coded_vinyl_type);

        /**
         * Destructor
         */
        virtual ~Digital_scratch();


    /* Methods */
    public:
        /**
         * Analyze recording datas and update playing parameters.\n
         * Define the pure virtual method in base class (Controller).
         * @param input_samples_1 are the samples of channel 1.
         * @param input_samples_2 are the samples of channel 2.
         * @return TRUE if all is OK, otherwise FALSE.
         *
         * @note input_samples_1 and input_samples_2 must have the same number
         *       of elements.
         */
        bool analyze_recording_data(vector<float> &input_samples_1,
                                    vector<float> &input_samples_2);

        /**
         * Set the maximum speed difference that we allow.
         * @param diff is max_speed_diff (must be > 0.0).
         * @return TRUE if all is OK, otherwise FALSE.
         */
        bool set_max_speed_diff(float diff);

        /**
         * Get the maximum speed difference that we allow.
         * @return max_speed_diff.
         */
        float get_max_speed_diff();

        /**
         * Set the minimum speed we will accept.
         * @param slow_speed is max_slow_speed (must be > 0.0).
         * @return TRUE if all is OK, otherwise FALSE.
         */
        bool set_max_slow_speed(float slow_speed);

        /**
         * Get the the minimum speed we will accept.
         * @return max_slow_speed.
         */
        float get_max_slow_speed();

        /**
         * Set the maximum number of buffer we will accumulate.
         * @param nb is max_nb_buffer (must be > 0).
         * @return TRUE if all is OK, otherwise FALSE.
         */
        bool set_max_nb_buffer(int nb);

        /**
         * Get the maximum number of buffer we will accumulate.
         * @return max_nb_buffer.
         */
        int get_max_nb_buffer();

        /**
         * Set the maximum number of speed we will use for stability algorythm.
         * @param nb is max_nb_speed_for_stability (must be > 0).
         * @return TRUE if all is OK, otherwise FALSE.
         */
        bool set_max_nb_speed_for_stability(int nb);

        /**
         * Get the maximum number of speed we will use for stability algorythm.
         * @return max_nb_speed_for_stability.
         */
        int get_max_nb_speed_for_stability();

        /**
         * Set detection of position.
         * @param is_enabled must be TRUE to enable position detection.
         * @return TRUE if all is OK, otherwise FALSE.
         */
        bool enable_position_detection(bool is_enabled);

        /**
         * True if detection of position is on.
         * @return TRUE if detection of position is on.
         */
        bool get_position_detection_state();

        /**
         * Get Coded_vinyl object.
         */
        Coded_vinyl* get_coded_vinyl();

        /**
         * Get Coded_vinyl object.
         */
        bool change_coded_vinyl(string coded_vinyl_type);

    private:
        bool init(string coded_vinyl_type);
        void clean();

        void  calculate_speed();
        void  calculate_average_speed();
        float get_speed();

        void  calculate_position();
        float get_position();

        void  calculate_volume();
        float get_volume();

        void calculate_position_and_volume();

        bool store_speed_for_stability();
        bool is_speed_stable();
        bool is_speed_slow();
        bool is_old_speed_close_to_current();

        bool add_new_input_samples(vector<float> &input_samples_1,
                                   vector<float> &input_samples_2);
        bool delete_total_input_samples();

        bool analyze_recording_data_unstable_speed(vector<float> &input_samples_1,
                                                   vector<float> &input_samples_2);

        bool analyze_recording_data_slow_speed(vector<float> &input_samples_1,
                                               vector<float> &input_samples_2);

        bool analyze_recording_data_stable_speed(vector<float> &input_samples_1,
                                                 vector<float> &input_samples_2);
};

#endif //_DIGITAL_SCRATCH_H_
