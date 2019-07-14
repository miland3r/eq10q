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

#ifndef PLOT_DYN_CURVE_H
  #define PLOT_DYN_CURVE_H
  
#include <gtkmm/drawingarea.h>

class PlotDynCurve : public Gtk::DrawingArea
{
  public:
    PlotDynCurve(bool isCompressor);
    virtual ~PlotDynCurve();
    void set_ratio(double ratio);
    void set_range(double range);
    void set_knee(double knee);
    void set_threshold(double threshold);
    void set_makeup(double makeup);
    void set_gainreduction(double gainreduction);
    void set_inputvu(double inputvu);
    
  protected:
    //Override default signal handler:
    virtual bool on_expose_event(GdkEventExpose* event);
    virtual void redraw();  
    
  private:
    int width, height; 
    double m_Ratio, m_Range, m_Knee, m_Threshold, m_Makeup, m_GainReduction, m_InputVu;
    bool m_bIsCompressor;
    
    double dB2PixelsX(double db);
    double dB2PixelsY(double db);
};
#endif