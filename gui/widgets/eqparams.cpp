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
#include "eqparams.h"
#include <stdlib.h>
#include <string>
#include <fstream>
#include <iomanip>

#define  EQ10Q_SIGNATURE 12871

//#define USE_SLV2 ///TODO: check this-> esta donant problemes amb algunes versions de ardour, ull slv2 esta anticuat!!!
#ifdef USE_SLV2 
#include <slv2/world.h>
#include <slv2/plugin.h>
#include <slv2/collections.h>
#include <slv2/value.h>
#include <slv2/types.h>
#endif


EqParams::EqParams(int iNumBands):
m_iNumberOfBands(iNumBands)
{
  m_ptr_BandArray = (EqBandStruct*)malloc(sizeof(EqBandStruct) * m_iNumberOfBands);
}

EqParams::~EqParams()
{
  free(m_ptr_BandArray);
}

bool EqParams::getBandEnabled(int iBand)
{
  return m_ptr_BandArray[iBand].bIsEnabled;
}

float EqParams::getBandFreq(int iBand)
{
  return m_ptr_BandArray[iBand].fFreq;
}

float EqParams::getBandGain(int iBand)
{
  return m_ptr_BandArray[iBand].fGain;
}

float EqParams::getBandQ(int iBand)
{
  return m_ptr_BandArray[iBand].fQ;
}

int EqParams::getBandType(int iBand)
{
  return m_ptr_BandArray[iBand].iType;
}

float EqParams::getInputGain()
{
  return m_fInGain;
}

float EqParams::getOutputGain()
{
  return m_fOutGain;
}

void EqParams::setBandEnabled(int iBand, bool bIsEnabled)
{
  m_ptr_BandArray[iBand].bIsEnabled = bIsEnabled;
}

void EqParams::setBandFreq(int iBand, float fFreq)
{
  m_ptr_BandArray[iBand].fFreq = fFreq;
}

void EqParams::setBandGain(int iBand, float fGain)
{
  m_ptr_BandArray[iBand].fGain = fGain;
}

void EqParams::setBandQ(int iBand, float fQ)
{
  m_ptr_BandArray[iBand].fQ = fQ;
}

void EqParams::setBandType(int iBand, int iType)
{
  m_ptr_BandArray[iBand].iType = iType;
}

void EqParams::setInputGain(float fInGain)
{
  m_fInGain = fInGain;
}

void EqParams::setOutputGain(float fOutGain)
{
  m_fOutGain = fOutGain;
}

void EqParams::loadFromTtlFile(const char *uri)
{
  #ifdef USE_SLV2
    //Load from ttl
    SLV2World lv2World = slv2_world_new();
    slv2_world_load_all(lv2World);
    SLV2Plugins lv2Plugins = slv2_world_get_all_plugins(lv2World);
    SLV2Plugin lv2ThisPlugin = slv2_plugins_get_by_uri(lv2Plugins, slv2_value_new_uri(lv2World, uri));
    
    //Load Data from plugin
    SLV2Value lv2Symbol;
    SLV2Value min, max, def;
    SLV2Port port;

    //Get InputGain
    lv2Symbol = slv2_value_new_string(lv2World, "input_gain");
    port = slv2_plugin_get_port_by_symbol(lv2ThisPlugin, lv2Symbol);
    slv2_port_get_range(lv2ThisPlugin, port, &def, &min, &max);
    m_fInGain = slv2_value_as_float(def);

    //Get OutputGain
    lv2Symbol = slv2_value_new_string(lv2World, "output_gain");
    port = slv2_plugin_get_port_by_symbol(lv2ThisPlugin, lv2Symbol);
    slv2_port_get_range(lv2ThisPlugin, port, &def, &min, &max);
    m_fOutGain = slv2_value_as_float(def);

    std::string sSymbol; 
    char buffer[50];
    for(int i = 0; i < m_iNumberOfBands; i ++)
    {
      //Get Band Gain
      sprintf (buffer, "filter%d_gain", i+1);
      sSymbol = buffer;
      lv2Symbol = slv2_value_new_string(lv2World, sSymbol.c_str());
      port = slv2_plugin_get_port_by_symbol(lv2ThisPlugin, lv2Symbol);
      slv2_port_get_range(lv2ThisPlugin, port, &def, &min, &max);
      m_ptr_BandArray[i].fGain = slv2_value_as_float(def);
      
      //Get Band Freq
      sprintf (buffer, "filter%d_freq", i+1);
      sSymbol = buffer;
      lv2Symbol = slv2_value_new_string(lv2World, sSymbol.c_str());
      port = slv2_plugin_get_port_by_symbol(lv2ThisPlugin, lv2Symbol);
      slv2_port_get_range(lv2ThisPlugin, port, &def, &min, &max);
      m_ptr_BandArray[i].fFreq = slv2_value_as_float(def);
      
      //Get Band Q
      sprintf (buffer, "filter%d_q", i+1);
      sSymbol = buffer;
      lv2Symbol = slv2_value_new_string(lv2World, sSymbol.c_str());
      port = slv2_plugin_get_port_by_symbol(lv2ThisPlugin, lv2Symbol);
      slv2_port_get_range(lv2ThisPlugin, port, &def, &min, &max);
      m_ptr_BandArray[i].fQ = slv2_value_as_float(def);
      
      //Get Band Type
      sprintf (buffer, "filter%d_type", i+1);
      sSymbol = buffer;
      lv2Symbol = slv2_value_new_string(lv2World, sSymbol.c_str());
      port = slv2_plugin_get_port_by_symbol(lv2ThisPlugin, lv2Symbol);
      slv2_port_get_range(lv2ThisPlugin, port, &def, &min, &max);
      m_ptr_BandArray[i].iType = (int) slv2_value_as_float(def);
      
      //Get Band Enabled
      sprintf (buffer, "filter%d_enable", i+1);
      sSymbol = buffer;
      lv2Symbol = slv2_value_new_string(lv2World, sSymbol.c_str());
      port = slv2_plugin_get_port_by_symbol(lv2ThisPlugin, lv2Symbol);
      slv2_port_get_range(lv2ThisPlugin, port, &def, &min, &max);
      m_ptr_BandArray[i].bIsEnabled = slv2_value_as_float(def) > 0.5;
    }
      
    //Free all
    slv2_value_free(lv2Symbol);
    slv2_value_free(min);
    slv2_value_free(max);
    slv2_value_free(def);
    slv2_plugins_free(lv2World, lv2Plugins);
    slv2_world_free(lv2World);
 
  #else
    m_fInGain = 0;
    m_fOutGain = 0;
    float cur_freq = 30.0;
    for(int i = 0; i < m_iNumberOfBands; i ++)
    {
      m_ptr_BandArray[i].fGain = 0;
      m_ptr_BandArray[i].fQ = 2;
      m_ptr_BandArray[i].iType = 11;
      m_ptr_BandArray[i].bIsEnabled = 0;
      m_ptr_BandArray[i].fFreq = cur_freq;
      
      switch(m_iNumberOfBands)
      {
	case 4:
	  cur_freq *=5.0;
	  break;
	
	case 6:
	  cur_freq *=3.0;
	  break;
	  
	case 10:
	  cur_freq *=2.0;
	  break;
	  
	default:
	  m_ptr_BandArray[i].fFreq = 1000;
	  break;
      }
      
    }
  #endif
}

bool EqParams::loadFromFile(const char* path)
{
  int preBandCount;
  int signature;
  std::ifstream f;
  f.open(path, std::ofstream::in);
  
  f.read((char*)(&signature), sizeof(signature));
  if (signature != EQ10Q_SIGNATURE)
  {
    f.close();
    return false;
  }
  
  f.read((char*)(&preBandCount), sizeof (preBandCount));
  if(m_iNumberOfBands != preBandCount)
  {
    f.close();
    return false;
  }
  
  f.read((char*)(&m_fInGain), sizeof (m_fInGain));
  f.read((char*)(&m_fOutGain), sizeof (m_fOutGain));
  f.read((char*)(m_ptr_BandArray), sizeof(EqBandStruct) * m_iNumberOfBands);  
  f.close();
  return true;
}

void EqParams::saveToFile(const char* path)
{
  std::ofstream f;  
  int signature = EQ10Q_SIGNATURE;
  f.open(path, std::ofstream::out);
  f.write((const char*)(&signature), sizeof(signature));
  f.write((const char*)(&m_iNumberOfBands), sizeof (m_iNumberOfBands));
  f.write((const char*)(&m_fInGain), sizeof (m_fInGain));
  f.write((const char*)(&m_fOutGain), sizeof (m_fOutGain));
  f.write((const char*)(m_ptr_BandArray), sizeof(EqBandStruct) * m_iNumberOfBands);  
  f.close();
}

