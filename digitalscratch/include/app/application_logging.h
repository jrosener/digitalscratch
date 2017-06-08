/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*--------------------------------------------------( application_logging.h )-*/
/*                                                                            */
/*  Copyright (C) 2003-2017                                                   */
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

#pragma once

#include <QLoggingCategory>

// Declare logging categories.
Q_DECLARE_LOGGING_CATEGORY(DS_OBJECTLIFE)
Q_DECLARE_LOGGING_CATEGORY(DS_APPSETTINGS)
Q_DECLARE_LOGGING_CATEGORY(DS_FILE)
Q_DECLARE_LOGGING_CATEGORY(DS_PLAYBACK)
Q_DECLARE_LOGGING_CATEGORY(DS_MUSICKEY)
Q_DECLARE_LOGGING_CATEGORY(DS_SOUNDCARD)
Q_DECLARE_LOGGING_CATEGORY(DS_DB)
Q_DECLARE_LOGGING_CATEGORY(DS_DICER)
