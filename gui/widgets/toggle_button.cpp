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
 
#include "toggle_button.h"
#include "colors.h"

#define OUTER_BORDER 3
 
ToggleButton::ToggleButton ( const Glib::ustring& label )
:Button ( label ),
m_bActive(false)
{

}

ToggleButton::~ToggleButton()
{

}

bool ToggleButton::on_button_release_event ( GdkEventButton* event )
{
    if( event->x > OUTER_BORDER &&
    event->x < (width - OUTER_BORDER) &&
    event->y > OUTER_BORDER &&
    event->y < (height - OUTER_BORDER))
  {
    m_bActive = !m_bActive;
    m_sigClick.emit();  
  }

  m_bPress = false;
  redraw();
  return true;
}

bool ToggleButton::get_active()
{
  return m_bActive;
}

void ToggleButton::set_active ( bool active )
{
  m_bActive = active;
  redraw();
}

bool ToggleButton::on_expose_event(GdkEventExpose* event)
{
 Glib::RefPtr<Gdk::Window> window = get_window();
  if(window)
  {
    Gtk::Allocation allocation = get_allocation();
    width = allocation.get_width();
    height = allocation.get_height();
    
    Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();

    //Paint backgroud
    cr->save();
    cr->set_source_rgb(BACKGROUND_R, BACKGROUND_G, BACKGROUND_B);
    cr->paint(); //Fill all with background color
    cr->restore();
    
    /*
    //Draw a box
    cr->save();
    cr->begin_new_sub_path();
    cr->arc( 3 + 0.5, 3  + 0.5, 3, M_PI, -0.5*M_PI);
    cr->arc( width - 1 - 3 - 0.5, 3  + 0.5, 3, -0.5*M_PI, 0);
    cr->arc( width - 1 - 3 - 0.5, height - 1 - 3  - 0.5, 3, 0.0,  0.5*M_PI);
    cr->arc( 3  + 0.5, height - 1 - 3  - 0.5, 3, 0.5*M_PI, M_PI);
    cr->close_path();
    cr->set_line_width(1);
    cr->set_source_rgba(1,1,1, 0.3);
    cr->stroke();
    cr->restore();
    */
    
    //The button
    ToggleButton::drawLedBtn(cr, m_bFocus, m_bActive, m_label.c_str(),  OUTER_BORDER, 3);
  }
  
  return true;
}


void ToggleButton::drawLedBtn(Cairo::RefPtr< Cairo::Context > cr, bool focus, bool enabled, std::string text, int margin, int radius, double red, double green, double blue)
{
  //Draw the FFT enable button
  cr->save();
  cr->begin_new_sub_path();
  cr->arc( margin + radius + 0.5, margin + radius  + 0.5, radius, M_PI, -0.5*M_PI);
  cr->arc( margin + 3*radius  + 0.5, margin + radius  + 0.5, radius, -0.5*M_PI, 0);
  cr->arc( margin + 3*radius  + 0.5, margin + 3*radius  + 0.5, radius, 0.0,  0.5*M_PI);
  cr->arc( margin + radius  + 0.5, margin + 3*radius  + 0.5, radius, 0.5*M_PI, M_PI);
  cr->close_path();
  
  //Daw focus on LED
  if(focus)
  {
    cr->set_line_width(3.5);
    cr->set_source_rgba(0.0, 1.0, 1.0, 0.5);
    cr->stroke_preserve();
    cr->set_source_rgb(0.1, 0.1, 0.1);
    cr->fill_preserve();
  }    
  
  Cairo::RefPtr< Cairo::RadialGradient > bkg_gradient_ptr = Cairo::RadialGradient::create( margin + 2 * radius - 2,  margin + 2 * radius - 2, 0, margin + 2 * radius,  margin + 2 * radius, radius*3);
  double alpha = 0.3;
  if(enabled)
  {
    alpha = 0.8;
  }
  bkg_gradient_ptr->add_color_stop_rgba (0.3, red, green, blue, alpha); 
  bkg_gradient_ptr->add_color_stop_rgba (1.0, 0.7, 0.4, 0.0, alpha); 
  cr->set_source(bkg_gradient_ptr);  
  cr->fill_preserve();
      
  cr->set_line_width(1.0);
  cr->set_source_rgba(0.1, 0.1, 0.1, 1.0);
  cr->stroke();
  cr->restore();

  //Draw extra birgthness on FFT LED
  if(enabled)
  {
    cr->save();
    cr->arc( margin + 2 * radius + 0.5,  margin + 2 * radius + 0.5, 4*radius, 0.0, 2.0*M_PI);
    bkg_gradient_ptr = Cairo::RadialGradient::create( margin + 2 * radius,  margin + 2 * radius, 0, margin + 2 * radius,  margin + 2 * radius, radius*4);   
    bkg_gradient_ptr->add_color_stop_rgba (0.0, 1.0, 1.0, 1.0, 0.4); 
    bkg_gradient_ptr->add_color_stop_rgba (1.0, 1.0, 1.0, 1.0, 0.0); 
    cr->set_source(bkg_gradient_ptr); 
    cr->fill();
    cr->restore();
  }
  
  //Draw Text FFT
  cr->save();
  Glib::RefPtr<Pango::Layout> pangoLayout = Pango::Layout::create(cr);
  Pango::FontDescription font_desc("sans 11px");
  pangoLayout->set_font_description(font_desc);
  pangoLayout->set_text(text.c_str());
  //a shadow
  cr->move_to(5+margin + 4*radius + 1, margin + 2*radius - 5);
  cr->set_source_rgba(0.1, 0.1, 0.1, 0.9);
  pangoLayout->show_in_cairo_context(cr);
  cr->stroke(); 
  //and text
  cr->move_to(5+margin + 4*radius, margin + 2*radius - 6);
  cr->set_source_rgba(0.9, 0.9, 0.9, 0.7);
  pangoLayout->show_in_cairo_context(cr);
  cr->stroke();  
  cr->restore();
}


