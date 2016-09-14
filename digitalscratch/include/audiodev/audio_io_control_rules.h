/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*-----------------------------------------------( audio_io_control_rules.h )-*/
/*                                                                            */
/*  Copyright (C) 2003-2016                                                   */
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

#include "app/application_const.h"

#ifdef ENABLE_TEST_MODE
#include <tracks/audio_track.h>
#include <QSharedPointer>
#endif

using namespace std;

class Audio_IO_control_rules : public QObject
{
    Q_OBJECT

 protected:
    unsigned short int       nb_channels;
    void                    *callback_param;
    bool                     running;
    bool                     do_capture;

 public:
    explicit Audio_IO_control_rules(const unsigned short int &nb_channels);
    virtual ~Audio_IO_control_rules();

 public:
    bool is_running();
    void set_capture(const bool &do_capture);
    virtual bool start(void *callback_param) = 0;
    virtual bool restart() = 0;
    virtual bool stop() = 0;
    virtual bool get_input_buffers(const unsigned short int &nb_buffer_frames, QList<float*> &io_buffers) = 0;
    virtual bool get_output_buffers(const unsigned short int &nb_buffer_frames, QList<float*> &io_buffers) = 0;

#ifdef ENABLE_TEST_MODE
 private:
    bool using_fake_timecode;
    unsigned int timecode_current_sample;
    QSharedPointer<Audio_track> timecode;

 public:
    bool use_timecode_from_file(const QString &path);

 protected:
    bool fill_input_buf(const unsigned short int &nb_buffer_frames, QList<float*> &io_buffers);
 #endif

 signals:
    void error_msg(const QString &error_message);
};
