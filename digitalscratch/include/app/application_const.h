/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------( application_const.h )-*/
/*                                                                            */
/*  Copyright (C) 2003-2016                                                   */
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
/*                         Constants for the application                      */
/*                                                                            */
/*============================================================================*/

#pragma once

#define MAX_MINUTES_TRACK   15                // Maximum number of minutes for an audio track
#define MAX_MINUTES_SAMPLER 1                 // Maximum number of minutes for a sample in the sampler

#define MAX_NB_CUE_POINTS   4                 // Number of cue points per deck.

// GUI image/icons
#define SKINS_PATH              ":/skins/"
#define PIXMAPS_PATH            ":/pixmaps/"
#define ICON_2                  PIXMAPS_PATH "digitalscratch-icon_2decks.png"
#define LOGO                    PIXMAPS_PATH "digitalscratch-logo.png"
#define ICON_DRIVE_SUFFIX       "-drive_white.png"
#define ICON_FOLDER_SUFFIX      "-folder_white.png"
#define ICON_AUDIO_FILE_SUFFIX  "-audio_file_white.png"
#define ICON_FILE_SUFFIX        "-file_white.png"
