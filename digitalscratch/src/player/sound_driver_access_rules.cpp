/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*--------------------------------------------( Sound_driver_access_rules.cpp )-*/
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
/*        Behavior class: access sound card (open, close, list, ...)          */
/*                                                                            */
/*============================================================================*/

#include <QtDebug>
#include "sound_driver_access_rules.h"
#include <singleton.h>
#include <application_settings.h>

Sound_driver_access_rules::Sound_driver_access_rules(unsigned short int in_nb_channels)
{
    qDebug() << "Sound_driver_access_rules::Sound_driver_access_rules: create object...";

    if ((in_nb_channels == 0) || (in_nb_channels > 4))
    {
        qFatal("Sound_driver_access_rules::Sound_driver_access_rules: DigitalScratch can only handle 2 decks maximum.");
        return;
    }

    this->nb_channels = in_nb_channels;
    this->callback_param = NULL;
    this->running = false;

    qDebug() << "Sound_driver_access_rules::Sound_driver_access_rules: create object done.";

    return;
}

Sound_driver_access_rules::~Sound_driver_access_rules()
{
    qDebug() << "Sound_driver_access_rules::~Sound_driver_access_rules: delete object...";

    qDebug() << "Sound_driver_access_rules::~Sound_driver_access_rules: delete object done.";

    return;
}

bool
Sound_driver_access_rules::is_running()
{
    return this->running;
}
