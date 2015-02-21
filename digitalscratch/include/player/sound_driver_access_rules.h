/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------( sound_card_access_rules.h )-*/
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
/*        Behavior class: access sound card (open, close, list, ...)          */
/*                                                                            */
/*============================================================================*/

#pragma once

#include <iostream>
#include <QObject>
#include <QString>

#include "application_const.h"

#ifdef ENABLE_TEST_MODE
#include <audio_track.h>
#include <QSharedPointer>
#endif

using namespace std;

class Sound_driver_access_rules : public QObject
{
    Q_OBJECT

 protected:
    unsigned short int       nb_channels;
    void                    *callback_param;
    bool                     running;
    bool                     do_capture;

 public:
    Sound_driver_access_rules(const unsigned short int &nb_channels);
    virtual ~Sound_driver_access_rules();

 public:
    bool is_running();
    void set_capture(bool in_do_capture);
    virtual bool start(void *in_callback_param) = 0;
    virtual bool restart() = 0;
    virtual bool stop() = 0;
    virtual bool get_input_buffers(const unsigned short int &nb_buffer_frames, QList<float*> &buffers) = 0;
    virtual bool get_output_buffers(const unsigned short int &nb_buffer_frames, QList<float*> &buffers) = 0;

#ifdef ENABLE_TEST_MODE
 private:
   bool using_fake_timecode;
   unsigned int timecode_current_sample;
   QSharedPointer<Audio_track> timecode;

 public:
    bool use_timecode_from_file(const QString &path);

 protected:
    bool fill_input_buf(unsigned short int in_nb_buffer_frames, QList<float*> &io_buffers);
 #endif

 signals:
   void error_msg(QString in_error_message);
};
