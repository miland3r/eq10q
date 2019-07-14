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

#include "mainwidget.h"
#include "colors.h"
#include <gtkmm/window.h>

#define BORDER 3
#define RADIUS 8

MainWidget::MainWidget()
{
  set_border_width(BORDER);  
}


MainWidget::~MainWidget()
{

}

void MainWidget::on_realize()
{
  Gtk::EventBox::on_realize();
  Glib::RefPtr<Gtk::Style> m_style =  get_style();
  m_bg_orgi = m_style->get_bg(Gtk::STATE_NORMAL);
  
  //Set Main widget Background
  Gdk::Color m_WinBgColor;
  m_WinBgColor.set_rgb(GDK_COLOR_MACRO( BACKGROUND_R ), GDK_COLOR_MACRO( BACKGROUND_G ), GDK_COLOR_MACRO( BACKGROUND_B ));
  modify_bg(Gtk::STATE_NORMAL, m_WinBgColor); 
  
  Gtk::Window* toplevel = dynamic_cast<Gtk::Window *>(this->get_toplevel()); 
  toplevel->set_resizable(false);  
}


bool MainWidget::on_expose_event(GdkEventExpose* event)
{
  bool ret = Gtk::EventBox::on_expose_event(event); //Call parent redraw()
  
  Glib::RefPtr<Gdk::Window> window = get_window();
  if(window)
  {
    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width() - 2*BORDER;
    const int height = allocation.get_height() - 2*BORDER;
    
    Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
    

    //Paint edges
    cr->save();
    cr->begin_new_sub_path();
    cr->arc( RADIUS, RADIUS, RADIUS, M_PI, -0.5*M_PI);
    cr->arc( width - RADIUS - 1, RADIUS, RADIUS, -0.5*M_PI, 0);
    cr->arc( width - RADIUS - 1, height - RADIUS - 1, RADIUS, 0.0,  0.5*M_PI);
    cr->arc( RADIUS, height - RADIUS - 1, RADIUS, 0.5*M_PI, M_PI);
    cr->line_to(0,height);
    cr->line_to(width,height);
    cr->line_to(width,0);
    cr->line_to(0,0);
    cr->close_path();
    cr->set_source_rgb(m_bg_orgi.get_red_p(), m_bg_orgi.get_green_p(), m_bg_orgi.get_blue_p()  );
    cr->fill();
    cr->restore();
  
    //Draw a line
    cr->save();
    cr->begin_new_sub_path();
    cr->arc( RADIUS, RADIUS, RADIUS, M_PI, -0.5*M_PI);
    cr->arc( width - RADIUS - 1, RADIUS, RADIUS, -0.5*M_PI, 0);
    cr->arc( width - RADIUS - 1, height - RADIUS - 1, RADIUS, 0.0,  0.5*M_PI);
    cr->arc( RADIUS, height - RADIUS - 1, RADIUS, 0.5*M_PI, M_PI);
    cr->close_path();
    cr->set_line_width(1);
    cr->set_source_rgba(0,0.3,0.3, 0.9);
    cr->stroke();
    cr->restore();    
  }

  return ret;
}
