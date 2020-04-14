/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*------------------------------------------------( dicer_control_process.h )-*/
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
/* Behavior class: drive the Novation Dicer external controller.              */
/*                                                                            */
/*============================================================================*/

#pragma once

#include <QObject>
#include <QThread>
#ifdef WIN32
// TODO: add Dicer support for Windows.
#else
#include <alsa/asoundlib.h>
#endif

using namespace std;

// Only 2 Dicers can work at the same time.
enum dicer_t
{
    DICER_LEFT = 0,
    DICER_RIGHT
};

// These are the 3 buttons used to change the Dicer mode.
enum dicer_button_mode_t
{
    BUTTON_MODE_HOT_CUE = 0,
    BUTTON_MODE_LOOP_ROLL,
    BUTTON_MODE_AUTO_LOOP
};

// These are the 6 functional modes.
enum dicer_mode_t
{
    HOT_CUE = 0,
    LOOP_ROLL,
    AUTO_LOOP,
    CLEAR_CUE,
    USER_MODE_1,
    USER_MODE_2
};

// These are the 5 pad buttons.
enum dicer_button_t
{
    BUTTON_1 = 0,
    BUTTON_2,
    BUTTON_3,
    BUTTON_4,
    BUTTON_5,
};

// User action on pad buttons.
enum dicer_button_pressed_t
{
    RELEASED = 0,
    PRESSED
};

// Pad buttons can be colored (lit) or not (unlit).
enum dicer_button_state_t
{
    UNLIT = 0,
    LIT
};

class Dicer_control_process : public QObject
{
    Q_OBJECT

 private:
#ifdef WIN32
    // TODO: add Dicer support for Windows.
#else
    snd_rawmidi_t *midi_in;       // Handler for reading MIDI messages.
    snd_rawmidi_t *midi_out;      // Handler for writing MIDI messages.
#endif
    bool           is_open;       // True is MIDI device is opened.

 public:
    Dicer_control_process();
    virtual ~Dicer_control_process();

    bool init();                 // Init and open Dicers.
    bool stop();                 // Stop reading commands and close Dicers.

 private:
    bool extract_midi_buffer(const unsigned char    buf[],
                             dicer_t                &out_dicer_index,
                             dicer_mode_t           &out_mode,
                             dicer_button_t         &out_button_index,
                             dicer_button_pressed_t &out_button_pressed);

    void build_midi_buffer(const dicer_t               &dicer_index,
                           const dicer_mode_t          &mode,
                           const dicer_button_t        &button_index,
                           const dicer_button_state_t  &button_state,
                           unsigned char               (&io_buf)[3]);

 private slots:
    void start();   // The main MIDI reader method (executed in the worker thread).

 public slots:
    bool set_button_state(const dicer_t              &dicer_index,
                          const dicer_mode_t         &mode,
                          const dicer_button_t       &button_index,  // Send a MIDI command to colorize or remove
                          const dicer_button_state_t &state);        // color on a button in the specified mode.

    bool clear_dicer(const dicer_t &dicer_index);                    // Remove color on all buttons.

 signals:
    void button_pressed(const dicer_t         &dicer_index,          // Sent when the user pressed on a pad button
                        const dicer_mode_t    &mode,                 // (or on the mode button + a pad button).
                        const dicer_button_t  &button_index);

    void button_released(const dicer_t        &dicer_index,          // Sent when the user released a pad button
                         const dicer_mode_t   &mode,                 // (or on the mode button + a pad button).
                         const dicer_button_t &button_index);

    void terminated();
};
