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
