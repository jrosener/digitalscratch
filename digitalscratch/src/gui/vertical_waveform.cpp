/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*--------------------------------------------------( vertical_waveform.cpp )-*/
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
/*                        Define a vertical waveform                          */
/*                                                                            */
/*============================================================================*/

#include <QLabel>
#include <QPainter>
#include <QtDebug>
#include <iostream>

#include "vertical_waveform.h"

Vertical_waveform::Vertical_waveform(Audio_track *in_at, QWidget *in_parent) : QLabel(in_parent)
{
    qDebug() << "Vertical_waveform::Vertical_waveform: create object...";

    // Get audio track.
    if (in_at == NULL)
    {
        qFatal("Waveform::Waveform: audio track can not be null");
    }
    this->at = in_at;

    // Create table of points to display.
    this->points = new QPointF[V_WAVEFORM_POINTS_MAX_SIZE];

    qDebug() << "Vertical_waveform::Vertical_waveform: create object done.";

    return;
}

Vertical_waveform::~Vertical_waveform()
{
    qDebug() << "Vertical_waveform::Vertical_waveform: delete object...";

    // Delete table of points.
    delete [] this->points;

    qDebug() << "Vertical_waveform::Vertical_waveform: delete object done.";

    return;
}

void
Vertical_waveform::get_area_size()
{
    qDebug() << "Vertical_waveform::get_area_size...";

    // Get current display area size.
    this->area_height = this->frameSize().height() - 1;
    this->area_width  = this->frameSize().width() - 1;
    qDebug() << "Vertical_waveform::get_area_size: Size of area: h=" << this->area_height << "X w=" << this->area_width;

    cout << "Vertical_waveform::get_area_size: Size of area: h=" << this->area_height << "X w=" << this->area_width << endl;

    qDebug() << "Vertical_waveform::get_area_size done.";

    return;
}

bool
Vertical_waveform::generate_polyline()
{
    qDebug() << "Vertical_waveform::generate_polyline...";

    // Get audio track table of samples.
    int j = 0;
    short signed int *samples        = this->at->get_samples();
    short signed int  current_sample = 0;

    // For each points take a sample (every 100 samples) and convert it to be
    // displayed in painting area.
    float x = 0.0;
    float y = 0.0;
    for (unsigned int i = 0; i < V_WAVEFORM_POINTS_MAX_SIZE; i++)
    {
        // Get sample.
        current_sample = samples[j];

        // Adapt value to paiting area.
        x = (float)(this->area_width * i) / (float)V_WAVEFORM_POINTS_MAX_SIZE;
        y = (float)(((float)(current_sample - 32768) * this->area_height) / (float)(-32768 * 2));
        this->points[i].setX(x);
        this->points[i].setY(y);

        // Next sample.
        j += 100;
    }

    qDebug() << "Vertical_waveform::generate_polyline done.";

    return true;
}

void
Vertical_waveform::paintEvent(QPaintEvent *)
{
    qDebug() << "Vertical_waveform::paintEvent...";

    // Get size of painting area.
    this->get_area_size();

    // Draw polyline on current area.
    QPainter painter(this);
    painter.setPen(QColor("grey"));
    painter.rotate(90);
    painter.translate(0, -this->area_width);
    painter.scale(50.0, 1.0);

    // After rotation area size changed.
    int tmp = this->area_height;
    this->area_height = this->area_width;
    this->area_width = tmp;

    // Generate list of points to draw.
    if (this->generate_polyline() == false)
    {
        qDebug() << "Vertical_waveform::paintEvent: can not generate polyline";
    }

    // Draw polyline.
//    painter.drawPolyline(this->points, V_WAVEFORM_POINTS_MAX_SIZE);
#if 0
    painter.drawText(10, 10, "0");
    painter.drawText(this->area_width - 10, 10, "1");
    painter.drawText(this->area_width - 10, this->area_height - 10, "2");
    painter.drawText(10, this->area_height - 10, "3");
    painter.drawPoint(QPoint(0,0));
    painter.drawPoint(QPoint(this->area_width,0));
    painter.drawPoint(QPoint(0,this->area_height));
    painter.drawPoint(QPoint(this->area_width,this->area_height));
#endif

    painter.end();

    qDebug() << "Vertical_waveform::paintEvent done.";

    return;
}
