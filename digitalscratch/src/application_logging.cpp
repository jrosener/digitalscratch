/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*------------------------------------------------( application_logging.cpp )-*/
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
/*                             Logging framework                              */
/*                                                                            */
/*============================================================================*/

#include <application_logging.h>

// Map logging categories to real name.
Q_LOGGING_CATEGORY(DS_OBJECTLIFE,  "ds.objectlife")
Q_LOGGING_CATEGORY(DS_APPSETTINGS, "ds.appsettings")
Q_LOGGING_CATEGORY(DS_FILE,        "ds.file")
Q_LOGGING_CATEGORY(DS_AUDIOTRACK,  "ds.audiotrack")
Q_LOGGING_CATEGORY(DS_MUSICKEY,    "ds.musickey")
Q_LOGGING_CATEGORY(DS_SOUNDCARD,   "ds.soundcard")
Q_LOGGING_CATEGORY(DS_DB,          "ds.db")
