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

#ifndef SET_WIDGET_COLORS_H
  #define SET_WIDGET_COLORS_H

#include <gtkmm/button.h>
#include <gtkmm/frame.h>
#include <gtkmm/widget.h>

class SetWidgetColors
{
  public:
    SetWidgetColors();
    void setButtonColors(Gtk::Button* widget);
    void setGenericWidgetColors(Gtk::Widget* widget);
    void setBandFrameColor(Gtk::Frame* widget, int band);
    Glib::RefPtr<Gtk::Style> getPlainButtonStyle();
    
  private:
    Gdk::Color m_Button_BgColorActive, m_Button_BgColorInactive, m_Button_BgColorNormal, m_Button_BgColorOver, m_Button_FgColor, m_Button_TextColor;
    Gdk::Color m_BandsColors[10];
    Glib::RefPtr<Gtk::Style> PlainButtonStyle;
};
#endif