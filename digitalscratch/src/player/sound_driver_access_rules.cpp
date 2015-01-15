/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*--------------------------------------------( Sound_driver_access_rules.cpp )-*/
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
/*        Behavior class: access sound card (open, close, list, ...)          */
/*                                                                            */
/*============================================================================*/

#include <QtDebug>

#include "sound_driver_access_rules.h"
#include "singleton.h"
#include "application_logging.h"

Sound_driver_access_rules::Sound_driver_access_rules(unsigned short int in_nb_channels)
{
    if (in_nb_channels < 2)
    {
        qCWarning(DS_SOUNDCARD) << "Number of channels must be at least 2.";
        return;
    }

    this->nb_channels = in_nb_channels;
    this->callback_param = NULL;
    this->do_capture = true;
    this->running = false;

    return;
}

Sound_driver_access_rules::~Sound_driver_access_rules()
{
    return;
}

bool
Sound_driver_access_rules::is_running()
{
    return this->running;
}

void
Sound_driver_access_rules::set_capture(bool in_do_capture)
{
    this->do_capture = in_do_capture;
}
