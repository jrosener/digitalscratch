/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*------------------------------------------------------( remining_time.cpp )-*/
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
/*                     Define a remaining time component                      */
/*                                                                            */
/*============================================================================*/

#include <QtDebug>
#include "remaining_time.h"

Remaining_time::Remaining_time()
{
    qDebug() << "Remaining_time::Remaining_time: create object...";

    this->minus = new QLabel("-");
    this->min   = new QLabel("00");
    this->sep1  = new QLabel(":");
    this->sec   = new QLabel("00");
    this->sep2  = new QLabel(":");
    this->msec  = new QLabel("000");

    this->minus->setObjectName("RemainingTime");
    this->min->setObjectName("RemainingTime");
    this->sep1->setObjectName("RemainingTime");
    this->sec->setObjectName("RemainingTime");
    this->sep2->setObjectName("RemainingTime");
    this->msec->setObjectName("RemainingTimeMsec");

    qDebug() << "Remaining_time::Remaining_time: create object done.";

    return;
}

Remaining_time::~Remaining_time()
{
    qDebug() << "Remaining_time::Remaining_time: delete object...";

    qDebug() << "Remaining_time::Remaining_time: delete object done.";

    return;
}
