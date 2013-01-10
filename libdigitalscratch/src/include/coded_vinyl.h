/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------( coded_vinyl.h )-*/
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
/*            Coded_vynil class : define a coded vinyl disk                   */
/*                                                                            */
/*============================================================================*/

#ifndef _CODED_VINYL_H_
#define _CODED_VINYL_H_

#include <string>
#include <vector>
#include "dscratch_parameters.h"
#include "digital_scratch_api.h"

#ifdef DISPLAY_CODE_LINE
    #define TRACE_PREFIX_CODED_VINYL "[Coded_vinyl:" + Utils::to_string(__LINE__) + "]\t\t"
#else
    #define TRACE_PREFIX_CODED_VINYL "[Coded_vinyl]\t\t\t"
#endif

#define UNDEFINE   0
#define MIN        1
#define MIN_SEARCH 2
#define MAX        3
#define MAX_SEARCH 4

#define DEFAULT_EXTREME_MIN                        0.01
#define DEFAULT_MAX_BUFFER_COEFF                   3
#define DEFAULT_NB_CYCLE_BEFORE_CHANGING_DIRECTION 1
#define DEFAULT_COEFF_RIGHT_DIST_MIN_BIT1_TO_BIT1  0.90
#define DEFAULT_COEFF_LEFT_DIST_MIN_BIT1_TO_BIT1   0.70
#define DEFAULT_COEFF_RIGHT_DIST_MAX_BIT0_TO_BIT0  0.50
#define DEFAULT_COEFF_LEFT_DIST_MAX_BIT0_TO_BIT0   0.40
#define DEFAULT_PROGRESSIVE_VOLUME_COEFF           1000.0
#define DEFAULT_FULL_VOLUME_AMPLITUDE              0.22
#define DEFAULT_LOW_PASS_FILTER_MAX_SPEED_USAGE    1.1
#define DEFAULT_SAMPLE_RATE                        44100

/**
 * Define a Coded_vinyl class.\n
 * A coded vinyl is the definition of a vinyl disc with a timecoded signal.
 * @author Julien Rosener
 */
class Coded_vinyl
{
    /* Attributes */
    private:
        /**
         * Structure to define an extreme entities, an extreme is a max or a min
         * of capture sample value (i.e. a max or min of a sinusoidal wave)
         */
        typedef struct extreme_struct
        {
            /**
             * Extreme index in the xxx_channel_extreme_list
             */
            short int index;
            /**
             * Extreme value
             */
            float value;
            /**
             * Extreme bit value (useful for positionning system)
             */
            short int bit_value;
            /**
             * This is the type of the Extreme : MAX, MIN, MAX_SEARCH or
             * MIN_SEARCH)
             */
            char type;
        } extreme_t;

        /**
         * A list of extremes corresponding to left channel captured datas
         */
        vector<extreme_t> left_channel_extreme_list;

        /**
         * A list of extremes corresponding to right channel captured datas
         */
        vector<extreme_t> right_channel_extreme_list;

        /**
         * Structure describing a bit used for position encoding.
         */
        typedef struct bit_struct
        {
            /**
             * Extreme value
             */
            float amplitude;
            /**
             * Extreme bit value
             */
            char value;
        } bit_t;

        /**
         * Current speed of Final-Scratch vinyl
         */
        float current_speed;

        /**
         * The playback vinyl direction (1:forward, -1:backward)
         */
        short int direction;

        /**
         * When direction change, number of consecutive same direction
         */
        short int nb_same_direction_change;

        /**
         * True if direction is changing, otherwise false
         */
        bool direction_is_changing;

        /**
         * Minimum acceptable value for recording sample
         */
        float extreme_min;

        /**
         * (max_buffer_coeff x capture_buffer_size) is the max acceptable number
         * of samples analyzed ate the same time
         */
        int max_buffer_coeff;

        /**
         * Number of cycle (motion detection) before changing direction (after a
         * new direction detection)
         */
        int nb_cycle_before_changing_direction;

        /**
         * If true : there is no input signal from turntable
         */
        bool no_input_signal;

        /**
         * Store the latest available detected speed
         */
        float latest_available_detected_speed;

        /**
         * Current number of buffer to wait before starting analyzis
         */
        short int nb_buffer_to_wait;

        /**
         * True if we are waiting for other buffers before starting analyzis
         */
        bool waiting_other_buffer;

        /**
         * Table containing the concatenation of input sample buffers 1
         */
        vector<float> total_input_samples_1;

        /**
         * Table containing the concatenation of input sample buffers 2
         */
        vector<float> total_input_samples_2;

        /**
         * Minimal distance beetween bit 1 and another bit 1 (right channel)
         */
        float right_dist_min_bit1_to_bit1;
        float coeff_right_dist_min_bit1_to_bit1;

        /**
         * Minimal distance beetween bit 1 and another bit 1 (left channel)
         */
        float left_dist_min_bit1_to_bit1;
        float coeff_left_dist_min_bit1_to_bit1;

        /**
         * Maximal distance beetween bit 0 and another bit 0 (right channel)
         */
        float right_dist_max_bit0_to_bit0;
        float coeff_right_dist_max_bit0_to_bit0;

        /**
         * Maximal distance beetween bit 0 and another bit 0 (left channel)
         */
        float left_dist_max_bit0_to_bit0;
        float coeff_left_dist_max_bit0_to_bit0;

        /**
         * Store the latest volume value.
         */
        float old_volume;

        /**
         * Coefficient used in progressive algorithm for volume changes.
         */
        float progressive_volume_coeff;

        /**
         * Amplitude (average input samples) value correponding to volume max.
         */
        float full_volume_amplitude;

        /**
         * Max speed under which low pass filter is applied on input data.
         */
        float low_pass_filter_max_speed_usage;

        /**
         * Sample rate of input data.
         */
        int sample_rate;

        /**
         * Boolean used to reverse direction (ex: Mixvibes vinyl).
         */
        bool is_reverse_direction;

    public: // TODO: add accessor and put attributes in private
        /**
         * A list of bits used to detect position (true = 1, false = 0)
         */
        vector<bit_t> bit_list;

        /**
         * Flag to know if we were using a bit from right channel
         */
        bool latest_was_right;


    /* Constructor / Destructor */
    public:
        /**
         * Constructor
         */
        Coded_vinyl();

        /**
         * Destructor
         */
        virtual ~Coded_vinyl();


    /* Methods */
    public:
        /**
         * Get the current speed value.
         * Define the pure virtual method in base class (Coded_vinyl)
         * @return the speed value or NO_NEW_SPEED_FOUND if nothing is found
         */
        float get_speed();

        /**
         * Get the current volume value.
         * Define the pure virtual method in base class (Coded_vinyl)
         * @return the volume value or NO_NEW_VOLUME_FOUND if nothing is found
         */
        float get_volume();

        /**
         * Add and analyze channels recording datas.
         * @param input_samples_1 are the samples of channel 1.
         * @param input_samples_2 are the samples of channel 2.
         *
         * @note input_samples_1 and input_samples_2 must have the same number
         *       of element.
         */
        void add_sound_data(vector<float> &input_samples_1,
                            vector<float> &input_samples_2);

        /**
         * Say if there is input signal from turntable
         * @return true if there is no signal, otherwise false
         */
        bool is_no_input_signal();

        /**
         * Set the minimal extreme value accepted.
         * @param extreme_min is the value to set.
         * @return TRUE if all is OK, otherwise FALSE.
         */
        bool set_extreme_min(float extreme_min);

        /**
         * Get the minimal extreme value accepted.
         * @return the value to get.
         */
        float get_extreme_min();

        /**
         * Set the maximum number of input sample buffers that we accept to
         * concat.
         * @param max_buffer_coeff is the value to set.
         * @return TRUE if all is OK, otherwise FALSE.
         */
        bool set_max_buffer_coeff(int max_buffer_coeff);

        /**
         * Get the maximum number of input sample buffers that we accept to
         * concat.
         * @return the value to get.
         */
        int get_max_buffer_coeff();

        /**
         * Set the number of processing cycles on input samples used to keep old
         * direction when a new direction is detected.
         * @param nb_cycle_before_changing_direction is the value to set.
         * @return TRUE if all is OK, otherwise FALSE.
         */
        bool set_nb_cycle_before_changing_direction(int nb_cycle_before_changing_direction);

        /**
         * Get the number of processing cycles on input samples used to keep old
         * direction when a new direction is detected.
         * @return the value to get.
         */
        int get_nb_cycle_before_changing_direction();

        /**
         * Set the coefficient (in percentage) of the biggest bit1<->bit1
         * amplitude to calculate the minimum acceptable value for an interval
         * bit1<->bit1 on right channel.
         * @param coeff_right_dist_min_bit1_to_bit1 is the value to set.
         * @return TRUE if all is OK, otherwise FALSE.
         */
        bool set_coeff_right_dist_min_bit1_to_bit1(float coeff_right_dist_min_bit1_to_bit1);

        /**
         * Get the coefficient (in percentage) of the biggest bit1<->bit1
         * amplitude to calculate the minimum acceptable value for an interval
         * bit1<->bit1 on right channel.
         * @return the value to get.
         */
        float get_coeff_right_dist_min_bit1_to_bit1();

        /**
         * Set the same as coeff_right_dist_min_bit1_to_bit1 but on left channel.
         * @param coeff_left_dist_min_bit1_to_bit1 is the value to set.
         * @return TRUE if all is OK, otherwise FALSE.
         */
        bool set_coeff_left_dist_min_bit1_to_bit1(float coeff_left_dist_min_bit1_to_bit1);

        /**
         * Get the same as coeff_right_dist_min_bit1_to_bit1 but on left channel.
         * @return the value to get.
         */
        float get_coeff_left_dist_min_bit1_to_bit1();

        /**
         * Set the coefficient (in percentage) of the biggest bit0<->bit0
         * amplitude to calculate the maximum acceptable value for an interval
         * bit0<->bit0 on right channel.
         * @param coeff_right_dist_max_bit0_to_bit0 is the value to set.
         * @return TRUE if all is OK, otherwise FALSE.
         */
        bool set_coeff_right_dist_max_bit0_to_bit0(float coeff_right_dist_max_bit0_to_bit0);

        /**
         * Get the coefficient (in percentage) of the biggest bit0<->bit0
         * amplitude to calculate the maximum acceptable value for an interval
         * bit0<->bit0 on right channel.
         * @return the value to get.
         */
        float get_coeff_right_dist_max_bit0_to_bit0();

        /**
         * Set the same as coeff_right_dist_max_bit0_to_bit0 but on left channel.
         * @param coeff_left_dist_max_bit0_to_bit0 is the value to set.
         * @return TRUE if all is OK, otherwise FALSE.
         */
        bool set_coeff_left_dist_max_bit0_to_bit0(float coeff_left_dist_max_bit0_to_bit0);

        /**
         * Get the same as coeff_right_dist_max_bit0_to_bit0 but on left channel.
         * @return the value to get.
         */
        float get_coeff_left_dist_max_bit0_to_bit0();

        /**
         * Set the coefficient used to play with volume progressive algorithm.
         * @param progressive_volume_coeff is the value to set.
         * @return TRUE if all is OK, otherwise FALSE.
         */
        bool set_progressive_volume_coeff(float progressive_volume_coeff);

        /**
         * Get the coefficient used to play with volume progressive algorithm.
         * @return the value to get.
         */
        float get_progressive_volume_coeff();

        /**
         * Set the amplitude value that we consider to correspond to the
         * maximum volume.
         * @param progressive_volume_coeff is the value to set.
         * @return TRUE if all is OK, otherwise FALSE.
         */
        bool set_full_volume_amplitude(float full_volume_amplitude);

        /**
         * Get the amplitude value that we consider to correspond to the
         * maximum volume.
         * @return the value to get.
         */
        float get_full_volume_amplitude();

        /**
         * Set the sample rate used to record timecoded vinyl.
         * @param sample_rate is the value to set.
         * @return TRUE if all is OK, otherwise FALSE.
         */
        bool set_sample_rate(int sample_rate);

        /**
         * Get the sample rate used to record timecoded vinyl.
         * @return the value to get.
         */
        int get_sample_rate();

        /**
         * Set speed under which low pass filter is applied on input data.
         * @param extreme_min is the value to set.
         * @return TRUE if all is OK, otherwise FALSE.
         */
        bool set_low_pass_filter_max_speed_usage(float low_pass_filter_max_speed_usage);

        /**
         * Get speed under which low pass filter is applied on input data.
         * @return the value to get.
         */
        float get_low_pass_filter_max_speed_usage();

        /**
         * Get bit type of each bit from bit_list.
         * @return TRUE if all is OK, otherwise FALSE.
         */
        bool get_bit_types();

        /**
         * Get the current position value of needle on vinyl
         */
        virtual float get_position() = 0;

        /**
         * Get the sinusoidal frequency
         */
        virtual int get_sinusoidal_frequency() = 0;

    protected:
        /**
         * Set boolean to reverse direction (useful for Mixvibes record).
         */
        bool set_reverse_direction(bool is_reverse_direction);

        /**
         * Is reverse direction used ?.
         */
        bool get_reverse_direction();

    private:
        /**
         * Clean a specified extreme list
         * @param left_or_right is the channel to use (LEFT_CHANNEL or
         *        RIGHT_CHANNEL)
         * @return true if all is OK, otherwise false
         */
        bool extreme_list_clean(char left_or_right);

        /**
         * Add a new extreme at the end from a channel_extreme_list
         * @param left_or_right is the channel to use (LEFT_CHANNEL or
         *        RIGHT_CHANNEL)
         * @param extreme is the extreme structure to add
         * @return true if all is OK, otherwise false
         */
        bool extreme_list_push_back(char left_or_right, extreme_t *extreme);

        /**
         * Remove the last extreme from a channel_extreme_list
         * @param left_or_right is the channel to use (LEFT_CHANNEL or
         *        RIGHT_CHANNEL)
         * @return true if all is OK, otherwise false
         */
        bool extreme_list_pop_back(char left_or_right);

        /**
         * Remove the first extreme from a channel_extreme_list
         * @param left_or_right is the channel to use (LEFT_CHANNEL or
         *        RIGHT_CHANNEL)
         * @return true if all is OK, otherwise false
         */
        bool extreme_list_pop_front(char left_or_right);

        /**
         * Remove first multiple extreme from a channel_extreme_list
         * @param left_or_right is the channel to use (LEFT_CHANNEL or
         *        RIGHT_CHANNEL)
         * @param nb_element is the number of element to remove from the list
         * @return true if all is OK, otherwise false
         */
        bool extreme_list_multiple_pop_front(char left_or_right,
                                             short int nb_element);

        /**
         * Remove multiple extreme from a channel_extreme_list
         * @param left_or_right is the channel to use (LEFT_CHANNEL or
         *        RIGHT_CHANNEL)
         * @param start_index is the index to start removing extreme element
         * @param nb_element is the number of element to remove from the list
         * @return true if all is OK, otherwise false
         */
        bool extreme_list_multiple_pop(char left_or_right,
                                       short int start_index,
                                       short int nb_element);

        /**
         * Remove one extreme from a channel_extreme_list
         * @param left_or_right is the channel to use (LEFT_CHANNEL or
         *        RIGHT_CHANNEL)
         * @param index is the index to remove extreme element
         * @return true if all is OK, otherwise false
         */
        bool extreme_list_pop_at(char left_or_right, short int index);

        /**
         * Display all extremes from a channel_extreme_list
         * @param left_or_right is the channel to use (LEFT_CHANNEL or
         *        RIGHT_CHANNEL)
         * @return true if all is OK, otherwise false
         */
        bool extreme_list_display(char left_or_right);

        /**
         * Get a string with properties of a specified extreme
         * @param extreme is the extreme to get properties
         * @return a string with all properties
         */
        string get_extreme_properties(extreme_t *extreme);

        /**
         * Get the specified extreme from a channel_extreme_list
         * @param left_or_right is the channel to use (LEFT_CHANNEL or
         *        RIGHT_CHANNEL)
         * @param index is the index of the extreme in the list
         * @param extreme is a pointer on an extreme pointer, it is this extreme
         *        that will be filled
         * with the element of extreme that we search
         * @return true if all is OK, otherwise false
         */
        bool extreme_list_get_element(char left_or_right,
                                      short int index,
                                      extreme_t **extreme);

        /**
         * Fill all channel extreme lists
         * @param input_samples_1 is the list of captured datas for channel 1
         * @param input_samples_2 is the list of captured datas for channel 2
         *        (same nb element as input_samples_1)
         * @param nb_frames is the number of frames in input_samples_1 or
         *        input_samples_2
         */
        void channels_extreme_list_fill(vector<float> &input_samples_1,
                                        vector<float> &input_samples_2);

        /**
         * Remove old extremes from left and right extreme lists
         */
        void remove_old_extremes_from_lists();

        /**
         * Prepare and fill extreme lists with input_samples
         * @param input_samples_1 is the list of captured datas for channel 1
         * @param input_samples_2 is the list of captured datas for channel 2
         *        (same nb element as input_samples_1)
         */
        void prepare_and_fill_extreme_lists(vector<float> &input_samples_1,
                                            vector<float> &input_samples_2);

        /**
         * Fill the extreme list specified
         * @param left_or_right is the channel to use (LEFT_CHANNEL or
         *        RIGHT_CHANNEL)
         */
        void fill_extreme_list(char           left_or_right,
                               vector<float> &input_samples,
                               short int      offset_mine,
                               short int      offset_other,
                               char           state,
                               float          last_extreme_value);

        /**
         * Can check if captured signal has enough amplitude to be analyzed
         * @param left_or_right is the channel to use (LEFT_CHANNEL or
         *        RIGHT_CHANNEL)
         * @return True if there is enough amplitude, otherwise false
         */
        bool is_enough_amplitude(char left_or_right);

        /**
         * Get extreme list size
         * @param left_or_right is the channel to use (LEFT_CHANNEL or
         *        RIGHT_CHANNEL)
         * @return the size of the extreme list
         */
        int extreme_list_get_size(char left_or_right);

        /**
         * Get vinyl direction according to left and right extremes list
         * @return 1 if direction is forward, -1 if it is backward
         */
        short int get_direction();

        /**
         * Get speed for the channel specified
         * @param left_or_right is the channel to use (LEFT_CHANNEL or
         *        RIGHT_CHANNEL)
         * @return the vinyl speed for the channel specified
         */
        float get_independant_channel_speed(char left_or_right);

        /**
         * True if we are re-using sample datas in the next sample buffer
         */
        bool reuse_sample_data;

        /**
         * Apply low-pass filter at specified cut frequency on samples
         * @param samples is the list of samples
         * @param cut_frequency is the cut frequency (Hz) for the low-pass
         *        filter
         * @return TRUE if all is OK, otherwise FALSE
         */
        bool low_pass_filter(vector<float> &samples,
                             int cut_frequency);

        /**
         * Get average amplitude of a buffer of samples
         * @param samples is the list of samples
         * @param average_amplitude is the average amplitude returned by this
         *        method
         * @return TRUE if all is OK, otherwise FALSE
         */
        bool get_average_amplitude(vector<float> &samples,
                                   float *average_amplitude);

        /**
         * Get cut frequency to be used by low-pass filter
         * @param cut_frequency is the cut frequency returned by this function
         * @return TRUE if all is OK, otherwise FALSE
         */
        bool get_cut_frequency(int *cut_frequency);

        /**
         * Set no_input_signal to true
         * @param signal is true or false
         */
        void set_no_input_signal(bool signal);

        /**
         * Check if there is or not enough input signal
         * @param samples is the list of samples
         * @return TRUE if all is OK, otherwise FALSE
         */
        bool check_signal_amplitude(vector<float> &samples);

        /**
         * Add new input samples buffers in total_input_samples_X
         * @param input_samples_1 is the list of captured datas for channel 1
         * @param input_samples_2 is the list of captured datas for channel 2
         *        (same nb element as input_samples_1)
         */
        void add_new_input_samples(vector<float> &input_samples_1,
                                   vector<float> &input_samples_2);

        /**
         * Delete total_input_samples_X from memory
         */
        void delete_total_input_samples();

        /**
         * Fill list of bit with 0 and 1 extract from extreme lists
         * @return TRUE if all is OK, otherwise FALSE
         */
        bool fill_bit_list();

        /**
         * Display bit list
         */
        void display_bit_list();

        /**
         * Extract a bit value from extreme list at specified index and add_sound_data
         * it at the end of bit_list
         * @param left_or_right is the channel to use (LEFT_CHANNEL or
         *        RIGHT_CHANNEL)
         * @param index is the position in the extreme list
         */
        void add_value_to_bit_list_from_extreme_list(char left_or_right, int index);

        /**
         * Get type of distance beetween 2 bits according to amplitude of each.
         * @param left_or_right is the channel to use (LEFT_CHANNEL or
         *        RIGHT_CHANNEL).
         * @param amplitude_1 is the amplitude of first bit.
         * @param amplitude_2 is the amplitude of second bit.
         * @param bit_type_1 will take the bit_value corresponding to amplitude_1.
         * @param bit_type_2 will take the bit_value corresponding to amplitude_2.
         * @return TRUE if all is OK, otherwise FALSE.
         */
        bool get_type_of_2_bits(char  left_or_right,
                                float amplitude_1,
                                float amplitude_2,
                                char *bit_type_1,
                                char *bit_type_2);

        /**
         * Set 4 reference values used to detect bit 1 and bit 0 according their
         * amplitude.
         * @return TRUE if all is OK, otherwise FALSE.
         */
        bool set_bit_reference_values();

        /**
         * Get the higher amplitude interval in bit_list (bit1<->bit1).
         * @return the higher interval value, otherwise 0.0.
         */
        float get_higher_interval_from_bit_list();

        /**
         * Check level of extreme values
         * @return TRUE if all is OK, otherwise FALSE
         */
        bool check_extremes_level();

        /**
         * Get volume corresponding to average input signal amplitude.
         * @return the volume value.
         */
        float get_real_volume();

        /**
         * Get a "smoothed" volume value, depending on old volume value.
         * @return the new volume value.
         */
        float get_smoothed_volume(float new_volume);

        float get_average_extreme();
};

#endif //_CODED_VINYL_H_
