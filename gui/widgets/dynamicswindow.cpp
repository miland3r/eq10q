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

#include <stdlib.h>
#include <iostream>

#include <cstring>
#include "dynamicswindow.h"
#include "guiconstants.h"
#include "setwidgetcolors.h"

#define KNOB_ICON_FILE "/knobs/knob2_32px.png"
#define KNOB_SIZE_X 75
#define KNOB_SIZE_Y 72
#define WIDGET_BORDER 3
#define LOGO_PATH "icons/logodynamics.png"


DynMainWindow::DynMainWindow(const char *uri, std::string bundlePath, std::string title, bool isCompressor, bool hasSideChain)
  :m_pluginUri(uri),
  m_bundlePath(bundlePath),
  m_bIsCompressor(isCompressor)
{ 
  m_InGainFader = Gtk::manage(new KnobWidget2(-20.0, 20.0, "In Gain", "dB", (m_bundlePath + KNOB_ICON_FILE).c_str(), KNOB_TYPE_LIN, true ));
  m_InputVu = Gtk::manage(new VUWidget(1, -48.0, 6.0, "In", false, true));
  m_GainReductionVu = Gtk::manage(new VUWidget(1, 0.0, 20.0, "GR", true));
  m_Attack = Gtk::manage(new KnobWidget2(0.01, 500.0, "Attack", "ms", (m_bundlePath + KNOB_ICON_FILE).c_str() , KNOB_TYPE_TIME ));
  m_Release = Gtk::manage(new KnobWidget2(5.0, 4000.0, "Release", "ms", (m_bundlePath + KNOB_ICON_FILE).c_str() , KNOB_TYPE_TIME ));
  m_Punch = Gtk::manage(new KnobWidget2(0.0, 100.0, "Punch", "%", (m_bundlePath + KNOB_ICON_FILE).c_str(), KNOB_TYPE_LIN ));
  m_HPF = Gtk::manage(new KnobWidget2(20.0, 20000.0, "Key HPF", "Hz",  (m_bundlePath + KNOB_ICON_FILE).c_str() , KNOB_TYPE_FREQ));
  m_LPF = Gtk::manage(new KnobWidget2(20.0, 20000.0, "Key LPF", "Hz",  (m_bundlePath + KNOB_ICON_FILE).c_str() , KNOB_TYPE_FREQ));
  m_DryWet = Gtk::manage(new KnobWidget2(0.0, 100.0, "Dry/Wet", "%", (m_bundlePath + KNOB_ICON_FILE).c_str(), KNOB_TYPE_LIN ));
  m_Ratio = Gtk::manage(new KnobWidget2(1.0, 100.0, "Ratio", "dB", (m_bundlePath + KNOB_ICON_FILE).c_str(), KNOB_TYPE_TIME ));
  m_Knee = Gtk::manage(new KnobWidget2(0.0, 20.0, "Knee", "dB", (m_bundlePath + KNOB_ICON_FILE).c_str() ));
  
  if(m_bIsCompressor)
  {
    //Is Compressor
    m_Hold_Makeup = Gtk::manage(new KnobWidget2(0.0, 20.0, "Makeup", "dB", (m_bundlePath + KNOB_ICON_FILE).c_str() ));
  }
  else
  {
    //Is Gate
    m_Hold_Makeup = Gtk::manage(new KnobWidget2(0.01, 3000.0, "Hold", "ms", (m_bundlePath + KNOB_ICON_FILE).c_str() , KNOB_TYPE_TIME ));
    m_Range = Gtk::manage(new KnobWidget2(-90.0, -1.0, "Range", "dB", (m_bundlePath + KNOB_ICON_FILE).c_str() ));
  }
  
  m_Plot = Gtk::manage(new PlotDynCurve(m_bIsCompressor));
  
  m_KeyButton.set_label("Key Listen");
  m_KeyButton.set_size_request(-1,20);
  m_KeyButtonAlign.add(m_KeyButton);
  m_KeyButtonAlign.set_padding(0,0, 10, 10);
  
  if(hasSideChain)
  {
    m_FeedBackMode_SideChainActive.set_label("SC Active");
  }
  else
  {
    m_FeedBackMode_SideChainActive.set_label("Feedback");
  }
  m_FeedBackMode_SideChainActive.set_size_request(-1,20);
  m_FeedBackModeAlign.add(m_FeedBackMode_SideChainActive);
  m_FeedBackModeAlign.set_padding(0,0, 10, 10);
  

  m_OptoMode.set_label("S-Release");
  m_OptoMode.set_size_request(-1,20);
  m_OptoModeAlign.add(m_OptoMode);
  m_OptoModeAlign.set_padding(0,0, 10, 10);

  
  m_LTitle.set_use_markup(true);
  m_LTitle.set_markup( "<span font_weight=\"bold\" font=\"12px\" font_family=\"Monospace\">"  + title + "</span>");
  m_LTitle.set_size_request(-1, 25);
  m_TitleAlign.add(m_LTitle);
  m_TitleAlign.set_padding(0,0, 60, 0);
  
  //load image logo
  image_logo = new Gtk::Image(m_bundlePath + "/" + LOGO_PATH);
   
  m_VuBox.pack_start(*m_InputVu, Gtk::PACK_SHRINK);
  m_VuBox.pack_start(*m_GainReductionVu, Gtk::PACK_SHRINK);
  m_VuBox.set_border_width(4);
  m_VuBox.show_all_children();

  m_SideChain2Box.set_border_width(WIDGET_BORDER);
  m_SideChain2Box.set_spacing(WIDGET_BORDER);
  m_SideChain2Box.pack_start(*m_LPF, Gtk::PACK_EXPAND_WIDGET);
  m_SideChain2Box.pack_start(*m_HPF, Gtk::PACK_EXPAND_WIDGET);
  
  m_SideChainBox.set_border_width(WIDGET_BORDER);
  m_SideChainBox.set_spacing(WIDGET_BORDER);
  m_SideChainBox.pack_start(m_KeyButtonAlign,Gtk::PACK_SHRINK);
  if(m_bIsCompressor)
  {
    m_SideChainBox.pack_start(m_FeedBackModeAlign,Gtk::PACK_SHRINK);
    m_SideChainBox.pack_start(m_OptoModeAlign,Gtk::PACK_SHRINK);
  }
  m_SideChainBox.pack_start(m_SideChain2Box,Gtk::PACK_SHRINK);
  m_SideChainBox.show_all_children();
  m_keyPadding.add(m_SideChainBox);
  m_keyPadding.set_padding(30,0, 0, 0);
  
  m_SCBox.add(m_keyPadding);
  m_sidchianAlign.set_padding(0, 3, 0, 0);
  m_sidchianAlign.add(m_SCBox);
  
  m_DynBox.set_border_width(WIDGET_BORDER);
  m_DynBox.set_spacing(WIDGET_BORDER);
  m_DynBox.pack_start(*m_InGainFader, Gtk::PACK_SHRINK);
  m_DynBox.pack_start(*m_Ratio, Gtk::PACK_SHRINK);
  m_DynBox.pack_start(*m_Knee, Gtk::PACK_SHRINK);

  m_DynBox.pack_start(*m_DryWet, Gtk::PACK_SHRINK);
  m_DynBox.show_all_children();
  
  m_BalBox.set_border_width(WIDGET_BORDER);
  m_BalBox.set_spacing(WIDGET_BORDER);
  m_BalBox.pack_start(*m_Attack, Gtk::PACK_SHRINK);
  m_BalBox.pack_start(*m_Release, Gtk::PACK_SHRINK);
  if(m_bIsCompressor)
  {
    m_BalBox.pack_start(*m_Punch, Gtk::PACK_SHRINK);
  }
  m_BalBox.pack_start(*m_Hold_Makeup, Gtk::PACK_SHRINK);
  if(!m_bIsCompressor)
  {
    m_BalBox.pack_start(*m_Range, Gtk::PACK_SHRINK);
  }
  m_BalBox.show_all_children();
  
  m_PlotBox.pack_start(m_DynBox, Gtk::PACK_SHRINK);
  m_PlotBox.pack_start(*m_Plot, Gtk::PACK_SHRINK);
  m_PlotBox.show_all_children();
  
  m_PlotLabelBox.pack_start(m_TitleAlign, Gtk::PACK_SHRINK);
  m_PlotLabelBox.pack_start(m_PlotBox, Gtk::PACK_SHRINK);
  m_PlotLabelBox.show_all_children();
  
  m_TitleBox.pack_start(m_BalBox, Gtk::PACK_SHRINK);
  m_TitleBox.pack_start(*image_logo, Gtk::PACK_SHRINK);
  m_TitleBox.show_all_children();
  
  m_BotBox.pack_start(m_TitleBox, Gtk::PACK_SHRINK);
  m_BotBox.pack_start(m_sidchianAlign, Gtk::PACK_EXPAND_PADDING);
  
  m_Main2Box.pack_start(m_PlotLabelBox, Gtk::PACK_SHRINK);
  m_Main2Box.pack_start(m_BotBox, Gtk::PACK_SHRINK);
  
  m_MainBox.pack_start(m_Main2Box, Gtk::PACK_SHRINK);
  m_MainBox.pack_start(m_VuBox, Gtk::PACK_SHRINK);
  
  show_all();
  add(m_MainBox);
    
  //Set cutom theme color:
  Gdk::Color m_WinBgColor;
  SetWidgetColors m_WidgetColors;
  m_WidgetColors.setGenericWidgetColors(&m_LTitle);
 
  //Connect signals
  m_InGainFader->signal_changed().connect(sigc::mem_fun(*this, &DynMainWindow::onGainChange));
  m_InputVu->signal_changed().connect(sigc::mem_fun(*this, &DynMainWindow::onThresholdChange));
  m_Ratio->signal_changed().connect(sigc::mem_fun(*this, &DynMainWindow::onRatioChange));
  m_Attack->signal_changed().connect(sigc::mem_fun(*this, &DynMainWindow::onAttackChange));
  m_Hold_Makeup->signal_changed().connect(sigc::mem_fun(*this, &DynMainWindow::onHoldChange));
  m_Release->signal_changed().connect(sigc::mem_fun(*this, &DynMainWindow::onReleaseChange));
  m_LPF->signal_changed().connect(sigc::mem_fun(*this, &DynMainWindow::onLPFChange));
  m_HPF->signal_changed().connect(sigc::mem_fun(*this, &DynMainWindow::onHPFChange));
  m_DryWet->signal_changed().connect(sigc::mem_fun(*this, &DynMainWindow::onDryWetChange));
  m_KeyButton.signal_clicked().connect(sigc::mem_fun(*this, &DynMainWindow::onKeyListenChange));
  m_Knee->signal_changed().connect(sigc::mem_fun(*this, &DynMainWindow::onKneeChange));
      
  if(m_bIsCompressor)
  {
    m_FeedBackMode_SideChainActive.signal_clicked().connect(sigc::mem_fun(*this, &DynMainWindow::onFeedbackModeChange));
    m_OptoMode.signal_clicked().connect(sigc::mem_fun(*this, &DynMainWindow::onModeCompressorChange));
    m_Punch->signal_changed().connect(sigc::mem_fun(*this, &DynMainWindow::onPunchChange));
  }
  else
  {
    m_Range->signal_changed().connect(sigc::mem_fun(*this, &DynMainWindow::onRangeChange));  
  }
}

DynMainWindow::~DynMainWindow()
{
  delete m_InputVu;
  delete m_GainReductionVu;
  delete m_InGainFader;
  delete m_Attack;
  delete m_Hold_Makeup;
  delete m_Release;
  delete m_Ratio;
  delete m_Knee;  
  delete m_Punch;
  if(!m_bIsCompressor)
  {
    delete m_Range;
  }
  delete m_HPF;
  delete m_LPF;
  delete m_DryWet;
  delete image_logo;
}

void DynMainWindow::onGainChange()
{ 
  //Write to LV2 port
  float aux;
  aux = m_InGainFader->get_value();
  write_function(controller, PORT_GAIN, sizeof(float), 0, &aux);
}

void DynMainWindow::onThresholdChange()
{
  //Write to LV2 port
  float aux;
  aux = m_InputVu->get_value_th();
  m_Plot->set_threshold(aux);
  write_function(controller, PORT_THRESHOLD, sizeof(float), 0, &aux);
}

void DynMainWindow::onRangeChange()
{
  //Write to LV2 port
  float aux;
  aux = m_Range->get_value();

  m_Plot->set_range(aux);
  write_function(controller, PORT_FEEDBACK_RANGE_SCACTIVE, sizeof(float), 0, &aux);
}

void DynMainWindow::onRatioChange()
{
  //Write to LV2 port
  float aux;
  aux = m_Ratio->get_value();
  m_Plot->set_ratio(aux);
  
  write_function(controller, PORT_RATIO, sizeof(float), 0, &aux);
}

void DynMainWindow::onAttackChange()
{
  //Write to LV2 port
  float aux;
  aux = m_Attack->get_value();
  write_function(controller, PORT_ATACK, sizeof(float), 0, &aux); 
}

void DynMainWindow::onHoldChange()
{
  //Write to LV2 port
  float aux;
  aux = m_Hold_Makeup->get_value();
  if(m_bIsCompressor)
  {
    m_Plot->set_makeup(aux);
  }
  write_function(controller, PORT_HOLD_MAKEUP, sizeof(float), 0, &aux);
}

void DynMainWindow::onReleaseChange()
{
  //Write to LV2 port
  float aux;
  aux = m_Release->get_value();
  write_function(controller, PORT_DECAY, sizeof(float), 0, &aux);
}

void DynMainWindow::onPunchChange()
{
  //Write to LV2 port
  float aux;
  aux = 0.01*m_Punch->get_value(); //Div by 100 to get 0% to 100% in range 0 to 1
  write_function(controller, PORT_PUNCH, sizeof(float), 0, &aux);
}

void DynMainWindow::onKneeChange()
{
  //Write to LV2 port
  float aux;
  aux = m_Knee->get_value();
  m_Plot->set_knee(aux);
  write_function(controller, PORT_KNEE, sizeof(float), 0, &aux);
}

void DynMainWindow::onHPFChange()
{
  //Write to LV2 port
  float aux;
  aux = m_HPF->get_value();
  write_function(controller, PORT_HPFFREQ, sizeof(float), 0, &aux);
}

void DynMainWindow::onLPFChange()
{
  //Write to LV2 port
  float aux;
  aux = m_LPF->get_value();
  write_function(controller, PORT_LPFFREQ, sizeof(float), 0, &aux);
}

void DynMainWindow::onDryWetChange()
{
  //Write to LV2 port
  float aux;
  aux = 0.01*m_DryWet->get_value(); //Div by 100 to get 0% to 100% in range 0 to 1
  write_function(controller, PORT_DRY_WET, sizeof(float), 0, &aux);
}

void DynMainWindow::onKeyListenChange()
{
  //Write to LV2 port
  float aux;
  aux = m_KeyButton.get_active() ? 1.0 : 0.0;
  write_function(controller, PORT_KEY_LISTEN, sizeof(float), 0, &aux);
}

void DynMainWindow::onFeedbackModeChange()
{
  //Write to LV2 port
  float aux;
  aux = m_FeedBackMode_SideChainActive.get_active() ? 1.0 : 0.0;
  write_function(controller, PORT_FEEDBACK_RANGE_SCACTIVE, sizeof(float), 0, &aux);
}

void DynMainWindow::onModeCompressorChange()
{
  //Write to LV2 port
  float aux;
  aux = m_OptoMode.get_active() ? 1.0 : 0.0;
  write_function(controller, PORT_COMP_MODE, sizeof(float), 0, &aux);
}

