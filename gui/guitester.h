#ifndef GTKMM_EXAMPLE_HELLOWORLD_H
#define GTKMM_EXAMPLE_HELLOWORLD_H

#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/scale.h>

#include "widgets/eqwindow.h"
#include "widgets/dynamicswindow.h"
#include "widgets/bassupwindow.h"
#include "widgets/midsidewindow.h"

class HelloWorld : public Gtk::Window
{

public:
  HelloWorld();
  virtual ~HelloWorld();

protected:
  EqMainWindow *m_EqWin;
  DynMainWindow *m_GateWin;
  BassUpMainWindow *m_BassUp;
  MidSideMainWindow *m_MidSide;
  Gtk::HBox m_hbox;
  Gtk::VScale m_TestScale;
  
  //Slots signal handlers
  void on_realize();
  void on_BypassChanged(bool bypass);
  void on_InputGainChanged(float gain);
  void on_OutputGainChanged(float gain);
  void on_BandGainChanged(int band, float gain);
  void on_BandFreqChanged(int band, float freq);
  void on_BandQChanged(int band, float q);
  void on_BandTypeChanged(int band, int type);
  void on_BandEnabledChanged(int band, bool enabled);
  
  //Test slot
  void on_TestScale_changed();
};

#endif // GTKMM_EXAMPLE_HELLOWORLD_H