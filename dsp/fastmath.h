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
#ifndef _EQ10Q_FAST_MATH
#define  _EQ10Q_FAST_MATH
#include <math.h>
#include <stdlib.h>

//LUT addr size
#define LUT_ADDR_LENGTH 8

// Compute fast log10 using DSP Guru trick
// Observa que akest metode no utilitza per res el signe, de forma que fa el valor absolut de forma intrinseca! t'estalvies el detector de pic!
// x: Input value RAW BITS format IEE745
// returns: Fast log10 value
static inline float fastLog10(int *x_bits, float *Log10LUT)
{
    // log10(N) = log10(M) + E*log10(2)
    return (Log10LUT[(*x_bits & 0x7FFFFF) >> (23 - LUT_ADDR_LENGTH)] + (float)((0x0FF & ((*x_bits >> 23))) - 127) * 0.30102999566398119521373889472449f);
}

// Compute fast log using DSP Guru trick
// Observa que akest metode no utilitza per res el signe, de forma que fa el valor absolut de forma intrinseca! t'estalvies el detector de pic!
// x: Input value RAW BITS format IEE745
// returns: Fast log value
static inline float fastLog(int *x_bits, float *LogLUT)
{
    // log(N) = log(M) + E*log(2)
    return (LogLUT[(*x_bits & 0x7FFFFF) >> (23 - LUT_ADDR_LENGTH)] + (float)((0x0FF & ((*x_bits >> 23))) - 127) * 0.693147180559945f);
}

// Conversion of binary mantissa 2 decimal
float GetBinaryFraction(int x)
{
    float res = 0;
    int i;
    for (i = 22; i >=0; i--)
    {
	int b = x >> i;
	b &= 0x01;
	res += (float)(b) * powf(2.0f, (float)(i) - 23.0f);
    }

    return res;
}

// Generate LUT for mantissa log
float* GenerateLogLUT()
{
    int i;
    float matissa = 1.0f;
    int size = (int)( powf(2.0f, LUT_ADDR_LENGTH));
    float *LogLUT = (float *)malloc(sizeof(float)*size);
    
    for (i = 0; i < size; i++)
    {
	int M = (int)i << (23 - LUT_ADDR_LENGTH);
	matissa = GetBinaryFraction(M) + 1.0f;
	LogLUT[i] = logf(matissa);
    }
    return LogLUT;
}

// Generate LUT for mantissa log10
float* GenerateLog10LUT()
{
    int i;
    float matissa = 1.0f;
    int size = (int)( powf(2.0f, LUT_ADDR_LENGTH));
    float *Log10LUT = (float *)malloc(sizeof(float)*size);

    for (i = 0; i < size; i++)
    {
	int M = (int)i << (23 - LUT_ADDR_LENGTH);
	matissa = GetBinaryFraction(M) + 1.0f;
	Log10LUT[i] = log10f(matissa);
    }
    return Log10LUT;
}

// Fast dB to Linear conversion using 8 Taylor Series members
// Is aprox a 14% faster than Fast_dB2Lin10() but less acurate
// Input Range at less 1 dB error: 70dB (from -52 dB to 18 dB)
// Input Range at less 0,5 dB error: 63dB (from -51 dB to 12 dB)
static inline float Fast_dB2Lin8(float x)
{
    float xa = x + 30.0f; //Error = 0 centrat a -30dB

    //k1	        k2	            k3	            k4	            k5	            
    //0.031622777f	0.003640707f	0.000209576f	8.04277e-6f	    2.3149e-7f  	

    //k6	        k7	            k8	            k9
    //5.33025e-9f 	1.02278e-10f	1.68217e-12f	2.42083e-14f

    if (x > -60.0f) //Clip on -60dB because the Taylor aprox starts to deviate at these point
    {
	return (0.031622777f + xa * (0.003640707f + xa * (0.000209576f + xa * (8.04277e-6f + xa * (2.3149e-7f + xa * (5.33025e-9f + xa * (1.02278e-10f + xa * (1.68217e-12f + xa * 2.42083e-14f))))))));
    }
    else
    {
	return 0.001f;
    }
}

// Fast dB to Linear conversion using 10 Taylor Series members
// Is aprox a 14% slower than Fast_dB2Lin8() but more acurate
// Input Range at less 1 dB error: 89dB (from -67 dB to 22 dB)
// Input Range at less 0,5 dB error: 81dB (from -65 dB to 16 dB)
static inline float Fast_dB2Lin10(float x)
{
    float xa = x + 40.0f; //Error = 0 centrat a -40dB
    
    //k1	k2	            k3	        k4	        
    //0.01f	0.001151293f	6.62737e-5f	2.54335e-6f

    //k5	        k6	        k7	            k8	            
    //7.32034e-8f	1.68557e-9f	3.23431e-11f	5.31948e-13f	

    //k9	        k10	            k11
    //7.65535e-15f	9.79283e-17f	1.12744e-18f

    if (x > -67.0f) //Clip on -67dB because the Taylor aprox starts to deviate at these point
    {
	return (0.01f + xa * (0.001151293f + xa * (6.62737e-5f + xa * (2.54335e-6f + xa * (7.32034e-8f + xa * (1.68557e-9f + xa * (3.23431e-11f + xa * (5.31948e-13f + xa * (7.65535e-15f + xa * (9.79283e-17f + xa * 1.12744e-18f))))))))));
    }
    else
    {
	return 0.0004466835921509630484560471330723885330371558666229248046875f;
    }
}
#endif