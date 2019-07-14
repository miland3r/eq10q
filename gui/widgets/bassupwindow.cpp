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

#include <stdlib.h>
#include <iostream>

#include <cstring>
#include "bassupwindow.h"


#define KNOB_ICON_FILE "/knobs/knob_bassup_84px.png"
#define KNOB_SIZE_X 84
#define KNOB_SIZE_Y 86
#define WIDGET_BORDER 3
#define LOGO_PATH "icons/logobassup.png"

BassUpMainWindow::BassUpMainWindow(const char *uri, std::string bundlePath)
  :m_pluginUri(uri),
  m_bundlePath(bundlePath)
{ 
  m_Amount = Gtk::manage(new KnobWidget2(0.0, 6.0, "Amount", "", (m_bundlePath + KNOB_ICON_FILE).c_str() ));
 
  //load image logo
  image_logo = new Gtk::Image(m_bundlePath + "/" + LOGO_PATH);
  m_KnobAlign.add(*m_Amount);
  m_KnobAlign.set(0.5, 0.5, 0.0, 0.0);
  m_Box.pack_start(*image_logo, Gtk::PACK_SHRINK );
  m_Box.pack_start(m_KnobAlign, Gtk::PACK_SHRINK );
  m_Box.show_all_children();
  m_Box.show();
  m_MainWidgetAlign.set_padding(3,3,3,3);
  m_MainWidgetAlign.add(m_Box);
  add(m_MainWidgetAlign);
  m_MainWidgetAlign.show();
  
 
  //Connect signals
  m_Amount->signal_changed().connect(sigc::mem_fun(*this, &BassUpMainWindow::onAmountChange));
}

BassUpMainWindow::~BassUpMainWindow()
{
  delete m_Amount;
}

void BassUpMainWindow::onAmountChange()
{ 
  //Write to LV2 port
  float aux;
  aux = m_Amount->get_value();
  write_function(controller, PORT_AMOUNT, sizeof(float), 0, &aux);
}