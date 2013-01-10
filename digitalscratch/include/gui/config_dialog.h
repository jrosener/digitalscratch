/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                           Digital Scratch Player                           */
/*                                                                            */
/*                                                                            */
/*--------------------------------------------------------( config_dialog.h )-*/
/*                                                                            */
/*  Copyright (C) 2003-2012                                                   */
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
/*                Creates configuration GUI for Digital-scratch player        */
/*                                                                            */
/*============================================================================*/

#ifndef CONFIG_DIALOG_H_
#define CONFIG_DIALOG_H_

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QString>
#include <QCloseEvent>
#include <QWidget>
#include "application_settings.h"

#define ICON PIXMAPS_PATH "/digitalscratch-icon.png"

using namespace std;

class Config_dialog : public QDialog
{
    Q_OBJECT

 public:

 private:
    QLineEdit            *base_dir_path;
    QComboBox            *gui_style_select;
    QComboBox            *vinyl_type_select;
    QLineEdit            *extreme_min;
    QLineEdit            *max_nb_buffer;
    QLineEdit            *max_buffer_coeff;
    QLineEdit            *max_speed_diff;
    QLineEdit            *slow_speed_algo_usage;
    QLineEdit            *max_nb_speed_for_stability;
    QLineEdit            *nb_cycle_before_changing_direction;
    QLineEdit            *low_pass_filter_max_speed_usage;
    Application_settings *settings;

 public:
    Config_dialog(QWidget              *parent,
                  Application_settings *in_settings);
    virtual ~Config_dialog();
    int show_config_dialog();

 private:
    void accept();
    void reject();
    bool init_config_file();

 private slots:
    bool show_browse_window();
};

#endif /* CONFIG_DIALOG_H_ */
