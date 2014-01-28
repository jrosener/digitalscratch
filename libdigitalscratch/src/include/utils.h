/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch System                           */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------( utils.h )-*/
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
/*               Utils class : a collection of methods-tools                  */
/*                                                                            */
/*============================================================================*/

#ifndef _UTILS_H_
#define _UTILS_H_

#include <iostream>
#include <cstdio>
#include <string>
#include <sstream>
#include "dscratch_parameters.h"
/*#include "RtAudio.h"*/


/**
 * Define a Utils class with static methods functions.\n
 * @author Julien Rosener
 */
class Utils
{
    /* Attributes */
    public:

    /* Constructor / Destructor */
    public:
        /**
        * Constructor
        */
        Utils();

        /**
        * Destructor
        */
        virtual ~Utils();


    /* Methods */
    public:
        /**
        * Cast a number type to a string
        * @param value is the number value to cast
        * @return is a string corresponding to the number value
        */
        template<typename T> static string to_string(const T & value)
        {
            // use outpout stream to create outpout string
            std::ostringstream oss;

            // write value into the stream
            oss << value;

            // return a string
            return oss.str();
        }

        /**
        * Write on output all debug messages
        * @param trace_msg is the debug message to write
        * @param trace_level is the trace level (like TRACE_OBJECT_LIFE, ...)
        */
        static void trace(string trace_msg, char trace_level);

        /**
        * Get the absolute value of a number
        * @param nb is the float to find the absolute value
        * @return the absolute value
        */
        static float abs(float nb);

        /**
        * Get all devices for each sound API managed by RtAudio
        */
        static string get_rtaudio_sound_cards_list();

        /**
        * Get all devices for a specified audio API (RtAudio)
        */
        /*static string get_rtaudio_sound_card_list_for_specified_api(RtAudio::RtAudioApi api);*/

        /**
         * Debug function : display error messages
         * @param trace_prefix is a prefix string to display before trace_msg
         * @param trace_msg is the message to display
         */
        inline static void trace_error(string trace_prefix, string trace_msg)
        {
            (void)trace_prefix;
            (void)trace_msg;
            #ifdef TRACE_ERROR
                cerr << "#/!\\ TRACE_ERROR\t" << trace_prefix << trace_msg << endl;
            #endif
        }

        /**
         * Debug function : display message about threads
         * @param trace_prefix is a prefix string to display before trace_msg
         * @param trace_msg is the message to display
         */
        inline static void trace_thread(string trace_prefix, string trace_msg)
        {
            (void)trace_prefix;
            (void)trace_msg;
            #ifdef TRACE_THREAD
                cout << "#TRACE_THREAD\t\t" << trace_prefix << trace_msg << endl;
            #endif
        }

        /**
         * Debug function : display message about life of object (new, delete, ...)
         * @param trace_prefix is a prefix string to display before trace_msg
         * @param trace_msg is the message to display
         */
        inline static void trace_object_life(string trace_prefix, string trace_msg)
        {
            (void)trace_prefix;
            (void)trace_msg;
            #ifdef TRACE_OBJECT_LIFE
                cout << "#TRACE_OBJECT_LIFE\t" << trace_prefix << trace_msg << endl;
            #endif
        }

        /**
         * Debug function : display message each time a value of an object attributes change
         * @param trace_prefix is a prefix string to display before trace_msg
         * @param trace_msg is the message to display
         */
        inline static void trace_object_attributs_change(string trace_prefix, string trace_msg)
        {
            (void)trace_prefix;
            (void)trace_msg;
            #ifdef TRACE_OBJECT_ATTRIBUTS_CHANGE
                cout << "#TRACE_OBJ_ATTR_CHANGE\t" << trace_prefix << trace_msg << endl;
            #endif
        }

        /**
         * Debug function : display message
         * @param trace_prefix is a prefix string to display before trace_msg
         * @param trace_msg is the message to display
         */
        inline static void trace_misc(string trace_prefix, string trace_msg)
        {
            (void)trace_prefix;
            (void)trace_msg;
            #ifdef TRACE_MISC
                cout << "#TRACE_MISC\t\t" << trace_prefix << trace_msg << endl;
            #endif
        }

        /**
         * Debug function : display message about capture processing
         * @param trace_prefix is a prefix string to display before trace_msg
         * @param trace_msg is the message to display
         */
        inline static void trace_capture(string trace_prefix, string trace_msg)
        {
            (void)trace_prefix;
            (void)trace_msg;
            #ifdef TRACE_CAPTURE
                cout << "#TRACE_CAPTURE\t\t" << trace_prefix << trace_msg << endl;
            #endif
        }

        /**
         * Debug function : display message about captured samples
         * @param trace_prefix is a prefix string to display before trace_msg
         * @param trace_msg is the message to display
         */
        inline static void trace_capture_sample(string trace_prefix, string trace_msg)
        {
            (void)trace_prefix;
            (void)trace_msg;
            #ifdef TRACE_CAPTURE_SAMPLE
                cout << "#TRACE_CAPTURE_SAMPLE\t" << trace_prefix << trace_msg << endl;
            #endif
        }

        /**
         * Debug function : display message about vinyl analyzis processing
         * @param trace_prefix is a prefix string to display before trace_msg
         * @param trace_msg is the message to display
         */
        inline static void trace_analyze_vinyl(string trace_prefix, string trace_msg)
        {
            (void)trace_prefix;
            (void)trace_msg;
            #ifdef TRACE_ANALYZE_VINYL
                cout << "#TRACE_ANALYZE_VINYL\t" << trace_prefix << trace_msg << endl;
            #endif
        }

        /**
         * Debug function : display message about FinalScratch vinyl extremes analyzis
         * @param trace_prefix is a prefix string to display before trace_msg
         * @param trace_msg is the message to display
         */
        inline static void trace_analyze_extreme(string trace_prefix, string trace_msg)
        {
            (void)trace_prefix;
            (void)trace_msg;
            #ifdef TRACE_ANALYZE_EXTREME
                cout << "#TRACE_ANALYZE_EXTREME\t" << trace_prefix << trace_msg << endl;
            #endif
        }

        /**
         * Debug function : display message about current vinyl speed
         * @param trace_prefix is a prefix string to display before trace_msg
         * @param trace_msg is the message to display
         */
        inline static void trace_speed(string trace_prefix, string trace_msg)
        {
            (void)trace_prefix;
            (void)trace_msg;
            #ifdef TRACE_SPEED
                cout << "#TRACE_SPEED\t\t" << trace_prefix << trace_msg << endl;
            #endif
        }

        /**
         * Debug function : display message about current vinyl volume
         * @param trace_prefix is a prefix string to display before trace_msg
         * @param trace_msg is the message to display
         */
        inline static void trace_volume(string trace_prefix, string trace_msg)
        {
            (void)trace_prefix;
            (void)trace_msg;
            #ifdef TRACE_VOLUME
                cout << "#TRACE_VOLUME\t\t" << trace_prefix << trace_msg << endl;
            #endif
        }

        /**
         * Debug function : display message about current vinyl direction
         * @param trace_prefix is a prefix string to display before trace_msg
         * @param trace_msg is the message to display
         */
        inline static void trace_direction(string trace_prefix, string trace_msg)
        {
            (void)trace_prefix;
            (void)trace_msg;
            #ifdef TRACE_DIRECTION
                cout << "#TRACE_DIRECTION\t" << trace_prefix << trace_msg << endl;
            #endif
        }
};

#endif //_UTILS_H_
