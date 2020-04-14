/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*-------------------------------------------( manual_control_process.cpp )-*/
/*                                                                            */
/*  Copyright (C) 2003-2020                                                   */
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
/* Behavior class: determine playback parameters based on keyboard and gui    */
/*                 buttons                                                    */
/*                                                                            */
/*============================================================================*/

#include <QtDebug>
#include <QString>

#include "control/manual_control_process.h"
#include "app/application_logging.h"

Manual_control_process::Manual_control_process(const QSharedPointer<Playback_parameters> &param) : Control_process(param)
{
    this->params = param;
    this->speed  = 1.0;
    this->do_temp_inc_speed = false;
    this->previous_speed = 0.0;
    this->nb_temp_speed_inc_cycles = 0;

    return;
}

Manual_control_process::~Manual_control_process()
{
    return;
}

bool
Manual_control_process::run()
{
    // If using temporary speed acceleration then set the accelerated speed. At the end of the acceleration, reset to
    // previous speed.
    if (this->do_temp_inc_speed == true)
    {
        if (this->nb_temp_speed_inc_cycles > 0)
        {
            // Temporary acceleration is still running.
            this->nb_temp_speed_inc_cycles--;
        }
        else
        {
            // Temporary acceleration is done. Switch back to the previous speed.
            this->do_temp_inc_speed = false;
            this->set_new_speed(this->previous_speed);
        }
    }

    // Calculate volume based on speed.
    this->params->set_volume(1.0); // TODO calculate volume based on speed.

    return true;
}

void
Manual_control_process::inc_speed(const float &speed_inc)
{
    this->set_new_speed(this->params->get_speed() + speed_inc);
}

void
Manual_control_process::inc_temporary_speed(const float              &temp_speed_inc,
                                            const unsigned short int &nb_cycles)
{
    if (this->do_temp_inc_speed == false)
    {
        // We are not already in a acceleration phase, so store the current speed.
        this->previous_speed = this->params->get_speed();
    }

    // Accelerate speed.
    this->nb_temp_speed_inc_cycles = nb_cycles;
    this->do_temp_inc_speed = true;
    this->inc_speed(temp_speed_inc);
}

void
Manual_control_process::reset_speed_to_100p()
{
    this->set_new_speed(1.0);
}

void
Manual_control_process::set_new_speed(const float &speed)
{
    this->params->set_speed(speed);
    emit speed_changed(this->params->get_speed());
}
