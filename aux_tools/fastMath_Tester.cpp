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
 This file contains the EQ10Q fastMath tester.
 This program is not included in the EQ10Q release because this is a auxiliar
 tool used for developers to test the dB and Lin Lookup table.
****************************************************************************/

//Compile using:
//g++  fastMath_Tester.cpp -o fastmath_test

#include <iostream>
#include <cmath>
#include "../dsp/fastmath.h"

#define TOLERANCE 0.1

#define NUM_OF_TESTS 50
#define LIN_START 0.01
#define LIN_STOP  10.0
#define DB_START -100.0
#define DB_STOP 10.0

using namespace std;

bool CheckResult(double r, double t)
{
  //Check tolerance
  if (abs(r-t) <= TOLERANCE)
  {
    return true;
  }
  
  return false;
}

int main()
{
  float real,test, in;
  bool result = true;
  bool pResult;
  
  //Generate LUTS
  float *LutLog10 = GenerateLog10LUT();
  float *LutLog = GenerateLogLUT();
    
  //Lin 2 dB Tests
  cout<<"========================== LIN 2 DB TESTER =========================="<<endl;
  const float linStep = (float)(LIN_STOP - LIN_START)/(float)NUM_OF_TESTS;
  in = LIN_START;
  while(in <= LIN_STOP)
  {
    real = 20.0f*log10(in);
    test = 20.0f * fastLog10((int*)(&in), LutLog10);
    cout<<"Test value is: "<<in<<"\tConverting to dB using log10() is: "<<real<<"\tConverting with Lin2dB() is: "<<test;
    pResult = CheckResult(real,test);
    result &=pResult;
    if(pResult)
    {
      cout<<"\tTest OK"<<endl;
    }
    else
    {
      cout<<"\tTest NOK"<<endl;
    }
    
    in += linStep;
  }
  cout<<endl<<endl;
  
  //dB 2 Lin Tests
  cout<<"========================== DB 2 LIN TESTER =========================="<<endl;
  const float dBStep = (float)(DB_STOP - DB_START)/(float)NUM_OF_TESTS;
  in = DB_START;
  while(in <= DB_STOP)
  {
    real = pow(10,in/20);
    test = Fast_dB2Lin8(in);
    cout<<"Test value is: "<<in<<"\tConverting to Lin using pow() is: "<<real<<"\tConverting with dB2Lin() is: "<<test;    
    pResult = CheckResult(real,test);
    result &=pResult;
    if(pResult)
    {
      cout<<"\tTest OK"<<endl;
    }
    else
    {
      cout<<"\tTest NOK"<<endl;
    }
    
    in += dBStep;
  }
  cout<<endl<<endl;
  
  //Log() Tests
  cout<<"========================== LOG() TESTER =========================="<<endl;
  //const float linStep = (float)(LIN_STOP - LIN_START)/(float)NUM_OF_TESTS;
  in = LIN_START;
  while(in <= LIN_STOP)
  {
    real = log(in);
    test = fastLog((int*)(&in), LutLog);
    cout<<"Test value is: "<<in<<"\tConverting to dB using log10() is: "<<real<<"\tConverting with Lin2dB() is: "<<test;
    pResult = CheckResult(real,test);
    result &=pResult;
    if(pResult)
    {
      cout<<"\tTest OK"<<endl;
    }
    else
    {
      cout<<"\tTest NOK"<<endl;
    }
    
    in += linStep;
  }
  cout<<endl<<endl;
  
  //dB 2 Lin Tests
  cout<<"========================== DB 2 LIN TESTER =========================="<<endl;
  //const float dBStep = (float)(DB_STOP - DB_START)/(float)NUM_OF_TESTS;
  in = DB_START;
  while(in <= DB_STOP)
  {
    real = pow(10,in/20);
    test = Fast_dB2Lin10(in);
    cout<<"Test value is: "<<in<<"\tConverting to Lin using pow() is: "<<real<<"\tConverting with dB2Lin() is: "<<test;    
    pResult = CheckResult(real,test);
    result &=pResult;
    if(pResult)
    {
      cout<<"\tTest OK"<<endl;
    }
    else
    {
      cout<<"\tTest NOK"<<endl;
    }
    
    in += dBStep;
  }
  cout<<endl<<endl;
  
  cout<<"Global Result is: ";
  if(result)
  {
    cout<<"All test pass with OK result"<<endl;
  }
  else
  {
    cout<<"There is some NOK test"<<endl;
  } 
  
  //Free LUT Memory
  free(LutLog);
  free(LutLog10);
  
  return 0;
}
