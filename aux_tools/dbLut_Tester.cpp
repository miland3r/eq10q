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
 This file contains the dB to linear convertion LUT tester.
 This program is not included in the EQ10Q release because this is a auxiliar
 tool used for developers to test the dB and Lin Lookup table.
****************************************************************************/

//Compile using:
//g++ dbLut_Tester.cpp -o dbLut_test ../dsp/db.c

#include <iostream>
#include <cmath>
#include "../dsp/db.h"

#define TOLERANCE 0.5

#define NUM_OF_TESTS 50
#define LIN_START 0.1
#define LIN_STOP  10.0
#define DB_START -20.0
#define DB_STOP 20.0

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
  double real,test, in;
  bool result = true;
  bool pResult;
    
  //Lin 2 dB Tests
  cout<<"========================== LIN 2 DB TESTER =========================="<<endl;
  const double linStep = (double)(LIN_STOP - LIN_START)/(double)NUM_OF_TESTS;
  in = LIN_START;
  while(in <= LIN_STOP)
  {
    real = 20*log10(in);
    test = Lin2dB((float)in);
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
  const double dBStep = (double)(DB_STOP - DB_START)/(double)NUM_OF_TESTS;
  in = DB_START;
  while(in <= DB_STOP)
  {
    real = pow(10,in/20);
    test = dB2Lin((float)in);
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
  return 0;
}

