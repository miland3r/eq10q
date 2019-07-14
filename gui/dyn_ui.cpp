/***************************************************************************
 *   Copyright (C) 2011 by Pere Ràfols Soler                               *
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
This file contains some definitions of the EQ plugin UI
This plugin is inside the Sapista Plugins Bundle
****************************************************************************/

//LV2 UI headers
#include <lv2/lv2plug.in/ns/extensions/ui/ui.h>
#include <gtkmm/main.h>
#include "widgets/dynamicswindow.h"
#include <string>
#include "../plugins_uris.h"

//Testing Headers TODO: comment define TESTING_EQ10Q for the final relase
//#define TESTING_EQ10Q
#ifdef TESTING_EQ10Q
#include <iostream>
using namespace std;
#endif

static LV2UI_Handle instantiateDyn_gui(const _LV2UI_Descriptor *descriptor, const char *plugin_uri, const char *bundle_path, LV2UI_Write_Function write_function, LV2UI_Controller controller, LV2UI_Widget *widget, const LV2_Feature *const *features)
{
  #ifdef TESTING_EQ10Q
  cout<<"instantiateDyn_gui Entring... ";
  #endif
  
   std::string sTitle;
   bool bIsCompressor, bHasSideChain;
   bool bAllOk = false;
  
  std::string str_plugin_uri(plugin_uri);
  if( str_plugin_uri == GATE_MONO_URI || str_plugin_uri == GATE_STEREO_URI)
  {
    sTitle = "GT10Q ~ Noise Gate";
    bIsCompressor = false;
    bHasSideChain = false;
    bAllOk = true;
  }
  if( str_plugin_uri == COMPRESSOR_MONO_URI || str_plugin_uri == COMPRESSOR_STEREO_URI)
  {
    sTitle = "CS10Q ~ Compressor";
    bIsCompressor = true;
    bHasSideChain = false;
    bAllOk = true;
  }
  if( str_plugin_uri == COMPRESSOR_SC_MONO_URI || str_plugin_uri == COMPRESSOR_SC_STEREO_URI)
  {
    sTitle = "CS10Q-SC ~ Side-Chain Compressor";
    bIsCompressor = true;
    bHasSideChain = true;
    bAllOk = true;
  }
  if(! bAllOk)
  {
    return NULL;
  }
  
  Gtk::Main::init_gtkmm_internals();
  DynMainWindow* gui_data = new DynMainWindow(plugin_uri, std::string(bundle_path), sTitle, bIsCompressor, bHasSideChain);
  gui_data->controller = controller;
  gui_data->write_function = write_function;
  *widget = gui_data->gobj();

  #ifdef TESTING_EQ10Q
  cout<<" Done"<<endl;
  #endif
  
  return (LV2UI_Handle)gui_data;
}


static void cleanupDyn_gui(LV2UI_Handle instance)
{
  #ifdef TESTING_EQ10Q
  cout<<"cleanupEq10q_gui Entring... ";
  #endif
  
  ///delete static_cast<DynMainWindow*>(instance);
  DynMainWindow *gui = (DynMainWindow *)instance;
  delete gui;
  
  #ifdef TESTING_EQ10Q
  cout<<" Done"<<endl;
  #endif
}

static void portEventDyn_gui(LV2UI_Handle ui, uint32_t port_index, uint32_t buffer_size, uint32_t format, const void *buffer)
{
  #ifdef TESTING_EQ10Q
  cout<<"portEventEq10q_gui Entring... "<<"Port Index = "<<port_index;
  #endif
  
  DynMainWindow *gui = (DynMainWindow *)ui;
  gui->gui_port_event(ui, port_index, buffer_size, format, buffer);
  
  #ifdef TESTING_EQ10Q
  cout<<" Done"<<endl;
  #endif
}

static const LV2UI_Descriptor dyn_guiDescriptor = {
  DYNAMICS_GUI_URI,
  instantiateDyn_gui,
  cleanupDyn_gui,
  portEventDyn_gui,
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
		    return &dyn_guiDescriptor;
	    default:
		    #ifdef TESTING_EQ10Q
		    cout<<" Done with NOK result (return NULL)"<<endl;
		    #endif
		    return NULL;
    }
    

}
