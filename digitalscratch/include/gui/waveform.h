/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*-------------------------------------------------------------( waveform.h )-*/
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
/*                        Define a waveform                                   */
/*                                                                            */
/*============================================================================*/

#ifndef WAVEFORM_H_
#define WAVEFORM_H_

#include <QLabel>
#include "audio_track.h"
#include <application_const.h>

#define POINTS_MAX_SIZE (this->at->get_max_nb_samples() / 100) // Number of samples for a track of 15 min (divided by 100).

using namespace std;

class Waveform : public QLabel
{
    Q_OBJECT

 private:
    int            area_height;
    int            area_width;
    Audio_track   *at;
    QLabel        *slider;
    int            slider_position_x;
    float          slider_absolute_position;
    QList<QLabel*> cue_sliders;
    QList<QLabel*> cue_sliders_number;
    QList<int>     cue_sliders_position_x;
    QList<float>   cue_sliders_absolute_position;
    unsigned int   end_of_waveform;

 public:
    QPointF *points; // Table of points to display.

 public:
    Waveform(Audio_track *in_at, QWidget *in_parent = 0);
    ~Waveform();

    bool move_slider(float in_position);                                      // Position is between 0.0 and 1.0.
    bool move_cue_slider(unsigned short in_cue_point_num, float in_position); // Position is between 0.0 and 1.0.

 private:
    void get_area_size();
    bool jump_slider(int in_x);
    bool generate_polyline();
    void draw_cue_slider(unsigned short in_cue_point_num);

 protected:
    virtual void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *in_mouse_event);

 signals:
    void slider_position_changed(float in_position);     // Position is between 0.0 and 1.0.
    void cue_slider_position_changed(float in_position); // Position is between 0.0 and 1.0.
};

#endif /* WAVEFORM_H_ */
