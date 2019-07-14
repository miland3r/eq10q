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
This file is the implementation of the DYN plugin
This plugin is inside the Sapista Plugins Bundle
This file implements functionalities for diferent dynamic plugins
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <lv2/lv2plug.in/ns/lv2core/lv2.h>
#include "dsp/db.h"
#include "dsp/fastmath.h"
#include "dsp/vu.h"
#include "dsp/filter.h"

#define @Plugin_Is_Dynamics_Compressor@
#define USE_EQ10Q_FAST_MATH
#define NUM_CHANNELS @Dyn_Channels_Count@

#define DYN_URI @Dyn_Uri@
#define PORT_OUTPUT_L 0
#define PORT_INPUT_L 1
#define PORT_KEY_LISTEN 2
#define PORT_THRESHOLD 3
#define PORT_ATACK 4
#define PORT_HOLD_MAKEUP 5
#define PORT_DECAY 6
#define PORT_RATIO 7
#define PORT_HPFFREQ 8
#define PORT_LPFFREQ 9
#define PORT_GAIN 10
#define PORT_INVU 11
#define PORT_GAINREDUCTION 12
#define PORT_KNEE 13
#define PORT_DRY_WET 14

#ifdef PLUGIN_IS_COMPRESSOR
#define PORT_FEEDBACK 15
#define PORT_COMP_MODE 16
#define PORT_PUNCH 17
#define PORT_OUTPUT_R 18
#define PORT_INPUT_R 19

#else
#ifdef PLUGIN_IS_COMPRESSOR_WITH_SC
#define PORT_SC_ACTIVE 15
#define PORT_COMP_MODE 16
#define PORT_PUNCH 17

#if NUM_CHANNELS==1
#define PORT_SC_AUDIO_IN 18
#else
#define PORT_OUTPUT_R 18
#define PORT_INPUT_R 19
#define PORT_SC_AUDIO_IN 20
#endif

#else
#define PORT_RANGE 15
#define PORT_OUTPUT_R 16
#define PORT_INPUT_R 17
#endif
#endif

#define K_BIAS_GAIN 0.0891250938133745507219174442070652730762958526611328125f //A trick to change the dinamic range of the fast_db2lin10 method for gate plugin
typedef struct {
  //Plugin ports
  float *key_listen;
  float *threshold;
  float *attack;
  float *hold_makeup; //Hold for gate makeup for compressor/expander
  float *decay;
  float *ratio;
  float *output[NUM_CHANNELS];
  float *gainreduction; 
  const float *input[NUM_CHANNELS];
  float *hpffreq;
  float *lpffreq;
  float *ingain;
  float *fVuIn;
  float *drywet;
  float *knee;
  #ifdef PLUGIN_IS_COMPRESSOR
  float *feedback;
  float *compressor_mode;
  #else
  #ifdef PLUGIN_IS_COMPRESSOR_WITH_SC
  float *sidechain_active;
  float *compressor_mode;
  const float *input_sidechain;
  #else
  float *range;
  #endif
  #endif
  
  #if defined(PLUGIN_IS_COMPRESSOR) || defined(PLUGIN_IS_COMPRESSOR_WITH_SC) 
  float *punch;
  float g_error0;
  #endif
  
  //Plugin Internal data
  float sample_rate;
  float g;
    
  int hold_count;
  Vu *InputVu[1];
  float detector_vu;
  float noise;
  Filter *LPF_fil, *HPF_fil;
  Buffers LPF_buf, HPF_buf;
  float *LutLog10;
  double PGAOut_L;
#if NUM_CHANNELS == 2
  double PGAOut_R;
#endif
} Dynamics;

static void cleanupDyn(LV2_Handle instance)
{
  Dynamics *plugin = (Dynamics *)instance;
  free(plugin->LutLog10);
  VuClean(plugin->InputVu[0]);
  FilterClean(plugin->HPF_fil);
  FilterClean(plugin->LPF_fil);
  free(instance);
}

static void connectPortDyn(LV2_Handle instance, uint32_t port, void *data)
{
  Dynamics *plugin = (Dynamics *)instance;
  switch (port) {
    case PORT_KEY_LISTEN:
	    plugin->key_listen = (float*)data;
	    break;
    case PORT_THRESHOLD:
	    plugin->threshold = (float*)data;
	    break;
    case PORT_ATACK:
	    plugin->attack = (float*)data;
	    break;
    case PORT_HOLD_MAKEUP:
	    plugin->hold_makeup = (float*)data;
	    break;
    case PORT_DECAY:
	    plugin->decay = (float*)data;
	    break;
    case PORT_RATIO:
	    plugin->ratio = (float*)data;
	    break;
    case PORT_INPUT_L:
	    plugin->input[0] = (const float*)data;
	    break;
    case PORT_OUTPUT_L:
	    plugin->output[0] = (float*)data;
	    break;
    case PORT_GAINREDUCTION:
	    plugin->gainreduction = (float*)data;
	    break;
    case PORT_HPFFREQ:
	    plugin->hpffreq = (float*)data;
	    break;
    case PORT_LPFFREQ:
	    plugin->lpffreq = (float*)data;
	    break;
    case PORT_GAIN:
	    plugin->ingain = (float*)data;
	    break;
    case PORT_INVU:
	    plugin->fVuIn=(float*)data;
	    break;

    case PORT_DRY_WET:
            plugin->drywet = (float*)data;
            break;
            
    case PORT_KNEE:
	    plugin->knee = (float*)data;
	    break;
	    
    #ifdef PLUGIN_IS_COMPRESSOR
    case PORT_FEEDBACK:
	    plugin->feedback = (float*)data;
	    break;
    case PORT_COMP_MODE:
	    plugin->compressor_mode = (float*)data;
	    break;
	    
    case PORT_PUNCH:
	    plugin->punch = (float*)data;
	    break;
    
    #else
    #ifdef  PLUGIN_IS_COMPRESSOR_WITH_SC     
    case PORT_SC_ACTIVE:
	    plugin->sidechain_active = (float*)data;
	    break;
    case PORT_COMP_MODE:
	    plugin->compressor_mode = (float*)data;
	    break;
    case PORT_SC_AUDIO_IN:
	    plugin->input_sidechain = (const float*)data;
	    break;
    case PORT_PUNCH:
	    plugin->punch = (float*)data;
	    break;
	   
    #else
    case PORT_RANGE:
	    plugin->range = (float*)data;
	    break;    
    #endif
    #endif
	    
    #if NUM_CHANNELS == 2
    case PORT_INPUT_R:
	    plugin->input[1] = (const float*)data;
	    break;
    case PORT_OUTPUT_R:
	    plugin->output[1] = (float*)data;
	    break;
    #endif
  } 
}

static LV2_Handle instantiateDyn(const LV2_Descriptor *descriptor, double s_rate, const char *path, const LV2_Feature *const * features)
{
  Dynamics *plugin_data = (Dynamics *)malloc(sizeof(Dynamics));  
  plugin_data->LutLog10 = GenerateLog10LUT();
  plugin_data->sample_rate = s_rate;
  plugin_data->hold_count = 1000000;
  
  #ifdef PLUGIN_IS_GATE
  plugin_data->g =  0.0f;
  #endif
  #if defined(PLUGIN_IS_COMPRESSOR) || defined(PLUGIN_IS_COMPRESSOR_WITH_SC) 
  plugin_data->g =  1.0f;
  plugin_data->g_error0 = 0.0f;
  #endif
    
  plugin_data->InputVu[0] = VuInit(s_rate);
  plugin_data->detector_vu = 0.0f;
  plugin_data->noise = 0.0001; //the noise to get the GR VU workin in GUI
  plugin_data->HPF_fil = FilterInit(s_rate);
  plugin_data->LPF_fil = FilterInit(s_rate);
  plugin_data->PGAOut_L = 0.0;
#if NUM_CHANNELS == 2
  plugin_data->PGAOut_R = 0.0;
#endif
  flushBuffers(&plugin_data->LPF_buf);
  flushBuffers(&plugin_data->HPF_buf);
  return (LV2_Handle)plugin_data;
}

#define DENORMAL_TO_ZERO_FLOAT(x) if (fabs(x) < (1e-30)) x = 0.f; //Min float is 1.1754943e-38
static void runDyn(LV2_Handle instance, uint32_t sample_count)
{
  Dynamics *plugin_data = (Dynamics *)instance;
   
  const float attack = *(plugin_data->attack);
  const float decay = *(plugin_data->decay);
  const float hpffreq = *(plugin_data->hpffreq);
  const float lpffreq = *(plugin_data->lpffreq);
  const float KeyListen = *(plugin_data->key_listen);
  const float InputGain = dB2Lin(*(plugin_data->ingain));
  const float DryWet =  *(plugin_data->drywet);
  float gr_meter = 1.0f;
  const float ratio =  *(plugin_data->ratio);
  const float threshold = *(plugin_data->threshold);
  const float knee = *(plugin_data->knee);
  
  //Read ports (gate)
  #ifdef PLUGIN_IS_GATE
  const float range = *(plugin_data->range);
  const float hold = *(plugin_data->hold_makeup);
  const float threshold_lin = Fast_dB2Lin10(*(plugin_data->threshold));
  #endif
  
  //Read ports (compressor/expander)
  #if defined(PLUGIN_IS_COMPRESSOR) || defined(PLUGIN_IS_COMPRESSOR_WITH_SC) 
  const float makeup = dB2Lin(*(plugin_data->hold_makeup));
  const float punch = *(plugin_data->punch);
  #else
  const float makeup = 1.0f;
  #endif
  
  #ifdef PLUGIN_IS_COMPRESSOR
  const double FeedBack = (double)*(plugin_data->feedback);
  const int bIsOptoCompressor = *(plugin_data->compressor_mode) > 0.5 ? 1 : 0; 
  const double SideChainActive = 0.0;
  #endif
  
  #ifdef PLUGIN_IS_COMPRESSOR_WITH_SC
  const double SideChainActive = (double)*(plugin_data->sidechain_active);
  const double FeedBack = 0.0;
  const int bIsOptoCompressor = *(plugin_data->compressor_mode) > 0.5 ? 1 : 0; 
  #endif
  
  //Plguin data
  float input_detector;
  float sample_rate = plugin_data->sample_rate;
  float g = plugin_data->g;
  int hold_count = plugin_data->hold_count;
  float x_dB, y_dB;
  float knee_range;
  
  //Processor vars (only for gate)
  #ifdef PLUGIN_IS_GATE
  const float range_lin = pow(10, range * 0.05); 
  const int hold_max = (int)round(hold * sample_rate * 0.001f);
  const float Kac = pow((range_lin/(1.0f-range_lin))*((1.0f-0.9f)/0.9f), 1.0f/(attack*0.001f*sample_rate)); //Attack constant for S curve in gate
  #endif
  
  //Processor vars (only COMPRESSOR)
  #if defined(PLUGIN_IS_COMPRESSOR) || defined(PLUGIN_IS_COMPRESSOR_WITH_SC) 
  float g_error = 0.0f;
  float scl = 0.0f;
  const float range_lin = pow(10, -12 * 0.05); //By various tests -12 dB is the optimal setting for the release time of a opto-compressor
  const float Kdc = pow((range_lin/(1.0f-range_lin))*((1.0f-0.6f)/0.6f), 1.0f/(decay*0.001f*sample_rate)); //Decay constant for S curve in compressor
  #endif
  
  //Processor vars  common  
  const float ac = exp(-6.0f/(attack * sample_rate * 0.001f)); //Attack constant in compressor 
  const float dc = exp(-2.0f/(decay * sample_rate * 0.001f)); //Decay constant 
  float detector_vu = plugin_data->detector_vu;
  float gain_reduction = 0.0f;
  float input_filtered = 0.0f;
  double dToFiltersChain = 0.0;
  float input_preL;
  #if defined(PLUGIN_IS_COMPRESSOR) || defined(PLUGIN_IS_COMPRESSOR_WITH_SC) 
  float input_sc = 0.0;
  #endif
  
  #if NUM_CHANNELS == 2
  float input_preR;
  #endif
  for (uint32_t i = 0; i < sample_count; ++i) 
  {
    //Compute filter coeficients
    if(hpffreq != plugin_data->HPF_fil->freq)
    {
      calcCoefs(plugin_data->HPF_fil, 0.0, hpffreq, 0.75, F_HPF_ORDER_2, 1.0);
    }
    if(lpffreq != plugin_data->LPF_fil->freq)
    {
      calcCoefs(plugin_data->LPF_fil, 0.0, lpffreq, 0.75, F_LPF_ORDER_2, 1.0);
    }

    //Input gain
    input_preL =  plugin_data->input[0][i] * InputGain;
    #ifdef PLUGIN_IS_COMPRESSOR_WITH_SC
    input_sc = plugin_data->input_sidechain[i];
    #endif
    #if defined(PLUGIN_IS_COMPRESSOR) || defined(PLUGIN_IS_COMPRESSOR_WITH_SC) 
    dToFiltersChain = ((double)input_preL * (1.0 - FeedBack) + plugin_data->PGAOut_L*FeedBack)*(1.0  - SideChainActive) + (double)input_sc * SideChainActive;
    #else
    dToFiltersChain = (double)input_preL;
    #endif
    #if NUM_CHANNELS == 2
    input_preR = plugin_data->input[1][i] * InputGain;
    #if defined(PLUGIN_IS_COMPRESSOR) || defined(PLUGIN_IS_COMPRESSOR_WITH_SC) 
    dToFiltersChain = (((double)(input_preL + input_preR)*(1.0 - FeedBack) + (plugin_data->PGAOut_L + plugin_data->PGAOut_R)*FeedBack )*0.5)*(1.0  - SideChainActive) + (double)input_sc * SideChainActive;
    #else
    dToFiltersChain = (double)(input_preL + input_preR)*0.5;
    #endif
    #endif
    

    //Apply Filters
    computeFilter(plugin_data->LPF_fil, &plugin_data->LPF_buf, &dToFiltersChain);
    computeFilter(plugin_data->HPF_fil, &plugin_data->HPF_buf, &dToFiltersChain);
    input_filtered = (float)dToFiltersChain;
    
    //Detector signal used for Threshold VU-meter in both gate and compressor, also used in expander hold
    input_detector = fabs(input_filtered); //This is the detector signal filtered
   
    //Thresholding and gain computer
    #ifdef USE_EQ10Q_FAST_MATH
    x_dB = 20.0f * fastLog10((int*)(&input_filtered), plugin_data->LutLog10);	
    #else
    x_dB = 20.0f*log10(fabs(input_filtered) + 0.00001f); //Add -100dB constant to avoid zero crozing
    #endif
    knee_range = 2.0f*(x_dB - threshold);
    
    //Sample to Input TH-Vumeter
    if(input_detector >= detector_vu)
    {
      detector_vu = input_detector - (input_detector - detector_vu)*ac;
    }
    else
    {
      detector_vu = input_detector - (input_detector - detector_vu)*dc;
    }
    SetSample(plugin_data->InputVu[0], detector_vu);    
    
    //===================== GATE CODE ================================
    #ifdef PLUGIN_IS_GATE
     if (knee_range < -knee)
    {
      //Under Threshold
      y_dB = threshold + (x_dB - threshold)*ratio;
    }
    else if(knee_range >= knee )
    {
      //Over Threshold
      y_dB = x_dB; 
    }
    else
    {
      //On Knee
      y_dB = x_dB + ((1.0 - ratio)*(x_dB - threshold - knee/2)*(x_dB - threshold - knee/2))/(2*knee);
    }
    if( y_dB < x_dB + range )
    {
      y_dB = x_dB + range;
    }
    
    //Linear gain computing 
    #ifdef USE_EQ10Q_FAST_MATH
      gain_reduction = K_BIAS_GAIN*Fast_dB2Lin10(y_dB - x_dB + 22); //22dB bias compensated with K_BIAS_GAIN linear multiplication. This allows a good response of Fast_dB2Lin10 at -90 dB range
    #else
      gain_reduction = pow(10.0f, 0.05f*(y_dB - x_dB));
    #endif
    
    //Ballistics and peak detector
    hold_count = input_detector > threshold_lin ? 0 : hold_count;
    if(gain_reduction > g)
    {       
      //Gate/Expander OFF (Opening)
       g = 1.0f/(1.0f + Kac*((1.0f - g)/(g + 1e-8f))); 
      gain_reduction = g > gain_reduction ? gain_reduction : g;
    }
    else
    {  
      //Gate/Expander ON (Closeing)
      if(hold_count > hold_max)
      {
	gain_reduction = gain_reduction - (gain_reduction - g)*dc; //Log-Curve release
      }
      else
      {
	//Holding...
	gain_reduction = g;
      }
    }
    hold_count++; 
          
    //-----------------------------------------------------------      
    #endif
    //===================== END OF GATE CODE =========================
    
    //=================== COMPRESSOR CODE ============================
    #if defined(PLUGIN_IS_COMPRESSOR) || defined(PLUGIN_IS_COMPRESSOR_WITH_SC)        
    if (knee_range < -knee)
    {
      //Under Threshold
      y_dB = x_dB; 
    }
    else if(knee_range >= knee )
    {
      //Over Threshold
      y_dB = threshold + (x_dB - threshold)/ratio; 
    }
    else
    {
      //On Knee
      y_dB = x_dB + ((1.0f/ratio -1.0f)*(x_dB - threshold + knee/2.0f)*(x_dB - threshold + knee/2.0f))/(2.0f*knee);
    }

    //Linear gain computing
    #ifdef USE_EQ10Q_FAST_MATH
      gain_reduction = Fast_dB2Lin8(y_dB - x_dB);
    #else
      gain_reduction = pow(10.0f, 0.05f*(y_dB - x_dB));
    #endif
        
    //Ballistics and peak detector
    if(gain_reduction > g)
    {
     
      //Compressor OFF
      if(bIsOptoCompressor)
      {
	gain_reduction = 1.0f/(1.0f + Kdc*((1.0f - g)/(g + 1e-8f))); //S-Curve release
      }
      else
      {
	gain_reduction = 1.0f - (1.0f - g)*dc; //Log-Curve release
      }
    }
    else
    {
      //Compressor ON
      g_error = gain_reduction - g;      
      scl = punch*(g_error*plugin_data->g_error0)/((1.0f-gain_reduction + 1e-8f)*(1.0f-gain_reduction + 1e-8f));
      gain_reduction = (gain_reduction - (g_error*ac))*(1.0f- scl) + scl*g;
      plugin_data->g_error0 = g_error;     
    }  
    #endif
    //=================== END OF COMPRESSOR CODE ======================
    
    DENORMAL_TO_ZERO_FLOAT(gain_reduction);
    g = gain_reduction;
    gr_meter = gain_reduction < gr_meter ? gain_reduction : gr_meter;
    
    plugin_data->PGAOut_L = (double)input_preL * gain_reduction;
    plugin_data->output[0][i] = input_filtered*(KeyListen) + (input_preL*(1.0f - DryWet) +  makeup*(float)plugin_data->PGAOut_L*DryWet)*(1-KeyListen); 
    
    #if NUM_CHANNELS == 2
    plugin_data->PGAOut_R = (double)input_preR * gain_reduction;
    plugin_data->output[1][i] = input_filtered*(KeyListen) + (input_preR*(1.0f - DryWet) +  makeup*(float)plugin_data->PGAOut_R*DryWet)*(1-KeyListen);
    #endif

  }
  plugin_data->g = g;
  plugin_data->hold_count = hold_count;
  plugin_data->noise *= -1.0;
  plugin_data->detector_vu = detector_vu;
  *(plugin_data->gainreduction) = 1.0/gr_meter + plugin_data->noise; // + ((float)(rand() % 100)/100.0);; //OK esta en lineal
  *(plugin_data->fVuIn) = ComputeVu(plugin_data->InputVu[0], sample_count);
}

static const LV2_Descriptor dynDescriptor = {
  DYN_URI,
  instantiateDyn,
  connectPortDyn,
  NULL,
  runDyn,
  NULL,
  cleanupDyn,
  NULL
};

LV2_SYMBOL_EXPORT
const LV2_Descriptor *lv2_descriptor(uint32_t index)
{
  switch (index) {
  case 0:
    return &dynDescriptor;
  default:
    return NULL;
  }
}
