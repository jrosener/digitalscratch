/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------( sound_device_access_rules.h )-*/
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
/* Behavior class: access internal sound card device (open, close, list, ...) */
/*                                                                            */
/*============================================================================*/

#pragma once

#include <iostream>
#include <QObject>
#include <QString>
#include <sound_driver_access_rules.h>
#include <application_const.h>
#include <QAudioDeviceInfo>

using namespace std;

class Audio_device_access_rules : public Sound_driver_access_rules
{
    Q_OBJECT

 public:
    Audio_device_access_rules(unsigned short int in_nb_channels);
    virtual ~Audio_device_access_rules();

 public:
    static QList<QString> get_device_list();
    bool start(void *in_callback_param);
    bool restart();
    bool stop();
    bool get_input_buffers(unsigned short int  in_nb_buffer_frames, QList<float*> &out_buffers);
    bool get_output_buffers(unsigned short int in_nb_buffer_frames, QList<float*> &out_buffers);
};
