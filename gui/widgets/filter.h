/***************************************************************************
 *   Copyright (C) 2011 by Pere Ràfols Soler                               *
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
This file contains the filter definitions

///TODO: codi duplicat entre aqui i el motor d'audio
****************************************************************************/

#ifndef FILTER_TYPE_API_H
  #define FILTER_TYPE_API_H
  
typedef enum
{
  NOT_SET,
  LPF_ORDER_1,
  LPF_ORDER_2,
  LPF_ORDER_3,
  LPF_ORDER_4,
  HPF_ORDER_1,
  HPF_ORDER_2,
  HPF_ORDER_3,
  HPF_ORDER_4,
  LOW_SHELF,
  HIGH_SHELF,
  PEAK,
  NOTCH,
}FilterType;

//Convert int  to FilterType
FilterType int2FilterType(int iType);

#endif