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
 
#include "abbutton.h"
#include "colors.h"

#define TXT_MARGIN 10

AbButton::AbButton(): ToggleButton()
{
  set_size_request( 60 ,20);
}

AbButton::~AbButton()
{

}

bool AbButton::on_expose_event(GdkEventExpose* event)
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
    
    //Draw a box
    cr->save();
    cr->begin_new_sub_path();
    cr->arc( 3 + 0.5, 3  + 0.5, 3, M_PI, -0.5*M_PI);
    cr->arc( width - 1 - 3 - 0.5, 3  + 0.5, 3, -0.5*M_PI, 0);
    cr->arc( width - 1 - 3 - 0.5, height - 1 - 3  - 0.5, 3, 0.0,  0.5*M_PI);
    cr->arc( 3  + 0.5, height - 1 - 3  - 0.5, 3, 0.5*M_PI, M_PI);
    cr->close_path();
    cr->set_source_rgba(0.1,0.1,0.1,0.8);
    cr->fill_preserve();
    if(m_bFocus)
    {
      cr->set_line_width(1.5);
      cr->set_source_rgba(0,1,1, 0.6);
      cr->stroke_preserve();
    }
    cr->set_line_width(1);
    cr->set_source_rgba(1,1,1, 0.4);
    cr->stroke();
    cr->restore();
    
    //Draw the text
    cr->save();
    Glib::RefPtr<Pango::Layout> pangoLayout = Pango::Layout::create(cr);
    Pango::FontDescription font_desc("sans 12px");
    pangoLayout->set_font_description(font_desc);
    pangoLayout->set_text("B");
    cr->move_to(TXT_MARGIN, height/2 - 7);
    cr->set_source_rgba(0.9, 0.9, 0.9, 0.8);
    pangoLayout->show_in_cairo_context(cr);
    cr->stroke();  
    pangoLayout->set_text("A");
    cr->move_to(width/2 + TXT_MARGIN, height/2 - 7);
    cr->set_source_rgba(0.9, 0.9, 0.9, 0.8);
    pangoLayout->show_in_cairo_context(cr);
    cr->stroke();  
    cr->restore();

    //Draw the button
    cr->save();
    cr->begin_new_sub_path();
    double x = 0;
    if(!m_bActive)
    {
      x = width/2;
    }
    cr->arc( 5 + 0.5 + x, 5  + 0.5, 3, M_PI, -0.5*M_PI);
    cr->arc( width/2 - 1 - 5 - 0.5 + x, 5  + 0.5, 3, -0.5*M_PI, 0);
    cr->arc( width/2 - 1 - 5 - 0.5 + x, height - 1 - 5  - 0.5, 3, 0.0,  0.5*M_PI);
    cr->arc( 5  + 0.5 + x, height - 1 - 5  - 0.5, 3, 0.5*M_PI, M_PI);
    cr->close_path();
    
    Cairo::RefPtr<Cairo::LinearGradient> bkg_gradient_ptr = Cairo::LinearGradient::create(width/2, 0, width/2, height - 1); 
    bkg_gradient_ptr->add_color_stop_rgba (0.0, 0.2, 0.2, 0.2, 1 );  
    bkg_gradient_ptr->add_color_stop_rgba (0.4, 0.5, 0.5, 0.5, 1 );
    bkg_gradient_ptr->add_color_stop_rgba (0.6, 0.5, 0.5, 0.5, 1 );  
    bkg_gradient_ptr->add_color_stop_rgba (1.0, 0.3, 0.3, 0.3, 1 );  
    cr->set_source(bkg_gradient_ptr);                       
    cr->fill_preserve();
    cr->set_line_width(1);
    cr->set_source_rgba(1,1,1, 0.5);
    cr->stroke();
    cr->restore();
    
    //Draw some vertical lines on button
    cr->save();
    cr->move_to(8 + 0.5 + x, 5  + 0.5);
    cr->line_to(8 + 0.5 + x,  height - 1 - 5  - 0.5);
    cr->move_to(12 + 0.5 + x, 5  + 0.5);
    cr->line_to(12 + 0.5 + x,  height - 1 - 5  - 0.5);
    cr->move_to(16 + 0.5 + x, 5  + 0.5);
    cr->line_to(16 + 0.5 + x,  height - 1 - 5  - 0.5);
    cr->move_to(20 + 0.5 + x, 5  + 0.5);
    cr->line_to(20 + 0.5 + x,  height - 1 - 5  - 0.5);
    cr->set_line_width(1);
    cr->set_source_rgba(1,1,1,0.4);
    cr->stroke();
    cr->restore();
  }
  return true;
}
