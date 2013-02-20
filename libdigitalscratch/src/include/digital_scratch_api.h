/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                                                                            */
/*                                                                            */
/*--------------------------------------------------( digital_scratch_api.h )-*/
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
/*              API header file to use Digital Scratch lib                    */
/*                                                                            */
/*============================================================================*/

#ifndef DIGITALSCRATCH_H_
#define DIGITALSCRATCH_H_

#ifdef WIN32
    #ifdef DLLIMPORT
        #undef DLLIMPORT
    #endif
    #define DLLIMPORT __declspec (dllexport)
#else
    #define DLLIMPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/******************** Internal stuff for trace system *************************/

#define TRACE_PREFIX_DIGITALSCRATCHAPI "[Digital_scratch_api:" \
                                       + Utils::to_string(__LINE__) + "]\t"


/******************************************************************************/
/**************************** Error codes *************************************/

/**< This is the speed value in case of no new value is found. */
#define NO_NEW_SPEED_FOUND -99.0

/**< This is the volume value in case of no new value is found. */
#define NO_NEW_VOLUME_FOUND -99.0

/**< This is the position value in case of no new value is found. */
#define NO_NEW_POSITION_FOUND -99.0


/******************************************************************************/
/********************* Supported timecoded vinyl type *************************/

/**< Stanton Final Scratch vinyl name. */
#define FINAL_SCRATCH_VINYL "final scratch standard 2.0"

/**< Serato Scratch Live vinyl name. */
#define SERATO_VINYL "serato cv02"

/**< Mixvibes vinyl name. */
#define MIXVIBES_VINYL "mixvibes dvs"

/******************************************************************************/
/************ API functions: create, delete, provide datas,... ****************/

/**
 * Create a new turntable. You turntable will be created with default values.
 * If you want to change them, take a look to vinyl specific functions and
 * get/set API functions.
 *
 * @param name is the name you want to give to the turntable.
 * @param coded_vinyl_type is the type of timecoded vinyl you want to use
 *        (e.g. FINAL_SCRATCH_VINYL, see above).
 * @param turntable_id will be returned, needed by some other API functions to
 *        specify which turntable you want to use.
 *
 * @return 0 if all is OK, otherwise 1.
 */
DLLIMPORT int dscratch_create_turntable(char *name,
                                        char *coded_vinyl_type,
                                        int  *turntable_id);

/**
 * Remove the specified turntable from turntable list and delete (deallocate
 * memory) it.
 *
 * @param turntable_id is the id of the turntable you want to delete. This id
 *        is provided by dscratch_create_turntable() function.
 *
 * @return 0 if all is OK, otherwise 1.
 */
DLLIMPORT int dscratch_delete_turntable(int turntable_id);

/**
 * Provide samples recorded from turntable (with timecoded vinyl) and analyze
 * them.
 *
 * @param turntable_id is the id of the turntable for which you want to provide
 *        recorded datas. This id is provided by dscratch_create_turntable()
 *        function.
 * @param input_samples_1 is a table containing samples from left channel.
 * @param input_samples_2 is a table containing samples from right channel.
 * @param nb_frames is the size (number of element) of input_samples_1.
 *
 * @return 0 if all is OK, otherwise 1.
 *
 * @note Warning: input_samples_1 and input_samples_2 must have the same number
 *                of elements.
 */
DLLIMPORT int dscratch_analyze_recorded_datas(int     turntable_id,
                                              float *input_samples_1,
                                              float *input_samples_2,
                                              int     nb_frames);


/**
 * Same as dscratch_analyze_recorded_datas() but with interleaved datas.
 * E.g. 0.12 0.98 -0.38 0.68 ...
 *      l[0] r[0] l[1]  r[1] ... (l=left, r=right)
 *
 * @param turntable_id is the id of the turntable for which you want to provide
 *        recorded datas. This id is provided by dscratch_create_turntable()
 *        function.
 * @param nb_channels is the number of channels used for interleaved datas (for
 *                    a stereo input it is 2, for a bi-stereo input it is 4,...).
 * @param left_index is the index of the first left channel you want to use (it
 *                   is 0 if you want to use the first stereo input, 2 for the
 *                   second one,...).
 * @param right_index is the index of the first right channel you want to use (it
 *                    is 1 if you want to use the first stereo input, 3 for the
 *                    second one,...).
 * @param input_samples_interleaved is a table containing interleaved samples
 *        from left and right channels.
 * @param nb_frames is the size (number of element) of input_samples_interleaved
 *        divided by nb_channels.
 *
 * @return 0 if all is OK, otherwise 1.
 *
 */
DLLIMPORT int dscratch_analyze_recorded_datas_interleaved(int    turntable_id,
                                                          int    nb_channels,
                                                          int    left_index,
                                                          int    right_index,
                                                          float *input_samples_interleaved,
                                                          int    nb_frames);

/**
 * Provide playing parameters (only relevant if dscratch_analyze_recorded_datas()
 * was called). Playing parameters are:
 *      - speed of the vinyl disc.
 *      - position of the needle on vinyl disc (if position detection is
 *        enabled).
 *      - volume of the sound (dependant of the speed).
 *
 * @param turntable_id is the id of the turntable of which you want to get
 *        playing parameters. This id is provided by dscratch_create_turntable()
 *        function.
 *
 * @param speed will be returned, this is the speed of the vinyl disc.
 *        1.0 should be mapped to 0.0% of your real turntable.
 *        If the speed is a negative value, it means that vinyl is playing
 *        backward.
 *        NO_NEW_SPEED_FOUND is returned if no speed is found.
 *
 * @param volume will be returned, this is the volume of the sound you want to
 *        play. Indeed, the volume of the sound is dependant of the speed, so
 *        the more is the speed the more will be the volume.
 *        0.0 correspond to mute. 1.0 should be mapped to 80% of the maximum of
 *        the volume for example, in fact DigitalScratch can return for example
 *        a volume equal to 1.26.
 *        NO_NEW_VOLUME_FOUND is returned if no volume is found.
 *
 * @param position will be returned, this is the position of the needle on the
 *        vinyl disc. It is the number of second from the beginning of the disc.
 *        Ex: position = 132.36sec, jump to 2min12sec36ms.
 *        NO_NEW_POSITION_FOUND is returned if no position is found.
 *
 * @return 0 if playing parameters are found, otherwise 1.
 */
DLLIMPORT int dscratch_get_playing_parameters(int    turntable_id,
                                              float *speed,
                                              float *volume,
                                              float *position);

/**
 * Enable needle position detection algorithm.
 *
 * @param turntable_id is the id of the turntable of which you want to enable
 *        position detection. This id is provided by dscratch_create_turntable()
 *        function.
 * @param enable_position equal 1 if you want to enable position detection,
 *        otherwise 0.
 *
 * @return 0 if all is OK, otherwise 1
 */
DLLIMPORT int dscratch_set_position_detection(int turntable_id,
                                              int enable_position);

/**
 * Get state of position detection (enable or not).
 *
 * @param turntable_id is the id of the turntable on which you want to work.
 *        This id is provided by dscratch_create_turntable() function.
 *
 * @return 1 if position detection algorithm is enabled, 0 if it is disabled and
 *         -1 for any error.
 */
DLLIMPORT int dscratch_get_position_detection_state(int turntable_id);

/**
 * Print which trace type are used to monitor DigitalScratch.
 */
DLLIMPORT void dscratch_print_trace_used();

/**
 * Get number of turntable registered in DigitalScratch.
 *
 * @return number of turntable.
 */
DLLIMPORT int dscratch_get_number_of_turntables();

/**
 * Get DigitalScratch version.
 *
 * @return a const string containing version number.
 *
 */
DLLIMPORT const char *dscratch_get_version();

/**
 * Display on stdout informations about specified turntable.
 *
 * @param turntable_id is the id of the turntable of which you want to display
 *        informations. This id is provided by dscratch_create_turntable()
 *        function.
 *
 * @return 0 if all is OK, otherwise 1
 */
DLLIMPORT int dscratch_display_turntable(int turntable_id);

/**
 * Get name of specified turntable.
 *
 * @param turntable_id is the id of the turntable of which you want to get the
 *        name. This id is provided by dscratch_create_turntable()
 *        function.
 * @param name buffer in which will be putted the name of the turntable.
 *
 * @note This function will allocate (malloc) the buffer in which it will put
 *       the name, so do not forget to deallocate it (free).
 *
 * @return 0 if all is OK, otherwise 1
 */
DLLIMPORT int dscratch_get_turntable_name(int    turntable_id,
                                          char **turntable_name);

/**
 * Get vinyl type used for specified turntable.
 *
 * @param turntable_id is the id of the turntable of which you want to get the
 *        vinyl type. This id is provided by dscratch_create_turntable()
 *        function.
 * @param vinyl_type buffer in which will be putted the vinyl type.
 *
 * @note This function will allocate (malloc) the buffer in which it will put
 *       the vinyl type, so do not forget to deallocate it (free).
 *
 * @return 0 if all is OK, otherwise 1
 */
DLLIMPORT int dscratch_get_vinyl_type(int    turntable_id,
                                      char **vinyl_type);

/**
 * Get default vinyl type.
 *
 * @return the default vinyl type (Serato vinyl).
 */
DLLIMPORT const char* dscratch_get_default_vinyl_type();


/**
 * Change vinyl type without deleting and recreating engine.
 *
 * @return 0 if all is OK, otherwise 1.
 */
DLLIMPORT int dscratch_change_vinyl_type(int   turntable_id,
                                         char *vinyl_type);



/******************************************************************************/
/**** API functions: General motion detection configuration parameters ********/

/**
 * Set the maximum speed difference that we allow beetween 2 consecutive speeds.
 *
 * @param turntable_id is the id of the turntable on which you want to work.
 *        This id is provided by dscratch_create_turntable() function.
 * @param diff is the value you want to update. It must be positive.
 *
 * @return 0 if all is OK, otherwise 1.
 */
DLLIMPORT int dscratch_set_max_speed_diff(int   turntable_id,
                                          float diff);

/**
 * Get the maximum speed difference that we allow beetween 2 consecutive speeds.
 *
 * @param turntable_id is the id of the turntable on which you want to work.
 *        This id is provided by dscratch_create_turntable() function.
 *
 * @return the maximum speed difference.
 */
DLLIMPORT float dscratch_get_max_speed_diff(int turntable_id);

/**
 * Get the default maximum speed difference that we allow beetween 2 consecutive speeds.
 *
 * @return the default maximum speed difference.
 */
DLLIMPORT float dscratch_get_default_max_speed_diff();

/**
 * Set the max speed under which slow speed algorithm is applied.
 *
 * @param turntable_id is the id of the turntable on which you want to work.
 *        This id is provided by dscratch_create_turntable() function.
 * @param speed is the value you want to update. It must be positive.
 *
 * @return 0 if all is OK, otherwise 1.
 */
DLLIMPORT int dscratch_set_slow_speed_algo_usage(int   turntable_id,
                                                 float speed);

/**
 * Get the max speed under which slow speed algorithm is applied.
 *
 * @param turntable_id is the id of the turntable on which you want to work.
 *        This id is provided by dscratch_create_turntable() function.
 *
 * @return the max speed under which slow speed algorithm is applied.
 */
DLLIMPORT float dscratch_get_slow_speed_algo_usage(int turntable_id);

/**
 * Get the default max speed under which slow speed algorithm is applied.
 *
 * @return the max speed under which slow speed algorithm is applied.
 */
DLLIMPORT float dscratch_get_default_slow_speed_algo_usage();

/**
 * Set the maximum number of buffer we will accumulate to get the speed.
 *
 * @param turntable_id is the id of the turntable on which you want to work.
 *        This id is provided by dscratch_create_turntable() function.
 * @param nb_buffer is the value you want to update. It must be positive.
 *
 * @return 0 if all is OK, otherwise 1.
 */
DLLIMPORT int dscratch_set_max_nb_buffer(int turntable_id,
                                         int nb_buffer);

/**
 * Get the maximum number of buffer we will accumulate to get the speed.
 *
 * @param turntable_id is the id of the turntable on which you want to work.
 *        This id is provided by dscratch_create_turntable() function.
 *
 * @return the maximum number of buffers.
 */
DLLIMPORT int dscratch_get_max_nb_buffer(int turntable_id);

/**
 * Get the default maximum number of buffer we will accumulate to get the speed.
 *
 * @return the maximum number of buffers.
 */
DLLIMPORT int dscratch_get_default_max_nb_buffer();

/**
 * Set the maximum number of speeds we will use for stability algorithm.
 *
 * @param turntable_id is the id of the turntable on which you want to work.
 *        This id is provided by dscratch_create_turntable() function.
 * @param nb_speed is the value you want to update. It must be positive.
 *
 * @return 0 if all is OK, otherwise 1.
 */
DLLIMPORT int dscratch_set_max_nb_speed_for_stability(int turntable_id,
                                                      int nb_speed);

/**
 * Get the maximum number of speeds we will use for stability algorithm.
 *
 * @param turntable_id is the id of the turntable on which you want to work.
 *        This id is provided by dscratch_create_turntable() function.
 *
 * @return the maximum number of speeds.
 */
DLLIMPORT int dscratch_get_max_nb_speed_for_stability(int turntable_id);

/**
 * Get the default maximum number of speeds we will use for stability algorithm.
 *
 * @return the maximum number of speeds.
 */
DLLIMPORT int dscratch_get_default_max_nb_speed_for_stability();

/**
 * Set the minimal extreme (a max or a min of a sinusoidal wave) value accepted.
 *
 * @param turntable_id is the id of the turntable on which you want to work.
 *        This id is provided by dscratch_create_turntable() function.
 * @param extreme_min is the value you want to update.
 *
 * @return 0 if all is OK, otherwise 1.
 */
DLLIMPORT int dscratch_set_extreme_min(int   turntable_id,
                                       float extreme_min);

/**
 * Get the minimal extreme (a max or a min of a sinusoidal wave) value accepted.
 *
 * @param turntable_id is the id of the turntable on which you want to work.
 *        This id is provided by dscratch_create_turntable() function.
 *
 * @return the minimal extreme accepted.
 */
DLLIMPORT float dscratch_get_extreme_min(int turntable_id);

/**
 * Get the default minimal extreme (a max or a min of a sinusoidal wave) value accepted.
 *
 * @return the minimal extreme accepted.
 */
DLLIMPORT float dscratch_get_default_extreme_min();

/**
 * Set the maximum number of buffers of recorded datas that can be queued
 * to find playing parameters.
 *
 * @param turntable_id is the id of the turntable on which you want to work.
 *        This id is provided by dscratch_create_turntable() function.
 * @param max_buffer_coeff is the value you want to update.
 *
 * @return 0 if all is OK, otherwise 1.
 */
DLLIMPORT int dscratch_set_max_buffer_coeff(int turntable_id,
                                            int max_buffer_coeff);

/**
 * Get the maximum number of buffers of recorded datas that can be queued to
 * find playing parameters.
 *
 * @param turntable_id is the id of the turntable on which you want to work.
 *        This id is provided by dscratch_create_turntable() function.
 *
 * @return the maximum number of buffers.
 */
DLLIMPORT int dscratch_get_max_buffer_coeff(int turntable_id);

/**
 * Get the default maximum number of buffers of recorded datas that can be queued to
 * find playing parameters.
 *
 * @return the maximum number of buffers.
 */
DLLIMPORT int dscratch_get_default_max_buffer_coeff();

/**
 * Set the number of processing cycles on input samples used to keep old
 * direction when a new direction is detected.
 *
 * @param turntable_id is the id of the turntable on which you want to work.
 *        This id is provided by dscratch_create_turntable() function.
 * @param nb_cycle is the value you want to update.
 *
 * @return 0 if all is OK, otherwise 1.
 */
DLLIMPORT int dscratch_set_nb_cycle_before_changing_direction(int turntable_id,
                                                              int nb_cycle);

/**
 * Get the number of processing cycles on input samples used to keep old
 * direction when a new direction is detected.
 *
 * @param turntable_id is the id of the turntable on which you want to work.
 *        This id is provided by dscratch_create_turntable() function.
 *
 * @return the number of cycle before to change direction.
 */
DLLIMPORT int dscratch_get_nb_cycle_before_changing_direction(int turntable_id);

/**
 * Get the default number of processing cycles on input samples used to keep old
 * direction when a new direction is detected.
 *
 * @return the number of cycle before to change direction.
 */
DLLIMPORT int dscratch_get_default_nb_cycle_before_changing_direction();

/**
 * Set the coefficient (in percentage) of the biggest bit1<->bit1 amplitude to
 * calculate the minimum acceptable value for an interval bit1<->bit1 on right
 * channel.
 *
 * @param turntable_id is the id of the turntable on which you want to work.
 *        This id is provided by dscratch_create_turntable() function.
 * @param coeff is the value you want to update.
 *
 * @return 0 if all is OK, otherwise 1.
 */
DLLIMPORT int dscratch_set_coeff_right_dist_min_bit1_to_bit1(int   turntable_id,
                                                             float coeff);
/**
 * Get the coefficient (in percentage) of the biggest bit1<->bit1 amplitude to
 * calculate the minimum acceptable value for an interval bit1<->bit1 on right
 * channel.
 *
 * @param turntable_id is the id of the turntable on which you want to work.
 *        This id is provided by dscratch_create_turntable() function.
 *
 * @return the coefficient of the biggest bit1<->bit1 amplitude (right channel).
 */
DLLIMPORT float dscratch_get_coeff_right_dist_min_bit1_to_bit1(int turntable_id);

/**
 * Get the default coefficient (in percentage) of the biggest bit1<->bit1 amplitude to
 * calculate the minimum acceptable value for an interval bit1<->bit1 on right
 * channel.
 *
 * @return the coefficient of the biggest bit1<->bit1 amplitude (right channel).
 */
DLLIMPORT float dscratch_get_default_coeff_right_dist_min_bit1_to_bit1();

/**
 * Set the coefficient (in percentage) of the biggest bit1<->bit1 amplitude to
 * calculate the minimum acceptable value for an interval bit1<->bit1 on left
 * channel.
 *
 * @param turntable_id is the id of the turntable on which you want to work.
 *        This id is provided by dscratch_create_turntable() function.
 * @param coeff is the value you want to update.
 *
 * @return 0 if all is OK, otherwise 1.
 */
DLLIMPORT int dscratch_set_coeff_left_dist_min_bit1_to_bit1(int   turntable_id,
                                                            float coeff);

/**
 * Get the coefficient (in percentage) of the biggest bit1<->bit1 amplitude to
 * calculate the minimum acceptable value for an interval bit1<->bit1 on left
 * channel.
 *
 * @param turntable_id is the id of the turntable on which you want to work.
 *        This id is provided by dscratch_create_turntable() function.
 *
 * @return the coefficient of the biggest bit1<->bit1 amplitude (left channel).
 */
DLLIMPORT float dscratch_get_coeff_left_dist_min_bit1_to_bit1(int turntable_id);

/**
 * Get the default coefficient (in percentage) of the biggest bit1<->bit1 amplitude to
 * calculate the minimum acceptable value for an interval bit1<->bit1 on left
 * channel.
 *
 * @return the coefficient of the biggest bit1<->bit1 amplitude (left channel).
 */
DLLIMPORT float dscratch_get_default_coeff_left_dist_min_bit1_to_bit1();

/**
 * Set the coefficient (in percentage) of the biggest bit0<->bit0 amplitude to
 * calculate the maximum acceptable value for an interval bit0<->bit0 on right
 * channel.
 *
 * @param turntable_id is the id of the turntable on which you want to work.
 *        This id is provided by dscratch_create_turntable() function.
 * @param coeff is the value you want to update.
 *
 * @return 0 if all is OK, otherwise 1.
 */
DLLIMPORT int dscratch_set_coeff_right_dist_max_bit0_to_bit0(int   turntable_id,
                                                             float coeff);

/**
 * Get the coefficient (in percentage) of the biggest bit0<->bit0 amplitude to
 * calculate the maximum acceptable value for an interval bit0<->bit0 on right
 * channel.
 *
 * @param turntable_id is the id of the turntable on which you want to work.
 *        This id is provided by dscratch_create_turntable() function.
 *
 * @return the coefficient of the biggest bit0<->bit0 amplitude (right channel).
 */
DLLIMPORT float dscratch_get_coeff_right_dist_max_bit0_to_bit0(int turntable_id);

/**
 * Get the default coefficient (in percentage) of the biggest bit0<->bit0 amplitude to
 * calculate the maximum acceptable value for an interval bit0<->bit0 on right
 * channel.
 *
 * @return the coefficient of the biggest bit0<->bit0 amplitude (right channel).
 */
DLLIMPORT float dscratch_get_default_coeff_right_dist_max_bit0_to_bit0();

/**
 * Set the coefficient (in percentage) of the biggest bit0<->bit0 amplitude to
 * calculate the maximum acceptable value for an interval bit0<->bit0 on left
 * channel.
 *
 * @param turntable_id is the id of the turntable on which you want to work.
 *        This id is provided by dscratch_create_turntable() function.
 * @param coeff is the value you want to update.
 *
 * @return 0 if all is OK, otherwise 1.
 */
DLLIMPORT int dscratch_set_coeff_left_dist_max_bit0_to_bit0(int   turntable_id,
                                                            float coeff);

/**
 * Get the coefficient (in percentage) of the biggest bit0<->bit0 amplitude to
 * calculate the minimum acceptable value for an interval bit0<->bit0 on left
 * channel.
 *
 * @param turntable_id is the id of the turntable on which you want to work.
 *        This id is provided by dscratch_create_turntable() function.
 *
 * @return the coefficient of the biggest bit0<->bit0 amplitude (left channel).
 */
DLLIMPORT float dscratch_get_coeff_left_dist_max_bit0_to_bit0(int turntable_id);

/**
 * Get the default coefficient (in percentage) of the biggest bit0<->bit0 amplitude to
 * calculate the minimum acceptable value for an interval bit0<->bit0 on left
 * channel.
 *
 * @return the coefficient of the biggest bit0<->bit0 amplitude (left channel).
 */
DLLIMPORT float dscratch_get_default_coeff_left_dist_max_bit0_to_bit0();

/**
 * Set the coefficient used to play with volume progressive algorithm.
 *
 * @param turntable_id is the id of the turntable on which you want to work.
 *        This id is provided by dscratch_create_turntable() function.
 * @param coeff is the value you want to update.
 *
 * @return 0 if all is OK, otherwise 1.
 */
DLLIMPORT int dscratch_set_progressive_volume_coeff(int   turntable_id,
                                                    float coeff);
/**
 * Get the coefficient used to play with volume progressive algorithm.
 *
 * @param turntable_id is the id of the turntable on which you want to work.
 *        This id is provided by dscratch_create_turntable() function.
 *
 * @return the progressive volume coefficient.
 */
DLLIMPORT float dscratch_get_progressive_volume_coeff(int turntable_id);

/**
 * Get the default coefficient used to play with volume progressive algorithm.
 *
 * @return the progressive volume coefficient.
 */
DLLIMPORT float dscratch_get_default_progressive_volume_coeff();

/**
 * Set the amplitude value that we consider to correspond to the maximum volume.
 *
 * @param turntable_id is the id of the turntable on which you want to work.
 *        This id is provided by dscratch_create_turntable() function.
 * @param volume is the value you want to update.
 *
 * @return 0 if all is OK, otherwise 1.
 */
DLLIMPORT int dscratch_set_full_volume_amplitude(int   turntable_id,
                                                 float volume);
/**
 * Get the amplitude value that we consider to correspond to the maximum volume.
 *
 * @param turntable_id is the id of the turntable on which you want to work.
 *        This id is provided by dscratch_create_turntable() function.
 *
 * @return the amplitude corresponding to the maximum volume.
 */
DLLIMPORT float dscratch_get_full_volume_amplitude(int turntable_id);

/**
 * Get the default amplitude value that we consider to correspond to the maximum volume.
 *
 * @return the amplitude corresponding to the maximum volume.
 */
DLLIMPORT float dscratch_get_default_full_volume_amplitude();

/**
 * Set speed under which low pass filter is applied on input data.
 *
 * @param turntable_id is the id of the turntable on which you want to work.
 *        This id is provided by dscratch_create_turntable() function.
 * @param low_pass_filter_max_speed_usage is the value you want to update.
 *
 * @return 0 if all is OK, otherwise 1.
 */
DLLIMPORT int dscratch_set_low_pass_filter_max_speed_usage(int   turntable_id,
                                                           float low_pass_filter_max_speed_usage);

/**
 * Get speed under which low pass filter is applied on input data.
 *
 * @param turntable_id is the id of the turntable on which you want to work.
 *        This id is provided by dscratch_create_turntable() function.
 *
 * @return the max speed under which low pass filter is applied on input data.
 */
DLLIMPORT float dscratch_get_low_pass_filter_max_speed_usage(int turntable_id);

/**
 * Get default speed under which low pass filter is applied on input data.
 *
 * @return the max speed under which low pass filter is applied on input data.
 */
DLLIMPORT float dscratch_get_default_low_pass_filter_max_speed_usage();

#ifdef __cplusplus
}
#endif

#endif /* DIGITALSCRATCH_H_ */
