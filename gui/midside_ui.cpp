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

/***************************************************************************
This file contains some definitions of the BassUp plugin UI
This plugin is inside the Sapista Plugins Bundle
****************************************************************************/

//LV2 UI headers
#include <lv2/lv2plug.in/ns/extensions/ui/ui.h>
#include <gtkmm/main.h>
#include "widgets/midsidewindow.h"
#include <string>
#include "../plugins_uris.h"

//Testing Headers TODO: comment define TESTING_EQ10Q for the final relase
//#define TESTING_EQ10Q
#ifdef TESTING_EQ10Q
#include <iostream>
using namespace std;
#endif

static LV2UI_Handle instantiateMidSide_gui(const _LV2UI_Descriptor *descriptor, const char *plugin_uri, const char *bundle_path, LV2UI_Write_Function write_function, LV2UI_Controller controller, LV2UI_Widget *widget, const LV2_Feature *const *features)
{
  #ifdef TESTING_EQ10Q
  cout<<"instantiateEq10q_gui Entring... ";
  #endif
  
  bool bIsLR2MS;
  bool bAllOk = false;
  
  std::string str_plugin_uri(plugin_uri);
  if( str_plugin_uri == LR2MS_URI )
  {
    bIsLR2MS = true;
    bAllOk = true;
  }
  if( str_plugin_uri == MS2LR_URI )
  {
    bIsLR2MS = false;
    bAllOk = true;
  }
  if(! bAllOk)
  {
    return NULL;
  }
  
  Gtk::Main::init_gtkmm_internals();
  MidSideMainWindow* gui_data = new MidSideMainWindow(plugin_uri, std::string(bundle_path), bIsLR2MS);
  gui_data->controller = controller;
  gui_data->write_function = write_function;
  *widget = gui_data->gobj();

  #ifdef TESTING_EQ10Q
  cout<<" Done"<<endl;
  #endif
  
  return (LV2UI_Handle)gui_data;
}


static void cleanupMidSide_gui(LV2UI_Handle instance)
{
  #ifdef TESTING_EQ10Q
  cout<<"cleanupEq10q_gui Entring... ";
  #endif
  
  ///delete static_cast<MidSideMainWindow*>(instance);
  MidSideMainWindow *gui = (MidSideMainWindow *)instance;
  delete gui;
  
  #ifdef TESTING_EQ10Q
  cout<<" Done"<<endl;
  #endif
}

static void portEventMidSide_gui(LV2UI_Handle ui, uint32_t port_index, uint32_t buffer_size, uint32_t format, const void *buffer)
{
  #ifdef TESTING_EQ10Q
  cout<<"portEventEq10q_gui Entring... "<<"Port Index = "<<port_index;
  #endif
  
  MidSideMainWindow *gui = (MidSideMainWindow *)ui;
  gui->gui_port_event(ui, port_index, buffer_size, format, buffer);
  
  #ifdef TESTING_EQ10Q
  cout<<" Done"<<endl;
  #endif
}

static const LV2UI_Descriptor MidSide_guiDescriptor = {
  MIDSIDEMAT_GUI_URI,
  instantiateMidSide_gui,
  cleanupMidSide_gui,
  portEventMidSide_gui,
  NULL
};

LV2_SYMBOL_EXPORT
const LV2UI_Descriptor *lv2ui_descriptor(uint32_t index)
{
  #ifdef TESTING_EQ10Q
  cout<<"lv2ui_descriptor Entring... ";
  #endif
  
    switch (index) {
            case 0:
                    #ifdef TESTING_EQ10Q
                    cout<<" Done with OK result (return LV2UI_Descriptor)"<<endl;
                    #endif
                    return &MidSide_guiDescriptor;
            default:
                    #ifdef TESTING_EQ10Q
                    cout<<" Done with NOK result (return NULL)"<<endl;
                    #endif
                    return NULL;
    }
    

}
