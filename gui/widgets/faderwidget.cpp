/***************************************************************************
 *   Copyright (C) 2012 by Pere Ràfols Soler                               *
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
#include <iomanip>
#include <cstring>
#include <cmath>
#include <gdkmm.h>//For the function Gdk::Cairo::set_source_pixbuf()
//#include <gdkmm/general.h> //Switched back to gdkmm.h for cairo portability problems

#include "colors.h"
#include "faderwidget.h"

FaderWidget::FaderWidget(double dMax, double dMin, const char *bundlePath, Glib::ustring title)
  :bMotionIsConnected(false), m_value(0), m_max(dMax), m_min(dMin), m_bundlePath(bundlePath), m_title(title)
{
  m_image_ptr =  Gdk::Pixbuf::create_from_file(m_bundlePath + "/" + std::string(FADER_ICON_FILE));

  // Detect transparent colors for loaded image
  Cairo::Format format = Cairo::FORMAT_RGB24;
  if (m_image_ptr->get_has_alpha())
  {
      format = Cairo::FORMAT_ARGB32;
  }
  
  // Create a new ImageSurface
  m_image_surface_ptr = Cairo::ImageSurface::create  (format, m_image_ptr->get_width(), m_image_ptr->get_height());
  
  // Create the new Context for the ImageSurface
  m_image_context_ptr = Cairo::Context::create (m_image_surface_ptr);     
  
  // Draw the image on the new Context
  Gdk::Cairo::set_source_pixbuf (m_image_context_ptr, m_image_ptr, 0.0, 0.0);
  m_image_context_ptr->paint();
  set_size_request(2*m_image_surface_ptr->get_width()+4*FADER_MARGIN, FADER_INITAL_HIGHT);
  
  //Connect mouse signals
  add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::POINTER_MOTION_MASK | Gdk::SCROLL_MASK);
  signal_button_press_event().connect(sigc::mem_fun(*this, &FaderWidget::on_button_press_event),true);
  signal_button_release_event().connect(sigc::mem_fun(*this, &FaderWidget::on_button_release_event),true);
  signal_scroll_event().connect(sigc::mem_fun(*this, &FaderWidget::on_scrollwheel_event),true); 
}
    
FaderWidget::~FaderWidget()
{
}


void FaderWidget::set_value(double value)
{
  m_value = value;
  m_value = m_value < m_min ? m_min : m_value;
  m_value = m_value > m_max ? m_max : m_value;
  redraw();
}

double FaderWidget::get_value()
{
  return m_value;
}

void FaderWidget::set_range(double max, double min)
{
  m_max = max;
  m_min = min;
  redraw();
}

double FaderWidget::get_max()
{
  return m_max;
}

double FaderWidget::get_min()
{
  return m_min;
}

void FaderWidget::redraw()
{
  Glib::RefPtr<Gdk::Window> win = get_window();
  if(win)
  {
    Gdk::Rectangle r(0, 0, get_allocation().get_width(), get_allocation().get_height());
    win->invalidate_rect(r, false);
  }
}

//Override default signal handler:
bool FaderWidget::on_expose_event(GdkEventExpose* event)
{
  double m, n;
  
  Glib::RefPtr<Gdk::Window> window = get_window();
  if(window)
  {
  
    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
    const int height = allocation.get_height();

    m = ((double)(FADER_MARGIN + TITLE_OFFSET - (height - FADER_MARGIN - m_image_surface_ptr->get_height())))/(m_max - m_min);
    n = (double)(height - FADER_MARGIN - m_image_surface_ptr->get_height()) - m_min*m;
    
    yFaderPosition = (int)(m*m_value + n);
    Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();

    //Draw fader backgroud rectangle and paint it
    cr->save();
    cr->set_source_rgb(BACKGROUND_R, BACKGROUND_G, BACKGROUND_B);
    cr->paint(); //Fill all with background color
    cr->restore();
   
    //Draw fader backgroud line
    cr->save();
    cr->set_line_cap(Cairo::LINE_CAP_ROUND);
    cr->move_to(round(width/2) + 0.5, FADER_MARGIN + m_image_surface_ptr->get_height()/2 + TITLE_OFFSET); 
    cr->line_to(round(width/2) + 0.5, height - FADER_MARGIN - m_image_surface_ptr->get_height()/2 + TITLE_OFFSET);
    cr->set_source_rgba(0.7, 0.7, 0.7, 0.7);
    cr->set_line_width(4);
    cr->stroke_preserve();
    cr->set_source_rgba(0.15, 0.15, 0.15, 1.0);
    cr->set_line_width(3);
    cr->stroke();
    cr->restore();
    
    //Draw fader dB scale
    double yBarPosition;
    
    //Draw thin lines for each dB
    cr->save();
    cr->set_source_rgba(0.8, 0.8, 0.8, 0.4);
    cr->set_line_width(1);
    for (double i = m_max; i >= m_min; i-= 0.5)  //The var step size is one dBu
    {
      yBarPosition = round((double)((int)(m*i + n +  (double)(m_image_surface_ptr->get_height()/2)))) + 0.5; //Sum 0.5 to center cairo to the pixel
      cr->move_to(width/2 - m_image_surface_ptr->get_width()/3 - FADER_MARGIN + 2, yBarPosition); 
      cr->line_to(width/2 - FADER_MARGIN, yBarPosition);
      cr->move_to(width/2 + FADER_MARGIN, yBarPosition); 
      cr->line_to(width/2 + m_image_surface_ptr->get_width()/3 + FADER_MARGIN - 2, yBarPosition);
    }
    cr->stroke();
    cr->restore();
    
    
    //Draw text with pango
    cr->save();
    cr->set_source_rgba(0.9, 0.9, 0.9, 0.5);
    Glib::RefPtr<Pango::Layout> pangoLayout = Pango::Layout::create(cr);
    Pango::FontDescription font_desc("sans 9px");
    pangoLayout->set_font_description(font_desc);
    pangoLayout->set_alignment(Pango::ALIGN_RIGHT);
    for (double i = m_max; i >= m_min; i -= m_max/2)  //The var step size is the half of max value because I like this way ;-)
    {
      std::stringstream ss;
      yBarPosition = (int)(m*i + n) +  m_image_surface_ptr->get_height()/2;
      ss<< std::setprecision(3) << abs(i);
      //cr->move_to(width/2 - m_image_surface_ptr->get_width()/2 - 3*FADER_MARGIN, yBarPosition - 4);
      cr->move_to(FADER_MARGIN/4, yBarPosition - 4);
      pangoLayout->set_text(ss.str());
      pangoLayout->show_in_cairo_context(cr);
      cr->stroke();  
    }
    cr->restore();
   
    //Draw title (using same pango)
    cr->save();
    Glib::RefPtr<Pango::Layout> pangoLayoutTil = Pango::Layout::create(cr);
    Pango::FontDescription font_desc_til("sans 11px");
    pangoLayoutTil->set_font_description(font_desc_til);
    pangoLayoutTil->set_alignment(Pango::ALIGN_LEFT);
    pangoLayoutTil->set_text(m_title.c_str());
    //a shadow
    cr->move_to(FADER_MARGIN + 1, FADER_MARGIN + 1);
    cr->set_source_rgba(0.1, 0.1, 0.1, 0.9);
    pangoLayoutTil->show_in_cairo_context(cr);
    cr->stroke(); 
    //and text
    cr->move_to(FADER_MARGIN, FADER_MARGIN);
    cr->set_source_rgba(0.9, 0.9, 0.9, 0.7);
    pangoLayoutTil->show_in_cairo_context(cr);
    cr->stroke();  
    cr->restore();
    
    //Draw strong lines with labels
    cr->save();
    cr->set_source_rgba(0.4, 0.4, 0.4, 1.0);
    cr->set_line_width(1);
    for (double i = m_max; i >= m_min; i -= m_max/2)  //The var step size is the hlaf of max because I like this way
    {
      yBarPosition = round((double)((int)(m*i + n +  (double)(m_image_surface_ptr->get_height()/2)))) + 0.5;  //Sum 0.5 to center cairo to the pixel
      cr->move_to(width/2 - m_image_surface_ptr->get_width() + FADER_MARGIN - 2, yBarPosition); 
      cr->line_to(width/2 - FADER_MARGIN + 1, yBarPosition);
      cr->move_to(width/2 + FADER_MARGIN - 1, yBarPosition); 
      cr->line_to(width/2 + m_image_surface_ptr->get_width() - FADER_MARGIN + 2, yBarPosition);
    }
    cr->stroke();
    cr->restore();
           
    //Draw the fader drop down shadow
    cr->save();    
    cr->translate(width/2 + 4, yFaderPosition + m_image_surface_ptr->get_height()/2 + 6);
    cr->scale(m_image_surface_ptr->get_width()/1.2, m_image_surface_ptr->get_height()/1.2);
    Cairo::RefPtr<Cairo::RadialGradient> bkg_gradient_ptr = Cairo::RadialGradient::create(0, 0, 0, 0, 0, 1);
    bkg_gradient_ptr->add_color_stop_rgba (0.3, 0.2, 0.2, 0.2, 1.0); 
    bkg_gradient_ptr->add_color_stop_rgba (1.0, 0.1, 0.1, 0.2, 0.0); 
    cr->set_source(bkg_gradient_ptr);  
    cr->arc(0.0, 0.0, 1.0, 0.0, 2.0*M_PI);
    cr->fill();
    cr->restore();
    
    //Draw the fader icon
    cr->save();
    cr->set_source (m_image_surface_ptr, width/2 - m_image_surface_ptr->get_width()/2, yFaderPosition);
    cr->rectangle (width/2 - m_image_surface_ptr->get_width()/2, yFaderPosition, m_image_surface_ptr->get_width()+1,  m_image_surface_ptr->get_height()+1);
    cr->clip();
    cr->paint();
    cr->restore();
  }
  return true;  
}


//Mouse grab signal handlers
bool  FaderWidget::on_button_press_event(GdkEventButton* event)
{
  //Act in case mouse pointer is inside faderwidget
  Gtk::Allocation allocation = get_allocation();
  const int width = allocation.get_width();
  int x,y;
  get_pointer(x,y);
  if( x > width/2 - m_image_surface_ptr->get_width()/2 &&
      x < width/2 + m_image_surface_ptr->get_width()/2 &&
      y > yFaderPosition &&
      y < yFaderPosition + m_image_surface_ptr->get_height())
  {

    if (!bMotionIsConnected)
    {
      m_motion_connection = signal_motion_notify_event().connect(sigc::mem_fun(*this, &FaderWidget::on_mouse_motion_event),true);
      bMotionIsConnected = true;
    }
  }
  return true;
}

bool  FaderWidget::on_button_release_event(GdkEventButton* event)
{
  m_motion_connection.disconnect();
  bMotionIsConnected = false;
  return true;
}

bool  FaderWidget::on_scrollwheel_event(GdkEventScroll* event)
{
  double increment;
  
  increment =  SCROLL_EVENT_PERCENT*(m_max - m_min);
  if (event->direction == GDK_SCROLL_UP) 
  { 
    // up code
    set_value (m_value + increment);

  } 
  else if (event->direction == GDK_SCROLL_DOWN) 
  { 
    // down code 
    set_value(m_value - increment);
  }
  m_FaderChangedSignal.emit();
  return true;
}

bool  FaderWidget::on_mouse_motion_event(GdkEventMotion* event)
{
    int yPixels;
    double m, n, fader_pos;
    Gtk::Allocation allocation = get_allocation();
    const int height = allocation.get_height();

    yPixels = event->y - m_image_surface_ptr->get_height()/2; //Offset fader icon to grab the center of the fader

    //Stoppers
    yPixels = yPixels < FADER_MARGIN ? FADER_MARGIN : yPixels;
    yPixels = yPixels > height - FADER_MARGIN - m_image_surface_ptr->get_height() ? height - FADER_MARGIN - m_image_surface_ptr->get_height() : yPixels;

    
    m = ((double)(FADER_MARGIN  + TITLE_OFFSET - (height - FADER_MARGIN - m_image_surface_ptr->get_height())))/(m_max - m_min);
    n = (double)(height - FADER_MARGIN - m_image_surface_ptr->get_height()) - m_min*m;
    
    fader_pos = ((double)yPixels - n)/m;

    //snap to 0 dB a little bit
    if(fader_pos < 0.5 && fader_pos > -0.5)
    {
      fader_pos = 0.0; 
    }
    set_value(fader_pos);
    m_FaderChangedSignal.emit();
    return true;
}

 FaderWidget::signal_FaderChanged  FaderWidget::signal_changed()
 {
  return m_FaderChangedSignal;
 }