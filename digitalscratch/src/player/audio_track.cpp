/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*--------------------------------------------------------( audio_track.cpp )-*/
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
/*                    Class defining an audio track                           */
/*                                                                            */
/*============================================================================*/


#include <iostream>
#include <QFileInfo>
#include <QtDebug>
#include <QDir>

#include "audio_track.h"
#include <utils.h>

Audio_track::Audio_track(unsigned int in_sample_rate)
{
    // Do not store audio samples.
    this->sample_rate = in_sample_rate;
    this->max_nb_samples = 0;
    this->samples = NULL;
    this->reset();

    return;
}

Audio_track::Audio_track(short unsigned int in_max_minutes, unsigned int in_sample_rate)
{
    // Create table of sample base of number of minutes.
    this->sample_rate = in_sample_rate;
    this->max_nb_samples = in_max_minutes * 2 * 60 * this->sample_rate;
    // Add also several seconds more, which is used to put more infos in decoding step.
    this->samples = new short signed int[this->max_nb_samples + this->get_security_nb_samples()];
    this->reset();

    return;
}

Audio_track::~Audio_track()
{
    delete [] this->samples;

    return;
}

void
Audio_track::reset()
{
    this->set_name("");
    this->end_of_samples = 0;
    this->length         = 0;
    this->hash           = "";
    this->path           = "";
    this->filename       = "";
    this->music_key      = "";
    this->music_key_tag  = "";
    if (this->samples != NULL)
    {
        std::fill(this->samples, this->samples + this->max_nb_samples + this->get_security_nb_samples(), 0);
    }

    return;
}

short signed int*
Audio_track::get_samples()
{
    return this->samples;
}

unsigned int
Audio_track::get_end_of_samples()
{
    return this->end_of_samples;
}

bool
Audio_track::set_end_of_samples(unsigned int in_end_of_samples)
{
    if (this->samples != NULL)
    {
        if (in_end_of_samples > this->get_max_nb_samples())
        {
            qCWarning(DS_AUDIOTRACK) << "in_end_of_samples too big";
            return false;
        }
        else
        {
            // Set end of sample index.
            this->end_of_samples = in_end_of_samples;

            // Set length of the track.
            this->length = (unsigned int)(1000.0 * ((float)this->end_of_samples + 1.0) / (2.0 * (float)this->sample_rate));
        }
    }
    else
    {
        return false;
    }

    return true;
}

unsigned int
Audio_track::get_max_nb_samples()
{
    return this->max_nb_samples;
}

unsigned int
Audio_track::get_sample_rate()
{
    return this->sample_rate;
}

unsigned int
Audio_track::get_security_nb_samples()
{
    return 2 * 10 * this->sample_rate; // Number of samples added at the end of *samples for decoding purpose.
}

unsigned int
Audio_track::get_length()
{
    return this->length;
}

QString
Audio_track::get_length_str()
{
    return Utils::get_str_time_from_sample_index(this->end_of_samples, this->sample_rate, false);
}

QString
Audio_track::get_name()
{
    return this->name;
}

bool
Audio_track::set_name(QString in_name)
{
    if (in_name != this->name)
    {
        this->name = in_name;
        if (in_name == "")
        {
            emit name_changed("--");
        }
        else
        {
            emit name_changed("[" + this->get_length_str() + "]  " + this->name);
        }
    }

    return true;
}

QString
Audio_track::get_path()
{
    return this->path;
}

bool
Audio_track::set_fullpath(QString in_fullpath)
{
    // Store path and filename
    QFileInfo path_info(in_fullpath);
    this->path     = path_info.absolutePath();
    this->filename = path_info.fileName();

    return true;
}

QString
Audio_track::get_filename()
{
    return this->filename;
}


QString
Audio_track::get_hash()
{
    return this->hash;
}

bool
Audio_track::set_hash(QString in_hash)
{
    this->hash = in_hash;

    return true;
}

QString
Audio_track::get_music_key()
{
    return this->music_key;
}

bool
Audio_track::set_music_key(QString in_key)
{
    this->music_key = in_key;
    emit key_changed(this->music_key);

    return true;
}

QString
Audio_track::get_music_key_tag()
{
    return this->music_key_tag;
}

bool
Audio_track::set_music_key_tag(QString in_key_tag)
{
    this->music_key_tag = in_key_tag;

    return true;
}

