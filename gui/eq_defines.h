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

//EQ10Q definitions
//TODO: Aquestes seran les definicions definitives del EQ tan de GUI com del audio dsp.
//La idea es fer desapareixer el header EQ_TYPES.H quan s'implementi la compilació amb CMake a tot el projecte (DSP inclos)
//TODO: aquest fitxer es un copy paste de eq.h elimnnant la inclusio del eq_types
#ifndef EQ_GUI_DEFINES_H
  #define EQ_GUI_DEFINES_H
#define EQ_BYPASS    0
#define EQ_INGAIN    1
#define EQ_OUTGAIN   2

#define PORT_OFFSET  3

#define FFT_N 4096 //FFT sample size, must be a power of 2
#endif