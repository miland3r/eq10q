/***************************************************************************
 *   Copyright (C) 2016 by Pere Ràfols Soler                               *
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
This file contains the parameter interpolation
****************************************************************************/

#ifndef  PARAM_INTERPOLATOR_H
  #define PARAM_INTERPOLATOR_H
  
#include <stdlib.h>
#include <math.h>

#define STEP_TIME_MS 60.0f
#define INTER_OF_DEADBAND 0.001f
#define INTERPOLATOR_CALC_K(x) (4e3f / (x * STEP_TIME_MS)) //Where X is sample_rate

static inline float computeParamInterpolation(float target, float current, float K, float enableInterpol)
{
  float res =  current + K*(target - current);
  res = fabs(res - target) < fabs(INTER_OF_DEADBAND*target) ? target : res;
  res = enableInterpol*res + (1.0f - enableInterpol)*target;
  return(res);
}
#endif