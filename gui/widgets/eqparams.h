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

#ifndef EQ_PARAMS_H
  #define EQ_PARAMS_H
class EqParams
{
  public:
    EqParams(int iNumBands);
    virtual ~EqParams();
    
    typedef struct
    {
      float fGain;
      float fFreq;
      float fQ;
      int iType;
      bool bIsEnabled;
    } EqBandStruct;
    
    void setInputGain(float fInGain);
    void setOutputGain(float fOutGain);
    void setBandGain(int iBand, float fGain);
    void setBandFreq(int iBand, float fFreq);
    void setBandQ(int iBand, float fQ);
    void setBandType(int iBand, int iType);
    void setBandEnabled(int iBand, bool bIsEnabled);
    
    float getInputGain();
    float getOutputGain();
    float getBandGain(int iBand);
    float getBandFreq(int iBand);
    float getBandQ(int iBand);
    int getBandType(int iBand);
    bool getBandEnabled(int iBand);
    
    void loadFromTtlFile(const char *uri);
    bool loadFromFile(const char *path);
    void saveToFile(const char *path);
  
  private:
    int m_iNumberOfBands;
    EqBandStruct *m_ptr_BandArray;
    float m_fInGain;
    float m_fOutGain;    
};
#endif