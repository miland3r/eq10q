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
This is coeficient generator tester
****************************************************************************/

//Compile using:
//g++ ../dsp/filter.c -fPIC -lm -c -o filter.o
//g++ coef_tester.c filter.o -fPIC -lm -o coef_tester

#include <cmath>
#include <iostream>
#include <fstream>
#include "../dsp/filter.h"

#define DB_MIN -20.0
#define DB_MAX 20.0
#define DB_STEP 0.1

#define FREQ_MIN 20
#define FREQ_MAX 20000
#define FREQ_STEP 1

#define Q_MIN 0.1
#define Q_MAX 16
#define Q_STEP 0.1

#define FILTER_TYPE F_PEAK
using namespace std;
int main()
{
  Filter* f;
  float fGain, fFreq, fQ;
  
  f=FilterInit(44100);
  
  ofstream of;
  of.open("results.txt", ofstream::out);
  of<<"TESTING FOR NAN"<<endl;
  of<<"Filter Type: "<<FILTER_TYPE<<endl;
  of<<"Gain from "<<DB_MIN<<" to "<<DB_MAX<<" step "<<DB_STEP<<endl;
  of<<"Freq from "<<FREQ_MIN<<" to "<<FREQ_MAX<<" step "<<FREQ_STEP<<endl;
  of<<"Q from "<<Q_MIN<<" to "<<Q_MAX<<" step "<<Q_STEP<<endl;
  cout<<"Test runing..."<<endl;
  bool bTestOk = true;
  for(fFreq = FREQ_MIN; fFreq <= FREQ_MAX; fFreq+=FREQ_STEP)
  {
    for(fGain = DB_MIN; fGain <= DB_MAX; fGain+=DB_STEP)
    {
      for(fQ = Q_MIN; fQ <= Q_MAX; fQ+=Q_STEP)
      {
	//printf("---Gain = %f freq = %f Q = %f ---\r\n",fGain, fFreq, fQ);
	calcCoefs(f, pow(10.0f,0.05f*fGain), fFreq, fQ, FILTER_TYPE, 1);
	
	if(isnan(f->a1) || isnan(f->a2) || isnan(f->b0) || isnan(f->b1) || isnan(f->b2))
	{
	  cout<<"NaN found on freq = "<<fFreq<< " Gain = " <<fGain<< " Q = "<<fQ<<endl;
	  of<<"NaN found on freq = "<<fFreq<< " Gain = " <<fGain<< " Q = "<<fQ<<endl;
	  bTestOk = false;
	}
	//printf("a1 = %f a2 = %f b0 = %f b1 = %f b2 = %f\r\n", f->a1, f->a2, f->b0, f->b1, f->b2);
      }
      
    }
    
  }
  
  if(bTestOk)
  {
    cout<<"All tests OK :-)"<<endl;
    of<<"All tests OK :-)"<<endl;
  }
  else
  {
    cout<<"Some test has fialed :-("<<endl;
    of<<"Some test has fialed :-("<<endl;  
  }
  
  of.close();
}