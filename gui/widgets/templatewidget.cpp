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

#include "templatewidget.h"

TemplateWidget::TemplateWidget(main_window *m_ptr, 
                   void (*f_set_ptr) (main_window *myptr, int b_ix, int type, float g, float f, float Q), 
                   void (*f_get_ptr) (main_window *myptr, int band_id, BandParams &m_params))
:b_load("Load Preset"), b_save("Save Preset"), b_remove("Delete Preset"), l_presets("Presets"){
  
  //External functions
  external_get_ptr = f_get_ptr;
  external_set_ptr = f_set_ptr;

  //Inicialment el botto load no fa res
  current_preset=-1;

  //Inicialitza punter a objecte principal
  main_win_ptr = m_ptr;

  //Dibuix de la GUI
  set(Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER, 0.0, 0.0);
  
  m_box.pack_start(l_presets);
  m_box.pack_start(preset_combo);
  m_box.pack_start(b_load);
  m_box.pack_start(b_save);
  m_box.pack_start(b_remove);
  m_box.set_homogeneous(false);
  m_box.set_size_request(560,-1);
  m_box.set_spacing(2);
  
  add(m_box);
  set_padding(2, 2, 2, 2);
  show_all_children();
  
  //Aseguremnos de k el directori arrel per desar presets exiseix
  redi::ipstream in("echo $HOME");
  std::string cmd;

  in >> strhomedir;
  //std::cout << strhomedir << std::endl;
  in.close();

  cmd = "mkdir " + strhomedir +  "/.RafolsEQ";
  system(cmd.c_str());

  //load_combo_list()  //<--Ho farem en el on_expose de la finestra principal

  //Signals
  b_load.signal_clicked().connect(sigc::mem_fun(*this, &TemplateWidget::on_load_clicked));
  b_save.signal_clicked().connect(sigc::mem_fun(*this, &TemplateWidget::on_save_clicked));
  b_remove.signal_clicked().connect(sigc::mem_fun(*this, &TemplateWidget::on_delete_clicked));
  preset_combo.signal_changed().connect(sigc::mem_fun(*this, &TemplateWidget::on_combo_move));
}

/*Si el load clika, comprobar k no es pq s'esta introduint text i k realment volem
canviar de preset 
en cas afirmatiu carregar el nou preset
*/
void TemplateWidget::on_load_clicked(){
  //std::cout<<"Button load cicked"<<std::endl;
  if(current_preset != -1){ //diferent de -1 implica k no estem introduint text
    std::string cmd = strhomedir + FILE_NAME;
    std::fstream ofs(cmd.c_str(), std::ios::in | std::ios::binary);
    if(!ofs) std::cerr<<"Error: file can't be open";
    else{
       ofs.clear();
       long desp = current_preset*(int)sizeof(f_filter);
       ofs.seekg(desp, std::ios::beg);
       ofs.read(reinterpret_cast<char *>(&f_filter),sizeof(f_filter));
       //cout<<"Row name readed = "<<f_filter.name<<endl;
       
       //Escriure tots els parametres al filtre
        for(int i=0; i<10; i++){ //10 es el nombre de bandes
          external_set_ptr(main_win_ptr, i,
                              f_filter.fparams[i].type,
                              f_filter.fparams[i].gain,
                              f_filter.fparams[i].freq,
                              f_filter.fparams[i].Q
                              );
        }//fi for
        }
        ofs.close();
     }

}

/*Si clickem el button save hem de fer el seguent:
  - Si no s'ha introduit un nom valid (entenent valid com no NUL i no repetit) donar un missatge d'error
  - Si el nom es valid, afegir al combo
  - generar una linea nova en el fitxer de presets
  - Afegir primer el nom del preset introduit en l'entry
  - Afegir despres els parametres del filtre segons: Type, Gain, Freq, Q
  Tancar el fitxer de text eq_presets.prs
*/
void TemplateWidget::on_save_clicked(){
  //std::cout<<"SAVE clicked!"<<std::endl;
  Gtk::Entry* entry = preset_combo.get_entry();


  if(entry){
    //std::string presetname = entry->get_text();
    Glib::ustring presetname = entry->get_text();
    std::string cmd = strhomedir + FILE_NAME;
    if(!presetname.empty()){
      std::fstream ofs(cmd.c_str(), std::ios::out | std::ios::app | std::ios::binary);
      if(!ofs) std::cerr<<"Error: file can't be open";
      else{
        ofs.clear();
        //gestio del nom del preset
        preset_combo.append_text(presetname);
        f_filter.name_length = (NAME_LONG-1 > presetname.length()? presetname.length(): NAME_LONG-1); 
        presetname.copy(f_filter.name,f_filter.name_length,0);
        f_filter.name[f_filter.name_length]='\0'; //ultim caracter
      
        //Escriure tots els parametres del filtre
        for(int i=0; i<10; i++){ //10 es el nombre de bandes
          external_get_ptr(main_win_ptr, i, band_param);

          f_filter.fparams[i].type = band_param.type;
          f_filter.fparams[i].gain = band_param.gain;
          f_filter.fparams[i].freq = band_param.freq;
          f_filter.fparams[i].Q = band_param.Q;
        }
      
        //Ho escribim al fitxer
        ofs.write(reinterpret_cast<char *>(&f_filter),sizeof(f_filter));
       
      }//fi else
      ofs.close();
    }
  }
}

/*Si clickem el butto de delete:
  - mostrem un misatge de text de si estem segurs de voler vorrar el preset
  - Eliminem l'entrada del combo
  - Eliminem la linea del fitxer de text eq_presets.prs
*/
void TemplateWidget::on_delete_clicked(){
  //std::cout<<"DELETE clicked!"<<std::endl;
  int row = preset_combo.get_active_row_number();
  long desp;
  int ix = 0;
  //std::string presetname;
  Glib::ustring presetname;
  std::string cmd;

  //Generem un missatge
  Gtk::MessageDialog dialog((Gtk::Window&)(*this->get_toplevel()),"This will delete the selected preset, are you sure?",
          false /* use_markup */, Gtk::MESSAGE_QUESTION,
          Gtk::BUTTONS_OK_CANCEL);
 
  int result = dialog.run();

 if(row != -1 && result == Gtk::RESPONSE_OK){
    cmd = strhomedir + FILE_NAME_AUX;
    std::fstream ofs(cmd.c_str(), std::ios::out | std::ios::binary);
    cmd = strhomedir + FILE_NAME;
    std::fstream ifs(cmd.c_str(), std::ios::in |  std::ios::binary);
     
    if(!ifs) std::cerr<<"Error: file can't be open";
    else{
      ifs.clear();
      ofs.clear();
      desp = (row)*(int)sizeof(f_filter);
      ifs.seekg(desp, std::ios::beg);
      ifs.read(reinterpret_cast<char *>(&f_filter),sizeof(f_filter));
  
      //pasem-ho a una cadena
      /*for(int i=0; i<f_filter.name_length; i++){
        presetname[i]=f_filter.name[i];
      }*/
      presetname=f_filter.name;
  
      preset_combo.remove_text(presetname);
  
      while(ix != row){
        desp = (ix)*(int)sizeof(f_filter);
        ifs.seekg(desp, std::ios::beg);
      
        ifs.read(reinterpret_cast<char *>(&f_filter),sizeof(f_filter));
        ofs.write(reinterpret_cast<char *>(&f_filter),sizeof(f_filter));
        ix++;
      }
      ix = 0;
      do{
     
        desp = (row+1+ix)*(int)sizeof(f_filter);
        ifs.seekg(desp, std::ios::beg);
        ifs.read(reinterpret_cast<char *>(&f_filter),sizeof(f_filter));
        if(ifs.eof()) break;

        ofs.write(reinterpret_cast<char *>(&f_filter),sizeof(f_filter));
      
        ix++;
      } while(true);
    
     
        std::string cmd = "mv ";
        cmd = cmd + strhomedir + FILE_NAME_AUX + " " +  strhomedir + FILE_NAME;
        system(cmd.c_str());
        
        Gtk::Entry* entry = preset_combo.get_entry();
        entry->set_text("");
        }//else
        ifs.close();
        ofs.close();
      }

}

/*Si el combo es mou, carrega el current_preset la fila k correspon del combo
*/
void TemplateWidget::on_combo_move(){
  current_preset = preset_combo.get_active_row_number();
}


void TemplateWidget::load_combo_list(){
  //std::cout<<"Omplint el combo ..."<<std::endl;
  //primer ho borrem tot
  preset_combo.clear_items();

  //Omplir a partir de fitxer eq_presets.prs
  //std::string presetname;
  Glib::ustring presetname;
  std::string cmd = strhomedir + FILE_NAME;
  std::fstream ofs(cmd.c_str(), std::ios::in | std::ios::binary);

  if(!ofs) std::cerr<<"Error: file can't be open";
  else{
    ofs.clear();
    do{
    ofs.read(reinterpret_cast<char *>(&f_filter),sizeof(f_filter));
    if(ofs.eof()) break;
    
   /* //pasem-ho a una cadena
    for(int i=0; i<f_filter.name_length; i++){
      presetname[i]=f_filter.name[i];
    }
    
    presetname[f_filter.name_length]='\0';
 */
   presetname=f_filter.name;
   preset_combo.append_text(presetname);
    
      }
      while(true);
      
    }
    ofs.close();
}


TemplateWidget::~TemplateWidget(){

}