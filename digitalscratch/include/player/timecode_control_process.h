/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*---------------------------------------------( timecode_control_process.h )-*/
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
/* Behavior class: analyze captured timecode and determine playback           */
/* parameters.                                                                */
/*                                                                            */
/*============================================================================*/

#pragma once

#include <iostream>
#include <QSharedPointer>

#include "playback_parameters.h"
#include "application_const.h"

using namespace std;

class Timecode_control_process
{
 private:
    unsigned short int                          nb_decks;
    QList<QSharedPointer<Playback_parameters>>  params;             // Table of nb_decks playback parameters.
    int                                        *dscratch_ids;       // Table of nb_decks dscratch ids.

 public:
    Timecode_control_process(QList<QSharedPointer<Playback_parameters>> &in_params,
                             unsigned short int                          in_nb_decks,
                             QString                                     in_vinyl_type,
                             unsigned int                                in_sample_rate);
    virtual ~Timecode_control_process();

    bool run(unsigned short int  in_nb_samples,
             float              *in_samples_1,
             float              *in_samples_2,
             float              *in_samples_3,
             float              *in_samples_4);

    int get_dscratch_id(unsigned short int in_index);
};
