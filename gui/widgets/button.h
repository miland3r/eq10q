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

#ifndef EQ10Q_BUTTON_H
  #define EQ10Q_BUTTON_H

#include <gtkmm/drawingarea.h>


class Button : public Gtk::DrawingArea
{
  public:
    Button(const Glib::ustring& label);
    virtual ~Button();
    void set_label(const Glib::ustring& label); 
  
    //Slot prototype: void signal_clicked();
    typedef sigc::signal<void> signal_Click;      
    signal_Click signal_clicked();
    signal_Click signal_press();
    signal_Click signal_release();
    
  protected:
    //Override default signal handler:
    virtual bool on_expose_event(GdkEventExpose* event);
    virtual bool on_mouse_motion_event(GdkEventMotion* event);
    virtual bool on_button_press_event(GdkEventButton* event);
    virtual bool on_button_release_event(GdkEventButton* event);
    virtual bool on_mouse_leave_widget(GdkEventCrossing* event);
    void redraw();
    
    Glib::ustring m_label; 
    bool m_bFocus, m_bPress;
    int width, height;
    signal_Click m_sigClick;
    signal_Click m_sigPress;
    signal_Click m_sigRelease;
};
#endif