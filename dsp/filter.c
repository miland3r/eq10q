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

//#include <stdio.h>
#include <stdlib.h>
#include "filter.h"

//Initialize filter
Filter *FilterInit(double rate)
{
  Filter *filter = (Filter *)malloc(sizeof(Filter));
  filter->fs= rate;
  filter->gain = 1.0f;
  filter->freq = 100.0f;
  filter->q = 1.0f;
  filter->enable = 0.0f;
  filter->iType = 0;
  
  //Interpolations
  filter->InterK = INTERPOLATOR_CALC_K((float)rate);
  filter->useInterpolation = 1.0f;
  
  return filter;
}

//Destroy a filter instance
void FilterClean(Filter *filter)
{ 
  free(filter);
}

//Clean buffers
void flushBuffers(Buffers *buf)
{
    buf->buf_0 = 0.0;
    buf->buf_1 = 0.0;
    buf->buf_2 = 0.0;
    buf->buf_e0 = 0.0;
    buf->buf_e1 = 0.0;
    buf->buf_e2 = 0.0;
}
