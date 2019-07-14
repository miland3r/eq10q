#include <iostream>
#include "guitester.h"
#include <gtkmm/main.h>


int main (int argc, char *argv[])
{ 
  //Gtk::RC::add_default_file("/home/sapista/build/LV2/GtkRCTest/gtkrc");
  //Gtk::RC::add_default_file("/usr/share/themes/Redmond/gtk-2.0/gtkrc");

  Gtk::Main kit(argc, argv);

  

  
  HelloWorld helloworld;

 
  //Shows the window and returns when it is closed.
  Gtk::Main::run(helloworld);

  return 0;
}