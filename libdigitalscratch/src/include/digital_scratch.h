/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*               libdigitalscratch: the Digital Scratch engine.               */
/*                                                                            */
/*                                                                            */
/*------------------------------------------------------( digital_scratch.h )-*/
/*                                                                            */
/*  Copyright (C) 2003-2017                                                   */
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
/*              API header file for the Digital Scratch lib                   */
/*                                                                            */
/*============================================================================*/

#pragma once

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

// Error codes.
enum dscratch_status_t
{
    DSCRATCH_SUCCESS = 0,
    DSCRATCH_ERROR
};

// Supported timecoded vinyl.
enum dscratch_vinyls_t
{
    FINAL_SCRATCH = 0,
    SERATO,
    MIXVIBES,
    NB_DSCRATCH_VINYLS
};

// Supported turntable speed.
enum dscratch_vinyl_rpm_t
{
    RPM_33 = 33,
    RPM_45 = 45
};

// Handle used by API functions to identify the turntable.
typedef void* dscratch_handle_t;

/**
 * Create a new turntable.
 *
 * @param coded_vinyl_type is the type of timecoded vinyl you want to use (e.g. FINAL_SCRATCH_VINYL, see above).
 * @param sample rate is the rate of the recorded input signal.
 * @param out_handle is used to identify the turntable (allocated and returned by this function).
 *
 * @return DSCRATCH_SUCCESS if all is OK.
 */
DLLIMPORT dscratch_status_t dscratch_create_turntable(dscratch_vinyls_t   coded_vinyl_type,
                                                      const unsigned int  sample_rate,
                                                      dscratch_handle_t  *out_handle);

/**
 * Delete the turntable created by dscratch_create_turntable() (free the handle).
 *
 * @param handle is used to identify the turntable.
 *
 * @return DSCRATCH_SUCCESS if all is OK.
 */
DLLIMPORT dscratch_status_t dscratch_delete_turntable(dscratch_handle_t handle);

/**
 * Analyze the recorded samples (coming from a timecoded vinyl). After this call, you can
 * call dscratch_get_playing_parameters() to retrieve the speed, volume,...
 *
 * @param handle is used to identify the turntable.
 * @param left_samples is a table containing samples from the left channel.
 * @param right_samples is a table containing samples from the right channel.
 * @param samples_table_size is the size (number of elements) of left_samples or right_samples.
 *
 * @return DSCRATCH_SUCCESS if all is OK.
 *
 * @note Warning: left_samples and right_samples must have the same number
 *                of elements (nb_frames elements).
 */
DLLIMPORT dscratch_status_t dscratch_process_captured_timecoded_signal(dscratch_handle_t  handle,
                                                                       const float       *left_samples,
                                                                       const float       *right_samples,
                                                                       int                samples_table_size);

/**
 * Returns the calculated speed of the vinyl on turntable
 * (only relevant if dscratch_process_captured_timecoded_signal() was called).
 *
 * @param handle is used to identify the turntable.
 * @param speed will be returned, this is the speed of the vinyl disc.
 *        1.0 should be mapped to 0.0% of your real turntable.
 *        If the speed is a negative value, it means that vinyl is playing
 *        backward.
 *
 * @return DSCRATCH_SUCCESS if all is OK.
 */
DLLIMPORT dscratch_status_t dscratch_get_speed(dscratch_handle_t  handle,
                                               float             *speed);

/**
 * Returns the volume of the signal captured from vinyl on turntable
 * (only relevant if dscratch_process_captured_timecoded_signal() was called).
 *
 * @param handle is used to identify the turntable.
 * @param volume will be returned, this is the amplitude of the signal given
 *               to dscratch_process_captured_timecoded_signal()
 *
 * @return DSCRATCH_SUCCESS if all is OK.
 */
DLLIMPORT dscratch_status_t dscratch_get_volume(dscratch_handle_t  handle,
                                                float             *volume);

/**
 * Get DigitalScratch version.
 *
 * @return a const string containing the version number.
 *
 */
DLLIMPORT const char *dscratch_get_version();

/**
 * Display on stdout informations about specified turntable.
 *
 * @param handle is used to identify the turntable.
 *
 * @return DSCRATCH_SUCCESS if all is OK.
 */
DLLIMPORT dscratch_status_t dscratch_display_turntable(dscratch_handle_t handle);

/**
 * Get vinyl type used for specified turntable.
 *
 * @param handle is used to identify the turntable.
 * @param vinyl_type is the vinyl type.
 *
 * @return DSCRATCH_SUCCESS if all is OK.
 */
DLLIMPORT dscratch_status_t dscratch_get_turntable_vinyl_type(dscratch_handle_t  handle,
                                                              dscratch_vinyls_t *vinyl_type);

/**
 * Transform a vinyl type into an explicit string name.
 *
 * @param vinyl_type is the vinyl type.
 *
 * @return A full string name corresponding to the type.
 */
DLLIMPORT const char* dscratch_get_vinyl_name_from_type(dscratch_vinyls_t vinyl_type);

/**
 * Get the default vinyl type.
 *
 * @return the default vinyl type (=> Serato vinyl).
 */
DLLIMPORT dscratch_vinyls_t dscratch_get_default_vinyl_type();


/**
 * Change vinyl type without deleting and recreating engine.
 *
 * @param handle is used to identify the turntable.
 * @param vinyl_type is the type of vinyl (@see dscratch_vinyls_t).
 *
 * @return DSCRATCH_SUCCESS if all is OK.
 */
DLLIMPORT dscratch_status_t dscratch_change_vinyl_type(dscratch_handle_t handle,
                                                       dscratch_vinyls_t vinyl_type);

/**
 * Set the number of RPM used to play the timecoded vinyl on the turntable.
 *
 * @param handle is used to identify the turntable.
 * @param rpm is the number of RPM of the turntable (45 or 33).
 *
 * @return DSCRATCH_SUCCESS if all is OK.
 */
DLLIMPORT dscratch_status_t dscratch_set_rpm(dscratch_handle_t    handle,
                                             dscratch_vinyl_rpm_t rpm);

/**
 * Get the turntable RPM value.
 *
 * @param handle is used to identify the turntable.
 * @param out_rpm is the number of RPM of the turntable (45 or 33)
 *        (returned by this function)..
 *
 * @return DSCRATCH_SUCCESS if all is OK.
 */
DLLIMPORT dscratch_status_t dscratch_get_rpm(dscratch_handle_t     handle,
                                             dscratch_vinyl_rpm_t *out_rpm);

/**
 * Get the default number of RPM.
 *
 * @return the default number of rpm (=> 33).
 */
DLLIMPORT dscratch_vinyl_rpm_t dscratch_get_default_rpm();

#ifdef __cplusplus
}
#endif

