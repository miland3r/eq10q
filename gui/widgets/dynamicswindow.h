/***************************************************************************
 *   Copyright (C) 2009 by Pere Ràfols Soler                               *
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

#ifndef DYN_MAIN_WIN_H
  #define DYN_MAIN_WIN_H

#include <iostream>
#include <string>

#include <gtkmm/alignment.h> 
#include <gtkmm/box.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h> 

#include <cmath>

//LV2 UI header
#include <lv2/lv2plug.in/ns/extensions/ui/ui.h>

#include "mainwidget.h"
#include "vuwidget.h"
#include "knob2.h"
#include "toggle_button.h"
#include "dynplot.h"
#include "sidechainbox.h"

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
#define PORT_FEEDBACK_RANGE_SCACTIVE 15
#define PORT_COMP_MODE 16
#define PORT_PUNCH 17

//Test print information, comment out for the final release
//#define PRINT_DEBUG_INFO

using namespace sigc;

class DynMainWindow : public MainWidget
{
  public:
    DynMainWindow(const char *uri, std::string bundlePath, std::string title, bool isCompressor, bool hasSideChain);
    virtual ~DynMainWindow();   
    
    // Informing GUI about changes in the control ports
    void gui_port_event(LV2UI_Handle ui, uint32_t port, uint32_t buffer_size, uint32_t format, const void * buffer)
    {
      float data = * static_cast<const float*>(buffer);
      
      #ifdef PRINT_DEBUG_INFO
	std::cout<<"gui_port_event Entring....... "<<std::endl;
      #endif
      
        // Checking if params are the same as specified in the LV2 documentation
        if (format != 0) {
	    #ifdef PRINT_DEBUG_INFO
	      std::cout<<"\t-- Return Format != 0"<<std::endl;
	    #endif
            return;
        }
        if (buffer_size != 4) {
	    #ifdef PRINT_DEBUG_INFO
	      std::cout<<"\t-- Return buffer_size != 4"<<std::endl;
	    #endif  
            return;
        }

        // Updating values in GUI ========================================================
	switch (port)
	{
	  case PORT_KEY_LISTEN:
	    m_KeyButton.set_active(data > 0.5);
	  break;
	  
	  case PORT_THRESHOLD:
	    m_InputVu->set_value_th(data);
            m_Plot->set_threshold(data);
	  break;
	  
	  case PORT_ATACK:
	    m_Attack->set_value(data);
	  break;
	  
	  case PORT_HOLD_MAKEUP:
	    m_Hold_Makeup->set_value(data);
            if(m_bIsCompressor)
            {
              m_Plot->set_makeup(data);
            }
	  break;
	  
	  case PORT_DECAY:
	    m_Release->set_value(data);
	  break;
	  
	  case PORT_RATIO:
	    m_Ratio->set_value(data);
	    m_Plot->set_ratio(data);
	  break;
	  
	  case PORT_GAINREDUCTION:
	    m_GainReductionVu->setValue(0,data);
            m_Plot->set_gainreduction(data);
	  break;
	  
	  case PORT_HPFFREQ:
	    m_HPF->set_value(data);
	  break;
	  
	  case PORT_LPFFREQ:
	    m_LPF->set_value(data);
	  break;
	  
	  case PORT_GAIN:
	    m_InGainFader->set_value(data);
	  break;
	  
	  case PORT_INVU:
	    m_InputVu->setValue(0,data);
            m_Plot->set_inputvu(data);
	  break;
	  
	  case PORT_KNEE:
              m_Knee->set_value(data);
              m_Plot->set_knee(data);
              break; 
              
          case PORT_DRY_WET:
              m_DryWet->set_value(100.0*data); //In range of 0% to 100%
            break;
	    
	  case PORT_FEEDBACK_RANGE_SCACTIVE:
	    if(m_bIsCompressor)
	    {
	      m_FeedBackMode_SideChainActive.set_active(data > 0.5);
	    }
	    else
	    {
      	      m_Range->set_value(data);
              m_Plot->set_range(data);
	    }
	    break;
	    
	  case PORT_COMP_MODE:
	     m_OptoMode.set_active(data > 0.5);
	    break;
	    
	  case PORT_PUNCH:
	    m_Punch->set_value(100.0*data);  //In range of 0% to 100%
	    break;
	}       
        
	#ifdef PRINT_DEBUG_INFO	    
	  std::cout<<"\t--  Return OK"<<std::endl;
	#endif
	
    }

    LV2UI_Controller controller;
    LV2UI_Write_Function write_function;

  protected:
    VUWidget *m_InputVu;
    VUWidget *m_GainReductionVu; 
    KnobWidget2 *m_InGainFader;
    KnobWidget2 *m_Attack;
    KnobWidget2 *m_Hold_Makeup;
    KnobWidget2 *m_Release;
    KnobWidget2 *m_Punch;
    KnobWidget2 *m_Range, *m_Ratio;
    KnobWidget2 *m_Knee;
    KnobWidget2 *m_HPF;
    KnobWidget2 *m_LPF;
    KnobWidget2 *m_DryWet;
    ToggleButton m_KeyButton, m_FeedBackMode_SideChainActive, m_OptoMode;
    PlotDynCurve *m_Plot;
    SideChainBox m_SCBox;
    Gtk::Alignment m_KeyButtonAlign, m_TitleAlign, m_sidchianAlign, m_keyPadding, m_FeedBackModeAlign, m_OptoModeAlign;
    Gtk::HBox m_VuBox, m_PlotBox, m_BalBox, m_MainBox, m_BotBox, m_SideChain2Box;
    Gtk::VBox m_SideChainBox, m_TitleBox, m_DynBox, m_Main2Box, m_PlotLabelBox;  
    Gtk::Image *image_logo;
    Gtk::Label m_LTitle; 
    
    //Signal Handlers
    void onGainChange();
    void onThresholdChange();
    void onRatioChange();
    void onRangeChange();
    void onAttackChange();
    void onHoldChange();
    void onReleaseChange();
    void onKneeChange();
    void onHPFChange();
    void onLPFChange();
    void onDryWetChange();
    void onKeyListenChange();
    void onFeedbackModeChange();
    void onModeCompressorChange();
    void onPunchChange();
    
  private:
    std::string m_pluginUri;
    std::string m_bundlePath;  
    bool m_bIsCompressor;
};

#endif

