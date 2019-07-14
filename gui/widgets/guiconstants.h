/***************************************************************************
 *   Copyright (C) 2009 by Pere R�fols Soler                               *
 *   sapista2@gmail.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef GUI_CONSTANTS_H
  #define GUI_CONSTANTS_H
 
#define GAIN_TYPE 0
#define FREQ_TYPE 1
#define Q_TYPE    2
#define FILTER_TYPE 3
#define ONOFF_TYPE 4

//Filter default values
#define GAIN_MIN -20.0
#define GAIN_MAX 20.0
#define FREQ_MIN 20.0
#define FREQ_MAX 20000.0
#define PEAK_Q_MIN 0.1
#define PEAK_Q_MAX 16.0
#define HPF_LPF_Q_DEFAULT 0.7
#define NOTCH_Q_DEFAULT 2
#define HIGH_LOW_SHELF_Q_DEFAULT  0.7
#define PEAK_Q_DEFAULT 2
#define GAIN_DEFAULT 0.0

#endif