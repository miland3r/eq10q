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

#ifndef PLOT_BODE_CURVE_H
  #define PLOT_BODE_CURVE_H

#include <iostream>
#include <gtkmm/drawingarea.h>
#include "filter.h"
#include "../eq_defines.h"

#define MIN_FREQ 18.0 
#define MAX_FREQ 22000.0
#define MIN_SPAN_DEC 0.5
#define CURVE_NUM_OF_POINTS 1000
#define GRID_VERTICAL_LINES 28
#define CURVE_MARGIN 8
#define CURVE_BORDER 1.5
#define CURVE_TEXT_OFFSET_X 18
#define CURVE_TEXT_OFFSET_Y 38
#define ZOOM_WIDGET_BORDER_Y 22
#define PLOT_HIGHT 300
#define PLOT_WIDTH 500
#define SCROLL_EVENT_INCREMENT 0.3
#define AUTO_REFRESH_TIMEOUT_MS 20
#define SPECTROGRAM_LINE_THICKNESS 3.0 

typedef struct
{
  float Gain;
  float Freq;
  float Q;
  bool bIsOn;
  FilterType fType;
}FilterBandParams;

class PlotEQCurve : public Gtk::DrawingArea
{
  public:
    PlotEQCurve(int iNumOfBands, int channels);
    virtual ~PlotEQCurve();
    void resetCurve();
    virtual void setBandGain(int bd_ix, float newGain);
    virtual void setBandFreq(int bd_ix, float newFreq);
    virtual void setBandQ(int bd_ix, float newQ);
    virtual void setBandType(int bd_ix, int newType);
    virtual void setBandEnable(int bd_ix, bool bIsEnabled);
    virtual void setBypass(bool bypass);
    virtual void setSampleRate(double samplerate);
    virtual void setFftData(double *fft_data);
    virtual void setFftActive(bool active, bool isSpectrogram);
    virtual void setFftGain(double g);
    virtual void setFftRange(double r);
    virtual void setFftHold(bool hold);
    virtual void setPlotdBRange(double range);
    virtual void glowBand(int band);
    virtual void unglowBands();
        
    enum MSState { ML, DUAL, SR, MONO};
    void setStereoState(int band, MSState state);
    
    //signal accessor:
    //Slot prototype: void on_band_changed(int band_ix, float Gain, float Freq, float Q);
    typedef sigc::signal<void, int, float, float, float> signal_BandChanged;
    signal_BandChanged signal_changed();
    
    //Slot prototype: void on_band_enabled(int band_ix, bool enabled);
    typedef sigc::signal<void, int, bool> signal_BandEnabled;
    signal_BandEnabled signal_enabled();
    
    //Slot prototype: void on_band_selected(int band_ix, bool enabled);
    typedef sigc::signal<void, int> signal_BandSelected;
    signal_BandSelected signal_selected();
    
    //Slot prototype: void on_band_selected(int band_ix, bool enabled);
    typedef sigc::signal<void> signal_BandUnselected;
    signal_BandUnselected signal_unselected();
       
  protected:    
      //Mouse grab signal handlers
      virtual bool on_button_press_event(GdkEventButton* event);
      virtual bool on_button_release_event(GdkEventButton* event);
      virtual bool on_scrollwheel_event(GdkEventScroll* event);
      virtual bool on_mouse_motion_event(GdkEventMotion* event);
      virtual bool on_timeout_redraw();
      virtual bool on_mouse_leave_widget(GdkEventCrossing* event);
      virtual void cueBandRedraws(int band);
      virtual void redraw_background_widget();
      virtual void redraw_zoom_widget();
      virtual void redraw_curve_widgets(int band);
      virtual void redraw_main_curve();
      virtual void redraw_grid_widget();
      virtual void redraw_xAxis_widget();
      virtual void redraw_yAxis_widget();
      virtual void redraw_fft_widget();
      virtual void redraw_cursor(double x, double y);
        
      //Override default signal handler:
      virtual bool on_expose_event(GdkEventExpose* event);
    
  private:
    int width, height; 
    int m_TotalBandsCount;
    int m_NumChannels;
    bool m_Bypass;
    int m_iBandSel;
    bool bMotionIsConnected;
    bool bBandFocus;
    bool *m_Bands2Redraw;
    bool m_BandRedraw, m_fullRedraw, m_justRedraw;
    double SampleRate;
    bool m_FftActive;
    double m_minFreq, m_maxFreq;
    double m_dB_plot_range;
        
    //To hadle mouse mouve events
    sigc::connection m_motion_connection;
    
    //Store filters data
    FilterBandParams **m_filters;  //This pointer is initialized by construcor to an array of total num of bands
    
    //X axes LUT tables
    int xPixels_Grid[GRID_VERTICAL_LINES]; //Pixels used to draw the grind in logspace
    double *f; //This pointer is initialized by construcor to an array of total num of points acording min/max freq define
    int *xPixels; //This pointer is initialized by construcor to an array of total num of points, each item is the pixel space transaltion of corresponding freq
    
    //Curve vector for Y axes in dB units
    double **main_y; //This pointer is initialized by construcor to an array of total num of points acording the format main_y[channel][num_points]
    double **band_y;  //This pointer is initialized by construcor to an array acording the format band_y[bd_ix][num_points]
    MSState *band_state; //A vector containing the Stereo states for each band
    
    //FFT vectors
    double *xPixels_fft, *xPixels_fft_bins;
    double *fft_pink_noise;
    double *fft_plot;
    double *fft_ant_data;
    double fft_gain;
    double fft_range;
    float *fft_log_lut;
    bool m_bIsSpectrogram, m_bFftHold;
    double *fft_raw_data;
    
    //Zoom widget data
    struct zoom_widget
    {
      bool center_focus;
      bool f1_focus;
      bool f2_focus;
      double x1;
      double x2;
      double x_ant;
      bool center_press;
      bool f1_press;
      bool f2_press;
    } m_zoom_widget;
    
    
    //Cairo surfaces
    Cairo::RefPtr<Cairo::ImageSurface> m_background_surface_ptr, m_fft_surface_ptr, m_zoom_surface_ptr, *m_curve_surface_ptr, m_maincurve_surface_ptr, m_grid_surface_ptr, m_xAxis_surface_ptr, m_yAxis_surface_ptr, m_cursor_surface_ptr; 
    
    //Bode change signal
    signal_BandChanged m_BandChangedSignal;
    signal_BandEnabled m_BandEnabledSignal;
    signal_BandSelected m_BandSelectedSignal;
    signal_BandUnselected m_BandUnselectedSignal;
        
    //Function for dB to pixels convertion
    double dB2Pixels(double db);
    
    //Function for Hz to pixels convertion
    double freq2Pixels(double f);
    
    //Function for pixels to dB convertion
    double Pixels2dB(double pixels);
    
    //Function for pixels to Hz convertion
    double Pixels2freq(double pixels);
    
    //Compute a filter points
    void ComputeFilter(int bd_ix); 
    
    //Curve math functions   
    void CalcBand_DigitalFilter(int bd_ix);
    
    //Compute zoom bar
    virtual void setCenterSpan(double center, double span);
    virtual void resetCenterSpan();   
    virtual void setCenter(double center);
    virtual void setSpan(double span);
    void recomputeMinFreq_fromX1Pixel(double x1);
    void recomputeMaxFreq_fromX2Pixel(double x2);
    void recomputeCenterFreq(double xDiff);
};
#endif