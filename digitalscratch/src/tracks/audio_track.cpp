/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*--------------------------------------------------------( audio_track.cpp )-*/
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
/*                    Class defining an audio track                           */
/*                                                                            */
/*============================================================================*/


#include <iostream>
#include <QFileInfo>
#include <QtDebug>
#include <QDir>

#include "tracks/audio_track.h"
#include "app/application_logging.h"
#include "utils.h"

Audio_track::Audio_track(const unsigned int &sample_rate)
{
    // Do not store audio samples.
    this->sample_rate = sample_rate;
    this->max_nb_samples = 0;
    this->samples = nullptr;
    this->reset();

    return;
}

Audio_track::Audio_track(const short unsigned int &max_minutes,
                         const unsigned int       &sample_rate)
{
    // Create table of sample base of number of minutes.
    this->sample_rate = sample_rate;
    this->max_nb_samples = max_minutes * 2 * 60 * this->sample_rate;
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
    if (this->samples != nullptr)
    {
        memset(&this->samples[0], 0, (this->max_nb_samples + this->get_security_nb_samples()) * sizeof this->samples[0]);
    }

    return;
}

short signed int*
Audio_track::get_samples() const
{
    return this->samples;
}

unsigned int
Audio_track::get_end_of_samples() const
{
    return this->end_of_samples;
}

bool
Audio_track::set_end_of_samples(const unsigned int &end_of_samples)
{
    if (this->samples != nullptr)
    {
        if (end_of_samples > this->get_max_nb_samples())
        {
            qCWarning(DS_PLAYBACK) << "end_of_samples too big";
            return false;
        }
        else
        {
            // Set end of sample index.
            this->end_of_samples = end_of_samples;

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
Audio_track::get_max_nb_samples() const
{
    return this->max_nb_samples;
}

unsigned int
Audio_track::get_sample_rate() const
{
    return this->sample_rate;
}

unsigned int
Audio_track::get_security_nb_samples() const
{
    return 2 * 10 * this->sample_rate; // Number of samples added at the end of *samples for decoding purpose.
}

unsigned int
Audio_track::get_length() const
{
    return this->length;
}

QString
Audio_track::get_length_str() const
{
    return Utils::get_str_time_from_sample_index(this->end_of_samples, this->sample_rate, false);
}

QString
Audio_track::get_name() const
{
    return this->name;
}

bool
Audio_track::set_name(const QString &name)
{
    if (name != this->name)
    {
        this->name = name;
    }

    return true;
}

QString
Audio_track::get_path() const
{
    return this->path;
}

bool
Audio_track::set_fullpath(const QString &fullpath)
{
    // Store path and filename
    QFileInfo path_info(fullpath);
    this->path     = path_info.absolutePath();
    this->filename = path_info.fileName();

    return true;
}

QString
Audio_track::get_filename() const
{
    return this->filename;
}


QString
Audio_track::get_hash() const
{
    return this->hash;
}

bool
Audio_track::set_hash(const QString &hash)
{
    this->hash = hash;

    return true;
}

QString
Audio_track::get_music_key() const
{
    return this->music_key;
}

bool
Audio_track::set_music_key(const QString &key)
{
    this->music_key = key;

    return true;
}

QString
Audio_track::get_music_key_tag() const
{
    return this->music_key_tag;
}

bool
Audio_track::set_music_key_tag(const QString &key_tag)
{
    this->music_key_tag = key_tag;

    return true;
}

