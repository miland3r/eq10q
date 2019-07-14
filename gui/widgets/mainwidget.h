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
 

#ifndef MAIN_WIDGET_H
  #define MAIN_WIDGET_H

#include <gtkmm/eventbox.h>

class MainWidget : public Gtk::EventBox
{
  public:
    MainWidget();
    virtual ~MainWidget();
    
  protected:
    //Override default signal handler:
    virtual bool on_expose_event(GdkEventExpose* event);
    
    virtual void on_realize();
    Gdk::Color m_bg_orgi;
};

#endif