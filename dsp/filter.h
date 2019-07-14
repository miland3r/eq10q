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
****************************************************************************/

#ifndef  FILTER_H
  #define FILTER_H

#include <math.h>
#include "param_interpolator.h"

//#include <stdio.h>

//Constants definitions
#ifndef PI
  #define PI 3.1416
#endif

#define  F_NOT_SET 0
#define  F_LPF_ORDER_1 1
#define  F_LPF_ORDER_2 2
#define  F_LPF_ORDER_3 3
#define  F_LPF_ORDER_4 4
#define  F_HPF_ORDER_1 5
#define  F_HPF_ORDER_2 6
#define  F_HPF_ORDER_3 7
#define  F_HPF_ORDER_4 8
#define  F_LOW_SHELF 9
#define  F_HIGH_SHELF 10
#define  F_PEAK 11
#define  F_NOTCH 12

typedef struct
{
  double b0, b1, b2, a1, a2; //Second Order coeficients
  double b1_0, b1_1, b1_2, a1_1, a1_2; //Second Order extra coeficients 
  int filter_order;  //filter order
  double fs; //sample rate
  float gain, freq, q;
  double enable;
  int iType; //Filter type
  
  //Interpolation K
  float InterK;  
  float useInterpolation;
  
}Filter;

typedef struct
{ 
  double buf_0;
  double buf_1;
  double buf_2;
  double buf_e0;
  double buf_e1;
  double buf_e2; 
}Buffers;

//Initialize filter instance
Filter *FilterInit(double rate);

//Destroy a filter instance
void FilterClean(Filter *f);

//Clean buffers
void flushBuffers(Buffers *buf);

//Compute filter coeficients
static inline void calcCoefs(Filter *filter, float fGain, float fFreq, float fQ, int iType, float iEnabled) //p2 = GAIN p3 = Q
{   
    double alpha, A, b0, b1, b2, a0, a1, a2, b1_0, b1_1, b1_2, a1_0, a1_1, a1_2;
    alpha = A = b0 = b1 = b2 = a0 = a1 = a2 = b1_0 = b1_1 = b1_2 = a1_0 = a1_1 = a1_2 = 1.0;
    filter->filter_order = 0;
    
    //Param Interpolation    
    filter->freq = computeParamInterpolation(fFreq, filter->freq, filter->InterK, filter->useInterpolation);
    filter->gain = computeParamInterpolation(fGain, filter->gain, filter->InterK, filter->useInterpolation);
    filter->q = computeParamInterpolation(fQ, filter->q, filter->InterK, filter->useInterpolation);
    filter->enable = (double)computeParamInterpolation(iEnabled, (float)filter->enable, filter->InterK, filter->useInterpolation);
           
    double w0=2*PI*(filter->freq/filter->fs);
    filter->iType = iType;
    
    switch(iType){

      case F_HPF_ORDER_1:
      {
	w0 = tanf(w0/2.0);
	b0 = 1.0;
	b1 = -1.0;
	b2 = 0.0;
	a0 = w0+1.0;
	a1 = w0-1.0;
	a2 = 0.0;
      }
      break;

      case F_HPF_ORDER_4:
      {
	filter->filter_order = 1;
      case F_HPF_ORDER_2: 
	alpha = sinf(w0)/(2*filter->q);
	b1_0 = b0 = (1 + cosf(w0))/2; //b0
	b1_1 = b1 = -(1 + cosf(w0)); //b1
	b1_2 = b2 = (1 + cosf(w0))/2; //b2
	a1_0 = a0 = 1 + alpha; //a0
	a1_1 = a1 = -2*cosf(w0); //a1
	a1_2 = a2 = 1 - alpha; //a2
      }
      break;

      case F_HPF_ORDER_3:
      {
	filter->filter_order = 1;
	alpha = sinf(w0)/(2*filter->q);
	b0 = (1 + cosf(w0))/2; //b0
	b1 = -(1 + cosf(w0)); //b1
	b2 = (1 + cosf(w0))/2; //b2
	a0 = 1 + alpha; //a0
	a1 = -2*cosf(w0); //a1
	a2 = 1 - alpha; //a2
	w0 = tanf(w0/2.0);
	b1_0 = 1.0;
	b1_1 = -1.0;
	b1_2 = 0.0;
	a1_0 = w0+1.0;
	a1_1 = w0-1.0;
	a1_2 = 0.0;
      }
      break;

      case F_LPF_ORDER_1: 
      {
	w0 = tanf(w0/2.0);
	b0 = w0;
	b1 = w0;
	b2 = 0.0;
	a0 = w0+1.0;
	a1 = w0-1.0;
	a2 = 0.0;
      }
      break;
 
      case F_LPF_ORDER_4:
      {
	filter->filter_order = 1;
      case F_LPF_ORDER_2:
	alpha = sinf(w0)/(2*filter->q);
	b1_0 = b0 = (1 - cosf(w0))/2; //b0
	b1_1 = b1 = 1 - cosf(w0); //b1
	b1_2 = b2 = (1 - cosf(w0))/2; //b2
	a1_0 = a0 = 1 + alpha; //a0
	a1_1 = a1 = -2*cosf(w0); //a1
	a1_2 = a2 = 1 - alpha; //a2
      }
      break;

      case F_LPF_ORDER_3:
      {
	filter->filter_order = 1;
	alpha = sinf(w0)/(2*filter->q);
	b0 = (1 - cosf(w0))/2; //b0
	b1 = 1 - cosf(w0); //b1
	b2 = (1 - cosf(w0))/2; //b2
	a0 = 1 + alpha; //a0
	a1 = -2*cosf(w0); //a1
	a2 = 1 - alpha; //a2
	w0 = tanf(w0/2.0);
	b1_0 = w0;
	b1_1 = w0;
	b1_2 = 0.0;
	a1_0 = w0+1.0;
	a1_1 = w0-1.0;
	a1_2 = 0.0;
      }
      break;

      case F_LOW_SHELF:
      {
	A = sqrtf((filter->gain));
	alpha =sinf(w0)/2 * (1/filter->q);
	b0 = A*((A+1)-(A-1)*cosf(w0)+2*sqrtf(A)*alpha); //b0
	b1 = 2*A*((A-1)-(A+1)*cosf(w0)); //b1
	b2 = A*((A+1)-(A-1)*cosf(w0)-2*sqrtf(A)*alpha); //b2
	a0 = (A+1) + (A-1)*cosf(w0) + 2*sqrtf(A)*alpha; //a0
	a1 = -2*((A-1) + (A+1)*cosf(w0)); //a1
	a2 = (A+1) + (A-1)*cosf(w0) - 2*sqrtf(A)*alpha; //a2
      }
      break;

      case F_HIGH_SHELF:
      {
	A = sqrtf((filter->gain));
	alpha =sinf(w0)/2 * (1/filter->q);
	b0 = A*( (A+1) + (A-1)*cosf(w0) + 2*sqrtf(A)*alpha ); //b0
	b1 = -2*A*( (A-1) + (A+1)*cosf(w0)); //b1
	b2 = A*( (A+1) + (A-1)*cosf(w0) - 2*sqrtf(A)*alpha ); //b2
	a0 = (A+1) - (A-1)*cosf(w0) + 2*sqrtf(A)*alpha; //a0
	a1 = 2*( (A-1) - (A+1)*cosf(w0)); //a1
	a2 = (A+1) - (A-1)*cosf(w0) - 2*sqrtf(A)*alpha; //a2
      }
      break;

      case F_PEAK:
      {
	A = sqrtf(filter->gain);
	double A2 = A*A;
	double PI2 = PI*PI;
	double Q2 = filter->q*filter->q;
	double w02 = w0 * w0;
	double w02_PI22 = (w02 - PI2)*(w02 - PI2);
	
	//Equivalent analog filter and analog gains
	double G1 = sqrtf((w02_PI22 + (A2*w02*PI2)/Q2)/(w02_PI22 + (w02*PI2)/(Q2*A2)));
	double GB = sqrt(G1*filter->gain);
	double GB2 = GB * GB;
	double G2 = filter->gain * filter->gain;
	double G12 = G1 * G1;
	
	//Digital filter
	double F   = fabsf(G2  - GB2);// + 0.00000001f; ///TODO akest petit num sumat en teoria no hi va pero he detectat div by 0 
	double G00 = fabsf(G2  - 1.0);// + 0.00000001f;  ///TODO akest petit num sumat en teoria no hi va pero he detectat div by 0
	double F00 = fabsf(GB2 - 1.0);// + 0.00000001f;  ///TODO akest petit num sumat en teoria no hi va pero he detectat div by 0
	double G01 = fabsf(G2  - G1);// + 0.00000001f;  ///TODO akest petit num sumat en teoria no hi va pero he detectat div by 0
	double G11 = fabsf(G2  - G12);// + 0.00000001f;  ///TODO akest petit num sumat en teoria no hi va pero he detectat div by 0
	double F01 = fabsf(GB2 - G1);// + 0.00000001f;  ///TODO akest petit num sumat en teoria no hi va pero he detectat div by 0
	double F11 = fabsf(GB2 - G12);// + 0.00000001f;  ///TODO akest petit num sumat en teoria no hi va pero he detectat div by 0
	double W2 = sqrtf(G11 / G00) * tanf(w0/2.0) * tanf(w0/2.0);

	//Bandwidth condition
	double Aw = (w0/(A*filter->q))*sqrtf((GB2-A2 * A2)/(1.0 - GB2)); //Analog filter bandwidth at GB
	double DW = (1.0 + sqrtf(F00 / F11) * W2) * tanf(Aw/2.0); //Prewarped digital bandwidth
	
	//printf("G1=%f Aw=%f DW=%f F11=%f GB2=%f G12=%f\r\n",G1,Aw,DW,F11,GB2,G12);
	
	//Digital coefs
	double C = F11 * DW * DW - 2.0 * W2 * (F01 - sqrtf(F00 * F11));
	double D = 2.0 * W2 * (G01 - sqrtf(G00 * G11));
	double A = sqrtf((C + D) / F);
	double B = sqrtf((G2 * C + GB2 * D) / F);
	
	//printf("A=%f B=%f C=%f D=%f W2=%f F=%f G2=%f GB2=%f\r\n", A, B, C, D, W2, F, G2, GB2 );
	
	if( filter->gain > 1.01 || filter->gain < 0.98 )
	{
	  b0 = G1 + W2 + B;
	  b1 =  -2.0*(G1 - W2);
	  b2 = G1 - B + W2;
	  a0 = 1.0 + W2 + A;
	  a1 = -2.0*(1.0 - W2);
	  a2 = 1.0 + W2 - A;
	}
	else
	{
	  b0 = 1.0;
	  b1 = 0.0;
	  b2 = 0.0;
	  a0 = 1.0;
	  a1 = 0.0;
	  a2 = 0.0;
	}
      }
      break;

      case F_NOTCH:
      {
	alpha = sinf(w0)/(2*filter->q);

	b0 =  1; //b0
	b1 = -2*cosf(w0); //b1
	b2 =  1; //b2
	a0 =  1 + alpha; //a0
	a1 = -2*cosf(w0); //a1
	a2 =  1 - alpha; //a2
      }
      break;
  } //End of switch

    //Normalice coeficients to a0=1 
    filter->b0 = (b0/a0); //b0
    filter->b1 = (b1/a0); //b1
    filter->b2 = (b2/a0); //b2
    filter->a1 = (a1/a0); //a1
    filter->a2 = (a2/a0); //a2
    filter->b1_0 = (b1_0/a1_0);
    filter->b1_1 = (b1_1/a1_0);
    filter->b1_2 = (b1_2/a1_0);
    filter->a1_1 = (a1_1/a1_0);
    filter->a1_2 = (a1_2/a1_0);  

    //Print coefs
    //printf("Coefs b0=%f b1=%f b2=%f a1=%f a2=%f\r\n",filter->b0,filter->b1,filter->b2,filter->a1,filter->a2);
    //printf("Gain = %f Freq = %f Q = %f\r\n", filter->gain, filter->freq, filter->q);
}

#define DENORMAL_TO_ZERO(x) if (fabs(x) < (1e-300)) x = 0.0; //Min float is 1.1754943e-38 (Min double is 2.23×10−308)

//Compute filter
static inline  void computeFilter(Filter *filter, Buffers *buf, double *inputSample)
{
  //Process 1, 2 orders
  //w(n)=x(n)-a1*w(n-1)-a2*w(n-2)
  buf->buf_0 = (*inputSample)-filter->a1*buf->buf_1-filter->a2*buf->buf_2;

  
  /*
  //Denomar hard TEST
  static unsigned int den_counter = 0;
  if (fabs(buf->buf_0) < (1e-30))
  {
    den_counter++;
    printf("#DENORMAL# %d\r\n",den_counter);
  }
  */
    
    
  DENORMAL_TO_ZERO(buf->buf_0);
  //y(n)=bo*w(n)+b1*w(n-1)+b2*w(n-2)
  *inputSample = (filter->b0*buf->buf_0 + filter->b1*buf->buf_1+ filter->b2*buf->buf_2) * filter->enable + (*inputSample)*(1.0 - filter->enable);

  buf->buf_2 = buf->buf_1;
  buf->buf_1 = buf->buf_0;
  
  //Process 3,4 orders if apply
  if(filter->filter_order)
  {
      //w(n)=x(n)-a1*w(n-1)-a2*w(n-2)
      buf->buf_e0 = (*inputSample)-filter->a1_1*buf->buf_e1-filter->a1_2*buf->buf_e2;
      //y(n)=bo*w(n)+b1*w(n-1)+b2*w(n-2)
      DENORMAL_TO_ZERO(buf->buf_e0);
      *inputSample =  (filter->b1_0*buf->buf_e0 + filter->b1_1*buf->buf_e1+ filter->b1_2*buf->buf_e2) * filter->enable + (*inputSample)*(1.0 - filter->enable);

      buf->buf_e2 = buf->buf_e1;
      buf->buf_e1 = buf->buf_e0;
  }
}
#endif