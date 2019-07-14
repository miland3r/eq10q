/***************************************************************************
 *   Copyright (C) 2012 by Pere Ràfols Soler                               *
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

/***************************************************************************
 This file contains the dB to linear convertion base on a LUT
****************************************************************************/

#ifndef _DB_2_LINEAR
#define  _DB_2_LINEAR

#include "dblut.h"

//Converts a value from dB to Linear 1 Linear = 0 dB
static inline float dB2Lin(float dbIn)
{
  int index = (int)(DB2LIN_M*dbIn + DB2LIN_N);
  index = index > 0 ? index : 0;
  index = index > LUT_TOP_INDEX ? LUT_TOP_INDEX : index;
  return dB2Lin_LUT[index];
}

//Converts a value from Linear to dB 1 Linear = 0 dB
static inline float Lin2dB(float LinIn)
{
  int index = (int)(LIN2DB_M*LinIn + LIN2DB_N);
  index = index > 0 ? index : 0;
  index = index > LUT_TOP_INDEX ? LUT_TOP_INDEX : index;
  return Lin2dB_LUT[index];
}


#endif