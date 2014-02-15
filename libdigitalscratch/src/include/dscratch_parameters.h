/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                                                                            */
/*                                                                            */
/*--------------------------------------------------( dscratch_parameters.h )-*/
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
/*                          Application parameters                            */
/*                                                                            */
/*============================================================================*/

#ifndef _DSCRATCH_PARAMETERS_
#define _DSCRATCH_PARAMETERS_


/******************************************************************************/
/*******************************Timecoded vinyl********************************/

/**
 * A bit that is not yet defined.
 */
#define BIT_UNDEFINED -1

/**
 * A bit 0 is a low sinusoidal peak
 */
#define BIT_0 0

/**
 * A bit 1 is a high sinusoidal peak
 */
#define BIT_1 1


/******************************************************************************/
/****************************Sound card channels*******************************/

/**
 * Left channel number value
 */
#define LEFT_CHANNEL 0

/**
 * Right channel number value
 */
#define RIGHT_CHANNEL 1


/******************************************************************************/
/**********************************Trace levels********************************/

//#define DISPLAY_CODE_LINE                // Display code line at each traces (warning decrease performances !)
//#define TRACE_ERROR                   0  // error message
//#define TRACE_THREAD                  1  // thread management
//#define TRACE_OBJECT_LIFE             2  // object construction/destruction
//#define TRACE_OBJECT_ATTRIBUTS_CHANGE 3  // object attributs management
//#define TRACE_MISC                    4  // miscellaneous trace
//#define TRACE_CAPTURE                 5  // capture management
//#define TRACE_CAPTURE_SAMPLE          6  // capture samples value only
//#define TRACE_ANALYZE_VINYL           7  // vinyl analyze trace (get speed, direction, ...)
//#define TRACE_ANALYZE_EXTREME         8  // extremes management
//#define TRACE_SPEED                   9  // vinyl speed
//#define TRACE_DIRECTION               10 // vinyl direction
//#define TRACE_VOLUME                  11 // vinyl sound volume
//#define TRACE_EXTREME_USED_FOR_DETECTING_SPEED 13 // extremes used for detecting speed

#endif //_DSCRATCH_PARAMETERS_
