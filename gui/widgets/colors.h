/***************************************************************************
 *   Copyright (C) 2009 by Pere Ràfols Soler                               *
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
#include <glibmm/ustring.h>

#ifndef EQ10Q_COLORS_H
  #define EQ10Q_COLORS_H
  
//Wdiget background colors 
#define BACKGROUND_R 0.19 //0.07
#define BACKGROUND_G 0.19 //0.08
#define BACKGROUND_B 0.22 //0.15

//Wdiget foreground colors 
#define FOREGROUND_R 0.8 //0.0 
#define FOREGROUND_G 0.8 //0.65
#define FOREGROUND_B 0.8 //0.65

//Text Label Color
#define TEXT_R 0.9
#define TEXT_G 0.9
#define TEXT_B 0.9

//Buttons background colors 
#define BUTTON_BACKGROUND_R 0.02
#define BUTTON_BACKGROUND_G 0.32
#define BUTTON_BACKGROUND_B 0.45

//Buttons Active background colors 
#define BUTTON_ACTIVE_BG_R 0.11
#define BUTTON_ACTIVE_BG_G 0.56
#define BUTTON_ACTIVE_BG_B 0.19

//Buttons inactive background colors 
#define BUTTON_INACTIVE_BG_R 0.00
#define BUTTON_INACTIVE_BG_G 0.15
#define BUTTON_INACTIVE_BG_B 0.25

//Buttons Mouse Over background colors 
#define BUTTON_OVER_BG_R 0.01
#define BUTTON_OVER_BG_G 0.46
#define BUTTON_OVER_BG_B 0.09

//Bands colors LUT
const  Glib::ustring bandColorLUT[] = {"#FF0000","#CDC009","#535EFB","#19FFAF","#FF01FF","#00FF00","#A52A2A","#FF8C2E","#B2DFEE","#7129EE" };

//Convert to Gdk::Color macro
#define GDK_COLOR_MACRO(_color) ((gushort)floor(_color * (double)G_MAXUSHORT))

#endif