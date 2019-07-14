/***************************************************************************
 *   Copyright (C) 2015 by Pere Ràfols Soler                               *
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
This file is the implementation of the MidSide Matrix plugin
This plugin is inside the Sapista Plugins Bundle
This file implements functionalities for lr2ms and ms2lr matrices
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <lv2/lv2plug.in/ns/lv2core/lv2.h>


#include "dsp/vu.h"
#include "dsp/db.h"
#include "dsp/midside.h"

//Data from CMake
#define MATRIX_URI @Matrix_Uri@
#define IS_LR2MS @Matrix_Is_LR2MS@ //If is defined to 1 is in LR2MS mode otherwise is MS2LR

//Port definitions
#define PORT_AUDIO_IN_1 0
#define PORT_AUDIO_IN_2 1
#define PORT_AUDIO_OUT_1 2
#define PORT_AUDIO_OUT_2 3
#define PORT_GAIN_IN_1 4
#define PORT_GAIN_IN_2 5
#define PORT_GAIN_OUT_1 6
#define PORT_GAIN_OUT_2 7
#define PORT_SOLO_IN_1 8
#define PORT_SOLO_IN_2 9
#define PORT_SOLO_OUT_1 10
#define PORT_SOLO_OUT_2 11
#define PORT_VU_IN_1 12
#define PORT_VU_IN_2 13
#define PORT_VU_OUT_1 14
#define PORT_VU_OUT_2 15

typedef struct {
  //Plugin ports
  float *fInGain1; //Its L for lr2ms and M for ms2lr
  float *fInGain2; //Its R for lr2ms and S for ms2lr
  float *fOutGain1;  //Its M for lr2ms and L for ms2lr
  float *fOutGain2;  //Its S for lr2ms and R for ms2lr
  
  float *fSoloIn1; //Its L for lr2ms and M for ms2lr
  float *fSoloIn2; //Its R for lr2ms and S for ms2lr
  float *fSoloOut1; //Its M for lr2ms and L for ms2lr
  float *fSoloOut2; //Its S for lr2ms and R for ms2lr
  
  const float *input[2];
  float *output[2];

  float *fVuIn[2];
  float *fVuOut[2];
 
  //Internal data
  Vu *InputVu[2];
  Vu *OutputVu[2];
  float sample_rate;
  
  //Matrix routing vars
  double RG_in1, RG_in2, RG_out11, RG_out12, RG_out21, RG_out22; 

} MidSideMatrix;

static void cleanupMatrix(LV2_Handle instance)
{
  MidSideMatrix *plugin = (MidSideMatrix *)instance;
  int i;
  for(i=0; i<2; i++)
  {
    VuClean(plugin->InputVu[i]);
    VuClean(plugin->OutputVu[i]);
  }
  free(instance);
}


static void connectPortMatrix(LV2_Handle instance, uint32_t port, void *data)
{
  MidSideMatrix *plugin = (MidSideMatrix *)instance;
  switch (port) {
    case PORT_AUDIO_IN_1:
	  plugin->input[0] = (const float*)data;;
	  break;
	    
    case PORT_AUDIO_IN_2:
	  plugin->input[1] = (const float*)data;;
	  break;
	    
    case PORT_AUDIO_OUT_1:
	  plugin->output[0] = (float*)data;
	  break;  
	    
    case PORT_AUDIO_OUT_2:
	  plugin->output[1] = (float*)data;
	  break;    
	        
    case PORT_GAIN_IN_1:
	  plugin->fInGain1 = (float*)data;
	  break;
	  
    case PORT_GAIN_IN_2:
	  plugin->fInGain2 = (float*)data;
	  break;

    case PORT_GAIN_OUT_1:
	  plugin->fOutGain1 = (float*)data;
	  break;

    case PORT_GAIN_OUT_2:
	  plugin->fOutGain2 = (float*)data;
	  break;	  
	  
    case PORT_SOLO_IN_1:
	  plugin->fSoloIn1 = (float*)data;
	  break;
	  
    case PORT_SOLO_IN_2:
	  plugin->fSoloIn2 = (float*)data;
	  break;
	  
    case PORT_SOLO_OUT_1:
	  plugin->fSoloOut1 = (float*)data;
	  break;
	 
    case PORT_SOLO_OUT_2:
	  plugin->fSoloOut2 = (float*)data;
	  break;  
	  
    case PORT_VU_IN_1:
	  plugin->fVuIn[0]=(float*)data;
	  break;
	  
    case PORT_VU_IN_2:
	  plugin->fVuIn[1]=(float*)data;
	  break;
	  
    case PORT_VU_OUT_1:
	  plugin->fVuOut[0]=(float*)data;
	  break;
	  
    case PORT_VU_OUT_2:
	  plugin->fVuOut[1]=(float*)data;
	  break;
  } 
}

static LV2_Handle instantiateMatrix(const LV2_Descriptor *descriptor, double s_rate, const char *path, const LV2_Feature *const * features)
{
  MidSideMatrix *plugin_data = (MidSideMatrix *)malloc(sizeof(MidSideMatrix));  
  plugin_data->sample_rate = s_rate;
  plugin_data->InputVu[0] = VuInit(s_rate);
  plugin_data->InputVu[1] = VuInit(s_rate);
  plugin_data->OutputVu[0] = VuInit(s_rate);
  plugin_data->OutputVu[1] = VuInit(s_rate);
  plugin_data->RG_in1 = 0.0f;
  plugin_data->RG_in2 = 0.0f;
  plugin_data->RG_out11 = 0.0f;
  plugin_data->RG_out12 = 0.0f;
  plugin_data->RG_out21 = 0.0f;
  plugin_data->RG_out22 = 0.0f;
 
  return (LV2_Handle)plugin_data;
}

static void runMatrix(LV2_Handle instance, uint32_t sample_count)
{
  MidSideMatrix *plugin_data = (MidSideMatrix *)instance;
     
  const double gain_in_1 = (double) dB2Lin(*(plugin_data->fInGain1)); 
  const double gain_in_2 = (double) dB2Lin(*(plugin_data->fInGain2));
  const double gain_out_1 = (double) dB2Lin(*(plugin_data->fOutGain1));
  const double gain_out_2 = (double) dB2Lin(*(plugin_data->fOutGain2));
  
  const float solo_in_1 =  *(plugin_data->fSoloIn1);
  const float solo_in_2 =  *(plugin_data->fSoloIn2);
  const float solo_out_1 = *(plugin_data->fSoloOut1);
  const float solo_out_2 = *(plugin_data->fSoloOut2);

  //Set RG constants acording SOLO state
  plugin_data-> RG_in1 = 0.0;
  plugin_data-> RG_in2 = 0.0;
  plugin_data->RG_out11 = 1.0;
  plugin_data->RG_out12 = 0.0;
  plugin_data->RG_out21 = 0.0;
  plugin_data->RG_out22 = 1.0;
  
  if( solo_out_1 > 0.5)
  {
    plugin_data-> RG_in1 = 0.0;
    plugin_data-> RG_in2 = 0.0;
    plugin_data->RG_out11 = 1.0;
    plugin_data->RG_out12 = 1.0;
    plugin_data->RG_out21 = 0.0;
    plugin_data->RG_out22 = 0.0;
  }
  
  if( solo_out_2 > 0.5)
  {
    plugin_data-> RG_in1 = 0.0;
    plugin_data-> RG_in2 = 0.0;
    plugin_data->RG_out11 = 0.0;
    plugin_data->RG_out12 = 0.0;
    plugin_data->RG_out21 = 1.0;
    plugin_data->RG_out22 = 1.0;
  }
  
  if( solo_in_1 > 0.5 )
  {
    plugin_data-> RG_in1 = 1.0;
    plugin_data-> RG_in2 = 0.0;
    plugin_data->RG_out11 = 0.0;
    plugin_data->RG_out12 = 0.0;
    plugin_data->RG_out21 = 0.0;
    plugin_data->RG_out22 = 0.0;
  }
  
  if( solo_in_2 > 0.5)
  {
    plugin_data-> RG_in1 = 0.0;
    plugin_data-> RG_in2 = 1.0;
    plugin_data->RG_out11 = 0.0;
    plugin_data->RG_out12 = 0.0;
    plugin_data->RG_out21 = 0.0;
    plugin_data->RG_out22 = 0.0;
  }
  
  double sgn1_pre = 0.0, sgn2_pre = 0.0, sgn1_post = 0.0, sgn2_post = 0.0; //Internal signals
  
  for (uint32_t i = 0; i < sample_count; ++i) 
  {
    //Input Gains
    sgn1_pre = (double) (plugin_data->input[0][i]) * gain_in_1;
    sgn2_pre = (double) (plugin_data->input[1][i]) * gain_in_2;

 
    
    sgn1_post = sgn1_pre;
    sgn2_post = sgn2_pre;
    #if IS_LR2MS == 1
      LR2MS(&sgn1_post, &sgn2_post, 1.0);
    #else
      MS2LR(&sgn1_post, &sgn2_post, 1.0);
    #endif
   
    //Output Gains  
    sgn1_post *=   gain_out_1;
    sgn2_post *=   gain_out_2;
    
    //Update VU's
    SetSample(plugin_data->InputVu[0], (float)sgn1_pre);    
    SetSample(plugin_data->InputVu[1], (float)sgn2_pre);   
    SetSample(plugin_data->OutputVu[0], (float)sgn1_post);  
    SetSample(plugin_data->OutputVu[1], (float)sgn2_post);  
    
    //Set outputs
    plugin_data->output[0][i] = plugin_data->RG_in1 * sgn1_pre + 
				plugin_data->RG_in2 * sgn2_pre + 
				plugin_data->RG_out11 * sgn1_post + 
				plugin_data->RG_out21 * sgn2_post;
				
    plugin_data->output[1][i] = plugin_data->RG_in1 * sgn1_pre + 
				plugin_data->RG_in2 * sgn2_pre + 
				plugin_data->RG_out12 * sgn1_post + 
				plugin_data->RG_out22 * sgn2_post;
  }

  //Refresh VU's
  *(plugin_data->fVuIn[0]) = ComputeVu(plugin_data->InputVu[0], sample_count);
  *(plugin_data->fVuIn[1]) = ComputeVu(plugin_data->InputVu[1], sample_count);
  *(plugin_data->fVuOut[0]) = ComputeVu(plugin_data->OutputVu[0], sample_count);
  *(plugin_data->fVuOut[1]) = ComputeVu(plugin_data->OutputVu[1], sample_count);
}

static const LV2_Descriptor msMatrixDescriptor = {
  MATRIX_URI,
  instantiateMatrix,
  connectPortMatrix,
  NULL,
  runMatrix,
  NULL,
  cleanupMatrix,
  NULL
};

LV2_SYMBOL_EXPORT
const LV2_Descriptor *lv2_descriptor(uint32_t index)
{
  switch (index) {
  case 0:
    return &msMatrixDescriptor;
  default:
    return NULL;
  }
}
