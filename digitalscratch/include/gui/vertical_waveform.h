/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------( vertical_waveform.h )-*/
/*                                                                            */
/*  Copyright (C) 2003-2012                                                   */
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
/*                        Define a vertical waveform                          */
/*                                                                            */
/*============================================================================*/

#ifndef VERTICAL_WAVEFORM_H_
#define VERTICAL_WAVEFORM_H_

#include <QLabel>
#include "audio_track.h"
#include <application_const.h>

#define V_WAVEFORM_POINTS_MAX_SIZE (this->at->get_max_nb_samples() / 100) // Number of samples for a track of 15 min (divided by 100).

using namespace std;

class Vertical_waveform : public QLabel
{
    Q_OBJECT

 private:
    int          area_height;
    int          area_width;
    Audio_track *at;
    QPointF     *points;

 public:
    Vertical_waveform(Audio_track *in_at, QWidget *in_parent = 0);
    ~Vertical_waveform();

 private:
    void get_area_size();
    bool generate_polyline();

 protected:
    virtual void paintEvent(QPaintEvent *);
};

#endif /* WAVEFORM_H_ */
