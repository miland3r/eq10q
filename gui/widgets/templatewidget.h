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

#include <iostream>

#include <fstream>
#include <string>

#include <pstreams/pstream.h>
#include <stdlib.h>

#include <gtkmm/alignment.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/comboboxentrytext.h>
#include <gtkmm/entry.h>
#include <gtkmm/label.h>
#include <gtkmm/messagedialog.h>

#include "types.h"

#define FILE_NAME "/.RafolsEQ/eq_presets.prs"
#define FILE_NAME_AUX "/.RafolsEQ/eq_presets.aux"
#define NAME_LONG 100

#ifndef  MAIN_WINDOW_WIDGET
  #define MAIN_WINDOW_WIDGET
  class main_window;
#endif
//using namespace std;

class TemplateWidget : public Gtk::Alignment{
  public:
    TemplateWidget(main_window *m_ptr, 
                   void (*f_set_ptr) (main_window *myptr, int b_ix, int type, float g, float f, float Q), 
                   void (*f_get_ptr) (main_window *myptr,int band_id, BandParams &m_params));
    virtual ~TemplateWidget();
    virtual void load_combo_list();
  
  protected:
    Gtk::HBox m_box;
    Gtk::Button b_load, b_save, b_remove;
    Gtk::ComboBoxEntryText preset_combo;
    Gtk::Label l_presets;
    
    virtual void on_load_clicked();
    virtual void on_save_clicked();
    virtual void on_delete_clicked();
    virtual void on_combo_move();
    
    int current_preset;

    struct filter_params{
      int type;
      float gain, freq, Q;    
       };
       
    struct f_register{
      char name[NAME_LONG];
      int name_length;
      filter_params fparams[10];
      };
    
    f_register f_filter;

    //Function ptr to main
    void (*external_set_ptr) (main_window *ptr, int b_ix, int type, float g, float f, float Q);
    void (*external_get_ptr) (main_window *ptr, int band_id, BandParams &m_param);

    //Structura tipus de filtre
    BandParams band_param;

    //Directori del home
    std::string strhomedir;

    //punter a main_window
    main_window *main_win_ptr;

};