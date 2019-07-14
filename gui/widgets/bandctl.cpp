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

#include "filter.h"
#include "colors.h"
#include "guiconstants.h"
#include "bandctl.h"
#include "toggle_button.h" //To draw the LED using a static function

#include <gdkmm.h>
#include <iostream>
#include <iomanip>
#include <cmath>

#define OUTER_BORDER 1.5
#define FONT_SIZE 11
#define ACCELERATION 15.0
#define SCROLL_EVENT_PERCENT 0.005
#define LPF_ICON_FILE "combopix/lpf.png"
#define HPF_ICON_FILE "combopix/hpf.png"
#define LOSHEL_ICON_FILE "combopix/loshelf.png"
#define HISHEL_ICON_FILE "combopix/hishelf.png"
#define PEAK_ICON_FILE "combopix/peak.png"
#define NOTCH_ICON_FILE "combopix/notch.png"

BandCtl::BandCtl( const int iBandNum, bool *bSemafor, const char* bundlepath, bool isStereo):
m_bBtnInitialized(false),
m_TypePopUp(0),
m_iBandNum(iBandNum),
m_bBandIsEnabled(false),
m_budlepath(bundlepath),
m_iAntValueX(0),
m_iAntValueY(0),
m_HpfLpf_slope(0),
m_bGlowBand(false),
m_bIsStereoPlugin(isStereo)
{
  //Init button values to something
  m_GainBtn.value = 0.0f;
  m_FreqBtn.value = 1e3f;
  m_QBtn.value = 2.0f;
  m_GainBtn.units = "dB";
  m_FreqBtn.units = "Hz";
  m_QBtn.units = "Q";
  
  m_FilterType = int2FilterType(0.0f);
  m_BandTitle = Glib::ustring::compose("Band %1", m_iBandNum + 1);
  m_Color  = Gdk::Color(bandColorLUT[m_iBandNum]);
  
  //Load ComboBox images
  m_img_ptr_lpf =  Gdk::Pixbuf::create_from_file(m_budlepath + "/" + std::string(LPF_ICON_FILE));
  m_img_ptr_hpf =  Gdk::Pixbuf::create_from_file(m_budlepath + "/" + std::string(HPF_ICON_FILE));
  m_img_ptr_loshelf =  Gdk::Pixbuf::create_from_file(m_budlepath + "/" + std::string(LOSHEL_ICON_FILE));
  m_img_ptr_hishelf =  Gdk::Pixbuf::create_from_file(m_budlepath + "/" + std::string(HISHEL_ICON_FILE));
  m_img_ptr_peak =  Gdk::Pixbuf::create_from_file(m_budlepath + "/" + std::string(PEAK_ICON_FILE));
  m_img_ptr_notch =  Gdk::Pixbuf::create_from_file(m_budlepath + "/" + std::string(NOTCH_ICON_FILE));
  
  m_FilterType = PEAK;
  loadTypeImg();
  set_size_request(46 + m_image_surface_ptr->get_width(), ( m_bIsStereoPlugin ? 80 : 65) + m_image_surface_ptr->get_height()); 
 
  //Fill Filter Type popup menu
  m_TypePopUp = new Gtk::Menu();
  
  icon_lpf = new Gtk::Image(m_img_ptr_lpf);
  icon_hpf = new Gtk::Image(m_img_ptr_hpf);
  icon_loShel = new Gtk::Image(m_img_ptr_loshelf);
  icon_hiShel = new Gtk::Image(m_img_ptr_hishelf);
  icon_peak = new Gtk::Image(m_img_ptr_peak);
  icon_notch = new Gtk::Image(m_img_ptr_notch);
  
  itm_lpf = new Gtk::ImageMenuItem(*icon_lpf,"Low pass");
  itm_hpf = new Gtk::ImageMenuItem(*icon_hpf,"High pass"); 
  itm_loShel = new Gtk::ImageMenuItem(*icon_loShel,"Low Shelf"); 
  itm_hiShel = new Gtk::ImageMenuItem(*icon_hiShel,"High Shelf"); 
  itm_peak = new Gtk::ImageMenuItem(*icon_peak,"Peak"); 
  itm_notch = new Gtk::ImageMenuItem(*icon_notch,"Notch"); 
  
  //Allow this widget to get keyboard focus
  set_flags(Gtk::CAN_FOCUS);
  set_can_focus(true);
  
  itm_lpf->set_always_show_image(true);
  itm_hpf->set_always_show_image(true);
  itm_loShel->set_always_show_image(true);
  itm_hiShel->set_always_show_image(true);
  itm_peak->set_always_show_image(true);
  itm_notch->set_always_show_image(true);
    
  itm_lpf->signal_activate().connect(sigc::mem_fun(*this, &BandCtl::on_menu_lpf));
  itm_hpf->signal_activate().connect(sigc::mem_fun(*this, &BandCtl::on_menu_hpf));
  itm_loShel->signal_activate().connect(sigc::mem_fun(*this, &BandCtl::on_menu_loshelf));
  itm_hiShel->signal_activate().connect(sigc::mem_fun(*this, &BandCtl::on_menu_hishelf));
  itm_peak->signal_activate().connect(sigc::mem_fun(*this, &BandCtl::on_menu_peak));
  itm_notch->signal_activate().connect(sigc::mem_fun(*this, &BandCtl::on_menu_notch));
  m_TypePopUp->signal_hide().connect(sigc::mem_fun(*this, &BandCtl::on_menu_hide));
  signal_focus_out_event().connect(sigc::mem_fun(*this, &BandCtl::on_focus_out_event)); 
  
  m_TypePopUp->append(*itm_lpf);
  m_TypePopUp->append(*itm_hpf);
  m_TypePopUp->append(*itm_loShel);
  m_TypePopUp->append(*itm_hiShel);
  m_TypePopUp->append(*itm_peak);
  m_TypePopUp->append(*itm_notch);
  m_TypePopUp->set_size_request(110 ,-1);
  
  //Setup MidSide button
  m_MidSideBtn.MidSideMode = false;
  m_MidSideBtn.State = DUAL;
  
  show();
  
  //Connect mouse signals
  add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::POINTER_MOTION_MASK | Gdk::SCROLL_MASK | Gdk::LEAVE_NOTIFY_MASK | Gdk::KEY_PRESS_MASK);
  signal_button_press_event().connect(sigc::mem_fun(*this, &BandCtl::on_button_press_event),true);
  signal_button_release_event().connect(sigc::mem_fun(*this, &BandCtl::on_button_release_event),true);
  signal_scroll_event().connect(sigc::mem_fun(*this, &BandCtl::on_scrollwheel_event),true);
  signal_motion_notify_event().connect(sigc::mem_fun(*this, &BandCtl::on_mouse_motion_event),true);
  signal_leave_notify_event().connect(sigc::mem_fun(*this, &BandCtl::on_mouse_leave_widget),true);
  
  signal_key_press_event().connect(sigc::mem_fun(*this, &BandCtl::on_key_press_event)); //TODO
  
  Glib::RefPtr<Gtk::Style> sty =  Gtk::Style::create();  
  sty->set_font(Pango::FontDescription("sans 11px"));
  sty->set_bg(Gtk::STATE_NORMAL, Gdk::Color("#3C3940"));
  sty->set_bg(Gtk::STATE_PRELIGHT, Gdk::Color("#408FC0"));
  sty->set_fg(Gtk::STATE_NORMAL, Gdk::Color("#CDCECE"));
  sty->set_fg(Gtk::STATE_PRELIGHT, Gdk::Color("#161B17"));
  
  
  m_TypePopUp->set_style(sty);
  
  itm_lpf->set_style(sty);
  itm_hpf->set_style(sty);
  itm_loShel->set_style(sty);
  itm_hiShel->set_style(sty);
  itm_peak->set_style(sty);
  itm_notch->set_style(sty);
  
  itm_lpf->get_child()->set_style(sty);
  itm_hpf->get_child()->set_style(sty);
  itm_loShel->get_child()->set_style(sty);
  itm_hiShel->get_child()->set_style(sty);
  itm_peak->get_child()->set_style(sty);
  itm_notch->get_child()->set_style(sty);
}

BandCtl::~BandCtl()
{
    delete m_TypePopUp;   
    
    delete icon_lpf;
    delete icon_hpf;
    delete icon_loShel;
    delete icon_hiShel;
    delete icon_peak;
    delete icon_notch;
    
    delete itm_lpf;
    delete itm_hpf;
    delete itm_loShel;
    delete itm_hiShel;
    delete itm_peak;
    delete itm_notch;
}

void BandCtl::glowBand(bool glow)
{
  m_bGlowBand = glow;
  redraw();
}


void BandCtl::loadTypeImg()
{
   Glib::RefPtr<Gdk::Pixbuf> img_ptr;
   
   switch(m_FilterType)
   {
     case HPF_ORDER_1:
      img_ptr = m_img_ptr_hpf;
      m_HpfLpf_slope = 20;
      m_GainBtn.units = "dB/dec";
     break;
     
     case HPF_ORDER_2:
      img_ptr = m_img_ptr_hpf;
      m_HpfLpf_slope = 40;
      m_GainBtn.units = "dB/dec";
     break;
     
     case HPF_ORDER_3:
      img_ptr = m_img_ptr_hpf;
      m_HpfLpf_slope = 60;
      m_GainBtn.units = "dB/dec";
     break;
     
     case HPF_ORDER_4:
      img_ptr = m_img_ptr_hpf;
      m_HpfLpf_slope = 80;
      m_GainBtn.units = "dB/dec";
     break;
     
     case LPF_ORDER_1:
       img_ptr = m_img_ptr_lpf;
       m_HpfLpf_slope = 20;
       m_GainBtn.units = "dB/dec";
    break;
       
     case LPF_ORDER_2:
       img_ptr = m_img_ptr_lpf;
       m_HpfLpf_slope = 40;
       m_GainBtn.units = "dB/dec";
    break;
    
     case LPF_ORDER_3:
       img_ptr = m_img_ptr_lpf;
       m_HpfLpf_slope = 60;
       m_GainBtn.units = "dB/dec";
    break;
    
     case LPF_ORDER_4:
       img_ptr = m_img_ptr_lpf;
       m_HpfLpf_slope = 80;
       m_GainBtn.units = "dB/dec";
     break;
     
     case HIGH_SHELF:
       img_ptr = m_img_ptr_hishelf;
       m_HpfLpf_slope = 0;
       m_GainBtn.units = "dB";
     break;
     
     case LOW_SHELF:
       img_ptr = m_img_ptr_loshelf;
       m_HpfLpf_slope = 0;
       m_GainBtn.units = "dB";
     break;
     
     case PEAK:
       img_ptr = m_img_ptr_peak;
       m_HpfLpf_slope = 0;
       m_GainBtn.units = "dB";
     break;
     
     case NOTCH:
       img_ptr = m_img_ptr_notch;
       m_HpfLpf_slope = 0;
       m_GainBtn.units = "dB";
     break;
     
     case NOT_SET:
      return;
     break; 
   }

   //Detect Format
  Cairo::Format format = Cairo::FORMAT_RGB24;
  if (img_ptr->get_has_alpha())
  {
    format = Cairo::FORMAT_ARGB32;
  }
  
  // Create a new ImageSurface
  m_image_surface_ptr = Cairo::ImageSurface::create  (format, img_ptr->get_width(), img_ptr->get_height());
  
  // Create the new Context for the ImageSurface
  m_image_context_ptr = Cairo::Context::create (m_image_surface_ptr);     
  
  // Draw the image on the new Context
  Gdk::Cairo::set_source_pixbuf (m_image_context_ptr, img_ptr, 0.0, 0.0);
  m_image_context_ptr->paint();
}


//Data accesors
float BandCtl::getGain(){
  return m_GainBtn.value;
}

float BandCtl::getFreq(){
  return m_FreqBtn.value;
}

float BandCtl::getQ(){
  return m_QBtn.value;
}

float BandCtl::getFilterType(){
  return (float)m_FilterType;
}

bool BandCtl::getEnabled()
{
  return m_bBandIsEnabled;
}

void BandCtl::setGain(float fGain){
  m_GainBtn.value = fGain;
  redraw();
}

void BandCtl::setFreq(float fFreq){
  m_FreqBtn.value = fFreq;
  redraw();
}

void BandCtl::setQ(float fQ){
  m_QBtn.value = fQ;
  redraw();
}

void BandCtl::setFilterType(float fType)
{
  m_FilterType = int2FilterType(fType);
  loadTypeImg();
  redraw();
}

void BandCtl::setEnabled(bool bIsEnabled)
{
  m_bBandIsEnabled = bIsEnabled;
  redraw();
}

BandCtl::signal_ctlBandChanged BandCtl::signal_changed()
{
  return m_bandChangedSignal;
}

BandCtl::signal_BandSelected BandCtl::signal_band_selected()
{
  return m_bandSelectedSignal;
}

BandCtl::signal_BandUnSelected BandCtl::signal_band_unselected()
{
  return m_bandUnSelectedSignal;
}

BandCtl::signal_MidSideChanged BandCtl::signal_mid_side_changed()
{
  return m_midsideChangedSignal;
}


void BandCtl::on_menu_lpf()
{
  m_FilterType = LPF_ORDER_2;
  loadTypeImg();
  m_bandChangedSignal.emit(m_iBandNum, FILTER_TYPE, getFilterType());
  m_bandChangedSignal.emit(m_iBandNum, GAIN_TYPE, m_GainBtn.value);
  m_bandChangedSignal.emit(m_iBandNum, FREQ_TYPE, m_FreqBtn.value);
  m_QBtn.value = HPF_LPF_Q_DEFAULT;
  m_bandChangedSignal.emit(m_iBandNum, Q_TYPE, m_QBtn.value); 
  redraw(); 
}

void BandCtl::on_menu_hpf()
{
  m_FilterType = HPF_ORDER_2;
  loadTypeImg();
  m_bandChangedSignal.emit(m_iBandNum, FILTER_TYPE, getFilterType());
  m_bandChangedSignal.emit(m_iBandNum, GAIN_TYPE, m_GainBtn.value);
  m_bandChangedSignal.emit(m_iBandNum, FREQ_TYPE, m_FreqBtn.value);
  m_QBtn.value = HPF_LPF_Q_DEFAULT;
  m_bandChangedSignal.emit(m_iBandNum, Q_TYPE, m_QBtn.value); 
  redraw();
}

void BandCtl::on_menu_loshelf()
{
  m_FilterType = LOW_SHELF;
  loadTypeImg();
  m_bandChangedSignal.emit(m_iBandNum, FILTER_TYPE, getFilterType());
  m_bandChangedSignal.emit(m_iBandNum, GAIN_TYPE, m_GainBtn.value);
  m_bandChangedSignal.emit(m_iBandNum, FREQ_TYPE, m_FreqBtn.value);
  m_QBtn.value = HIGH_LOW_SHELF_Q_DEFAULT;
  m_bandChangedSignal.emit(m_iBandNum, Q_TYPE, m_QBtn.value); 
  redraw(); 
}

void BandCtl::on_menu_hishelf()
{
  m_FilterType = HIGH_SHELF;
  loadTypeImg();
  m_bandChangedSignal.emit(m_iBandNum, FILTER_TYPE, getFilterType());
  m_bandChangedSignal.emit(m_iBandNum, GAIN_TYPE, m_GainBtn.value);
  m_bandChangedSignal.emit(m_iBandNum, FREQ_TYPE, m_FreqBtn.value);
  m_QBtn.value = HIGH_LOW_SHELF_Q_DEFAULT;
  m_bandChangedSignal.emit(m_iBandNum, Q_TYPE, m_QBtn.value); 
  redraw(); 
}

void BandCtl::on_menu_peak()
{
  m_FilterType = PEAK;
  loadTypeImg();
  m_bandChangedSignal.emit(m_iBandNum, FILTER_TYPE, getFilterType());
  m_bandChangedSignal.emit(m_iBandNum, GAIN_TYPE, m_GainBtn.value);
  m_bandChangedSignal.emit(m_iBandNum, FREQ_TYPE, m_FreqBtn.value);
  m_QBtn.value = PEAK_Q_DEFAULT;
  m_bandChangedSignal.emit(m_iBandNum, Q_TYPE, m_QBtn.value); 
  redraw(); 
}

void BandCtl::on_menu_notch()
{
  m_FilterType = NOTCH;
  loadTypeImg();
  m_bandChangedSignal.emit(m_iBandNum, FILTER_TYPE, getFilterType());
  m_bandChangedSignal.emit(m_iBandNum, GAIN_TYPE, m_GainBtn.value);
  m_bandChangedSignal.emit(m_iBandNum, FREQ_TYPE, m_FreqBtn.value);
  m_QBtn.value = NOTCH_Q_DEFAULT;
  m_bandChangedSignal.emit(m_iBandNum, Q_TYPE, m_QBtn.value); 
  redraw(); 
}

void BandCtl::on_menu_hide()
{
  m_TypeBtn.focus = false;
  m_TypeBtn.pressed = false;
  redraw();
}

//TODO: En ardour4 event de tecla mai arriba
bool BandCtl::on_button_press_event(GdkEventButton* event)
{  
  //Notify band over
  m_bandSelectedSignal.emit(m_iBandNum);
  
  //Check if is a double click or simple
  if(event->button == 1)
  {
    if(event->type == GDK_2BUTTON_PRESS)
    {
      //Double click on the 1st button
      m_GainBtn.text = (m_HpfLpf_slope == 0) & m_bBandIsEnabled & (event->x > m_GainBtn.x0 && event->x < m_GainBtn.x1 && event->y > m_GainBtn.y0 && event->y < m_GainBtn.y1);
      m_FreqBtn.text = m_bBandIsEnabled & (event->x > m_FreqBtn.x0 && event->x < m_FreqBtn.x1 && event->y > m_FreqBtn.y0 && event->y < m_FreqBtn.y1);
      m_QBtn.text = m_bBandIsEnabled & (event->x > m_QBtn.x0 && event->x < m_QBtn.x1 && event->y > m_QBtn.y0 && event->y < m_QBtn.y1);
      if(m_GainBtn.text)
      {
        m_GainBtn.ss.str(""); //Clear stringstream
        m_GainBtn.ss<<std::setprecision(2)<< std::fixed << m_GainBtn.value;
	grab_focus();
        //keyPressEvent = signal_key_press_event().connect(sigc::mem_fun(*this, &BandCtl::on_key_press_event)); //TODO
      }
      else if(m_FreqBtn.text)
      {
        m_FreqBtn.ss.str(""); //Clear stringstream
        m_FreqBtn.ss<<std::setprecision(2)<< std::fixed <<m_FreqBtn.value;
        grab_focus();
        //keyPressEvent = signal_key_press_event().connect(sigc::mem_fun(*this, &BandCtl::on_key_press_event)); //TODO
      }
      else if(m_QBtn.text)
      {
        m_QBtn.ss.str(""); //Clear stringstream
        m_QBtn.ss<<std::setprecision(2)<< std::fixed <<m_QBtn.value;
        grab_focus();
        //keyPressEvent = signal_key_press_event().connect(sigc::mem_fun(*this, &BandCtl::on_key_press_event)); //TODO
      }
      
    }
    else
    {
      m_EnableBtn.pressed = (event->x > m_EnableBtn.x0 && event->x < m_EnableBtn.x1 && event->y > m_EnableBtn.y0 && event->y < m_EnableBtn.y1);
      
      //Simple click on the 1st button
      m_TypeBtn.pressed = m_bBandIsEnabled & (event->x > m_TypeBtn.x0 && event->x < m_TypeBtn.x1 && event->y > m_TypeBtn.y0 && event->y < m_TypeBtn.y1);
      if(m_TypeBtn.pressed)
      {
        m_TypePopUp->popup(event->button, event->time);
        m_TypePopUp->show_all();
      }
 
      m_iAntValueX = event->x;
      m_iAntValueY = event->y;
      m_GainBtn.pressed = m_bBandIsEnabled & (event->x > m_GainBtn.x0 && event->x < m_GainBtn.x1 && event->y > m_GainBtn.y0 && event->y < m_GainBtn.y1);
      m_FreqBtn.pressed = m_bBandIsEnabled & (event->x > m_FreqBtn.x0 && event->x < m_FreqBtn.x1 && event->y > m_FreqBtn.y0 && event->y < m_FreqBtn.y1);
      m_QBtn.pressed = m_bBandIsEnabled & (event->x > m_QBtn.x0 && event->x < m_QBtn.x1 && event->y > m_QBtn.y0 && event->y < m_QBtn.y1);
           
      if(m_bIsStereoPlugin)
      {
        m_MidSideBtn.ML_pressed = m_bBandIsEnabled & (event->x > m_MidSideBtn.Mx && event->x < m_MidSideBtn.Dx && event->y > m_MidSideBtn.y0 && event->y < m_MidSideBtn.y1);
        m_MidSideBtn.Dual_pressed = m_bBandIsEnabled & (event->x > m_MidSideBtn.Dx && event->x < m_MidSideBtn.Sx && event->y > m_MidSideBtn.y0 && event->y < m_MidSideBtn.y1);
        m_MidSideBtn.SR_pressed = m_bBandIsEnabled & (event->x > m_MidSideBtn.Sx && event->x < m_MidSideBtn.x1 && event->y > m_MidSideBtn.y0 && event->y < m_MidSideBtn.y1);
        redraw_MidSide_widget(); 
      }
      
      //Disable Q for LPF1 and LPF2
      m_QBtn.pressed &= !(m_FilterType == LPF_ORDER_1);
      m_QBtn.pressed &= !(m_FilterType == HPF_ORDER_1);
      
      //Disable Gain for NOTCH
      m_GainBtn.pressed &= !(m_FilterType == NOTCH);
    }
  }
  redraw();
  return true;
}

bool BandCtl::on_button_release_event(GdkEventButton* event)
{ 
  //Check for enable button
  if(m_EnableBtn.pressed && (event->x > m_EnableBtn.x0 && event->x < m_EnableBtn.x1 && event->y > m_EnableBtn.y0 && event->y < m_EnableBtn.y1))
  {
    m_bBandIsEnabled = !m_bBandIsEnabled;
    m_bandChangedSignal.emit(m_iBandNum, ONOFF_TYPE, m_bBandIsEnabled);
  }
  
  //Check MidSide buttons
  if(m_bIsStereoPlugin && m_MidSideBtn.ML_pressed && (event->x > m_MidSideBtn.Mx && event->x < m_MidSideBtn.Dx && event->y > m_MidSideBtn.y0 && event->y < m_MidSideBtn.y1))
  {
    m_MidSideBtn.State = ML;
    m_midsideChangedSignal.emit(m_iBandNum);
  }
  if(m_bIsStereoPlugin && m_MidSideBtn.Dual_pressed && (event->x > m_MidSideBtn.Dx && event->x < m_MidSideBtn.Sx && event->y > m_MidSideBtn.y0 && event->y < m_MidSideBtn.y1))
  {
    m_MidSideBtn.State = DUAL;
    m_midsideChangedSignal.emit(m_iBandNum);
  }
  if(m_bIsStereoPlugin && m_MidSideBtn.SR_pressed &&  (event->x > m_MidSideBtn.Sx && event->x < m_MidSideBtn.x1 && event->y > m_MidSideBtn.y0 && event->y < m_MidSideBtn.y1))
  {
    m_MidSideBtn.State = SR;
    m_midsideChangedSignal.emit(m_iBandNum);
  }
  
  m_EnableBtn.pressed = false;
  m_TypeBtn.pressed = false;
  m_GainBtn.pressed = false;
  m_FreqBtn.pressed = false;
  m_QBtn.pressed    = false;
  
  if(m_bIsStereoPlugin)
  {
    m_MidSideBtn.Dual_pressed = false;
    m_MidSideBtn.ML_pressed = false;
    m_MidSideBtn.SR_pressed = false;
    redraw_MidSide_widget(); 
  }
  //m_iAntValueX = m_iAntValueY = 0;
  
  //Inform bode plot
  m_bandUnSelectedSignal.emit();
  
  redraw();
  return true;
}

bool BandCtl::on_mouse_motion_event(GdkEventMotion* event)
{
  //If some gain, freq, q button is pressed compute new values
  if(m_GainBtn.pressed)
  {
    if(m_HpfLpf_slope)
    {
      //HPF and LPF case
      m_HpfLpf_slope += -1*(event->y - m_iAntValueY);
      m_HpfLpf_slope = m_HpfLpf_slope < 20 ? 20 : m_HpfLpf_slope;
      m_HpfLpf_slope = m_HpfLpf_slope > 80 ? 80: m_HpfLpf_slope;
      setFilterTypeLPFHPFAcordSlope();
    }
    else
    {
      //Gain type
      m_GainBtn.value += (float)(event->y - m_iAntValueY)/-ACCELERATION;
      m_GainBtn.value = m_GainBtn.value > GAIN_MAX ? GAIN_MAX : m_GainBtn.value;
      m_GainBtn.value = m_GainBtn.value < GAIN_MIN ? GAIN_MIN : m_GainBtn.value;
      m_bandChangedSignal.emit(m_iBandNum, GAIN_TYPE, m_GainBtn.value);
    }
  }
  
  else if(m_FreqBtn.pressed)
  {
    m_FreqBtn.value  += (m_FreqBtn.value/7.0f)*((float)(event->x-m_iAntValueX)/ACCELERATION);
    m_FreqBtn.value = m_FreqBtn.value > FREQ_MAX ? FREQ_MAX : m_FreqBtn.value;
    m_FreqBtn.value = m_FreqBtn.value < FREQ_MIN ? FREQ_MIN : m_FreqBtn.value;
    m_bandChangedSignal.emit(m_iBandNum, FREQ_TYPE, m_FreqBtn.value);
  }
  
  else if(m_QBtn.pressed)
  {
    m_QBtn.value += (float)(event->x - m_iAntValueX)/(-ACCELERATION * 5.0);
    m_QBtn.value = m_QBtn.value > PEAK_Q_MAX ? PEAK_Q_MAX : m_QBtn.value;
    m_QBtn.value = m_QBtn.value < PEAK_Q_MIN ? PEAK_Q_MIN : m_QBtn.value;
    m_bandChangedSignal.emit(m_iBandNum, Q_TYPE, m_QBtn.value);
  }
  
  else
  {
    //Check if mouse over some button
    m_EnableBtn.focus = (event->x > m_EnableBtn.x0 && event->x < m_EnableBtn.x1 && event->y > m_EnableBtn.y0 && event->y < m_EnableBtn.y1);
    m_TypeBtn.focus   = (event->x > m_TypeBtn.x0 && event->x < m_TypeBtn.x1 && event->y > m_TypeBtn.y0 && event->y < m_TypeBtn.y1);
    m_GainBtn.focus   = m_bBandIsEnabled & (event->x > m_GainBtn.x0 && event->x < m_GainBtn.x1 && event->y > m_GainBtn.y0 && event->y < m_GainBtn.y1);
    m_FreqBtn.focus   = m_bBandIsEnabled & (event->x > m_FreqBtn.x0 && event->x < m_FreqBtn.x1 && event->y > m_FreqBtn.y0 && event->y < m_FreqBtn.y1);
    m_QBtn.focus      = m_bBandIsEnabled & (event->x > m_QBtn.x0 && event->x < m_QBtn.x1 && event->y > m_QBtn.y0 && event->y < m_QBtn.y1);
    
    if(m_bIsStereoPlugin)
    {
      m_MidSideBtn.Dual_focus = m_bBandIsEnabled & (event->x > m_MidSideBtn.Dx && event->x < m_MidSideBtn.Sx && event->y > m_MidSideBtn.y0 && event->y < m_MidSideBtn.y1);
      m_MidSideBtn.ML_focus = m_bBandIsEnabled & (event->x > m_MidSideBtn.Mx && event->x < m_MidSideBtn.Dx && event->y > m_MidSideBtn.y0 && event->y < m_MidSideBtn.y1);
      m_MidSideBtn.SR_focus = m_bBandIsEnabled & (event->x > m_MidSideBtn.Sx && event->x < m_MidSideBtn.x1 && event->y > m_MidSideBtn.y0 && event->y < m_MidSideBtn.y1);
      redraw_MidSide_widget(); 
    }
    
    //Disable Q for LPF1 and LPF2
    m_QBtn.focus &= !(m_FilterType == LPF_ORDER_1);
    m_QBtn.focus &= !(m_FilterType == HPF_ORDER_1);

    //Disable Gain for NOTCH
    m_GainBtn.focus &= !(m_FilterType == NOTCH);
  }
  
  //Update ant values
  m_iAntValueX = event->x;
  m_iAntValueY = event->y;
  
  //Notify band over
  if(m_GainBtn.focus || m_FreqBtn.focus || m_QBtn.focus || m_TypeBtn.focus || m_EnableBtn.focus || m_MidSideBtn.Dual_focus || m_MidSideBtn.ML_focus || m_MidSideBtn.SR_focus)
  {
    m_bandSelectedSignal.emit(m_iBandNum);
  }
  redraw();
  return true;
}

bool BandCtl::on_scrollwheel_event(GdkEventScroll* event)
{
  double  increment = 0.0;
  
  if (event->direction == GDK_SCROLL_UP) 
  { 
    // up code
    increment = 1.0;

  } 
  else if (event->direction == GDK_SCROLL_DOWN) 
  { 
    // down code 
    increment = -1.0;
  }
  
  if ( m_bBandIsEnabled & (event->x > m_GainBtn.x0 && event->x < m_GainBtn.x1 && event->y > m_GainBtn.y0 && event->y < m_GainBtn.y1))
  {
    if(m_HpfLpf_slope)
    {
      //HPF and LPF case
      increment *= 20.0;
      m_HpfLpf_slope += increment;
      m_HpfLpf_slope = m_HpfLpf_slope < 20 ? 20 : m_HpfLpf_slope;
      m_HpfLpf_slope = m_HpfLpf_slope > 80 ? 80: m_HpfLpf_slope;
      setFilterTypeLPFHPFAcordSlope();
    }
    else
    {
      //Gain type
      increment *=  SCROLL_EVENT_PERCENT*(GAIN_MAX - GAIN_MIN);
      m_GainBtn.value += increment;
      m_GainBtn.value = m_GainBtn.value > GAIN_MAX ? GAIN_MAX : m_GainBtn.value;
      m_GainBtn.value = m_GainBtn.value < GAIN_MIN ? GAIN_MIN : m_GainBtn.value;
      m_bandChangedSignal.emit(m_iBandNum, GAIN_TYPE, m_GainBtn.value);
    }
  }
  else if( m_bBandIsEnabled & (event->x > m_FreqBtn.x0 && event->x < m_FreqBtn.x1 && event->y > m_FreqBtn.y0 && event->y < m_FreqBtn.y1))
  {
    increment *=  SCROLL_EVENT_PERCENT*(FREQ_MAX - FREQ_MIN)*0.0001*m_FreqBtn.value;
    m_FreqBtn.value += increment;
    m_FreqBtn.value = m_FreqBtn.value > FREQ_MAX ? FREQ_MAX : m_FreqBtn.value;
    m_FreqBtn.value = m_FreqBtn.value < FREQ_MIN ? FREQ_MIN : m_FreqBtn.value;
    m_bandChangedSignal.emit(m_iBandNum, FREQ_TYPE, m_FreqBtn.value);
  }
  else if( m_bBandIsEnabled & (event->x > m_QBtn.x0 && event->x < m_QBtn.x1 && event->y > m_QBtn.y0 && event->y < m_QBtn.y1))
  {
    increment *=  SCROLL_EVENT_PERCENT*(PEAK_Q_MAX - PEAK_Q_MIN);
    m_QBtn.value += increment;
    m_QBtn.value = m_QBtn.value > PEAK_Q_MAX ? PEAK_Q_MAX : m_QBtn.value;
    m_QBtn.value = m_QBtn.value < PEAK_Q_MIN ? PEAK_Q_MIN : m_QBtn.value;
    m_bandChangedSignal.emit(m_iBandNum, Q_TYPE, m_QBtn.value);
  }
  
  redraw();
  return true;
}

bool BandCtl::on_key_press_event(GdkEventKey* event)
{ 
  
  std::cout<<"BandCtl::on_key_press_event()"<<" event = "<<event->keyval<<std::endl; //TODO reomve
  
  switch(event->keyval)
  {
    case GDK_KEY_Return:
    case GDK_KEY_KP_Enter:
      //Parse the string to get new value
      if(m_GainBtn.text)
      {
        if(parseBtnString(&m_GainBtn))
        {
          m_bandChangedSignal.emit(m_iBandNum, GAIN_TYPE, m_GainBtn.value);
        }        
      }
      else if(m_FreqBtn.text)
      {
        if(parseBtnString(&m_FreqBtn))
        {
          m_bandChangedSignal.emit(m_iBandNum, FREQ_TYPE, m_FreqBtn.value);
        }
      }
      else if(m_QBtn.text)
      {
        if(parseBtnString(&m_QBtn))
        {
          m_bandChangedSignal.emit(m_iBandNum, Q_TYPE, m_QBtn.value);
        }
      }
      break;
      
    case GDK_KEY_Escape:
       m_GainBtn.text = false;
       m_FreqBtn.text = false;
       m_QBtn.text = false;
       //keyPressEvent.disconnect(); //TODO
      break;
      
    case GDK_KEY_BackSpace:
      if(m_GainBtn.text)
      {
        if(m_GainBtn.ss.str().length() > 0)
        {
          std::string aux = m_GainBtn.ss.str().erase(m_GainBtn.ss.str().length() - 1);
          m_GainBtn.ss.str("");
          m_GainBtn.ss<<aux;
        }
      }
      else if(m_FreqBtn.text)
      {
        if(m_FreqBtn.ss.str().length() > 0)
        {
          std::string aux = m_FreqBtn.ss.str().erase(m_FreqBtn.ss.str().length() - 1);
          m_FreqBtn.ss.str("");
          m_FreqBtn.ss<<aux;
        }
      }
      else if(m_QBtn.text)
      {
        if(m_QBtn.ss.str().length() > 0)
        {
          std::string aux = m_QBtn.ss.str().erase(m_QBtn.ss.str().length() - 1);
          m_QBtn.ss.str("");
          m_QBtn.ss<<aux;
        }
      }
      break;
      
    case GDK_KEY_KP_0: case GDK_KEY_0:
    case GDK_KEY_KP_1: case GDK_KEY_1:
    case GDK_KEY_KP_2: case GDK_KEY_2:
    case GDK_KEY_KP_3: case GDK_KEY_3:
    case GDK_KEY_KP_4: case GDK_KEY_4:
    case GDK_KEY_KP_5: case GDK_KEY_5:
    case GDK_KEY_KP_6: case GDK_KEY_6:
    case GDK_KEY_KP_7: case GDK_KEY_7:
    case GDK_KEY_KP_8: case GDK_KEY_8:
    case GDK_KEY_KP_9: case GDK_KEY_9:
    case GDK_KEY_KP_Subtract: case GDK_KEY_minus: 
      if(m_GainBtn.text)
      {
        m_GainBtn.ss << event->string;
      }
      else if(m_FreqBtn.text)
      {
        m_FreqBtn.ss << event->string;
      }
      else if(m_QBtn.text)
      {
        m_QBtn.ss << event->string;
      }
      break;
      
    case GDK_KEY_K: case GDK_KEY_k:
      if(m_GainBtn.text)
      {
        m_GainBtn.ss << "k";
      }
      else if(m_FreqBtn.text)
      {
        m_FreqBtn.ss << "k";
      }
      else if(m_QBtn.text)
      {
        m_QBtn.ss << "k";
      }
      break;
      
    case GDK_KEY_comma: case GDK_KEY_decimalpoint: case GDK_KEY_KP_Decimal: case GDK_KEY_period:
      if(m_GainBtn.text)
      {
        m_GainBtn.ss << ".";
      }
      else if(m_FreqBtn.text)
      {
        m_FreqBtn.ss << ".";
      }
      else if(m_QBtn.text)
      {
        m_QBtn.ss << ".";
      }
      break;
  }
   
  redraw();
  return true;
}


//TODO no estic segur de necessitar aquest senyal per a res.
bool BandCtl::on_focus_out_event(GdkEventFocus* event)
{
  std::cout <<"on_focus_out_event"<<std::endl; //TODO Remove
  m_GainBtn.text = false;
  m_FreqBtn.text = false;
  m_QBtn.text = false;
  //keyPressEvent.disconnect(); //TODO
  redraw();
  return true;
}

void BandCtl::setFilterTypeLPFHPFAcordSlope()
{
  if(m_HpfLpf_slope < 40)
  {
    if(m_FilterType == LPF_ORDER_1 || m_FilterType == LPF_ORDER_2 || m_FilterType == LPF_ORDER_3 || m_FilterType == LPF_ORDER_4)
    {
      m_FilterType = LPF_ORDER_1;
    }
    else
    {
      m_FilterType = HPF_ORDER_1;
    }
  }
  else if(m_HpfLpf_slope < 60)
  {
    if(m_FilterType == LPF_ORDER_1 || m_FilterType == LPF_ORDER_2 || m_FilterType == LPF_ORDER_3 || m_FilterType == LPF_ORDER_4)
    {
      m_FilterType = LPF_ORDER_2;
    }
    else
    {
      m_FilterType = HPF_ORDER_2;
    }
  }
  else if(m_HpfLpf_slope < 80)
  {
    if(m_FilterType == LPF_ORDER_1 || m_FilterType == LPF_ORDER_2 || m_FilterType == LPF_ORDER_3 || m_FilterType == LPF_ORDER_4)
    {
      m_FilterType = LPF_ORDER_3;
    }
    else
    {
      m_FilterType = HPF_ORDER_3;
    }
  }
  else
  {
    if(m_FilterType == LPF_ORDER_1 || m_FilterType == LPF_ORDER_2 || m_FilterType == LPF_ORDER_3 || m_FilterType == LPF_ORDER_4)
    {
      m_FilterType = LPF_ORDER_4;
    }
    else
    {
      m_FilterType = HPF_ORDER_4;
    }
  }
  m_bandChangedSignal.emit(m_iBandNum, FILTER_TYPE, getFilterType());
  m_bandChangedSignal.emit(m_iBandNum, GAIN_TYPE, m_GainBtn.value);
  m_bandChangedSignal.emit(m_iBandNum, FREQ_TYPE, m_FreqBtn.value);
  m_bandChangedSignal.emit(m_iBandNum, Q_TYPE, m_QBtn.value); 
}

bool BandCtl::parseBtnString(BandCtl::Button* btn)
{
  std::string str = btn->ss.str();
  std::string str_k = "";
  std::string str_d = "";
  
  unsigned int pos_d = str.find('.');
  unsigned int pos_k = str.find('k');
  
  if(pos_k > pos_d && pos_k < str.length() && pos_d < str.length() )
  {
    //Found both, k and decimal but in inverse order, rise an error
    btn->text = false;
    //keyPressEvent.disconnect(); //TODO
    return false;
  }
  
  if(pos_d >= str.length())
  { 
    if(pos_k >= str.length())
    {
      //Nor k neither d found so, use the whole string
      str = str; //Do nothing!
    }
    else
    {
      //No d is found but k is found
      str_k = str.substr(0, pos_k);
      str = str.substr(pos_k + 1, str.length() - pos_k);
    }
  }
  else
  {
    if(pos_k >= str.length())
    {
      //No k but d is found
      str_d = str.substr(pos_d + 1, str.length() - pos_d - 1);
      str = str.substr(0, pos_d);
    }
    else
    {
      //Both k and d are found
      str_k = str.substr(0, pos_k);
      str_d = str.substr(pos_d + 1, str.length() - pos_d - 1);
      str = str.substr(pos_k + 1, pos_d - pos_k - 1);
    }
  }
  
  double val_k = 0.0;
  double val_d = 0.0;
  double val = atof(str.c_str());
  
  if(str_k.length() > 0)
  {
    val_k = atof(str_k.c_str()) * 1e3;
    val *= exp10(3.0 - str.length());
    if(str.length() > 3)
    {
      //throw an error, imposible to match str > 3 with k
      btn->text = false;
      //keyPressEvent.disconnect(); //TODO
      return false;
    }
  }
  if(str_d.length() > 0)
  {
    val_d = atof(str_d.c_str())/ exp10((double)str_d.length());
  }
   
  btn->value = val + val_k + val_d;
  
  //clip value to the widget limits!
  btn->value = btn->value > btn->max ? btn->max : btn->value;
  btn->value = btn->value < btn->min ? btn->min : btn->value;
  btn->text = false;
  //keyPressEvent.disconnect(); //TODO
  return true;
}


bool BandCtl::on_mouse_leave_widget(GdkEventCrossing* event)
{
  m_EnableBtn.focus = false;
  m_TypeBtn.focus = false;
  m_GainBtn.focus = m_GainBtn.pressed; //Lost focus only if is no pressed
  m_FreqBtn.focus = m_FreqBtn.pressed; //Lost focus only if is no pressed
  m_QBtn.focus = m_QBtn.pressed; //Lost focus only if is no pressed
  //keyPressEvent.disconnect(); //TODO
  if(m_bIsStereoPlugin)
  {
    m_MidSideBtn.Dual_focus = false;
    m_MidSideBtn.ML_focus = false;
    m_MidSideBtn.SR_focus = false;
    redraw_MidSide_widget(); 
  }
  redraw();
  m_bandUnSelectedSignal.emit();
  return true;
}

void BandCtl::redraw()
{
  Glib::RefPtr<Gdk::Window> win = get_window();
  if(win)
  {
    Gdk::Rectangle r(0, 0, get_allocation().get_width(), get_allocation().get_height());
    win->invalidate_rect(r, false);
  }
}

bool BandCtl::on_expose_event(GdkEventExpose* event)
{ 
 Glib::RefPtr<Gdk::Window> window = get_window();
  if(window)
  {
    Gtk::Allocation allocation = get_allocation();
    width = allocation.get_width();
    height = allocation.get_height();

    if(!m_midSide_surface_ptr && m_bIsStereoPlugin)
    {  
      //The Mid Side button surface
      m_midSide_surface_ptr = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, width, (height - 4.0*OUTER_BORDER)/4.0);
    }
   
    if(!m_bBtnInitialized)
    {
      m_EnableBtn.focus = false;
      m_EnableBtn.pressed = false;
      m_EnableBtn.x0 = OUTER_BORDER + 5;
      m_EnableBtn.x1 = width - OUTER_BORDER - m_image_surface_ptr->get_width() - 2;
      m_EnableBtn.y0 = OUTER_BORDER + 2;
      m_EnableBtn.y1 = OUTER_BORDER + m_image_surface_ptr->get_height() - 1;
      m_EnableBtn.text = false;
     
      m_TypeBtn.focus = false;
      m_TypeBtn.pressed = false;
      m_TypeBtn.x0 = width - OUTER_BORDER - m_image_surface_ptr->get_width();
      m_TypeBtn.x1 = width - OUTER_BORDER;
      m_TypeBtn.y0 = OUTER_BORDER + 1;
      m_TypeBtn.y1 = OUTER_BORDER + m_image_surface_ptr->get_height();
      m_TypeBtn.text = false;
      
      m_GainBtn.focus = false;
      m_GainBtn.pressed = false;
      m_GainBtn.x0 = OUTER_BORDER + 1;
      m_GainBtn.x1 = width - OUTER_BORDER;
      m_GainBtn.y0 = m_EnableBtn.y1 + 1;
      m_GainBtn.y1 = m_EnableBtn.y1 + (height - 4.0*OUTER_BORDER - m_EnableBtn.y1)/ ( m_bIsStereoPlugin ? 4.0 : 3.0);
      m_GainBtn.text = false;
      m_GainBtn.min = GAIN_MIN;
      m_GainBtn.max = GAIN_MAX;
      
      m_FreqBtn.focus = false;
      m_FreqBtn.pressed = false;
      m_FreqBtn.x0 = OUTER_BORDER + 1;
      m_FreqBtn.x1 = width - OUTER_BORDER;
      m_FreqBtn.y0 = m_GainBtn.y1 + 1;
      m_FreqBtn.y1 = m_EnableBtn.y1 +  2.0*(height - 4.0*OUTER_BORDER - m_EnableBtn.y1)/ (m_bIsStereoPlugin ? 4.0 : 3.0);
      m_FreqBtn.text = false;
      m_FreqBtn.min = FREQ_MIN;
      m_FreqBtn.max = FREQ_MAX;
      
      m_QBtn.focus = false;
      m_QBtn.pressed = false;
      m_QBtn.x0 = OUTER_BORDER + 1;
      m_QBtn.x1 = width - OUTER_BORDER;
      m_QBtn.y0 = m_FreqBtn.y1 + 1;
      m_QBtn.y1 =  m_EnableBtn.y1 +  3.0*(height - 4.0*OUTER_BORDER - m_EnableBtn.y1)/ (m_bIsStereoPlugin ? 4.0 : 3.0);
      m_QBtn.text = false;
      m_QBtn.min = PEAK_Q_MIN;
      m_QBtn.max = PEAK_Q_MAX;
  
      if(m_bIsStereoPlugin)
      {
        m_MidSideBtn.Dual_focus = false;
        m_MidSideBtn.ML_focus = false;
        m_MidSideBtn.SR_focus = false;
        m_MidSideBtn.Dual_pressed = false;
        m_MidSideBtn.ML_pressed = false;
        m_MidSideBtn.SR_pressed = false;
        m_MidSideBtn.x0 = 4.0*OUTER_BORDER;
        m_MidSideBtn.x1 =  width - 4.0*OUTER_BORDER;
        m_MidSideBtn.y0 = round(m_QBtn.y1 + 2.0);
        m_MidSideBtn.y1 =  height - 2.0*OUTER_BORDER;
        m_MidSideBtn.Mx = m_MidSideBtn.x0;
        m_MidSideBtn.Dx = (m_MidSideBtn.x1 - m_MidSideBtn.x0)/3.0 + m_MidSideBtn.Mx;
        m_MidSideBtn.Sx = (m_MidSideBtn.x1 - m_MidSideBtn.x0)/3.0 + m_MidSideBtn.Dx;
        redraw_MidSide_widget(); 
      }
      
      m_bBtnInitialized = true;
    }
  
    Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
    int radius =  (int)round(((double)height) / 20.0);
    double degrees = M_PI / 180.0;   
    

    //Paint backgroud
    cr->save();
    cr->set_source_rgb(BACKGROUND_R, BACKGROUND_G, BACKGROUND_B);
    cr->paint(); //Fill all with background color
    cr->restore();
    
    //Draw background box
    cr->save();         
    cr->begin_new_sub_path();
    cr->arc ( OUTER_BORDER + radius, OUTER_BORDER + radius, radius, 180 * degrees, 270 * degrees);
    cr->arc (width - OUTER_BORDER - radius, OUTER_BORDER + radius, radius, -90 * degrees, 0 * degrees);  
    cr->line_to(width - OUTER_BORDER, height - OUTER_BORDER);
    cr->line_to(OUTER_BORDER, height - OUTER_BORDER);
    cr->close_path();

    cr->set_line_width(1);
    if(m_bBandIsEnabled)
    {
      cr->set_source_rgba(m_Color.get_red_p(), m_Color.get_green_p(), m_Color.get_blue_p(), 0.5);
    }
    else
    {
      cr->set_source_rgba(0.3,0.3,0.3,0.6);
    }
    cr->stroke_preserve();
 
    if(m_bGlowBand)
    {
      cr->set_line_width(2.5);
      cr->set_source_rgba(0.0,1.0,1.0,0.5);
      cr->stroke_preserve();
    }

    Cairo::RefPtr<Cairo::LinearGradient> bkg_gradient_ptr = Cairo::LinearGradient::create(width/2, OUTER_BORDER, width/2, height - OUTER_BORDER);   
    if(m_bBandIsEnabled)
    {
      bkg_gradient_ptr->add_color_stop_rgba (0.0, m_Color.get_red_p(), m_Color.get_green_p(), m_Color.get_blue_p(), 0.6 ); 
      bkg_gradient_ptr->add_color_stop_rgba (1.0, m_Color.get_red_p(), m_Color.get_green_p(), m_Color.get_blue_p(), 0.3 );  
    }
    else
    {
      bkg_gradient_ptr->add_color_stop_rgba (0.0, 0.8, 0.8, 0.8, 0.4 ); 
      bkg_gradient_ptr->add_color_stop_rgba (1.0, 0.8, 0.8, 0.8, 0.2 );
    }
    bkg_gradient_ptr->add_color_stop_rgba (0.3, m_Color.get_red_p(), m_Color.get_green_p(), m_Color.get_blue_p(), 0.1 );  
    bkg_gradient_ptr->add_color_stop_rgba (0.5, m_Color.get_red_p(), m_Color.get_green_p(), m_Color.get_blue_p(), 0.0 );  
    bkg_gradient_ptr->add_color_stop_rgba (0.9, m_Color.get_red_p(), m_Color.get_green_p(), m_Color.get_blue_p(), 0.05 );  

    cr->set_source(bkg_gradient_ptr);                       
    cr->fill();
    cr->restore();

    //Draw Enable LED
    cr->save();
    cr->translate(m_EnableBtn.x0 - 0.5, m_EnableBtn.y0 + 1.5);
    ToggleButton::drawLedBtn(cr, m_EnableBtn.focus, m_bBandIsEnabled, "On", 0, 3);
    cr->restore();
        
    //Draw Gan, Freq, Q Buttons
    drawBandButton(&m_GainBtn, cr);
    drawBandButton(&m_FreqBtn, cr);
    drawBandButton(&m_QBtn, cr);
    
    //Draw mid Side surface
    if(m_midSide_surface_ptr && m_bIsStereoPlugin)
    {
      cr->save();          
      cr->set_source(m_midSide_surface_ptr, 0, m_MidSideBtn.y0);    
      cr->paint();
      cr->restore();
    }
  
    //Draw ComboBox Filter Type icon 
    cr->save();
    cr->set_source (m_image_surface_ptr, m_TypeBtn.x0 - 0.5, m_TypeBtn.y0 - 0.5);
    cr->rectangle (m_TypeBtn.x0 - 0.5, m_TypeBtn.y0 - 0.5, m_image_surface_ptr->get_width(), m_image_surface_ptr->get_height());
    cr->clip();
    cr->paint();
    cr->restore();
    
    //Draw FilterType Focus ComboBox
    if(m_bBandIsEnabled)
    {
      cr->save();
      cr->begin_new_sub_path();
      cr->arc (m_TypeBtn.x1 - radius - 1 - 0.5, m_TypeBtn.y0 + radius + 0.5, radius, -90 * degrees, 0 * degrees);
      cr->arc (m_TypeBtn.x1- radius -1 - 0.5, m_TypeBtn.y1 - radius - 0.5, radius, 0 * degrees, 90 * degrees);
      cr->arc (m_TypeBtn.x0 + radius + 0.5, m_TypeBtn.y1 - radius - 0.5, radius, 90 * degrees, 180 * degrees);
      cr->arc (m_TypeBtn.x0 + radius + 0.5, m_TypeBtn.y0 + radius + 0.5, radius, 180 * degrees, 270 * degrees);
      cr->close_path();
      
      cr->set_source_rgba(0.2, 0.2, 0.2, 0.9);
      cr->set_line_width(1);
      cr->stroke_preserve();
      
      if(m_TypeBtn.focus)
      {
        cr->set_source_rgba(0.0, 1.0, 1.0, 0.7);
        cr->set_line_width(2);
        cr->stroke();       
      }
      cr->restore();
    }

  }  
  return true;
}

void BandCtl::drawBandButton(BandCtl::Button* btn, Cairo::RefPtr<Cairo::Context> cr)
{
  //Avoid the drawig of button if it is notch filter gain button or LPF1/HPF1 Q button
  if(btn->units == "dB" && m_FilterType == NOTCH)
  {
    return;
  }
  if(btn->units == "Q" && ( m_FilterType == LPF_ORDER_1 || m_FilterType == HPF_ORDER_1 ))
  {
    return;
  }
  
  std::stringstream ss;
  ss<<"sans "<< FONT_SIZE << "px";  
  Glib::RefPtr<Pango::Layout> pangoLayout = Pango::Layout::create(cr);
  Pango::FontDescription font_desc(ss.str());
  pangoLayout->set_font_description(font_desc);
  pangoLayout->set_alignment(Pango::ALIGN_CENTER);
  pangoLayout->set_width(Pango::SCALE * (btn->x1 - btn->x0));    
   
  int radius =  (int)round(((double)height) / 20.0);
  double degrees = M_PI / 180.0; 
    
  //Draw the button in text entry mode
  if(btn->text)
  {
    cr->save();
    cr->begin_new_sub_path();
    cr->arc (btn->x1 - radius - 3, btn->y0 + radius + 3, radius, -90 * degrees, 0 * degrees);
    cr->arc (btn->x1- radius -3 , btn->y1 - radius -3, radius, 0 * degrees, 90 * degrees);
    cr->arc (btn->x0 + radius + 3,btn->y1 - radius -3, radius, 90 * degrees, 180 * degrees);
    cr->arc (btn->x0 + radius + 3, btn->y0 + radius +3, radius, 180 * degrees, 270 * degrees);
    cr->close_path();
    cr->set_line_width(1.0);
    cr->set_source_rgba(0.0, 1.0, 1.0, 0.5);
    cr->stroke();
    cr->restore();
        
    cr->save();
    cr->move_to(btn->x0,  btn->y0 + 0.5*(btn->y1 - btn->y0) - FONT_SIZE/2 - 2); 
    cr->set_source_rgb(m_Color.get_red_p(), m_Color.get_green_p(), m_Color.get_blue_p());
    pangoLayout->set_text(btn->ss.str() + "|");
    pangoLayout->show_in_cairo_context(cr);
    cr->stroke();  
    cr->restore();
    return;
  }

  //Draw Btn Focus
  if(btn->focus)
  {
    Cairo::RefPtr<Cairo::LinearGradient> bkg_gradient_ptr = Cairo::LinearGradient::create(btn->x0 + 0.5*(btn->x1 - btn->x0), btn->y0, btn->x0 + 0.5*(btn->x1 - btn->x0), btn->y1);   
    bkg_gradient_ptr->add_color_stop_rgba (0.1, m_Color.get_red_p() + 0.2, m_Color.get_green_p() + 0.2, m_Color.get_blue_p() + 0.2, 0.0 );
    bkg_gradient_ptr->add_color_stop_rgba (0.45, m_Color.get_red_p() + 0.2, m_Color.get_green_p() + 0.2, m_Color.get_blue_p() + 0.2, 0.3 );  
    bkg_gradient_ptr->add_color_stop_rgba (0.55, m_Color.get_red_p() + 0.2, m_Color.get_green_p() + 0.2, m_Color.get_blue_p() + 0.2, 0.3 );  
    bkg_gradient_ptr->add_color_stop_rgba (0.9, m_Color.get_red_p() + 0.2, m_Color.get_green_p() + 0.2, m_Color.get_blue_p() + 0.2, 0.0 );                          
    cr->save();                 
    cr->set_source(bkg_gradient_ptr);  
    cr->rectangle(btn->x0, btn->y0, btn->x1 - btn->x0, btn->y1 - btn->y0);
    cr->fill();
    cr->restore();
  }
  
  //Draw Text
  cr->save();
 
  ss.str(""); //Clear stringstream
  if(btn->units != "dB/dec")
  {
    int precision = 1;
    if(btn->value < 100 || (btn->value >= 1e3 && btn->value < 1e4)) precision = 2;
    ss<< std::setprecision(precision)<< std::fixed <<(btn->value < 1e3 ? btn->value : (0.001*(btn->value))) << (btn->value < 1e3 ? " " : " k") << (btn->units);
  }
  else
  {
    int slope = m_HpfLpf_slope;
    if(m_HpfLpf_slope < 40) slope = 20;
    else if(m_HpfLpf_slope < 60) slope = 40;
    else if(m_HpfLpf_slope < 80) slope = 60;
    else slope = 80;
    ss<< std::setprecision(0)<< std::fixed << slope << " " << (btn->units);
  }
  
  pangoLayout->set_text(ss.str());
  
  //Shadow
  cr->move_to(btn->x0 + 1,  btn->y0 + 0.5*(btn->y1 - btn->y0) - FONT_SIZE/2 - 2 + 1); 
  cr->set_source_rgba(0,0,0,0.5);
  pangoLayout->show_in_cairo_context(cr);  
  cr->stroke();
  
  //Text
  cr->move_to(btn->x0,  btn->y0 + 0.5*(btn->y1 - btn->y0) - FONT_SIZE/2 - 2); 
  if(!m_bBandIsEnabled)
  {
    cr->set_source_rgba(1.0, 1.0, 1.0, 0.4);
  }
  else if(btn->pressed)
  {
    cr->set_source_rgba(1.0, 1.0, 1.0, 1.0);
  }
  else
  {
    cr->set_source_rgba(1.0, 1.0, 1.0, 0.8);
  }
  pangoLayout->show_in_cairo_context(cr);
  cr->stroke();
  

  
  cr->restore();
}

void BandCtl::redraw_MidSide_widget()
{
  if(m_midSide_surface_ptr && m_bIsStereoPlugin)
  {  
    //Create cairo context using the buffer surface
    Cairo::RefPtr<Cairo::Context> cr = Cairo::Context::create(m_midSide_surface_ptr);
    const double btnH = round(m_midSide_surface_ptr->get_height() - 6.0*OUTER_BORDER);
        
    //Clear current context  
    cr->save();
    cr->set_operator(Cairo::OPERATOR_CLEAR);
    cr->paint();
    cr->restore();
    
    //Draw Color background on selected button
    cr->save();
    Cairo::RefPtr<Cairo::LinearGradient> bkg_gradient_ptr = Cairo::LinearGradient::create(0, 0, 0, m_midSide_surface_ptr->get_height()); 
    switch(m_MidSideBtn.State)
    {
      case DUAL:
        cr->rectangle(m_MidSideBtn.Dx, 0, m_MidSideBtn.Sx - m_MidSideBtn.Dx, btnH);
        break;
        
      case ML:
        cr->begin_new_sub_path();
        cr->arc( m_MidSideBtn.x0 + btnH/2.0, btnH/2.0 + 0.5, btnH/2.0, M_PI/2.0, -M_PI/2.0);
        cr->line_to( m_MidSideBtn.Dx,  0);
        cr->line_to( m_MidSideBtn.Dx,  btnH);
        cr->close_path();
        break;
        
      case SR:
        cr->begin_new_sub_path();
        cr->arc( m_MidSideBtn.x1 - btnH/2.0, btnH/2.0 + 0.5, btnH/2.0 , -M_PI/2.0, M_PI/2.0);   
        cr->line_to( m_MidSideBtn.Sx,  btnH);
        cr->line_to( m_MidSideBtn.Sx,  0);
        cr->close_path();
        break;
    }
      
    bkg_gradient_ptr->add_color_stop_rgba (0.1, 0.4, 0.4, 0.4, 0.5 );
    bkg_gradient_ptr->add_color_stop_rgba (0.7, m_Color.get_red_p(), m_Color.get_green_p(), m_Color.get_blue_p(), 0.6 );  
    bkg_gradient_ptr->add_color_stop_rgba (0.9, 0.2, 0.2, 0.2, 0.3 );                          
    cr->set_source(bkg_gradient_ptr);  
    cr->fill();
    cr->restore();
    
    //Draw a box
    cr->save();
    cr->begin_new_sub_path();
    cr->arc( m_MidSideBtn.x0 + btnH/2.0, btnH/2.0 + 0.5, btnH/2.0, M_PI/2.0, -M_PI/2.0);
    cr->arc( m_MidSideBtn.x1 - btnH/2.0, btnH/2.0 + 0.5, btnH/2.0 , -M_PI/2.0, M_PI/2.0);
    cr->close_path();
    cr->set_source_rgba(0.05,0.05,0.05,0.2);
    cr->fill_preserve();
    cr->set_line_width(1.0);
    if(m_bBandIsEnabled)
    {
      cr->set_source_rgba(m_Color.get_red_p(), m_Color.get_green_p(), m_Color.get_blue_p(), 0.7);
      cr->stroke_preserve();
    }
    cr->set_source_rgba(0.5, 0.5, 0.5, 0.7);
    cr->stroke();
    cr->move_to(m_MidSideBtn.Dx, 0);
    cr->line_to(m_MidSideBtn.Dx, btnH);
    cr->stroke();
    cr->move_to(m_MidSideBtn.Sx, 0);
    cr->line_to(m_MidSideBtn.Sx, btnH);
    cr->stroke();
    cr->restore();
    
    //Draw Text
    Glib::RefPtr<Pango::Layout> pangoLayout = Pango::Layout::create(cr);
    Pango::FontDescription font_desc("sans bold 10px");
    pangoLayout->set_font_description(font_desc);
    pangoLayout->set_alignment(Pango::ALIGN_CENTER);
 
    cr->save();
    if(m_MidSideBtn.ML_focus)
    {
      cr->set_source_rgba(0.0, 1.0, 1.0, 1.0);
    }
    else if(m_MidSideBtn.State == BandCtl::ML)
    {
      cr->set_source_rgba(0.0, 1.0, 1.0, 0.8);
    }
    else
    {
      cr->set_source_rgba(0.5, 0.5, 0.5, 0.6);
    }
    pangoLayout->set_width(Pango::SCALE * (m_MidSideBtn.Dx - m_MidSideBtn.Mx));  
    cr->move_to(m_MidSideBtn.Mx,  btnH/2.0 - 5); 
    if(m_MidSideBtn.MidSideMode)
    {
      pangoLayout->set_text("M");
    }
    else
    {
      pangoLayout->set_text("L");
    }
    pangoLayout->show_in_cairo_context(cr);
    cr->stroke();
    
    if(m_MidSideBtn.SR_focus)
    {
      cr->set_source_rgba(0.0, 1.0, 1.0, 1.0);
    }
    else if(m_MidSideBtn.State == BandCtl::SR)
    {
      cr->set_source_rgba(1.0, 1.0, 1.0, 0.8);
    }
    else
    {
      cr->set_source_rgba(0.5, 0.5, 0.5, 0.6);
    }
    pangoLayout->set_width(Pango::SCALE * (m_MidSideBtn.x1 - m_MidSideBtn.Sx));    
    cr->move_to(m_MidSideBtn.Sx,  btnH/2.0 - 5); 
    if(m_MidSideBtn.MidSideMode)
    {
      pangoLayout->set_text("S");
    }
    else
    {
      pangoLayout->set_text("R");
    }
    pangoLayout->show_in_cairo_context(cr);
    cr->stroke();
    cr->restore();
   
    //Draw circle in the center
    cr->save();
    cr->set_line_width(1.5);
    if(m_MidSideBtn.Dual_focus)
    {
      cr->set_source_rgba(0.0, 1.0, 1.0, 1.0);
    }
    else if( m_MidSideBtn.State == DUAL )
    {
      cr->set_source_rgba(0.0, 1.0, 1.0, 0.6);
    }
    else
    {
      cr->set_source_rgba(0.5, 0.5, 0.5, 0.6);
    }
    const double Xcenter = (m_midSide_surface_ptr->get_width()/2.0);
    cr->arc(Xcenter - btnH/4.0 + (btnH/10.0), (btnH/2.0) + 0.5, btnH/3.5, 0.0, 2.0*M_PI);
    cr->stroke();
    
    if(m_MidSideBtn.Dual_focus)
    {
      cr->set_source_rgba(0.0, 1.0, 1.0, 1.0);
    }
    else if( m_MidSideBtn.State == DUAL )
    {
      cr->set_source_rgba(1.0, 1.0, 1.0, 0.6);
    }
    else
    {
      cr->set_source_rgba(0.5, 0.5, 0.5, 0.6);
    }
    cr->arc(Xcenter + btnH/4.0 - (btnH/10.0), (btnH/2.0) + 0.5, btnH/3.5, 0.0, 2.0*M_PI);
    cr->stroke();
    cr->restore();
  
  }
}

void BandCtl::setStereoMode(bool bIsMidSide)
{
  if(m_bIsStereoPlugin)
  {
    m_MidSideBtn.MidSideMode = bIsMidSide;
    redraw_MidSide_widget(); 
    redraw();
  }
}

BandCtl::MSState BandCtl::getStereoState()
{
  return m_MidSideBtn.State;
}

void BandCtl::setStereoState(BandCtl::MSState state)
{
  if(m_bIsStereoPlugin)
  {
    m_MidSideBtn.State = state;
    redraw_MidSide_widget(); 
    redraw();
  }
}

