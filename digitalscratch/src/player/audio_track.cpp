/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*--------------------------------------------------------( audio_track.cpp )-*/
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
/*                    Class defining an audio track                           */
/*                                                                            */
/*============================================================================*/


#include <iostream>
#include <mpg123.h>
#include <QFileInfo>
#include <QtDebug>

#include "audio_track.h"

Audio_track::Audio_track(short unsigned int in_max_minutes)
{
    qDebug() << "Audio_track::Audio_trac: create object...";

    // Create table of sample base of number of minutes.
    this->max_nb_samples = in_max_minutes * 2 * 60 * SAMPLE_RATE;
    // Add also several seconds more, which is used to put more infos in decoding step.
    this->samples = new short signed int[this->max_nb_samples + this->get_security_nb_samples()];
    this->reset();

    qDebug() << "Audio_track::Audio_trac: create object done";

    return;
}

Audio_track::~Audio_track()
{
    qDebug() << "Audio_track::~Audio_track: delete object...";

    delete this->samples;

    qDebug() << "Audio_track::~Audio_track: delete object done.";

    return;
}

void
Audio_track::reset()
{
    qDebug() << "Audio_track::reset...";

    this->end_of_samples = 0;
    this->name           = "";
    this->length         = 0;

    std::fill(this->samples, this->samples + this->get_max_nb_samples(), 0);

    qDebug() << "Audio_track::reset done.";

    return;
}

short signed int*
Audio_track::get_samples()
{
    qDebug() << "Audio_track::get_samples...";
    qDebug() << "Audio_track::get_samples done.";

    return this->samples;
}

unsigned int
Audio_track::get_end_of_samples()
{
    qDebug() << "Audio_track::get_end_of_samples...";
    qDebug() << "Audio_track::get_end_of_samples done.";

    return this->end_of_samples;
}

bool
Audio_track::set_end_of_samples(unsigned int in_end_of_samples)
{
    qDebug() << "Audio_track::set_end_of_samples...";

    if (in_end_of_samples > this->get_max_nb_samples())
    {
        qCritical() << "Audio_track::set_end_of_samples: in_end_of_samples too big.";
        return FALSE;
    }
    else
    {
        // Set end of sample index.
        this->end_of_samples = in_end_of_samples;

        // Set length of the track.
        this->length = (unsigned int)(1000.0 * ((float)this->end_of_samples + 1.0) / (2.0 * (float)SAMPLE_RATE));
    }

    qDebug() << "Audio_track::set_end_of_samples done.";

    return TRUE;
}

unsigned int
Audio_track::get_max_nb_samples()
{
    qDebug() << "Audio_track::get_max_nb_samples...";
    qDebug() << "Audio_track::get_max_nb_samples done.";

    return this->max_nb_samples;
}

signed int
Audio_track::get_sample_rate()
{
    qDebug() << "Audio_track::get_sample_rate...";
    qDebug() << "Audio_track::get_sample_rate done.";

    return SAMPLE_RATE;
}

unsigned int
Audio_track::get_security_nb_samples()
{
    qDebug() << "Audio_track::get_security_nb_samples...";
    qDebug() << "Audio_track::get_security_nb_samples done.";

    return SECURITY_NB_SAMPLES;
}

unsigned int
Audio_track::get_length()
{
    qDebug() << "Audio_track::get_length...";
    qDebug() << "Audio_track::get_length done.";

    return this->length;
}

QString
Audio_track::get_name()
{
    qDebug() << "Audio_track::get_name...";
    qDebug() << "Audio_track::get_name done.";

    return this->name;
}

bool
Audio_track::set_name(QString in_name)
{
    qDebug() << "Audio_track::set_name...";

    if (in_name != this->name)
    {
        this->name = in_name;
        emit name_changed(this->name);
    }

    qDebug() << "Audio_track::set_name done.";

    return true;
}
