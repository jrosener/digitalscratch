/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------( dicer_control_process.cpp )-*/
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
/* Behavior class: drive the Novation Dicer external controller.              */
/*                                                                            */
/*============================================================================*/

#include <QtDebug>
#include <iomanip>
#include <QThread>

#include "control/dicer_control_process.h"
#include "app/application_logging.h"
#include "app/application_settings.h"

#define DEVICE_ID "hw:Dicer"

Dicer_control_process::Dicer_control_process()
{
    // Flag used to protect methods that send MIDI messages.
    this->is_open = false;

    // Do not use the event loop of the object that created that one.
    moveToThread(&this->reader_thread);
    connect(&this->reader_thread, SIGNAL(started()), this, SLOT(exec_midi_commands_reader_process()));

    return;
}

Dicer_control_process::~Dicer_control_process()
{
    return;
}

bool Dicer_control_process::extract_midi_buffer(const char              buf[],
                                                dicer_t                &out_dicer_index,
                                                dicer_mode_t           &out_mode,
                                                dicer_button_t         &out_button_index,
                                                dicer_button_pressed_t &out_button_pressed)
{
    // Get Dicer index (left or right).
    //   Info is in buf[0].
    //   9A/9B/9C for left.
    //   9D/9E/9F for right.
    char dicer_and_mode_btn = buf[0] & 0x0F;
    if (dicer_and_mode_btn < 0x0D)
        out_dicer_index = DICER_LEFT;
    else
        out_dicer_index = DICER_RIGHT;

    // Get mode button (hotcue, looproll or autoloop).
    //   Info is in buf[0].
    //   9A/9D for hotcue.
    //   9B/9E for looproll.
    //   9C/9F for autoloop.
    dicer_button_mode_t btn_mode;
    switch(dicer_and_mode_btn)
    {
        case 0x0A:
        case 0x0D:
            btn_mode = BUTTON_MODE_HOT_CUE;
            break;
        case 0x0B:
        case 0x0E:
            btn_mode = BUTTON_MODE_LOOP_ROLL;
            break;
        case 0x0C:
        case 0x0F:
            btn_mode = BUTTON_MODE_AUTO_LOOP;
            break;
        default:
            return false;
    }

    // Get button (1, 2, 3, 4 or 5).
    //   Info is in buf[1].
    //   3C/41 for btn 1.
    //   3D/42 for btn 2.
    //   3E/43 for btn 3.
    //   3F/44 for btn 4.
    //   40/45 for btn 5.
    switch (buf[1])
    {
        case 0x3C:
        case 0x41:
            out_button_index = BUTTON_1;
            break;
        case 0x3D:
        case 0x42:
            out_button_index = BUTTON_2;
            break;
        case 0x3E:
        case 0x43:
            out_button_index = BUTTON_3;
            break;
        case 0x3F:
        case 0x44:
            out_button_index = BUTTON_4;
            break;
        case 0x40:
        case 0x45:
            out_button_index = BUTTON_5;
            break;
        default:
            return false;
    }

    // Get running mode (hotcue, looproll, autoloop, clearcue, usermode1, usermode2).
    //   Info is a mix between btn_mode and buf[1].
    //   BUTTON_MODE_HOT_CUE   and 3C/3D/3E/3F/40 for hotcue
    //   BUTTON_MODE_HOT_CUE   and 41/42/43/44/45 for clearcue
    //   BUTTON_MODE_LOOP_ROLL and 3C/3D/3E/3F/40 for looproll
    //   BUTTON_MODE_LOOP_ROLL and 41/42/43/44/45 for usermode1
    //   BUTTON_MODE_AUTO_LOOP and 3C/3D/3E/3F/40 for autoloop
    //   BUTTON_MODE_AUTO_LOOP and 41/42/43/44/45 for usermode2
    switch (btn_mode)
    {
        case BUTTON_MODE_HOT_CUE:
            if (buf[1] < 0x41)
                out_mode = HOT_CUE;
            else
                out_mode = CLEAR_CUE;
            break;
        case BUTTON_MODE_LOOP_ROLL:
            if (buf[1] < 0x41)
                out_mode = LOOP_ROLL;
            else
                out_mode = USER_MODE_1;
            break;
        case BUTTON_MODE_AUTO_LOOP:
            if (buf[1] < 0x41)
                out_mode = AUTO_LOOP;
            else
                out_mode = USER_MODE_2;
            break;
        default:
            return false;
    }

    // Get button action: pressed or released.
    //   Info is in buf[2].
    //   7F for pressed
    //   00 for released
    if (buf[2] == 0x7F)
        out_button_pressed = PRESSED;
    else
        out_button_pressed = RELEASED;

    return true;
}

void Dicer_control_process::build_midi_buffer(const dicer_t               &dicer_index,
                                              const dicer_mode_t          &mode,
                                              const dicer_button_t        &button_index,
                                              const dicer_button_state_t  &button_state,
                                              char                       (&io_buf)[3])
{
    // Set Dicer index and button mode.
    //   Info is in buf[0].
    //   9A/9B/9C for left  (hotcue/clearcue, looproll/usermode1, autoloop/usermode2).
    //   9D/9E/9F for right (hotcue/clearcue, looproll/usermode1, autoloop/usermode2).
    switch (mode)
    {
        case HOT_CUE:
        case CLEAR_CUE:
            if (dicer_index == DICER_LEFT)
                io_buf[0] = 0x9A;
            else
                io_buf[0] = 0x9D;
            break;
        case LOOP_ROLL:
        case USER_MODE_1:
            if (dicer_index == DICER_LEFT)
                io_buf[0] = 0x9B;
            else
                io_buf[0] = 0x9E;
            break;
        case AUTO_LOOP:
        case USER_MODE_2:
            if (dicer_index == DICER_LEFT)
                io_buf[0] = 0x9C;
            else
                io_buf[0] = 0x9F;
            break;
    }

    // Set button.
    //   Info is in buf[1].
    //   3C/3D/3E/3F/40 for btn 1/2/3/4/5 (hotcue, looproll and autoloop).
    //   41/42/43/44/45 for btn 1/2/3/4/5 (clearcue, usermode1 and usermode2).
    switch (mode)
    {
        case HOT_CUE:
        case LOOP_ROLL:
        case AUTO_LOOP:
            if (button_index == BUTTON_1)
                io_buf[1] = 0x3C;
            if (button_index == BUTTON_2)
                io_buf[1] = 0x3D;
            if (button_index == BUTTON_3)
                io_buf[1] = 0x3E;
            if (button_index == BUTTON_4)
                io_buf[1] = 0x3F;
            if (button_index == BUTTON_5)
                io_buf[1] = 0x40;
            break;
        case CLEAR_CUE:
        case USER_MODE_1:
        case USER_MODE_2:
            if (button_index == BUTTON_1)
                io_buf[1] = 0x41;
            if (button_index == BUTTON_2)
                io_buf[1] = 0x42;
            if (button_index == BUTTON_3)
                io_buf[1] = 0x43;
            if (button_index == BUTTON_4)
                io_buf[1] = 0x44;
            if (button_index == BUTTON_5)
                io_buf[1] = 0x45;
            break;
    }

    // Set button state.
    //   Info is in buf[2].
    if (button_state == UNLIT)
    {
        // Remove color (unlit button).
        io_buf[2] = 0x00;
    }
    else
    {
        // Put the color corresponding to the mode (lit button).
        switch (mode)
        {
            case HOT_CUE:
            case CLEAR_CUE:
                io_buf[2] = 0x0F; // red
                break;
            case LOOP_ROLL:
            case USER_MODE_1:
                io_buf[2] = 0x7F; // green
                break;
            case AUTO_LOOP:
            case USER_MODE_2:
                io_buf[2] = 0x4F; // orange
                break;
        }
    }
}

bool Dicer_control_process::start()
{
    int err = 0;

    // Open the Dicer (RAW MIDI interface).
#ifdef WIN32
    // TODO: add Dicer support for Windows.
    return false;
#else
    if ((err = snd_rawmidi_open(&this->midi_in, &this->midi_out, DEVICE_ID, 0)) < 0) // Open in blocking mode (for the read function).
    {
        qCWarning(DS_DICER) << "can not open MIDI interface on Dicer: " << snd_strerror(err);
        return false;
    }
#endif

    // Dicer is ready to work.
    this->is_open = true;

    // Clear all buttons for both Dicer.
    if (this->clear_dicer(DICER_LEFT) == false)
    {
        qCWarning(DS_DICER) << "can not clear Dicer left";
        return false;
    }
    if (this->clear_dicer(DICER_RIGHT) == false)
    {
        qCWarning(DS_DICER) << "can not clear Dicer right";
        return false;
    }

    // Run the loop which reads incoming MIDI command.
    this->reader_thread.start();

    return true;
}

void Dicer_control_process::exec_midi_commands_reader_process()
{
    int err = 0;
    char midi_buf[3] = {0x00, 0x00, 0x00};
    dicer_t                dicer_index;
    dicer_mode_t           mode;
    dicer_button_t         button_index;
    dicer_button_pressed_t button_hitted;

    // Start infinite loop which read MIDI commands.
    for (;;)
    {
        // Read MIDI command from DICER (blocking).
#ifdef WIN32
        // TODO: add Dicer support for Windows.
        return;
#else
        if ((err = snd_rawmidi_read(this->midi_in, midi_buf, sizeof(midi_buf))) < 0)
        {
            qCWarning(DS_DICER) << "can not read MIDI command on Dicer: " << snd_strerror(err);
        }
#endif
        else
        {
#if 0
            cout << "read"
                 << "\tl/r+mode:" << hex << "0x"<< static_cast<unsigned int>(static_cast<unsigned char>(midi_buf[0]))
                 << "\tbutton:"   << hex << "0x"<< static_cast<unsigned int>(static_cast<unsigned char>(midi_buf[1]))
                 << "\tdown/up::" << hex << "0x"<< static_cast<unsigned int>(static_cast<unsigned char>(midi_buf[2]))
                 << endl;
#endif
            // Extracting command.
            if (this->extract_midi_buffer(midi_buf, dicer_index, mode, button_index, button_hitted) == true)
            {
#if 0
                cout << "extract"
                     << "\tdicer:"          << dicer_index
                     << "\tmode:"           << mode
                     << "\tbutton_index:"   << button_index
                     << "\tbutton_pressed:" << button_hitted
                     << endl;
#endif
                // Send a signal: a button was pressed or released on the Dicer.
                switch (button_hitted)
                {
                    case PRESSED:
                        emit this->button_pressed(dicer_index, mode, button_index);
                        break;
                    case RELEASED:
                        emit this->button_released(dicer_index, mode, button_index);
                        break;
                    default:
                        qCWarning(DS_DICER) << "can not extract MIDI command on Dicer";
                }
            }
            else
            {
                qCWarning(DS_DICER) << "can not extract MIDI command on Dicer";
            }
        }
    }

    //QThread::currentThread()->quit();

    return;
}

bool Dicer_control_process::stop()
{
    int err = 0;

    if (this->is_open == true)
    {
        // Clear all buttons for both Dicer.
        if (this->clear_dicer(DICER_LEFT) == false)
        {
            qCWarning(DS_DICER) << "can not clear Dicer left";
            return false;
        }
        if (this->clear_dicer(DICER_RIGHT) == false)
        {
            qCWarning(DS_DICER) << "can not clear Dicer right";
            return false;
        }

        // Close all MIDI handlers.
#ifdef WIN32
        // TODO: add Dicer support for Windows.
        return false;
#else
        if ((err = snd_rawmidi_close(this->midi_in)) < 0)
        {
            qCWarning(DS_DICER) << "can not close MIDI IN on Dicer: " << snd_strerror(err);
            return false;
        }
        if ((err = snd_rawmidi_close(this->midi_out)) < 0)
        {
            qCWarning(DS_DICER) << "can not close MIDI OUT on Dicer: " << snd_strerror(err);
            return false;
        }
#endif
        this->is_open = false;
    }

    // Stop the loop which reads incoming MIDI command.
    //this->reader_thread.wait();
    this->reader_thread.exit();
    //this->reader_thread.terminate();

    return true;
}

bool Dicer_control_process::set_button_state(const dicer_t              &dicer_index,
                                             const dicer_mode_t         &mode,
                                             const dicer_button_t       &button_index,
                                             const dicer_button_state_t &state)
{
    int err = 0;
    char midi_buf[3] = {0x0, 0x0, 0x0};

    if (this->is_open == true)
    {
        // Build the MIDI command which changes the state of a button on the Dicer.
        this->build_midi_buffer(dicer_index, mode, button_index, state, midi_buf);
    #if 0
        cout << "write"
             << "\tl/r+mode:"   << hex << "0x"<< static_cast<unsigned int>(static_cast<unsigned char>(midi_buf[0]))
             << "\tbutton:"     << hex << "0x"<< static_cast<unsigned int>(static_cast<unsigned char>(midi_buf[1]))
             << "\tlit+color::" << hex << "0x"<< static_cast<unsigned int>(static_cast<unsigned char>(midi_buf[2]))
             << endl;
    #endif
        // Send the MIDI command to the Dicer.
#ifdef WIN32
        // TODO: add Dicer support for Windows.
        return false;
#else
        if ((err = snd_rawmidi_write(this->midi_out, midi_buf, sizeof(midi_buf))) < 0)
        {
            qCWarning(DS_DICER) << "can not set button state to Dicer: " << snd_strerror(err);
            return false;
        }
#endif
    }

    return true;
}

bool Dicer_control_process::clear_dicer(const dicer_t &dicer_index)
{
    if (this->is_open == true)
    {
        // Iterate over all buttons (for every modes) and unlit them.
        for (int mode = HOT_CUE; mode <= USER_MODE_2; mode++)
        {
            for (int button = BUTTON_1; button <= BUTTON_5; button++)
            {
                if (this->set_button_state(dicer_index,
                                           static_cast<dicer_mode_t>(mode),
                                           static_cast<dicer_button_t>(button),
                                           UNLIT) == false)
                {
                    return false;
                }
            }
        }
    }

    return true;
}

