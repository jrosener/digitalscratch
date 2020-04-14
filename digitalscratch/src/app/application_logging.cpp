/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*------------------------------------------------( application_logging.cpp )-*/
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
/*                             Logging framework                              */
/*                                                                            */
/*============================================================================*/

#include "app/application_logging.h"

// Map logging categories to real name.
Q_LOGGING_CATEGORY(DS_OBJECTLIFE,  "ds.objlife",  QtWarningMsg)
Q_LOGGING_CATEGORY(DS_APPSETTINGS, "ds.appstgs",  QtInfoMsg)
Q_LOGGING_CATEGORY(DS_FILE,        "ds.file",     QtInfoMsg)
Q_LOGGING_CATEGORY(DS_PLAYBACK,    "ds.playback", QtWarningMsg)
Q_LOGGING_CATEGORY(DS_MUSICKEY,    "ds.musickey", QtWarningMsg)
Q_LOGGING_CATEGORY(DS_SOUNDCARD,   "ds.sndcard",  QtWarningMsg)
Q_LOGGING_CATEGORY(DS_DB,          "ds.db",       QtWarningMsg)
Q_LOGGING_CATEGORY(DS_DICER,       "ds.dicer",    QtWarningMsg)
Q_LOGGING_CATEGORY(DS_GUI,         "ds.gui",      QtInfoMsg)
