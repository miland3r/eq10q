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

#ifndef FADER_WIDGET_H
  #define FADER_WIDGET_H

#include <gtkmm/drawingarea.h>
#include <gdkmm/pixbuf.h>
#include <glibmm/refptr.h>
#include <cairomm/surface.h>

#define FADER_ICON_FILE "knobs/fader_dark.png"
#define FADER_INITAL_HIGHT 350
#define FADER_MARGIN 7
#define SCROLL_EVENT_PERCENT 0.02
#define TITLE_OFFSET 12

class  FaderWidget : public Gtk::DrawingArea
{
  public:
    FaderWidget(double dMax, double dMin, const char *bundlePath, Glib::ustring title);
    virtual ~FaderWidget();
    
    //Data accessors
    void set_value(double value);
    double get_value();
    
    void set_range(double max, double min);
    double get_max();
    double get_min();
    
    //signal accessor:
    typedef sigc::signal<void> signal_FaderChanged;
    signal_FaderChanged signal_changed();
    
  protected:
      //Override default signal handler:
      virtual bool on_expose_event(GdkEventExpose* event);
      
      //Mouse grab signal handlers
      virtual bool on_button_press_event(GdkEventButton* event);
      virtual bool on_button_release_event(GdkEventButton* event);
      virtual bool on_scrollwheel_event(GdkEventScroll* event);
      virtual bool on_mouse_motion_event(GdkEventMotion* event);
      
      void redraw();
      
  private:
      bool bMotionIsConnected;
      int yFaderPosition;
      double m_value, m_max, m_min;
      sigc::connection m_motion_connection;
      Cairo::RefPtr<Cairo::ImageSurface> m_image_surface_ptr;
      Glib::RefPtr<Gdk::Pixbuf> m_image_ptr;
      Cairo::RefPtr< Cairo::Context> m_image_context_ptr;
      std::string m_bundlePath;
      Glib::ustring m_title;
      
      //Fader change signal
      signal_FaderChanged m_FaderChangedSignal;
};
#endif