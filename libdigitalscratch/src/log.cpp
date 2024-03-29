/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*               libdigitalscratch: the Digital Scratch engine.               */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------( log.cpp )-*/
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

#include <log.h>

// Map logging categories to real name.
Q_LOGGING_CATEGORY(DSLIB_ANALYZEVINYL,  "dslib.analyzevinyl", QtWarningMsg)
Q_LOGGING_CATEGORY(DSLIB_CONTROLLER,    "dslib.controller", QtWarningMsg)
Q_LOGGING_CATEGORY(DSLIB_API,           "dslib.api, QtWarningMsg")
Q_LOGGING_CATEGORY(DSLIB_SPEED,         "dslib.speed", QtWarningMsg)
