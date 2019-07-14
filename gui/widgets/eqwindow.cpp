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

#include <iostream>
#include <cstring>
#include <gtkmm/rc.h>
#include <gtkmm/filechooserdialog.h>
#include "eqwindow.h"
#include "guiconstants.h"
#include "setwidgetcolors.h"

#define KNOB_ICON_FILE "/knobs/knob2_32px.png"
#define KNOB_ICON_FILE_MINI "/knobs/knob2_25px.png"

//Constructor
EqMainWindow::EqMainWindow(int iAudioChannels, int iNumBands, const char *uri, const char *bundlePath, const LV2_Feature *const *features)
  :m_BypassButton("Eq On"),
  m_FftRtaActive("RTA"),
  m_FftSpecActive("Spec"),
  m_dB10Scale("10 dB"),
  m_dB25Scale("25 dB"),
  m_dB50Scale("50 dB"),
  m_LRStereoMode("L/R"),
  m_MSStereoMode("M/S"),
  m_FlatButton("Flat"),
  m_SaveButton("Save"),
  m_LoadButton("Load"), 
  m_FftHold("Hold"),
  m_iNumOfChannels(iAudioChannels),
  m_iNumOfBands(iNumBands),
  m_bMutex(false),
  m_port_event_InGain(false),
  m_port_event_OutGain(false),
  m_port_event_Bypass(false),
  m_port_event_Curve(false),
  m_pluginUri(uri),
  m_bundlePath(bundlePath)
{ 
  //Get LV2_Feature
  map = NULL;
  for (int i = 0; features[i]; ++i) 
  {
    if (!strcmp(features[i]->URI, LV2_URID_URI "#map"))
    {
      map = (LV2_URID_Map*)features[i]->data;
    }
  }

  if (!map)
  {
    std::cout<<"Eq10q UI: Host does not support urid:map"<<std::endl;
  }
  else
  {
    //Map uris and init forge
    map_eq10q_uris(map, &uris);
    lv2_atom_forge_init(&forge, map);
  }
  
  //Prepare curve Events vectors
  m_port_event_Curve_Gain = new bool[m_iNumOfBands];
  m_port_event_Curve_Freq = new bool[m_iNumOfBands];
  m_port_event_Curve_Q = new bool[m_iNumOfBands];
  m_port_event_Curve_Type = new bool[m_iNumOfBands];
  m_port_event_Curve_Enable = new bool[m_iNumOfBands];
  
  //load image logo
  image_logo_center = new Gtk::Image(m_bundlePath + std::string(IMAGE_LOGO_PATH));

  m_MainWidgetAlign.set_padding(3,3,3,3);
  
  m_AButton.set_active(true);
  m_ButtonAAlign.add(m_AButton);
  m_BypassAlign.add(m_BypassButton);
  m_ButtonAAlign.set(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER, 0.0, 0.0);
  m_BypassAlign.set(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER, 0.0, 0.0);
  m_FlatAlign.add(m_FlatButton);
  m_FlatAlign.set(Gtk::ALIGN_RIGHT, Gtk::ALIGN_CENTER,0.0, 0.0);
  m_LoadAlign.add(m_LoadButton);
  m_SaveAlign.add(m_SaveButton);
  m_LoadAlign.set(Gtk::ALIGN_RIGHT, Gtk::ALIGN_CENTER,0.0, 0.0);
  m_SaveAlign.set(Gtk::ALIGN_RIGHT, Gtk::ALIGN_CENTER,0.0, 0.0);
  m_BypassAlign.set_size_request(80, -1);
  
  m_GainFaderIn = Gtk::manage(new KnobWidget2(-20.0, 20.0, "In Gain", "dB", (m_bundlePath + KNOB_ICON_FILE).c_str(), KNOB_TYPE_LIN, true ));
  m_GainFaderOut = Gtk::manage(new KnobWidget2(-20.0, 20.0, "Out Gain", "dB", (m_bundlePath + KNOB_ICON_FILE).c_str(), KNOB_TYPE_LIN, true ));
  m_VuMeterIn = Gtk::manage(new VUWidget(m_iNumOfChannels, -36.0, 6.0, "In")); 
  m_VuMeterOut = Gtk::manage(new VUWidget(m_iNumOfChannels, -36.0, 6.0, "Out"));
   
  m_FftRange = Gtk::manage(new KnobWidget2(20.0, 100.0, "Range", "dB", (m_bundlePath + KNOB_ICON_FILE_MINI).c_str(), KNOB_TYPE_LIN ));
  m_FftGain = Gtk::manage(new KnobWidget2(-20.0, 20.0, "Gain", "dB", (m_bundlePath + KNOB_ICON_FILE_MINI).c_str(), KNOB_TYPE_LIN, true ));
  m_FftRange->set_value(80.0);
  m_FftGain->set_value(0.0);
  m_FftCtlVBox.pack_start(m_FftRtaActive, Gtk::PACK_EXPAND_PADDING);
  m_FftCtlVBox.pack_start(m_FftSpecActive, Gtk::PACK_EXPAND_PADDING);
  m_FftAlngGain.add(*m_FftGain); 
  m_FftAlngGain.set_padding(2, 5, 8, 12);
  m_FftCtlVBox.pack_start(m_FftAlngGain, Gtk::PACK_SHRINK);
  m_FftAlngRange.add(*m_FftRange);
  m_FftAlngRange.set_padding(2, 5, 8, 12);
  m_FftCtlVBox.pack_start(m_FftAlngRange, Gtk::PACK_SHRINK);
  m_FftCtlVBox.pack_start(m_FftHold, Gtk::PACK_EXPAND_PADDING);
  
  m_FftAlignInner.add(m_FftCtlVBox);
  m_FftAlignInner.set_padding(25, 8, 6, 6);
  m_FftBox = Gtk::manage(new SideChainBox("   FFT ", 10));
  m_FftBox->add(m_FftAlignInner);
  m_FftAlign.set_padding(0, 3, 0, 0);
  m_FftAlign.add(*m_FftBox);
  
  m_dBScaleBox.pack_start(m_dB10Scale, Gtk::PACK_EXPAND_PADDING);
  m_dBScaleBox.pack_start(m_dB25Scale, Gtk::PACK_EXPAND_PADDING);
  m_dBScaleBox.pack_start(m_dB50Scale, Gtk::PACK_EXPAND_PADDING);
  
  m_dBScaleAlignInner.add(m_dBScaleBox);
  m_dBScaleAlignInner.set_padding(25, 8, 6, 6);  
  m_dBScaleFrame = Gtk::manage(new SideChainBox(" Range ", 10));
  m_dBScaleFrame->add(m_dBScaleAlignInner);
  m_dBScaleAlign.set_padding(0, 3, 0, 0);
  m_dBScaleAlign.add(*m_dBScaleFrame);
  
  //Stereo mode pack
  if(m_iNumOfChannels == 2)
  {
    m_LRStereoMode.set_active(true);
    m_MSStereoMode.set_active(false);
    m_StereoBox.pack_start(m_LRStereoMode, Gtk::PACK_EXPAND_PADDING);
    m_StereoBox.pack_start(m_MSStereoMode, Gtk::PACK_EXPAND_PADDING);
    m_StereoInnerAlng.add(m_StereoBox);
    m_StereoInnerAlng.set_padding(25, 8, 6, 6);  
    m_MidSideBox = Gtk::manage(new SideChainBox(" Mode ", 10));
    m_MidSideBox->add(m_StereoInnerAlng);
    m_StereAlng.set_padding(0, 3, 0, 0);
    m_StereAlng.add(*m_MidSideBox);
  }
  
  //dB Scale & Fft Ctl box packing
  if(m_iNumOfChannels == 2)
  {
    m_FftdBBox.pack_start(m_StereAlng,Gtk::PACK_SHRINK);
  }
  m_FftdBBox.pack_start(m_dBScaleAlign,Gtk::PACK_SHRINK);
  m_FftdBBox.pack_start(m_FftAlign,Gtk::PACK_SHRINK);
  
  m_Bode = Gtk::manage(new PlotEQCurve(m_iNumOfBands, m_iNumOfChannels));
  
  m_BandBox.set_spacing(0);
  m_BandBox.set_homogeneous(true);
  m_BandCtlArray = new BandCtl*[m_iNumOfBands];
  
  for (int i = 0; i< m_iNumOfBands; i++)
  {
    m_BandCtlArray[i] = Gtk::manage(new BandCtl(i, &m_bMutex, m_bundlePath.c_str(), m_iNumOfChannels == 2));
    m_BandBox.pack_start(*m_BandCtlArray[i], Gtk::PACK_SHRINK);
    m_BandCtlArray[i] -> signal_changed().connect( sigc::mem_fun(*this, &EqMainWindow::onBandChange));
    m_BandCtlArray[i] -> signal_band_selected().connect( sigc::mem_fun(*this, &EqMainWindow::onBandCtlSelectBand));
    m_BandCtlArray[i] -> signal_band_unselected().connect( sigc::mem_fun(*this, &EqMainWindow::onBandCtlUnselectBand));
    m_BandCtlArray[i] -> signal_mid_side_changed().connect( sigc::mem_fun(*this, &EqMainWindow::onBandCtlMidSideChanged));
  }

  //Bode plot layout
  m_PlotBox.set_spacing(0);
  m_PlotBox.pack_start(*m_Bode);
  m_PlotBox.pack_start(m_FftdBBox,Gtk::PACK_SHRINK);

  //Box layout
  m_ABFlatBox.set_homogeneous(false);
  m_ABFlatBox.pack_start(m_BypassAlign,Gtk::PACK_SHRINK);
  m_ABFlatBox.pack_start(m_ButtonAAlign,Gtk::PACK_SHRINK);
  m_ABFlatBox.pack_start(*image_logo_center);
  m_ABFlatBox.pack_start(m_FlatAlign,Gtk::PACK_SHRINK);
  m_ABFlatBox.pack_start(m_LoadAlign,Gtk::PACK_SHRINK);
  m_ABFlatBox.pack_start(m_SaveAlign,Gtk::PACK_SHRINK);
  m_LoadButton.show();
  m_SaveButton.show();
  m_LoadAlign.show();
  m_SaveAlign.show();
  
  m_CurveBypassBandsBox.pack_start(m_PlotBox ,Gtk::PACK_SHRINK);
  m_CurveBypassBandsBox.pack_start(m_ABFlatBox ,Gtk::PACK_SHRINK);
  m_CurveBypassBandsBox.pack_start(m_BandBox ,Gtk::PACK_SHRINK);

  m_InGainBox.pack_start(*m_VuMeterIn, Gtk::PACK_EXPAND_WIDGET);
  m_InGainBox.pack_start(*m_GainFaderIn, Gtk::PACK_SHRINK);
  
  m_OutGainBox.pack_start(*m_VuMeterOut, Gtk::PACK_EXPAND_WIDGET);
  m_OutGainBox.pack_start(*m_GainFaderOut, Gtk::PACK_SHRINK);

  m_GainEqBox.pack_start(m_CurveBypassBandsBox, Gtk::PACK_SHRINK);
  m_GainEqBox.pack_start(m_InGainBox, Gtk::PACK_SHRINK);
  m_GainEqBox.pack_start(m_OutGainBox, Gtk::PACK_SHRINK);

  m_GainEqBox.set_spacing(2);
  m_MainBox.pack_start(m_GainEqBox);
  m_MainBox.set_spacing(0);

  image_logo_center->show();
 
  m_MainWidgetAlign.add(m_MainBox);
  add(m_MainWidgetAlign);
  m_MainWidgetAlign.show();
  
  //Add some tooltips
  m_AButton.set_tooltip_text("A/B eq comparation");
  m_BypassButton.set_tooltip_text("Enable/Disable the equalizer");
  m_FlatButton.set_tooltip_text("Reset all values to default");
  m_GainFaderIn->set_tooltip_text("Adjust the input gain");
  m_GainFaderOut->set_tooltip_text("Adjust the output gain");
  m_LoadButton.set_tooltip_text("Load curve from file");
  m_SaveButton.set_tooltip_text("Save curve to file");
  m_dB10Scale.set_tooltip_text("Change plot range to 10 dB");
  m_dB25Scale.set_tooltip_text("Change plot range to 25 dB");
  m_dB50Scale.set_tooltip_text("Change plot range to 50 dB");
  
  //connect signals
  m_BypassButton.signal_clicked().connect(sigc::mem_fun(*this, &EqMainWindow::onButtonBypass));
  m_AButton.signal_clicked().connect( sigc::mem_fun(*this, &EqMainWindow::onButtonA));
  m_FlatButton.signal_clicked().connect( sigc::mem_fun(*this, &EqMainWindow::onButtonFlat));
  
  m_GainFaderIn->signal_changed().connect(sigc::mem_fun(*this, &EqMainWindow::onInputGainChange));
  m_GainFaderOut->signal_changed().connect(sigc::mem_fun(*this, &EqMainWindow::onOutputGainChange));
  
  m_Bode->signal_changed().connect(sigc::mem_fun(*this, &EqMainWindow::onCurveChange));
  m_Bode->signal_enabled().connect(sigc::mem_fun(*this, &EqMainWindow::onCurveBandEnable));
  m_Bode->signal_selected().connect(sigc::mem_fun(*this, &EqMainWindow::onBodeSelectBand));
  m_Bode->signal_unselected().connect(sigc::mem_fun(*this, &EqMainWindow::onBodeUnselectBand));
  Glib::signal_timeout().connect( sigc::mem_fun(*this, &EqMainWindow::on_timeout), TIMER_VALUE_MS);
  m_SaveButton.signal_clicked().connect( sigc::mem_fun(*this, &EqMainWindow::saveToFile));
  m_LoadButton.signal_clicked().connect( sigc::mem_fun(*this, &EqMainWindow::loadFromFile));
  
  //FFT Control
  m_FftRtaActive.signal_clicked().connect( sigc::mem_fun(*this, &EqMainWindow::onButtonFftRta));
  m_FftSpecActive.signal_clicked().connect( sigc::mem_fun(*this, &EqMainWindow::onButtonFftSpc));
  m_FftHold.signal_press().connect( sigc::mem_fun(*this, &EqMainWindow::onHoldFft_press));
  m_FftHold.signal_release().connect( sigc::mem_fun(*this, &EqMainWindow::onHoldFft_release)); 
  m_FftGain->signal_changed().connect(sigc::mem_fun(*this, &EqMainWindow::onFftGainScale));
  m_FftRange->signal_changed().connect(sigc::mem_fun(*this, &EqMainWindow::onFftRangeScale));
  
  //dB Scale controlrs
  m_Bode->setPlotdBRange(25.0);
  m_dB25Scale.set_active(true);
  m_dB10Scale.signal_clicked().connect( sigc::mem_fun(*this, &EqMainWindow::onDbScale10Changed));
  m_dB25Scale.signal_clicked().connect( sigc::mem_fun(*this, &EqMainWindow::onDbScale25Changed));
  m_dB50Scale.signal_clicked().connect( sigc::mem_fun(*this, &EqMainWindow::onDbScale50Changed));
  
  //MidSide Mode Selector
  if(m_iNumOfChannels == 2)
  {
    m_LRStereoMode.signal_clicked().connect( sigc::mem_fun(*this, &EqMainWindow::onLeftRightModeSelected));
    m_MSStereoMode.signal_clicked().connect( sigc::mem_fun(*this, &EqMainWindow::onMidSideModeSelected));
  }
  
  //Load the EQ Parameters objects, the params for A curve will be loaded by host acording previous session plugin state
  m_AParams = new EqParams(m_iNumOfBands);
  m_BParams = new EqParams(m_iNumOfBands);
  m_AParams->loadFromTtlFile(m_pluginUri.c_str());
  m_BParams->loadFromTtlFile(m_pluginUri.c_str());   
  m_CurParams = m_AParams;

  //Set cutom theme color:
  Gdk::Color m_WinBgColor;
  SetWidgetColors m_WidgetColors;
  
}

EqMainWindow::~EqMainWindow()
{
  //Send FFT_OFF to DSP
  sendAtomFftOn(false);

  delete image_logo_center;
  delete m_AParams;
  delete m_BParams;
  delete m_GainFaderIn;
  delete m_GainFaderOut;
  delete m_VuMeterIn;
  delete m_VuMeterOut;
  delete m_Bode;  
  delete[] m_port_event_Curve_Gain;
  delete[] m_port_event_Curve_Freq;
  delete[] m_port_event_Curve_Q;
  delete[] m_port_event_Curve_Type;
  delete[] m_port_event_Curve_Enable;
  delete m_FftGain;
  delete m_FftRange;
  delete m_FftBox;

  if(m_iNumOfChannels == 2)
  {
    delete m_MidSideBox;
  }

  for(int i = 0; i < m_iNumOfBands; i++)
  {
    delete m_BandCtlArray[i];
  }
  delete[] m_BandCtlArray;
}

void EqMainWindow::request_sample_rate()
{
  //Request Sample rate
  int AtomPortNumber = PORT_OFFSET + 2*m_iNumOfChannels + 5*m_iNumOfBands + 2*m_iNumOfChannels + 1; 
  uint8_t obj_buf[64];
  lv2_atom_forge_set_buffer(&forge, obj_buf, sizeof(obj_buf)); 
  LV2_Atom_Forge_Frame frame;
  LV2_Atom* msg = (LV2_Atom*)lv2_atom_forge_object(&forge, &frame, 0, uris.atom_sample_rate_request);
  lv2_atom_forge_pop(&forge, &frame); 
  write_function(controller, AtomPortNumber, lv2_atom_total_size(msg), uris.atom_eventTransfer, msg);
}


//Timer to redraw all widgets in case of host port events
bool EqMainWindow::on_timeout()
{ 
  if(m_port_event_Bypass)
  {
    m_port_event_Bypass = false;
    m_BypassButton.set_active(m_bypassValue > 0.5f ? false : true);
    m_Bode->setBypass(m_bypassValue > 0.5f ? true : false); 
  }

  if(m_port_event_InGain)
  {
    m_port_event_InGain = false;
    m_GainFaderIn->set_value((double)m_CurParams->getInputGain());
  }
  
  if(m_port_event_OutGain)
  {
    m_port_event_OutGain = false;
    m_GainFaderOut->set_value((double)m_CurParams->getOutputGain());
  }
  
  if(m_port_event_Curve)
  {
    m_port_event_Curve = false;
    for(int i = 0; i < m_iNumOfBands; i++)
    {      
      if(m_port_event_Curve_Gain[i])
      {
	m_port_event_Curve_Gain[i] = false;
	m_BandCtlArray[i]->setGain(m_CurParams->getBandGain(i));
        m_Bode->setBandGain(i, m_CurParams->getBandGain(i)); 
      }
      if(m_port_event_Curve_Freq[i])
      {
	m_port_event_Curve_Freq[i] = false;
	m_BandCtlArray[i]->setFreq(m_CurParams->getBandFreq(i));
        m_Bode->setBandFreq(i, m_CurParams->getBandFreq(i));
      }
      if(m_port_event_Curve_Q[i])
      {
	m_port_event_Curve_Q[i] = false;
	m_BandCtlArray[i]->setQ(m_CurParams->getBandQ(i));
        m_Bode->setBandQ(i, m_CurParams->getBandQ(i));
      }
      if(m_port_event_Curve_Enable[i])
      {
	m_port_event_Curve_Enable[i] = false;
	m_BandCtlArray[i]->setEnabled(m_CurParams->getBandEnabled(i));
        m_Bode->setBandEnable(i, m_CurParams->getBandEnabled(i));
      }    
      if(m_port_event_Curve_Type[i])
      {
	m_port_event_Curve_Type[i] = false;
	m_BandCtlArray[i]->setFilterType(m_CurParams->getBandType(i));
        m_Bode->setBandType(i, m_CurParams->getBandType(i)); 
      }
    }
  } 
  return true;
}

void EqMainWindow::changeAB(EqParams *toBeCurrent)
{ 
  m_CurParams = toBeCurrent;
  
  //Reload All data
  m_GainFaderIn->set_value((double)m_CurParams->getInputGain());
  m_GainFaderOut->set_value((double)m_CurParams->getOutputGain());

   //Write to LV2 port
   float aux;
   aux =(float) m_GainFaderIn->get_value();
   write_function(controller, EQ_INGAIN, sizeof(float), 0, &aux);
   aux =(float) m_GainFaderOut->get_value();
   write_function(controller, EQ_OUTGAIN, sizeof(float), 0, &aux);
  
  //Reset the Curve Plot
  m_Bode->resetCurve();
  
  //changeAB action will overwrite Q values by defaults to keep the values we use this var
  float usedQ;
  
  for(int i = 0; i < m_iNumOfBands; i++)
  {
    usedQ = m_CurParams->getBandQ(i);
    //It's very important to set de values for bandCtl and bode object.
    //Because if there are some value that does not change between A/B curves this could be not correctly updated
    m_BandCtlArray[i]->setFreq(m_CurParams->getBandFreq(i));    
    m_BandCtlArray[i]->setGain(m_CurParams->getBandGain(i));   
    m_BandCtlArray[i]->setEnabled(m_CurParams->getBandEnabled(i));
    m_BandCtlArray[i]->setFilterType(m_CurParams->getBandType(i)); 
    m_BandCtlArray[i]->setQ(usedQ);
    m_CurParams->setBandQ(i, usedQ);
    
    m_Bode->setBandGain(i, m_CurParams->getBandGain(i));
    m_Bode->setBandFreq(i, m_CurParams->getBandFreq(i));
    m_Bode->setBandQ(i, m_CurParams->getBandQ(i));
    m_Bode->setBandEnable(i, m_CurParams->getBandEnabled(i));
    m_Bode->setBandType(i, m_CurParams->getBandType(i));
    
    //Write to LV2 ports
    aux = m_CurParams->getBandGain(i);
    write_function(controller, i + PORT_OFFSET + 2*m_iNumOfChannels, sizeof(float), 0, &aux); //Gain
    aux = m_CurParams->getBandFreq(i);
    write_function(controller, i + PORT_OFFSET + 2*m_iNumOfChannels + m_iNumOfBands, sizeof(float), 0, &aux); //Freq
    aux = m_CurParams->getBandQ(i);
    write_function(controller, i + PORT_OFFSET + 2*m_iNumOfChannels + 2*m_iNumOfBands, sizeof(float), 0, &aux); //Q
    aux = m_CurParams->getBandEnabled(i);
    write_function(controller, i + PORT_OFFSET + 2*m_iNumOfChannels + 4*m_iNumOfBands, sizeof(float), 0, &aux); //Enable
    aux = m_CurParams->getBandType(i);
    write_function(controller, i + PORT_OFFSET + 2*m_iNumOfChannels + 3*m_iNumOfBands, sizeof(float), 0, &aux); //Filter type
  }
}

void EqMainWindow::onButtonA()
{
  if(m_AButton.get_active())
  {
    changeAB(m_AParams);
  }
  else
  {
     changeAB(m_BParams);
  }
}

void EqMainWindow::onButtonFlat()
{ 
  //Popup a waring message
  Gtk::MessageDialog dialog((Gtk::Window&)(*this->get_toplevel()),"This will flat the current curve, are you sure?",
          false /* use_markup */, Gtk::MESSAGE_QUESTION,
          Gtk::BUTTONS_OK_CANCEL);

  if(dialog.run() == Gtk::RESPONSE_OK)loadEqParams();
}

void EqMainWindow::onButtonBypass()
{
  #ifdef PRINT_DEBUG_INFO
    std::cout<<"onButtonBypass... ";
  #endif
 
  m_Bode->setBypass(!m_BypassButton.get_active());
  if (m_BypassButton.get_active())
  {
    m_bypassValue = 0.0f;
  }
  else
  {
    m_bypassValue = 1.0f;
  }

  write_function(controller, EQ_BYPASS, sizeof(float), 0, &m_bypassValue);

  #ifdef PRINT_DEBUG_INFO
    std::cout<<"  Return"<<std::endl;
  #endif
}

void EqMainWindow::onBandChange(int iBand, int iField, float fValue)
{

  #ifdef PRINT_DEBUG_INFO
    std::cout<<"onBandChange...  Band = "<<iBand<<" Field = "<<iField;
  #endif
  
  switch(iField)
  {
    case GAIN_TYPE: 
      write_function(controller, iBand + PORT_OFFSET + 2*m_iNumOfChannels, sizeof(float), 0, &fValue);
      m_CurParams->setBandGain(iBand, fValue);
      m_Bode->setBandGain(iBand, fValue);
      break;
      
    case FREQ_TYPE:
      write_function(controller, iBand + PORT_OFFSET + 2*m_iNumOfChannels + m_iNumOfBands, sizeof(float), 0, &fValue);
      m_CurParams->setBandFreq(iBand, fValue);
      m_Bode->setBandFreq(iBand, fValue);
      break;
      
    case Q_TYPE:
      write_function(controller, iBand + PORT_OFFSET + 2*m_iNumOfChannels + 2*m_iNumOfBands, sizeof(float), 0, &fValue);
      m_CurParams->setBandQ(iBand, fValue);
      m_Bode->setBandQ(iBand, fValue);
      break;
      
    case FILTER_TYPE:
      write_function(controller, iBand + PORT_OFFSET + 2*m_iNumOfChannels + 3*m_iNumOfBands, sizeof(float), 0, &fValue);
      m_CurParams->setBandType(iBand, (int) fValue);
      m_Bode->setBandType(iBand, (int) fValue);
      break;
      
    case ONOFF_TYPE:
      int ival = (int) fValue;
      if(m_iNumOfChannels == 2)
      {
        switch(m_BandCtlArray[iBand]->getStereoState())
        {
          case BandCtl::DUAL:
            ival |= 0x00;
            break;
            
          case BandCtl::ML:
            ival |= 0x02;
            break;
            
          case BandCtl::SR:
            ival |= 0x04;
            break;
        }
      }
      float fVal = (float)ival;
      write_function(controller, iBand + PORT_OFFSET + 2*m_iNumOfChannels + 4*m_iNumOfBands, sizeof(float), 0, &fVal);
      m_CurParams->setBandEnabled(iBand, (fValue > 0.5)); 
      m_Bode->setBandEnable(iBand, (fValue > 0.5));
      break;  
  }
  
  #ifdef PRINT_DEBUG_INFO
    std::cout<<"Return"<<std::endl;
  #endif
}

void EqMainWindow::onBandCtlMidSideChanged(int band)
{ 
  int ival = m_CurParams->getBandEnabled(band) ? 1 : 0;
  if(m_iNumOfChannels == 2)
  {
    switch(m_BandCtlArray[band]->getStereoState())
    {
      case BandCtl::DUAL:
        ival |= 0x00;
        m_Bode->setStereoState(band, PlotEQCurve::DUAL);
        break;
        
      case BandCtl::ML:
        ival |= 0x02;
        m_Bode->setStereoState(band, PlotEQCurve::ML);
        break;
        
      case BandCtl::SR:
        ival |= 0x04;
        m_Bode->setStereoState(band, PlotEQCurve::SR);
        break;
    }
  }
  float fValue = (float)ival;
  write_function(controller, band + PORT_OFFSET + 2*m_iNumOfChannels + 4*m_iNumOfBands, sizeof(float), 0, &fValue);
  
  //TODO maybe I want to use AB comparation with MID-SIDE????
  //m_CurParams->setBandEnabled(iBand, (fValue > 0.5)); 
}


void EqMainWindow::onInputGainChange()
{

  #ifdef PRINT_DEBUG_INFO
    std::cout<<"onInputGainChange... ";
  #endif
  
  //Save data Change
  m_CurParams->setInputGain((float) m_GainFaderIn->get_value());  
    
  //Write to LV2 port
  float aux;
  aux = (float) m_GainFaderIn->get_value();
  write_function(controller, EQ_INGAIN, sizeof(float), 0, &aux);
  
  #ifdef PRINT_DEBUG_INFO
    std::cout<<"Return"<<std::cout;
  #endif
}

void EqMainWindow::onOutputGainChange()
{
  #ifdef PRINT_DEBUG_INFO
    std::cout<<"onOutputGainChange... ";
  #endif
  
  //Save data Change
  m_CurParams->setOutputGain((float) m_GainFaderOut->get_value());  
  
  //Write to LV2 port
  float aux;
  aux = (float) m_GainFaderOut->get_value();
  write_function(controller, EQ_OUTGAIN, sizeof(float), 0, &aux);
   
  #ifdef PRINT_DEBUG_INFO
    std::cout<<"Return"<<std::cout;
  #endif
}

void EqMainWindow::onCurveChange(int band_ix, float Gain, float Freq, float Q)
{
  m_BandCtlArray[band_ix]->setGain(Gain);
  m_BandCtlArray[band_ix]->setFreq(Freq);
  m_BandCtlArray[band_ix]->setQ(Q);
  
  //Write to LV2 plugin ports
  //Gain
  write_function(controller, band_ix + PORT_OFFSET + 2*m_iNumOfChannels, sizeof(float), 0, &Gain);
  m_CurParams->setBandGain(band_ix, Gain);

  //Freq
  write_function(controller, band_ix + PORT_OFFSET + 2*m_iNumOfChannels + m_iNumOfBands, sizeof(float), 0, &Freq);
  m_CurParams->setBandFreq(band_ix, Freq);

  //Q
  write_function(controller, band_ix + PORT_OFFSET + 2*m_iNumOfChannels + 2*m_iNumOfBands, sizeof(float), 0, &Q);
  m_CurParams->setBandQ(band_ix, Q);

}

void EqMainWindow::onCurveBandEnable(int band_ix, bool IsEnabled)
{
  m_BandCtlArray[band_ix]->setEnabled(IsEnabled);

  int ival = IsEnabled ? 1 : 0;
  if(m_iNumOfChannels == 2)
  {
    switch(m_BandCtlArray[band_ix]->getStereoState())
    {
      case BandCtl::DUAL:
        ival |= 0x00;
        break;
        
      case BandCtl::ML:
        ival |= 0x02;
        break;
        
      case BandCtl::SR:
        ival |= 0x04;
        break;
    }
  }
  float fVal = (float)ival;
  write_function(controller, band_ix + PORT_OFFSET + 2*m_iNumOfChannels + 4*m_iNumOfBands, sizeof(float), 0, &fVal);
  m_CurParams->setBandEnabled(band_ix, IsEnabled); 
}

void EqMainWindow::onBodeSelectBand(int band)
{
  m_BandCtlArray[band]->glowBand(true);
}

void EqMainWindow::onBodeUnselectBand()
{
  for(int i = 0; i < m_iNumOfBands; i++)
  {
     m_BandCtlArray[i]->glowBand(false);
  }
}

void EqMainWindow::onBandCtlSelectBand(int band)
{
  m_Bode->unglowBands();
  m_Bode->glowBand(band);
}

void EqMainWindow::onBandCtlUnselectBand()
{
  m_Bode->unglowBands();
}

void EqMainWindow::loadEqParams()
{
  m_CurParams->loadFromTtlFile(m_pluginUri.c_str());
  changeAB(m_CurParams);
}

void EqMainWindow::saveToFile()
{
  Gtk::FileChooserDialog *fileChosser = new Gtk::FileChooserDialog("Save curve to file", Gtk::FILE_CHOOSER_ACTION_SAVE);
  fileChosser->add_button("Save", Gtk::RESPONSE_ACCEPT);
  fileChosser->add_button("Cancel", Gtk::RESPONSE_CANCEL);
  fileChosser->set_current_folder( getenv("HOME"));
  fileChosser->set_select_multiple(false);
  fileChosser->set_do_overwrite_confirmation(true);
  
  //File filter
  Gtk::FileFilter filter;
  std::stringstream ss;
  ss << "EQ" << m_iNumOfBands << "Q Curve File";
  filter.set_name(ss.str());
  ss.str( std::string() );
  ss.clear();
  ss << "*.eq" << m_iNumOfBands << "q";
  filter.add_pattern(ss.str());
  fileChosser->add_filter(filter);
  
  if (fileChosser->run() == Gtk::RESPONSE_ACCEPT)
  { 
    ss.str( std::string() );
    ss.clear();
    ss << fileChosser->get_filename() << ".eq" << m_iNumOfBands << "q";   
    m_CurParams->saveToFile(ss.str().c_str());
  }
  delete fileChosser;
}

void EqMainWindow::loadFromFile()
{
  Gtk::FileChooserDialog *fileChosser = new Gtk::FileChooserDialog("Load curve from file", Gtk::FILE_CHOOSER_ACTION_OPEN);
  fileChosser->add_button("Load", Gtk::RESPONSE_ACCEPT);
  fileChosser->add_button("Cancel", Gtk::RESPONSE_CANCEL);
  fileChosser->set_current_folder( getenv("HOME"));
  fileChosser->set_select_multiple(false);
  
  //File filter
  Gtk::FileFilter filter;
  std::stringstream ss;
  ss << "EQ" << m_iNumOfBands << "Q Curve File";
  filter.set_name(ss.str());
  ss.str( std::string() );
  ss.clear();
  ss << "*.eq" << m_iNumOfBands << "q";
  filter.add_pattern(ss.str());
  fileChosser->add_filter(filter);
  
  if (fileChosser->run() == Gtk::RESPONSE_ACCEPT)
  {     
      if (m_CurParams->loadFromFile(fileChosser->get_filename().c_str()))
      {
	changeAB(m_CurParams);
      }
      else //This is the error case
      {
      	//Popup a error message
	Gtk::MessageDialog dialog((Gtk::Window&)(*this->get_toplevel()),"Error loading curve file, number of bands does not match or this is not a valid eq10q file.\n\rNothing is loaded.",
        false /* use_markup */, Gtk::MESSAGE_ERROR,
        Gtk::BUTTONS_OK);
	dialog.run();
      }
  }
  
  delete fileChosser;
}

void EqMainWindow::onButtonFftRta()
{ 
  sendAtomFftOn( m_FftRtaActive.get_active() );
  m_Bode->setFftActive(m_FftRtaActive.get_active(), false);
  if(m_FftRtaActive.get_active())
  {
    m_FftSpecActive.set_active(false);
  }
}

void EqMainWindow::onButtonFftSpc()
{
  sendAtomFftOn( m_FftSpecActive.get_active() );
  m_Bode->setFftActive(m_FftSpecActive.get_active(), true);
  if(m_FftSpecActive.get_active())
  {
    m_FftRtaActive.set_active(false);
  }
}


void EqMainWindow::onHoldFft_press()
{
  m_Bode->setFftHold(true);
}

void EqMainWindow::onHoldFft_release()
{
  m_Bode->setFftHold(false);
}

void EqMainWindow::sendAtomFftOn(bool fft_activated)
{
  int AtomPortNumber = PORT_OFFSET + 2*m_iNumOfChannels + 5*m_iNumOfBands + 2*m_iNumOfChannels + 1; 
  uint8_t obj_buf[64];
  lv2_atom_forge_set_buffer(&forge, obj_buf, sizeof(obj_buf)); 
  LV2_Atom_Forge_Frame frame;
  LV2_Atom* msg = (LV2_Atom*)lv2_atom_forge_object(&forge, &frame, 0, fft_activated? uris.atom_fft_on : uris.atom_fft_off);
  lv2_atom_forge_pop(&forge, &frame); 
  write_function(controller, AtomPortNumber, lv2_atom_total_size(msg), uris.atom_eventTransfer, msg);
}

void EqMainWindow::onFftGainScale()
{
  m_Bode->setFftGain(m_FftGain->get_value());
}

void EqMainWindow::onFftRangeScale()
{
  m_Bode->setFftRange(m_FftRange->get_value());
}

void EqMainWindow::onDbScale10Changed()
{
  m_dB10Scale.set_active(true);
  m_dB25Scale.set_active(false);
  m_dB50Scale.set_active(false);
  m_Bode->setPlotdBRange(10.0);
}

void EqMainWindow::onDbScale25Changed()
{
  m_dB10Scale.set_active(false);
  m_dB25Scale.set_active(true);
  m_dB50Scale.set_active(false);
  m_Bode->setPlotdBRange(25.0);
}

void EqMainWindow::onDbScale50Changed()
{
  m_dB10Scale.set_active(false);
  m_dB25Scale.set_active(false);
  m_dB50Scale.set_active(true);
  m_Bode->setPlotdBRange(50.0);
}

void EqMainWindow::setStereoMode(bool isMidSide)
{ 
  m_MSStereoMode.set_active(isMidSide);
  m_LRStereoMode.set_active(!isMidSide);
  for(int i = 0; i < m_iNumOfBands; i++)
  {
    m_BandCtlArray[i]->setStereoMode(isMidSide);
  }
  int PortNumber = PORT_OFFSET + 2*m_iNumOfChannels + 5*m_iNumOfBands + 2*m_iNumOfChannels + 2; 
  float fValue = isMidSide ? 1.0f : 0.0f;
  write_function(controller, PortNumber, sizeof(float), 0, &fValue);
}


void EqMainWindow::onLeftRightModeSelected()
{
  setStereoMode(false);
}

void EqMainWindow::onMidSideModeSelected()
{
  setStereoMode(true);
}
