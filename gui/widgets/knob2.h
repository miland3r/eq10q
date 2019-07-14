/***************************************************************************
 *   Copyright (C) 2011 by Pere Rafols Soler                               *
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

#ifndef KNOB_WIDGET2_H
#define KNOB_WIDGET2_H

#include <cmath>
#include <string>
#include <vector>
#include <gtkmm/drawingarea.h>
#include <gdkmm/pixbuf.h>
#include <glibmm/refptr.h>
#include <cairomm/surface.h>

#define KNOB_TYPE_LIN 0
#define KNOB_TYPE_FREQ 1
#define KNOB_TYPE_TIME 2

class KnobWidget2 : public Gtk::DrawingArea
{
  public:
  KnobWidget2(float fMin, float fMax, std::string sLabel, std::string sUnits, const char *knobIconPath, int iType = KNOB_TYPE_LIN, bool snap2ZerodB = false);
  ~KnobWidget2();
  void set_value(float fValue);
  double get_value();
    
  //signal accessor:
  typedef sigc::signal<void> signal_KnobChanged;
  signal_KnobChanged signal_changed();
  
  
  protected:
    //Override default signal handler:
    virtual bool on_expose_event(GdkEventExpose* event);
    virtual bool on_mouse_leave_widget(GdkEventCrossing* event);
    void redraw();
	
    //Mouse grab signal handlers
    virtual bool on_button_press_event(GdkEventButton* event);
    virtual bool on_button_release_event(GdkEventButton* event);
    virtual bool on_scrollwheel_event(GdkEventScroll* event);
    virtual bool on_mouse_motion_event(GdkEventMotion* event);   
    
    float m_fMin; //Min representable value
    float m_fMax; //Max representable value
    bool bMotionIsConnected;
    float m_Value;
    std::string m_Label;
    std::string m_Units;
    int m_TypeKnob;

    int width;
    int height;
    int mouse_move_ant;
    bool m_snap2Zero;
    bool m_focus;
    double m_slowMultiplier;
      
    //Fader change signal
    signal_KnobChanged m_KnobChangedSignal; 
    
  private:
    std::string m_knobIconPath;
    Cairo::RefPtr<Cairo::ImageSurface> m_image_surface_ptr;
    Glib::RefPtr<Gdk::Pixbuf> m_image_ptr;
    Cairo::RefPtr< Cairo::Context> m_image_context_ptr;
};
#endif