/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*-----------------------------------------------------------( waveform.cpp )-*/
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
/*                        Define a waveform                                   */
/*                                                                            */
/*============================================================================*/

#include <QLabel>
#include <QPainter>
#include <QtDebug>
#include <QMouseEvent>
#include <iostream>
#include <math.h>

#include "waveform.h"
#include "application_logging.h"

Waveform::Waveform(QSharedPointer<Audio_track> &in_at, QWidget *in_parent) : QLabel(in_parent)
{
    // Get audio track.
    if (in_at.data() == NULL)
    {
        qCCritical(DS_OBJECTLIFE) << "audio track can not be null";
    }
    this->at = in_at;

    // Init.
    this->area_height = 0;
    this->area_width  = 0;
    this->slider_absolute_position = 0;

    // Create table of points to display.
    this->points = new QPointF[POINTS_MAX_SIZE];
    this->end_of_waveform = 0;
    this->force_regenerate_polyline = true;

    // Create slider.
    this->slider_position_x = 0;
    this->slider = new QLabel(this);
    this->slider->setGeometry(0, 0, 0, 0);
    this->slider->setObjectName("Slider");
    this->slider->setStyleSheet("background-color: orange;");

    // Create cue sliders.
    for (unsigned short int i = 0; i < MAX_NB_CUE_POINTS; i++)
    {
        this->cue_sliders << new QLabel(this);
        this->cue_sliders_number << new QLabel(QString::number(i + 1), this);
        this->cue_sliders[i]->setObjectName("CueSlider");
        this->cue_sliders[i]->setStyleSheet("background-color: white;");
        this->cue_sliders_number[i]->setStyleSheet("background-color: white; color: black; font: 6pt; qproperty-alignment: AlignCenter;");
        this->cue_sliders_position_x << 0;
        this->cue_sliders_absolute_position << 0.0;
        this->draw_cue_slider(i);
    }

    return;
}

Waveform::~Waveform()
{
    // Delete table of points.
    delete [] this->points;

    return;
}

void
Waveform::reset()
{
    this->force_regenerate_polyline = true;

    return;
}

void
Waveform::get_area_size()
{
    // Get current display area size.
    int  new_height = this->frameSize().height() - 1;
    int  new_width  = this->frameSize().width() - 1;

    if ((this->area_height != new_height) ||
        (this->area_width  != new_width))
    {
        this->area_height = new_height;
        this->area_width  = new_width;
        this->force_regenerate_polyline = true;
    }

    return;
}

bool
Waveform::generate_polyline()
{
    // Get audio track table of samples.
    unsigned int j = 0;
    short signed int *samples        = this->at->get_samples();
    short signed int  current_sample = 0;

    // For each points take a sample (every 100 samples) and convert it to be
    // displayed in painting area.
    float x = 0.0;
    float y = 0.0;
    this->end_of_waveform = 0;
    for (unsigned int i = 0; i < POINTS_MAX_SIZE; i++)
    {
        // Get sample.
        current_sample = samples[j];

        // Adapt value to paiting area.
        x = (float)(this->area_width * i) / (float)POINTS_MAX_SIZE;
        if (j <= this->at->get_end_of_samples())
        {
            y = (float)(((float)(current_sample - SHRT_MAX) * this->area_height) / (float)(SHRT_MAX * -1 * 2));
            this->end_of_waveform = i;
        }
        else
        {
            // There is no more sample (track is finish), set polyline to 0.
            y = (float)(this->area_height / 2);
        }

        this->points[i].setX(x);
        this->points[i].setY(y);

        // Next sample.
        j += 100;
    }

    this->force_regenerate_polyline = false;

    return true;
}

void
Waveform::paintEvent(QPaintEvent *)
{
    // Get area size.
    this->get_area_size();

    // Generate list of points to draw if size of the waveform changed.
    if (this->force_regenerate_polyline == true)
    {
        this->generate_polyline();
    }

    QPainter painter(this);

    // Draw polyline on current area.
    painter.setPen(QColor("grey"));
    painter.drawPolyline(this->points, POINTS_MAX_SIZE); // waveform from track

    // Draw minute separators.
    painter.setPen(QColor(0, 102, 0)); // kind of green
    painter.drawRect(0, 0, this->area_width, this->area_height);
    float x = 0.0;
    for (int i = 0; i < MAX_MINUTES_TRACK; i++)
    {
        x = i * (float)this->area_width / (float)MAX_MINUTES_TRACK;
        painter.drawLine(qRound(x), 0, qRound(x), this->area_height);
    }

    painter.end();

    // Move sliders.
    this->move_slider(this->slider_absolute_position);
    for (unsigned short int i = 0; i < MAX_NB_CUE_POINTS; i++)
    {
        this->move_cue_slider(i, this->cue_sliders_absolute_position[i]);
    }

    return;
}

bool
Waveform::jump_slider(int in_x)
{
    // Check boundaries.
    if (in_x > this->area_width)
    {
        return false;
    }

    // Move slider to new position if possible.
    unsigned int x_index = floor(((float)in_x * (float)this->at->get_max_nb_samples())
                                   / ((float)this->area_width * 100.0));
    if (x_index <= this->end_of_waveform)
    {
        this->slider_position_x = in_x;
        this->slider->setGeometry(this->slider_position_x, 0, 2, this->area_height);

        // Emit signal to change position in track.
        emit slider_position_changed((float)in_x / (float)this->area_width);

        // Store absolute position.
        this->slider_absolute_position = (float)in_x / (float)this->area_width;
    }

    return true;
}

void
Waveform::mousePressEvent(QMouseEvent *in_mouse_event)
{
    // Move slider to mouse position.
    this->jump_slider(in_mouse_event->x());

    return;
}

bool
Waveform::move_slider(float in_position)
{
    // Check position.
    if (in_position > 1.0)
    {
        return false;
    }

    // Store absolute position.
    this->slider_absolute_position = in_position;

    // Move slider to new position.
    this->slider_position_x = in_position * this->area_width;
    this->slider->setGeometry(this->slider_position_x, 0, 2, this->area_height);

    return true;
}

bool
Waveform::move_cue_slider(unsigned short int in_cue_point_num, float in_position)
{
    // Check position.
    if (in_position > 1.0)
    {
        return false;
    }

    // Store absolute position.
    this->cue_sliders_absolute_position[in_cue_point_num] = in_position;

    // Move slider to new position.
    this->cue_sliders_position_x[in_cue_point_num] = in_position * this->area_width;
    this->draw_cue_slider(in_cue_point_num);

    return true;
}

void
Waveform::draw_cue_slider(unsigned short int in_cue_point_num)
{
    // Show cue slider if defined.
    if (this->cue_sliders_position_x[in_cue_point_num] > 0)
    {
        this->cue_sliders[in_cue_point_num]->setGeometry(this->cue_sliders_position_x[in_cue_point_num], 0, 1, this->area_height);
        this->cue_sliders_number[in_cue_point_num]->setGeometry(this->cue_sliders_position_x[in_cue_point_num], 0, 10, 10);
        this->cue_sliders[in_cue_point_num]->show();
        this->cue_sliders_number[in_cue_point_num]->show();
    }
    else
    {
        // Cue point not defined (= 0), so hide it.
        this->cue_sliders[in_cue_point_num]->hide();
        this->cue_sliders_number[in_cue_point_num]->hide();
    }
}
