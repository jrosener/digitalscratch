/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*--------------------------------------------------------------( utils.cpp )-*/
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

#include <utils.h>
#include <QCryptographicHash>
#include <algorithm>
#include <QtDebug>
#include <QFile>
#include <libkeyfinder/keyfinder.h>

QString Utils::get_file_hash(QString in_path, unsigned int in_kbytes)
{   
    qDebug() << "Utils::get_file_hash...";

    // Init.
    QString hash("");

    // Check if path is defined.
    if (in_path == NULL)
    {
        qWarning() << "Utils::get_file_hash: path is NULL.";
        return "";
    }

    // Check number of kbytes.
    if (in_kbytes == 0)
    {
        qWarning() << "Utils::get_file_hash: nb bytes to hash is 0.";
        return "";
    }

    // Check if file exists.
    QFile file(in_path);
    if (file.exists() == FALSE)
    {
        qWarning() << "Utils::get_file_hash: file " << in_path << " does not exists.";
        return "";
    }

    // Open file as binary.
    if (file.open(QIODevice::ReadOnly) == FALSE)
    {
        return "";
    }

    // Get a hash of the first bytes of data (or the size of the file if it is less).
    QByteArray bin  = file.read(std::min((qint64)(in_kbytes*1024), file.size()));
    hash = QString(QCryptographicHash::hash(bin, QCryptographicHash::Md5).toHex());

    // Cleanup.
    file.close();

    qDebug() << "Utils::get_file_hash: done.";

    return hash;
}

QString Utils::get_music_key(short signed int   *in_samples,
                             unsigned int        in_nb_samples,
                             short unsigned int  in_frame_rate,
                             short unsigned int  in_nb_channels)
{
    // Check input parameter.
    if ((in_samples == NULL) || (in_nb_samples == 0))
    {
        return "";
    }

    // Static because it retains useful resources for repeat use
    static KeyFinder::KeyFinder k;

    // Build an empty audio object
    KeyFinder::AudioData a;

    // Prepare the object for your audio stream
    a.setFrameRate(in_frame_rate);
    a.setChannels(in_nb_channels);
    a.addToSampleCount(in_nb_samples);

    // Copy your audio into the object
    for (unsigned int i = 0; i < in_nb_samples; i++)
    {
        a.setSample(i, in_samples[i]);
    }

    // Run the analysis
    KeyFinder::KeyDetectionResult r =  k.keyOfAudio(a);

    // And do something with the result!
    KeyFinder::key_t key = r.globalKeyEstimate;

    switch(key)
    {
        case KeyFinder::A_MAJOR:      return "AM";
        case KeyFinder::A_MINOR:      return "Am";
        case KeyFinder::B_FLAT_MAJOR: return "BbM";
        case KeyFinder::B_FLAT_MINOR: return "Bbm";
        case KeyFinder::B_MAJOR:      return "BM";
        case KeyFinder::B_MINOR:      return "Bm";
        case KeyFinder::C_MAJOR:      return "CM";
        case KeyFinder::C_MINOR:      return "Cm";
        case KeyFinder::D_FLAT_MAJOR: return "DbM";
        case KeyFinder::D_FLAT_MINOR: return "Dbm";
        case KeyFinder::D_MAJOR:      return "DM";
        case KeyFinder::D_MINOR:      return "Dm";
        case KeyFinder::E_FLAT_MAJOR: return "EbM";
        case KeyFinder::E_FLAT_MINOR: return "Ebm";
        case KeyFinder::E_MAJOR:      return "EM";
        case KeyFinder::E_MINOR:      return "Em";
        case KeyFinder::F_MAJOR:      return "FM";
        case KeyFinder::F_MINOR:      return "Fm";
        case KeyFinder::G_FLAT_MAJOR: return "GbM";
        case KeyFinder::G_FLAT_MINOR: return "Gbm";
        case KeyFinder::G_MAJOR:      return "GM";
        case KeyFinder::G_MINOR:      return "Gm";
        case KeyFinder::A_FLAT_MAJOR: return "AbM";
        case KeyFinder::A_FLAT_MINOR: return "Abm";
        case KeyFinder::SILENCE:      return "";
        default:                      return "";
    }
}
