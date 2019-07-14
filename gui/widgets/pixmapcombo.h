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

#ifndef PIXMAP_COMBOBOX_H
  #define PIXMAP_COMBOBOX_H
  
#include <iostream>

#include <gtkmm/combobox.h>
#include <gtkmm/liststore.h>
#include <gtkmm/image.h>
#include <gtkmm/stock.h>

#define RUTA_OFF  "combopix/off.png"
#define RUTA_LPF1 "combopix/lpf1.png"
#define RUTA_LPF2 "combopix/lpf2.png"
#define RUTA_LPF3 "combopix/lpf3.png"
#define RUTA_LPF4 "combopix/lpf4.png"
#define RUTA_HPF1 "combopix/hpf1.png"
#define RUTA_HPF2 "combopix/hpf2.png"
#define RUTA_HPF3 "combopix/hpf3.png"
#define RUTA_HPF4 "combopix/hpf4.png"
#define RUTA_LOSHELF "combopix/loshelf.png"
#define RUTA_HISHELF "combopix/hishelf.png"
#define RUTA_PEAK "combopix/peak.png"
#define RUTA_NOTCH "combopix/notch.png"

class PixMapCombo : public Gtk::ComboBox
{
  public:
    PixMapCombo(const char *bundlePath);
    virtual ~PixMapCombo();
    
  protected:
    class ModelColumns : public Gtk::TreeModel::ColumnRecord
    {
      public:
        ModelColumns()
        {
          add(m_col_pix);
        }
        
        Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > m_col_pix;
     };
    
    ModelColumns m_Columns;
    Glib::RefPtr<Gtk::ListStore> m_refTreeModel; 
    
  private:
    std::string m_bundlePath;
};
#endif
