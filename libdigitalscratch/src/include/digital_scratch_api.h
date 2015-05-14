/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                                                                            */
/*                                                                            */
/*--------------------------------------------------( digital_scratch_api.h )-*/
/*                                                                            */
/*  Copyright (C) 2003-2015                                                   */
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

// Supported timecoded vinyl type
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

/******************************************************************************/
/*********************************** API **************************************/

/**< Handle type used by API functions to identify the turntable. */
typedef void* dscratch_handle_t;

/**
 * Create a new turntable.
 *
 * @param coded_vinyl_type is the type of timecoded vinyl you want to use
 *        (e.g. FINAL_SCRATCH_VINYL, see above).
 * @param sample rate is the rate of the timecoded input signal.
 * @param out_handle is used to identify the turntable (returned by this function).
 *
 * @return DSCRATCH_SUCCESS if all is OK.
 */
DLLIMPORT dscratch_status_t dscratch_create_turntable(dscratch_vinyls_t   coded_vinyl_type,
                                                      const unsigned int  sample_rate,
                                                      dscratch_handle_t  *out_handle);

/**
 * Remove the specified turntable from turntable list and delete (deallocate
 * memory) it.
 *
 * @param handle is used to identify the turntable.
 *
 * @return DSCRATCH_SUCCESS if all is OK.
 */
DLLIMPORT dscratch_status_t dscratch_delete_turntable(dscratch_handle_t handle);

/**
 * Provide samples recorded from turntable (with timecoded vinyl) and analyze
 * them.
 *
 * @param handle is used to identify the turntable.
 * @param input_samples_1 is a table containing samples from left channel.
 * @param input_samples_2 is a table containing samples from right channel.
 * @param nb_frames is the size (number of element) of input_samples_1.
 *
 * @return DSCRATCH_SUCCESS if all is OK.
 *
 * @note Warning: input_samples_1 and input_samples_2 must have the same number
 *                of elements.
 */
DLLIMPORT dscratch_status_t dscratch_analyze_recorded_datas(dscratch_handle_t  handle,
                                                            const float       *input_samples_1,
                                                            const float       *input_samples_2,
                                                            int                nb_frames);

/**
 * Provide playing parameters (only relevant if dscratch_analyze_recorded_datas()
 * was called). Playing parameters are:
 *      - speed of the vinyl disc (sign is the direction).
 *      - volume of the sound (dependant of the speed).
 *
* @param handle is used to identify the turntable.
 *
 * @param speed will be returned, this is the speed of the vinyl disc.
 *        1.0 should be mapped to 0.0% of your real turntable.
 *        If the speed is a negative value, it means that vinyl is playing
 *        backward.
 *
 * @param volume will be returned, this is the volume of the sound you want to
 *        play. Indeed, the volume of the sound is dependant of the speed, so
 *        the more is the speed the more will be the volume.
 *        0.0 = mute. 1.0 = 100% volume.
 *
 * @return 0 if playing parameters are found, otherwise 1.
 * @return DSCRATCH_SUCCESS if playing parameters are found.
 */
DLLIMPORT dscratch_status_t dscratch_get_playing_parameters(dscratch_handle_t  handle,
                                                            float             *speed,
                                                            float             *volume);

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
 * Transform a vinyl type to an explicit string name.
 *
 * @param vinyl_type is the vinyl type.
 *
 * @return A full string name corresponding to the type.
 */
DLLIMPORT const char* dscratch_get_vinyl_name_from_type(dscratch_vinyls_t vinyl_type);

/**
 * Get default vinyl type.
 *
 * @return the default vinyl type (Serato vinyl).
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



/******************************************************************************/
/***************   Motion detection configuration parameters   ****************/

/**
 * Set the number of RPM.
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
 * @return the default number of rpm (45 or 33).
 */
DLLIMPORT dscratch_vinyl_rpm_t dscratch_get_default_rpm();

/**
 * Getter/Setter for the minimal detectable amplitude.
 */
DLLIMPORT dscratch_status_t dscratch_set_min_amplitude(dscratch_handle_t handle, float amplitude);
DLLIMPORT dscratch_status_t dscratch_get_min_amplitude(dscratch_handle_t handle, float *out_ampl);
DLLIMPORT dscratch_status_t dscratch_get_default_min_amplitude(dscratch_handle_t handle, float *out_ampl);
DLLIMPORT float dscratch_get_default_min_amplitude_from_vinyl_type(dscratch_vinyls_t vinyl_type);

#ifdef __cplusplus
}
#endif

