/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
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
/*                         Static utility functions                           */
/*                                                                            */
/*============================================================================*/

#ifndef UTILS_H_
#define UTILS_H_

#include <QString>
#include <application_const.h>
#include <audio_track.h>
using namespace std;

class Utils
{
 public:
    // Get a MD5 hash from in_kbytes bytes of the specified file.
    static QString get_file_hash(QString in_path, unsigned int in_kbytes);

    // Get the musical key of the specified audio track (use KeyFinder).
    static QString get_music_key(short signed int   *in_samples,
                                 unsigned int        in_nb_samples,
                                 short unsigned int  in_frame_rate,
                                 short unsigned int  in_nb_channels);
};

#endif /* UTILS_H_ */
