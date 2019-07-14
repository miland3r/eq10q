/***************************************************************************
 *   Copyright (C) 2015 by Pere Ràfols Soler                               *
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
This file contains Mid/Side stereo signals convertion methods
****************************************************************************/

#ifndef  MIDSIDE_H
  #define MIDSIDE_H
  
#define MS_DUAL_CHANNEL 0
#define MS_L_MID_MODE 1
#define MS_R_SIDE_MODE 2

//Converts a LR stereo signal to a mid/side signal. M is stored in L pointer and S in R pointer
inline void LR2MS(double *inL_M, double *inR_S, double enable)
{
  double M = 0.5*(*inL_M + *inR_S);
  double S = 0.5*(*inL_M - *inR_S);
  *inL_M = enable*M + (1.0 - enable)*(*inL_M);
  *inR_S = enable*S + (1.0 - enable)*(*inR_S);
}

//Converts a M/S stereo signal to a LR signal. L is stored in M pointer and R in S pointer
inline void MS2LR(double *inM_L, double *inS_R, double enable)
{
  double L = (*inM_L + *inS_R);
  double R = (*inM_L - *inS_R);
  *inM_L = enable*L + (1.0 - enable)*(*inM_L);
  *inS_R = enable*R + (1.0 - enable)*(*inS_R);
}

#endif