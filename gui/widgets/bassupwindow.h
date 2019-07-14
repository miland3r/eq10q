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

#ifndef BASSUP_MAIN_WIN_H
  #define BASSUP_MAIN_WIN_H

#include <iostream>
#include <string>

#include <gtkmm/alignment.h>
#include <gtkmm/box.h>
#include <gtkmm/image.h>

#include <cmath>

//LV2 UI header
#include <lv2/lv2plug.in/ns/extensions/ui/ui.h>

#include "mainwidget.h"
#include "knob2.h"

#define PORT_OUTPUT 0
#define PORT_INPUT 1
#define PORT_AMOUNT 2

using namespace sigc;

class BassUpMainWindow : public MainWidget
{
  public:
    BassUpMainWindow(const char *uri, std::string bundlePath);
    virtual ~BassUpMainWindow();   
    
    // Informing GUI about changes in the control ports
    void gui_port_event(LV2UI_Handle ui, uint32_t port, uint32_t buffer_size, uint32_t format, const void * buffer)
    {
      float data = * static_cast<const float*>(buffer);
      
        // Checking if params are the same as specified in the LV2 documentation
        if (format != 0) {
            return;
        }
        if (buffer_size != 4) {
            return;
        }

        // Updating values in GUI ========================================================
        switch (port)
        {
          case PORT_AMOUNT:
            m_Amount->set_value(data);
          break;
        }               
    }

    LV2UI_Controller controller;
    LV2UI_Write_Function write_function;

  protected:
    KnobWidget2 *m_Amount;
    Gtk::HBox m_Box;       
    Gtk::Alignment m_MainWidgetAlign, m_KnobAlign;
    Gtk::Image *image_logo;
        
    //Signal Handlers
    void onAmountChange();
    
  private:
    std::string m_pluginUri;
    std::string m_bundlePath;  
};

#endif