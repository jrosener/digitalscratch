/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*-------------------------------------------( timecode_control_process.cpp )-*/
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
/* Behavior class: analyze captured timecode and determine playback           */
/* parameters.                                                                */
/*                                                                            */
/*============================================================================*/

#include <QtDebug>
#include <QString>
#include <cmath>

#include "control/timecode_control_process.h"
#include "app/application_logging.h"

Timecode_control_process::Timecode_control_process(const QSharedPointer<Playback_parameters> &param,
                                                   const dscratch_vinyls_t                   &vinyl_type,
                                                   const unsigned int                        &sample_rate) : Control_process(param)
{
    //
    // Initialize DigitalScratch library.
    //
    // Create turntable.
    if (dscratch_create_turntable(vinyl_type, sample_rate, &this->dscratch_handle) != DSCRATCH_SUCCESS)
    {
        qCCritical(DS_PLAYBACK) << "can not create turntable";
    }

    this->waitfor_emit_speed_changed = 0;

    return;
}

Timecode_control_process::~Timecode_control_process()
{
    // Delete dscratch turntable.
    dscratch_delete_turntable(this->dscratch_handle);

    return;
}

bool Timecode_control_process::run(const unsigned short int &nb_samples,
                                   const float              *samples_1,
                                   const float              *samples_2)
{
    int   are_new_params = 0;
    float speed          = 0.0;
    float volume         = 0.0;

    // Iterate over decks and analyze captured timecode.
    if (dscratch_analyze_recorded_datas(this->dscratch_handle,
                                        samples_1,
                                        samples_2,
                                        nb_samples) != DSCRATCH_SUCCESS)
    {
        qCWarning(DS_PLAYBACK) << "cannot analyze captured data";
    }

    // Update playing parameters.
    if ((are_new_params = dscratch_get_playing_parameters(this->dscratch_handle,
                                                          &speed,
                                                          &volume)) == DSCRATCH_SUCCESS)
    {
        if (are_new_params == 0)
        {
            this->params->set_data_state(true); // FIXME:  still needed ?
            this->params->set_speed(speed);
            this->params->set_speed_state(true);// FIXME:  still needed ?

            // Change speed label in Gui only every 10 times.
            if (this->waitfor_emit_speed_changed > 10)
            {
                // FIXME: it looks like if we change regularly the speed on the gui, sometimes the app is crashing.
                //        so, for the moment, we do not send the signal for changing speed.
                emit speed_changed(this->params->get_speed());
                this->waitfor_emit_speed_changed = 0;
            }
            else
            {
                this->waitfor_emit_speed_changed++;
            }

            this->params->set_volume(volume);
            this->params->set_volume_state(true); // FIXME:  still needed ?
            emit volume_changed((double)(floorf((this->params->get_volume() * 100.0) * 10.0) / 10.0));
        }
        else
        {
            this->params->set_data_state(false);
        }
    }

    return true;
}

dscratch_handle_t Timecode_control_process::get_dscratch_handle()
{
    return this->dscratch_handle;
}
