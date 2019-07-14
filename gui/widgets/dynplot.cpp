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

#include "dynplot.h"
#include "colors.h"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cmath>

#define CURVE_BORDER 1.5
#define CURVE_MARGIN 15
#define CURVE_TEXT_OFFSET 18
#define PLOT_WIDTH_HEIGHT 250
#define DATA_RANGE_MIN -60.0
#define DATA_RANGE_MAX 10.0
#define GRID_STEP 10.0

PlotDynCurve::PlotDynCurve(bool isCompressor):
m_Ratio(1.0), m_Range(-100.0), m_Knee(0.0), m_Threshold(0.0), m_Makeup(0.0), m_GainReduction(0.0), m_InputVu(-100.0), m_bIsCompressor(isCompressor)
{
  set_size_request(PLOT_WIDTH_HEIGHT, PLOT_WIDTH_HEIGHT);
}

PlotDynCurve::~PlotDynCurve()
{

}

void PlotDynCurve::set_gainreduction(double gainreduction)
{
  m_GainReduction = gainreduction != 0.0 ? 20.0*log10(gainreduction) : -100.0;
  redraw();
}

void PlotDynCurve::set_inputvu(double inputvu)
{
  m_InputVu = inputvu != 0.0 ?  20.0*log10(inputvu) : -100.0;
  redraw();
}

void PlotDynCurve::set_knee(double knee)
{
  m_Knee = knee;
  redraw();
}

void PlotDynCurve::set_makeup(double makeup)
{
  m_Makeup = makeup;
  redraw();
}

void PlotDynCurve::set_range(double range)
{
  m_Range = range;
  redraw();
}

void PlotDynCurve::set_ratio(double ratio)
{
  m_Ratio = ratio;
  redraw();
}

void PlotDynCurve::set_threshold(double threshold)
{
  m_Threshold = threshold;
  redraw();
}

void PlotDynCurve::redraw()
{
  Glib::RefPtr<Gdk::Window> win = get_window();
  if(win)
  {
    Gdk::Rectangle r(0, 0, get_allocation().get_width(), get_allocation().get_height());
    win->invalidate_rect(r, false);
  }
}

double PlotDynCurve::dB2PixelsX(double db)
{
  const double m = ((double)width - 2*CURVE_MARGIN - CURVE_TEXT_OFFSET)/((double)(DATA_RANGE_MAX - DATA_RANGE_MIN));
  const double n = CURVE_MARGIN + CURVE_TEXT_OFFSET - m*DATA_RANGE_MIN;
  return m*db + n;
}

double PlotDynCurve::dB2PixelsY(double db)
{
  const double m = (2*CURVE_MARGIN + CURVE_TEXT_OFFSET - (double)height)/((double)(DATA_RANGE_MAX - DATA_RANGE_MIN));
  const double n = ((double)height) - CURVE_MARGIN - CURVE_TEXT_OFFSET - m*DATA_RANGE_MIN;
  return m*db + n;
}

bool PlotDynCurve::on_expose_event(GdkEventExpose* event)
{
 
  Glib::RefPtr<Gdk::Window> window = get_window();
  if(window)
  {
  
    Gtk::Allocation allocation = get_allocation();
    width = allocation.get_width();
    height = allocation.get_height();
  
    
    Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();

    //Paint backgroud
    cr->save();
    cr->set_source_rgb(BACKGROUND_R, BACKGROUND_G, BACKGROUND_B);
    cr->paint(); //Fill all with background color
    cr->restore();
    
    
    //Draw an interesting frame
    cr->save();         
    double radius = height / 50.0;
    double degrees = M_PI / 180.0;
    cr->begin_new_sub_path();
    cr->arc (width - CURVE_BORDER - radius, CURVE_BORDER + radius, radius, -90 * degrees, 0 * degrees);
    cr->arc (width - CURVE_BORDER - radius, height - CURVE_BORDER - radius, radius, 0 * degrees, 90 * degrees);
    cr->arc (CURVE_BORDER + radius, height- CURVE_BORDER - radius, radius, 90 * degrees, 180 * degrees);
    cr->arc ( CURVE_BORDER + radius, CURVE_BORDER + radius, radius, 180 * degrees, 270 * degrees);
    cr->close_path();  
    Cairo::RefPtr<Cairo::LinearGradient> bkg_gradient_ptr = Cairo::LinearGradient::create(width/2, CURVE_BORDER, width/2, height - CURVE_BORDER);   
    bkg_gradient_ptr->add_color_stop_rgba (0.0, 0.1, 0.1, 0.1, 0.6 ); 
    bkg_gradient_ptr->add_color_stop_rgba (0.5, 0.2, 0.3, 0.3, 0.3 ); 
    bkg_gradient_ptr->add_color_stop_rgba (1.0, 0.1, 0.1, 0.1, 0.6 ); 
    cr->set_source(bkg_gradient_ptr);
    cr->fill_preserve();
    cr->set_line_width(1.0);
    cr->set_source_rgb(0.3, 0.3, 0.4);
    cr->stroke(); 
    cr->restore();
    
    //Draw the grid
    cr->save();
    cr->set_source_rgb(0.3, 0.3, 0.3);
    cr->set_line_width(1);
    for(double i = DATA_RANGE_MIN; i <= DATA_RANGE_MAX; i+=GRID_STEP)
    {
      //Vertical grid
      cr->move_to( dB2PixelsX(i) + 0.5, CURVE_MARGIN); 
      cr->line_to( dB2PixelsX(i) + 0.5, height - CURVE_MARGIN - CURVE_TEXT_OFFSET);
      //Horizontal grid
      cr->move_to( CURVE_MARGIN + CURVE_TEXT_OFFSET, dB2PixelsY(i) + 0.5); 
      cr->line_to( width - CURVE_MARGIN, dB2PixelsY(i) + 0.5);  
      cr->stroke();
    }
    cr->restore();
    
    //Draw text with pango to grid
    cr->save();
    cr->set_source_rgb(0.6, 0.6, 0.6);
    Glib::RefPtr<Pango::Layout> pangoLayout = Pango::Layout::create(cr);
    Pango::FontDescription font_desc("sans 9px");
    pangoLayout->set_font_description(font_desc);
    pangoLayout->set_alignment(Pango::ALIGN_RIGHT);
    for(double i = DATA_RANGE_MIN; i <= DATA_RANGE_MAX; i+=GRID_STEP)
    {
      std::stringstream ss;
      ss<< std::setprecision(2) << i;
      pangoLayout->set_text(ss.str());
      cr->move_to(dB2PixelsX(i) - 3.5, height - CURVE_MARGIN - CURVE_TEXT_OFFSET + 3.5);
      pangoLayout->show_in_cairo_context(cr);
      cr->move_to(CURVE_MARGIN, dB2PixelsY(i) - 3.5);
      pangoLayout->show_in_cairo_context(cr);
      cr->stroke();  
    }
    cr->restore();
    
    //Plot InputVU and GainReduction
    double prange = 0.05*(m_InputVu - DATA_RANGE_MIN);
    cr->save();
    cr->rectangle(CURVE_MARGIN + CURVE_TEXT_OFFSET + 0.5, CURVE_MARGIN + 0.5, width - 2*CURVE_MARGIN - CURVE_TEXT_OFFSET, height -  2*CURVE_MARGIN - CURVE_TEXT_OFFSET);
    cr->clip();
    cr->begin_new_sub_path();
    cr->move_to(dB2PixelsX(DATA_RANGE_MIN), dB2PixelsY(DATA_RANGE_MIN));
    cr->line_to(dB2PixelsX(DATA_RANGE_MIN), dB2PixelsY(DATA_RANGE_MIN  + m_Makeup));
    cr->line_to( dB2PixelsX(m_InputVu + prange),  dB2PixelsY(m_InputVu + prange - m_GainReduction + m_Makeup) );
    cr->line_to( dB2PixelsX(m_InputVu + prange),  dB2PixelsY(DATA_RANGE_MIN));
    cr->close_path();
    bkg_gradient_ptr = Cairo::LinearGradient::create(dB2PixelsX(DATA_RANGE_MIN), dB2PixelsY(DATA_RANGE_MIN),dB2PixelsX(m_InputVu + prange), dB2PixelsY(DATA_RANGE_MIN) );
    bkg_gradient_ptr->add_color_stop_rgba (0.0, 0.1, 0.2, 0.8, 0.4 ); 
    bkg_gradient_ptr->add_color_stop_rgba (0.9, 0.1, 0.6, 0.4, 0.3 ); 
    bkg_gradient_ptr->add_color_stop_rgba (1.0, 0.1, 0.6, 0.4, 0.0 ); 
    cr->set_source(bkg_gradient_ptr);
    cr->fill();
    cr->restore();
    
    
    //Draw the curve
    cr->save();
    cr->rectangle(CURVE_MARGIN + CURVE_TEXT_OFFSET + 0.5, CURVE_MARGIN + 0.5, width - 2*CURVE_MARGIN - CURVE_TEXT_OFFSET, height -  2*CURVE_MARGIN - CURVE_TEXT_OFFSET);
    cr->clip();
    cr->move_to( dB2PixelsX(DATA_RANGE_MIN) + 0.5, dB2PixelsY(DATA_RANGE_MIN) + 0.5 );
    double knee_range, y_dB;
    
    if(m_bIsCompressor)
    {
      for(double x_dB = DATA_RANGE_MIN; x_dB <= DATA_RANGE_MAX; x_dB += 1.0)
      {
        knee_range = 2.0*(x_dB - m_Threshold);
        if (knee_range < -m_Knee)
        {
          //Under Threshold
          y_dB = x_dB; 
        }
        else if(knee_range >= m_Knee )
        {
          //Over Threshold
          y_dB = m_Threshold + (x_dB - m_Threshold)/m_Ratio; 
        }
        else
        {
          //On Knee
          y_dB = x_dB + ((1.0/m_Ratio -1.0)*(x_dB - m_Threshold + m_Knee/2.0)*(x_dB - m_Threshold + m_Knee/2.0))/(2.0*m_Knee);
        }
        y_dB += m_Makeup;
        cr->line_to( dB2PixelsX(x_dB) + 0.5, dB2PixelsY(y_dB) + 0.5);
      }
    }
    else
    {    
      //Draw Expander/Gate
      for(double x_dB = DATA_RANGE_MIN; x_dB <= DATA_RANGE_MAX; x_dB += 1.0)
      {
        knee_range = 2.0*(x_dB - m_Threshold);
        if (knee_range < -m_Knee)
        {
          //Under Threshold
          y_dB = m_Threshold + (x_dB - m_Threshold)*m_Ratio;
        }
        else if(knee_range >= m_Knee )
        {
          //Over Threshold
          y_dB = x_dB;
        }
        else
        {
          //On Knee
          y_dB = x_dB + ((1.0 - m_Ratio)*(x_dB - m_Threshold - m_Knee/2)*(x_dB - m_Threshold - m_Knee/2))/(2*m_Knee);
        }
           
	if( y_dB < x_dB + m_Range )
	{
	  y_dB = x_dB + m_Range;
	}
        cr->line_to( dB2PixelsX(x_dB) + 0.5, dB2PixelsY(y_dB) + 0.5);
      }
      
    }
    cr->set_line_width(1.0);
    cr->set_line_cap(Cairo::LINE_CAP_ROUND);
    cr->set_source_rgb(1,1,1);
    cr->stroke();
    cr->restore();
    
    
    //draw de outer grind box
    cr->save();
    cr->set_source_rgb(0.3, 0.3, 0.3);
    cr->set_line_width(1);
    cr->move_to(CURVE_MARGIN + CURVE_TEXT_OFFSET + 0.5, CURVE_MARGIN + 0.5); 
    cr->line_to(width - CURVE_MARGIN + 0.5, CURVE_MARGIN + 0.5);
    cr->line_to(width - CURVE_MARGIN + 0.5, height - CURVE_MARGIN - CURVE_TEXT_OFFSET + 0.5);
    cr->line_to(CURVE_MARGIN + CURVE_TEXT_OFFSET + 0.5 , height - CURVE_MARGIN - CURVE_TEXT_OFFSET + 0.5);
    cr->line_to(CURVE_MARGIN + CURVE_TEXT_OFFSET + 0.5, CURVE_MARGIN + 0.5); 
    cr->stroke();
    cr->restore();
  }
  return true;
}
