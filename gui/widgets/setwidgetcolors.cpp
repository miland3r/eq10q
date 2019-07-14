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

#include "setwidgetcolors.h"
#include "colors.h"

SetWidgetColors::SetWidgetColors()
{
  m_Button_BgColorNormal.set_rgb(GDK_COLOR_MACRO(BUTTON_BACKGROUND_R), GDK_COLOR_MACRO(BUTTON_BACKGROUND_G), GDK_COLOR_MACRO(BUTTON_BACKGROUND_B));
  m_Button_BgColorActive.set_rgb(GDK_COLOR_MACRO(BUTTON_ACTIVE_BG_R), GDK_COLOR_MACRO(BUTTON_ACTIVE_BG_G), GDK_COLOR_MACRO(BUTTON_ACTIVE_BG_B));
  m_Button_BgColorInactive.set_rgb(GDK_COLOR_MACRO(BUTTON_INACTIVE_BG_R), GDK_COLOR_MACRO(BUTTON_INACTIVE_BG_G), GDK_COLOR_MACRO(BUTTON_INACTIVE_BG_B));
  m_Button_BgColorOver.set_rgb(GDK_COLOR_MACRO(BUTTON_OVER_BG_R), GDK_COLOR_MACRO(BUTTON_OVER_BG_G), GDK_COLOR_MACRO(BUTTON_OVER_BG_B));
  m_Button_FgColor.set_rgb(GDK_COLOR_MACRO( FOREGROUND_R ), GDK_COLOR_MACRO( FOREGROUND_G ), GDK_COLOR_MACRO( FOREGROUND_B ));
  m_Button_TextColor.set_rgb(GDK_COLOR_MACRO( TEXT_R ), GDK_COLOR_MACRO( TEXT_G ), GDK_COLOR_MACRO( TEXT_B ));
  
  for(int i = 0; i<10; i++)
  {
    m_BandsColors[i].set(bandColorLUT[i]);
  }
  
  //Initialize the ButtonStyle Object
  PlainButtonStyle = Gtk::Style::create(); 
  
  //Backround colors
  PlainButtonStyle->set_bg(Gtk::STATE_NORMAL, m_Button_BgColorNormal);
  PlainButtonStyle->set_bg(Gtk::STATE_ACTIVE, m_Button_BgColorActive);
  PlainButtonStyle->set_bg(Gtk::STATE_INSENSITIVE, m_Button_BgColorInactive);
  PlainButtonStyle->set_bg(Gtk::STATE_PRELIGHT, m_Button_BgColorOver);
  PlainButtonStyle->set_bg(Gtk::STATE_SELECTED, m_Button_BgColorNormal);
  
  //Base colors
  PlainButtonStyle->set_base(Gtk::STATE_NORMAL, m_Button_BgColorNormal);
  PlainButtonStyle->set_base(Gtk::STATE_ACTIVE, m_Button_BgColorActive);
  PlainButtonStyle->set_base(Gtk::STATE_INSENSITIVE, m_Button_BgColorInactive);
  PlainButtonStyle->set_base(Gtk::STATE_PRELIGHT, m_Button_BgColorOver);
  PlainButtonStyle->set_base(Gtk::STATE_SELECTED, m_Button_BgColorNormal);
  
  //Foreground
  PlainButtonStyle->set_fg(Gtk::STATE_NORMAL, m_Button_FgColor);
  PlainButtonStyle->set_fg(Gtk::STATE_ACTIVE, m_Button_FgColor);
  PlainButtonStyle->set_fg(Gtk::STATE_INSENSITIVE, m_Button_FgColor);
  PlainButtonStyle->set_fg(Gtk::STATE_PRELIGHT, m_Button_FgColor);
  PlainButtonStyle->set_fg(Gtk::STATE_SELECTED, m_Button_FgColor);
  
  //Text colors
  PlainButtonStyle->set_text(Gtk::STATE_NORMAL, Gdk::Color("#FFFFFF"));
  PlainButtonStyle->set_text(Gtk::STATE_ACTIVE,  Gdk::Color("#FFFFFF"));
  PlainButtonStyle->set_text(Gtk::STATE_INSENSITIVE,  Gdk::Color("#FFFFFF"));
  PlainButtonStyle->set_text(Gtk::STATE_PRELIGHT,  Gdk::Color("#FFFFFF"));
  PlainButtonStyle->set_text(Gtk::STATE_SELECTED,  Gdk::Color("#FFFFFF"));  
}

Glib::RefPtr< Gtk::Style > SetWidgetColors::getPlainButtonStyle()
{
  return PlainButtonStyle;
}


void SetWidgetColors::setButtonColors(Gtk::Button* widget)
{
  widget->modify_bg(Gtk::STATE_NORMAL, m_Button_BgColorNormal);
  widget->modify_bg(Gtk::STATE_ACTIVE, m_Button_BgColorActive);
  widget->modify_bg(Gtk::STATE_INSENSITIVE, m_Button_BgColorInactive);
  widget->modify_bg(Gtk::STATE_PRELIGHT, m_Button_BgColorOver);
  widget->modify_bg(Gtk::STATE_SELECTED, m_Button_BgColorNormal);

  widget->modify_fg(Gtk::STATE_NORMAL, m_Button_FgColor);
  widget->modify_fg(Gtk::STATE_ACTIVE, m_Button_FgColor);
  widget->modify_fg(Gtk::STATE_INSENSITIVE, m_Button_FgColor);
  widget->modify_fg(Gtk::STATE_PRELIGHT, m_Button_FgColor);
  widget->modify_fg(Gtk::STATE_SELECTED, m_Button_FgColor);

  widget->get_child()->modify_fg(Gtk::STATE_NORMAL, m_Button_TextColor);
  widget->get_child()->modify_fg(Gtk::STATE_ACTIVE, m_Button_TextColor);
  widget->get_child()->modify_fg(Gtk::STATE_INSENSITIVE, m_Button_TextColor);
  widget->get_child()->modify_fg(Gtk::STATE_PRELIGHT, m_Button_TextColor);
  widget->get_child()->modify_fg(Gtk::STATE_SELECTED, m_Button_TextColor);
}

void SetWidgetColors::setGenericWidgetColors(Gtk::Widget* widget)
{
  widget->modify_bg(Gtk::STATE_NORMAL, m_Button_BgColorNormal);
  widget->modify_bg(Gtk::STATE_ACTIVE, m_Button_BgColorActive);
  widget->modify_bg(Gtk::STATE_INSENSITIVE, m_Button_BgColorInactive);
  widget->modify_bg(Gtk::STATE_PRELIGHT, m_Button_BgColorOver);

  widget->modify_fg(Gtk::STATE_NORMAL, m_Button_FgColor);
  widget->modify_fg(Gtk::STATE_ACTIVE, m_Button_FgColor);
  widget->modify_fg(Gtk::STATE_INSENSITIVE, m_Button_FgColor);
  widget->modify_fg(Gtk::STATE_PRELIGHT, m_Button_FgColor);
}

void SetWidgetColors::setBandFrameColor(Gtk::Frame* widget, int band)
{
  widget->modify_bg(Gtk::STATE_NORMAL,  m_BandsColors[band]);
  widget->modify_bg(Gtk::STATE_ACTIVE,  m_BandsColors[band]);
  widget->modify_bg(Gtk::STATE_INSENSITIVE,  m_BandsColors[band]);
  widget->modify_bg(Gtk::STATE_PRELIGHT,  m_BandsColors[band]);
  
  widget->modify_fg(Gtk::STATE_NORMAL,  m_BandsColors[band]);
  widget->modify_fg(Gtk::STATE_ACTIVE,  m_BandsColors[band]);
  widget->modify_fg(Gtk::STATE_INSENSITIVE,  m_BandsColors[band]);
  widget->modify_fg(Gtk::STATE_PRELIGHT,  m_BandsColors[band]);

  widget->get_label_widget()->modify_fg(Gtk::STATE_NORMAL,  m_BandsColors[band]);
  widget->get_label_widget()->modify_fg(Gtk::STATE_ACTIVE,  m_BandsColors[band]);
  widget->get_label_widget()->modify_fg(Gtk::STATE_INSENSITIVE,  m_BandsColors[band]);
  widget->get_label_widget()->modify_fg(Gtk::STATE_PRELIGHT,  m_BandsColors[band]);
}




