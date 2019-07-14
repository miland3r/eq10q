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
This file is the implementation of the BassUp plugin
This plugin is inside the Sapista Plugins Bundle
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <lv2/lv2plug.in/ns/lv2core/lv2.h>
#include "dsp/filter.h"

#define BASSUP_URI "http://eq10q.sourceforge.net/bassup"
#define PORT_OUTPUT 0
#define PORT_INPUT 1
#define PORT_AMOUNT 2

#define NUM_OF_HPF_STAGES 4
#define HPF_FREQ 50.0f
#define LPF_FREQ 200.0f

typedef struct {
  //Plugin ports
  float *amount;
  float *output;
  const float *input;
  
  //Plugin Internal data
  float sample_rate;
  Filter *LPF_fil, *HPF_fil[4];
  Buffers LPF_buf, HPF_buf[4];
} BassUp;

static void cleanupBassUp(LV2_Handle instance)
{
  BassUp *plugin = (BassUp *)instance;
  int i;
  for(i=0; i<NUM_OF_HPF_STAGES; i++)
  {
      FilterClean(plugin->HPF_fil[i]);
  }
  FilterClean(plugin->LPF_fil);
  free(instance);
}

static void connectPortBassUp(LV2_Handle instance, uint32_t port, void *data)
{
  BassUp *plugin = (BassUp *)instance;
  switch (port) {
    case PORT_AMOUNT:
            plugin->amount = (float*)data;
            break;
      case PORT_INPUT:
            plugin->input = (const float*)data;
            break;
    case PORT_OUTPUT:
            plugin->output = (float*)data;
            break;
  } 
}

static LV2_Handle instantiateBassUp(const LV2_Descriptor *descriptor, double s_rate, const char *path, const LV2_Feature *const * features)
{
  BassUp *plugin_data = (BassUp *)malloc(sizeof(BassUp));  
  plugin_data->sample_rate = s_rate;
  int i;
  for(i =0; i<NUM_OF_HPF_STAGES; i++)
  {
    plugin_data->HPF_fil[i] = FilterInit(s_rate);
    flushBuffers(&plugin_data->HPF_buf[i]);
    calcCoefs(plugin_data->HPF_fil[i], 0.0, HPF_FREQ, 0.75, F_HPF_ORDER_2, 1.0);
  }
  plugin_data->LPF_fil = FilterInit(s_rate);
  flushBuffers(&plugin_data->LPF_buf);
  calcCoefs(plugin_data->LPF_fil, 0.0, LPF_FREQ, 0.75, F_LPF_ORDER_2, 1.0);
  return (LV2_Handle)plugin_data;
}

#define DENORMAL_TO_ZERO_FLOAT(x) if (fabs(x) < (1e-30)) x = 0.f; //Min float is 1.1754943e-38
static void runBassUp(LV2_Handle instance, uint32_t sample_count)
{
  BassUp *plugin_data = (BassUp *)instance; 
  const float amount = *(plugin_data->amount);
  double bassSignal;
  
  for (uint32_t i = 0; i < sample_count; ++i) 
  {
    //bassSignal
    bassSignal = (double)fabs(plugin_data->input[i]);
    
    //Apply Filters
    int j;
    for(j=0; j<NUM_OF_HPF_STAGES; j++)
    {
      computeFilter(plugin_data->HPF_fil[j], &plugin_data->HPF_buf[j], &bassSignal);
    }
    computeFilter(plugin_data->LPF_fil, &plugin_data->LPF_buf, &bassSignal);
    
    //Output
    plugin_data->output[i] = (float)bassSignal*amount + plugin_data->input[i];
  }
}

static const LV2_Descriptor bassupDescriptor = {
  BASSUP_URI,
  instantiateBassUp,
  connectPortBassUp,
  NULL,
  runBassUp,
  NULL,
  cleanupBassUp,
  NULL
};

LV2_SYMBOL_EXPORT
const LV2_Descriptor *lv2_descriptor(uint32_t index)
{
  switch (index) {
  case 0:
    return &bassupDescriptor;
  default:
    return NULL;
  }
}
