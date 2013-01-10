/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*-----------------------------------------------------------( waveform.cpp )-*/
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

#include <QLabel>
#include <QPainter>
#include <QtDebug>
#include <QMouseEvent>
#include <iostream>
#include <math.h>

#include "waveform.h"

Waveform::Waveform(Audio_track *in_at, QWidget *in_parent) : QLabel(in_parent)
{
    qDebug() << "Waveform::Waveform: create object...";

    // Get audio track.
    if (in_at == NULL)
    {
        qFatal("Waveform::Waveform: audio track can not be null");
    }
    this->at = in_at;

    // Create table of points to display.
    this->points = new QPointF[POINTS_MAX_SIZE];
    this->end_of_waveform = 0;

    // Create slider.
    this->slider_position_x = 0;
    this->slider = new QLabel(this);
    this->slider->setGeometry(0, 0, 0, 0);
    this->slider->setObjectName("Slider");
    this->slider->setStyleSheet("background-color: orange;");

    // Create cue slider.
    this->cue_slider_position_x = 0;
    this->cue_slider = new QLabel(this);
    this->draw_cue_slider();
    this->cue_slider->setObjectName("CueSlider");
    this->cue_slider->setStyleSheet("background-color: white;");

    qDebug() << "Waveform::Waveform: create object done.";

    return;
}

Waveform::~Waveform()
{
    qDebug() << "Waveform::Waveform: delete object...";

    // Delete table of points.
    delete [] this->points;

    // TODO : delete slider and cue_slider ???

    qDebug() << "Waveform::Waveform: delete object done.";

    return;
}

void
Waveform::get_area_size()
{
    qDebug() << "Waveform::get_area_size...";

    // Get current display area size.
    this->area_height = this->frameSize().height() - 1;
    this->area_width  = this->frameSize().width() - 1;
    qDebug() << "Waveform::get_area_size: Size of area: h=" << this->area_height << "X w=" << this->area_width;

    qDebug() << "Waveform::get_area_size done.";

    return;
}

bool
Waveform::generate_polyline()
{
    qDebug() << "Waveform::generate_polyline...";

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
            y = (float)(((float)(current_sample - 32768) * this->area_height) / (float)(-32768 * 2));
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

    qDebug() << "Waveform::generate_polyline done.";

    return true;
}

void
Waveform::paintEvent(QPaintEvent *)
{
    qDebug() << "Waveform::paintEvent...";

    // Get size of painting area.
    this->get_area_size();

    // Generate list of points to draw.
    if (this->generate_polyline() == false)
    {
        qDebug() << "Waveform::paintEvent: can not generate polyline";
    }

    // Draw polyline on current area.
    QPainter painter(this);
    painter.setPen(QColor("grey"));
//painter.scale(10.0, 1.0);
    painter.drawPolyline(this->points, POINTS_MAX_SIZE);
    painter.end();

    // Resize sliders height.
    this->slider->setGeometry(this->slider_position_x, 0, 2, this->area_height);
    this->draw_cue_slider();

    qDebug() << "Vertical::paintEvent done.";

    return;
}

bool
Waveform::jump_slider(int in_x)
{
    qDebug() << "Waveform::jump_slider...";

    // Check boundaries.
    if (in_x > this->area_width)
    {
        qDebug() << "Waveform::jump_slider: can not move slider to x =" << in_x;
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
    }

    qDebug() << "Waveform::jump_slider done.";

    return true;
}

void
Waveform::mousePressEvent(QMouseEvent *in_mouse_event)
{
    qDebug() << "Waveform::mousePressEvent...";

    // Move slider to mouse position.
    this->jump_slider(in_mouse_event->x());
    qDebug() << "Waveform::mousePressEvent: new x = " << in_mouse_event->x();

    qDebug() << "Waveform::mousePressEvent done.";

    return;
}

bool
Waveform::move_slider(float in_position)
{
    qDebug() << "Waveform::move_slider...";

    // Check position.
    if (in_position > 1.0)
    {
        qDebug() << "Waveform::move_slider: can not move slider to position =" << in_position;
        return false;
    }

    // Move slider to new position.
    this->slider_position_x = in_position * this->area_width;
    this->slider->setGeometry(this->slider_position_x, 0, 2, this->area_height);

    qDebug() << "Waveform::move_slider done.";

    return true;
}

bool
Waveform::move_cue_slider(float in_position)
{
    qDebug() << "Waveform::move_cue_slider...";

    // Check position.
    if (in_position > 1.0)
    {
        qDebug() << "Waveform::move_cue_slider: can not move cue slider to position =" << in_position;
        return false;
    }

    // Move slider to new position.
    this->cue_slider_position_x = in_position * this->area_width;
    this->draw_cue_slider();

    qDebug() << "Waveform::move_cue_slider done.";

    return true;
}

void
Waveform::draw_cue_slider()
{
    this->cue_slider->setGeometry(this->cue_slider_position_x, 4, 1, this->area_height - 8);
}
