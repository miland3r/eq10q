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

#include <stdlib.h>
#include <iostream>

#include <cstring>
#include "midsidewindow.h"
#include "guiconstants.h"
#include "setwidgetcolors.h"

#define KNOB_ICON_FILE "/knobs/knob2_32px.png"
#define KNOB_SIZE_X 75
#define KNOB_SIZE_Y 72
#define WIDGET_BORDER 3

MidSideMainWindow::MidSideMainWindow(const char* uri, std::string bundlePath, bool isLR2MS)
:m_pluginUri(uri),
  m_bundlePath(bundlePath),
  m_bisLR2MS(isLR2MS)
{
  std::string sIn1, sIn2, sOut1, sOut2;
  m_LabTitle.set_use_markup(true);
  //Set cutom theme color:
  Gdk::Color m_WinBgColor;
  SetWidgetColors m_WidgetColors;
  m_WidgetColors.setGenericWidgetColors(&m_LabTitle);
 
  if(m_bisLR2MS)
  {
    sIn1 = "In Left";
    sIn2 = "In Right";
    sOut1 = "Out Mid";
    sOut2 = "Out Side";
    m_LabTitle.set_markup( "<span font_weight=\"bold\" font=\"12px\" font_family=\"Monospace\"> Matrix: Stereo to Mid/Side </span>");
  }
  else
  {
    sIn1 = "In Mid";
    sIn2 = "In Side";
    sOut1 = "Out Left";
    sOut2 = "Out Right";
    m_LabTitle.set_markup( "<span font_weight=\"bold\" font=\"12px\" font_family=\"Monospace\"> Matrix: Mid/Side to Stereo </span>");
  }
    
  m_InGain1 = Gtk::manage(new KnobWidget2(-20.0, 20.0, "Level", "dB", (m_bundlePath + KNOB_ICON_FILE).c_str(), KNOB_TYPE_LIN, true ));
  m_InGain2 = Gtk::manage(new KnobWidget2(-20.0, 20.0, "Level", "dB", (m_bundlePath + KNOB_ICON_FILE).c_str(), KNOB_TYPE_LIN, true ));
  m_OutGain1 = Gtk::manage(new KnobWidget2(-20.0, 20.0, "Level", "dB", (m_bundlePath + KNOB_ICON_FILE).c_str(), KNOB_TYPE_LIN, true ));
  m_OutGain2 = Gtk::manage(new KnobWidget2(-20.0, 20.0, "Level", "dB", (m_bundlePath + KNOB_ICON_FILE).c_str(), KNOB_TYPE_LIN, true ));
  
  m_InSolo1.set_label("Solo");
  m_InSolo2.set_label("Solo");
  m_OutSolo1.set_label("Solo");
  m_OutSolo2.set_label("Solo");
  
  m_In1Frame.set_label( sIn1 );
  m_In2Frame.set_label( sIn2 );
  m_Out1Frame.set_label( sOut1 );
  m_Out2Frame.set_label( sOut2 );
  
  m_InAlng1.set_border_width(16);
  m_InAlng2.set_border_width(16);
  m_OutAlng1.set_border_width(16);
  m_OutAlng2.set_border_width(16);
  
  m_InSoloAlng1.set_padding(20, 0, 0, 0);
  m_InSoloAlng2.set_padding(20, 0, 0, 0);
  m_OutSoloAlng1.set_padding(20, 0, 0, 0);
  m_OutSoloAlng2.set_padding(20, 0, 0, 0);
  
  m_HInBox.set_border_width(10);
  m_HOutBox.set_border_width(10);
  
  m_In1Box.set_border_width(2);
  m_In2Box.set_border_width(2);
  m_Out1Box.set_border_width(2);
  m_Out2Box.set_border_width(2);
    
  m_InputVu1 = Gtk::manage(new VUWidget(1, -48.0, 6.0, ""));
  m_InputVu2 = Gtk::manage(new VUWidget(1, -48.0, 6.0, ""));
  m_OutputVu1 = Gtk::manage(new VUWidget(1, -48.0, 6.0, ""));
  m_OutputVu2 = Gtk::manage(new VUWidget(1, -48.0, 6.0, ""));
  
  set_size_request(-1, 450); 
  
  m_InSoloAlng1.add(m_InSolo1);
  m_In1Box.pack_start( m_InSoloAlng1  ,Gtk::PACK_SHRINK); 
  m_InVuAlng1.add(*m_InputVu1);
  m_In1Box.pack_start( m_InVuAlng1);
  m_In1Box.pack_start( *m_InGain1 ,Gtk::PACK_SHRINK);
  m_InAlng1.add(m_In1Box);
  m_In1Frame.add(m_InAlng1);
  
  m_InSoloAlng2.add(m_InSolo2);
  m_In2Box.pack_start( m_InSoloAlng2  ,Gtk::PACK_SHRINK);
  m_InVuAlng2.add(*m_InputVu2);
  m_In2Box.pack_start( m_InVuAlng2);
  m_In2Box.pack_start( *m_InGain2 ,Gtk::PACK_SHRINK);
  m_InAlng2.add(m_In2Box);
  m_In2Frame.add(m_InAlng2);
  
  m_OutSoloAlng1.add(m_OutSolo1);
  m_Out1Box.pack_start( m_OutSoloAlng1  ,Gtk::PACK_SHRINK);
  m_OutVuAlng1.add(*m_OutputVu1);
  m_Out1Box.pack_start( m_OutVuAlng1);
  m_Out1Box.pack_start( *m_OutGain1 ,Gtk::PACK_SHRINK);
  m_OutAlng1.add(m_Out1Box);
  m_Out1Frame.add(m_OutAlng1);
  
  m_OutSoloAlng2.add(m_OutSolo2);
  m_Out2Box.pack_start( m_OutSoloAlng2  ,Gtk::PACK_SHRINK);
  m_OutVuAlng2.add(*m_OutputVu2);
  m_Out2Box.pack_start( m_OutVuAlng2);
  m_Out2Box.pack_start( *m_OutGain2 ,Gtk::PACK_SHRINK);
  m_OutAlng2.add(m_Out2Box);
  m_Out2Frame.add(m_OutAlng2);
    
  m_HInBox.pack_start( m_In1Frame, Gtk::PACK_EXPAND_PADDING);
  m_HInBox.pack_start( m_In2Frame, Gtk::PACK_EXPAND_PADDING);
  m_HOutBox.pack_start( m_Out1Frame, Gtk::PACK_EXPAND_PADDING);
  m_HOutBox.pack_start( m_Out2Frame, Gtk::PACK_EXPAND_PADDING);
  
  m_HTopBox.pack_start ( m_HInBox);
  m_HTopBox.pack_start ( m_HOutBox);
  
  m_labAlng.set_padding(10,0,0,0);
  m_labAlng.add(m_LabTitle);
  m_VTopBox.pack_start(m_labAlng,Gtk::PACK_SHRINK);
  m_VTopBox.pack_start(m_HTopBox);
  
  show_all();
  add(m_VTopBox);

  //Signal handlers
  m_InGain1->signal_changed().connect(sigc::mem_fun(*this, &MidSideMainWindow::onInGain1Change));
  m_InGain2->signal_changed().connect(sigc::mem_fun(*this, &MidSideMainWindow::onInGain2Change));
  m_OutGain1->signal_changed().connect(sigc::mem_fun(*this, &MidSideMainWindow::onOutGain1Change));
  m_OutGain2->signal_changed().connect(sigc::mem_fun(*this, &MidSideMainWindow::onOutGain2Change));
  m_InSolo1.signal_clicked().connect(sigc::mem_fun(*this, &MidSideMainWindow::onInSolo1Change));
  m_InSolo2.signal_clicked().connect(sigc::mem_fun(*this, &MidSideMainWindow::onInSolo2Change));
  m_OutSolo1.signal_clicked().connect(sigc::mem_fun(*this, &MidSideMainWindow::onOutSolo1Change));
  m_OutSolo2.signal_clicked().connect(sigc::mem_fun(*this, &MidSideMainWindow::onOutSolo2Change));
}

MidSideMainWindow::~MidSideMainWindow()
{
 delete m_InGain1;
 delete m_InGain2;
 delete m_OutGain1;
 delete m_OutGain2;
 delete m_InputVu1;
 delete m_InputVu2;
 delete m_OutputVu1;
 delete m_OutputVu2;
}

void MidSideMainWindow::onInGain1Change()
{
  //Write to LV2 port
  float aux;
  aux = m_InGain1->get_value();
  write_function(controller, PORT_GAIN_IN_1, sizeof(float), 0, &aux);
}

void MidSideMainWindow::onInGain2Change()
{
  //Write to LV2 port
  float aux;
  aux = m_InGain2->get_value();
  write_function(controller, PORT_GAIN_IN_2, sizeof(float), 0, &aux);
}

void MidSideMainWindow::onOutGain1Change()
{
  //Write to LV2 port
  float aux;
  aux = m_OutGain1->get_value();
  write_function(controller, PORT_GAIN_OUT_1, sizeof(float), 0, &aux);
}

void MidSideMainWindow::onOutGain2Change()
{
  //Write to LV2 port
  float aux;
  aux = m_OutGain2->get_value();
  write_function(controller, PORT_GAIN_OUT_2, sizeof(float), 0, &aux);
}

void MidSideMainWindow::onInSolo1Change()
{
  if(m_InSolo1.get_active())
  {
    resetSoloState();
    m_InSolo1.set_active(true);
  }
  
  //Write to LV2 port the new value of this solo button
  float aux;
  aux = m_InSolo1.get_active() ? 1.0 : 0.0;
  write_function(controller, PORT_SOLO_IN_1, sizeof(float), 0, &aux);
}

void MidSideMainWindow::onInSolo2Change()
{
  if(m_InSolo2.get_active())
  {
    resetSoloState();
    m_InSolo2.set_active(true);
  }
  
  //Write to LV2 port the new value of this solo button
  float aux;
  aux = m_InSolo2.get_active() ? 1.0 : 0.0;
  write_function(controller, PORT_SOLO_IN_2, sizeof(float), 0, &aux);
}

void MidSideMainWindow::onOutSolo1Change()
{
  if(m_OutSolo1.get_active())
  {
    resetSoloState();
    m_OutSolo1.set_active(true);
  }
  
  //Write to LV2 port the new value of this solo button
  float aux;
  aux = m_OutSolo1.get_active() ? 1.0 : 0.0;
  write_function(controller, PORT_SOLO_OUT_1, sizeof(float), 0, &aux);
}

void MidSideMainWindow::onOutSolo2Change()
{
  if(m_OutSolo2.get_active())
  {
    resetSoloState();
    m_OutSolo2.set_active(true);
  }
  
  //Write to LV2 port the new value of this solo button
  float aux;
  aux = m_OutSolo2.get_active() ? 1.0 : 0.0;
  write_function(controller, PORT_SOLO_OUT_2, sizeof(float), 0, &aux);
}

void MidSideMainWindow::resetSoloState()
{
  m_InSolo1.set_active(false);
  m_InSolo2.set_active(false);
  m_OutSolo1.set_active(false);
  m_OutSolo2.set_active(false);
    
  //Write to LV2 ports the null solo state
  float aux;
  aux = 0.0;
  write_function(controller, PORT_SOLO_IN_1, sizeof(float), 0, &aux);
  write_function(controller, PORT_SOLO_IN_2, sizeof(float), 0, &aux);
  write_function(controller, PORT_SOLO_OUT_1, sizeof(float), 0, &aux);
  write_function(controller, PORT_SOLO_OUT_2, sizeof(float), 0, &aux);
}
