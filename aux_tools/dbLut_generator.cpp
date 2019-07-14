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
 This file contains the dB to linear convertion LUT generator.
 This program is not included in the EQ10Q release because this is a auxiliar
 tool used for developers to generate the dB and Lin Lookup table.
 Running this program produces the dblut.h file that is used in db.h
****************************************************************************/

//Compile using:
//g++ dbLut_generator.cpp -o dbLut_generator

#include <cmath>
#include <iostream>
#include <fstream>

#define DB_MIN -20.0
#define DB_MAX 20.0
#define DB_STEP 0.05

using namespace std;

int main()
{
  ofstream f;
  f.open("dblut.h", ofstream::out);
  int NumOfPoints = (int)((DB_MAX - DB_MIN)/DB_STEP) + 1;
  f<<"#define LUT_TOP_INDEX "<< NumOfPoints -1 <<endl;
  
  float m,n;
  //dB2Lin index convertion equation
  m = ((float)NumOfPoints -1.0)/(float)(DB_MAX - DB_MIN);
  n = (float)NumOfPoints -1.0 - m*(float)DB_MAX;
  f<<"#define DB2LIN_M "<< m <<endl;
  f<<"#define DB2LIN_N "<< n <<endl;
  
  //Lin2dB index convertion equation
  m = ((float)NumOfPoints -1.0)/(float)(pow(10,DB_MAX/20) - pow(10,DB_MIN/20));
  n = (float)NumOfPoints -1.0 - m*(float)pow(10,DB_MAX/20);
  f<<"#define LIN2DB_M "<< m <<endl;
  f<<"#define LIN2DB_N "<< n <<endl;
  
  //Generate the dB2Lin table and print to file
  float db = DB_MIN;
  f<<"const float dB2Lin_LUT["<< NumOfPoints << "] = { ";
  for(int i = 0; i < NumOfPoints; i++)
  {
    f<<pow(10, db/20);
    if(i<NumOfPoints -1)
    {
      f<<", ";
    }
    db += DB_STEP;
  }
  f<<"};"<<endl;
  
  //Generate the Lin2dB table and print to file
  const float lin_step = (float)(pow(10,DB_MAX/20) - pow(10,DB_MIN/20))/(float)NumOfPoints;
  float lin = (float)pow(10,DB_MIN/20);
  f<<"const float Lin2dB_LUT["<< NumOfPoints << "] = { ";
  for(int i = 0; i < NumOfPoints; i++)
  {
    f<<20*log10(lin);
    if(i<NumOfPoints -1)
    {
      f<<", ";
    }
    lin += lin_step;
  }
  f<<"};"<<endl;
  
  f.close();
  return 0;
}