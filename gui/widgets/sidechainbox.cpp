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
 
#include "sidechainbox.h"
#include "colors.h"

#define MARGIN 6
#define RADIUS 4

SideChainBox::SideChainBox( std::string sTitle, int top_padding): m_title(sTitle), m_top_padding(top_padding)
{

}

SideChainBox::~SideChainBox()
{

}

void SideChainBox::set_label(const Glib::ustring& label)
{
  m_title = label; 
  Glib::RefPtr<Gdk::Window> win = get_window();
  if(win)
  {
    Gdk::Rectangle r(0, 0, get_allocation().get_width(), get_allocation().get_height());
    win->invalidate_rect(r, false);
  }
}

bool SideChainBox::on_expose_event(GdkEventExpose* event)
{
  bool ret = Gtk::EventBox::on_expose_event(event); //Call parent redraw()
  
  Glib::RefPtr<Gdk::Window> window = get_window();
  if(window)
  {
    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
    const int height = allocation.get_height();
     
    Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();

    //Paint backgroud
    cr->save();
    cr->set_source_rgb(BACKGROUND_R, BACKGROUND_G, BACKGROUND_B);
    cr->paint(); //Fill all with background color
    cr->restore();
    
    //Draw a box
    cr->save();
    cr->arc( MARGIN + 0.5, MARGIN + m_top_padding + 0.5, RADIUS, M_PI, -0.5*M_PI);
    cr->line_to(width/6 , MARGIN + m_top_padding + 0.5 - RADIUS);
    cr->move_to(5*width/6 , MARGIN + m_top_padding + 0.5 - RADIUS);
    cr->line_to(width - 1 - MARGIN - 0.5, MARGIN + m_top_padding + 0.5 - RADIUS);
    cr->arc( width - 1 - MARGIN - 0.5, MARGIN + m_top_padding + 0.5, RADIUS, -0.5*M_PI, 0);
    cr->line_to(width - 1 - MARGIN - 0.5 + RADIUS, height - 1 - MARGIN  - 0.5);
    cr->arc( width - 1 - MARGIN - 0.5, height - 1 - MARGIN  - 0.5, RADIUS, 0.0,  0.5*M_PI);
    cr->line_to( MARGIN + 0.5, height - 1 - MARGIN  - 0.5 + RADIUS);
    cr->arc( MARGIN  + 0.5, height - 1 - MARGIN  - 0.5, RADIUS, 0.5*M_PI, M_PI);
    cr->line_to( MARGIN + 0.5 - RADIUS,  MARGIN + m_top_padding + 0.5 );
    
    cr->set_line_width(1);
    cr->set_source_rgba(1,1,1, 0.3);
    cr->stroke();
    cr->restore();
    
    //Draw Text
    cr->save();
    Glib::RefPtr<Pango::Layout> pangoLayout = Pango::Layout::create(cr);
    Pango::FontDescription font_desc("sans 12px");
    pangoLayout->set_font_description(font_desc);
    pangoLayout->set_text(m_title);

    int stringWidth, stringHeight;
    pangoLayout->get_pixel_size(stringWidth, stringHeight);

    //and text
    cr->move_to(0.5*(width - stringWidth),  m_top_padding - 0.5*stringHeight );
    cr->set_source_rgba(0.9, 0.9, 0.9, 0.7);
    pangoLayout->show_in_cairo_context(cr);
    cr->stroke();  
    cr->restore();
  }

  return ret;
}


