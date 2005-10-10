#include <stdexcept>
#include <iostream>
#include <cairo.h>
#include <cairo-xlib.h>
#include "utility.hpp"
#include "input.hpp"
#include "x11/application.hpp"
#include "x11/window.hpp"
#include "device.hpp"
#include <boost/bind.hpp>

using namespace std;
using namespace gott::gui;
using gott::gui::x11::application;

class window : public x11::window
{
  private:

  public:
    window( application& app, rect const& r, std::string const& title, pixelformat const& p )
      : x11::window( app, r, title, p, window_flags::Defaults ) {
        set_on_key(boost::bind(&window::on_key, this, _1));
        set_on_redraw(boost::bind(&window::on_redraw, this));
        set_on_configure(boost::bind(&window::on_configure, this, _1));
      }
    window( rect const& r, std::string const& title, pixelformat const& p )
      : x11::window( r, title, p, window_flags::Defaults ){
        set_on_key(boost::bind(&window::on_key, this, _1));
        set_on_redraw(boost::bind(&window::on_redraw, this));
        set_on_configure(boost::bind(&window::on_configure, this, _1));
      }

    void on_key( gott::gui::key_event const& ev )
    {
      using namespace gott::gui;
      if(ev.type == gott::gui::key_event::Press )
      {
        switch(ev.code)
        {
          case KeyLeft:  break;
          case KeyRight: break;
          case KeyUp:  break;
          case KeyDown:  break;
          default: break;
        }
      }
    }

    void on_redraw() 
    {
      set_render_context();
      x11::window::on_redraw();
 
      swap_buffer();
    }

    void on_configure( gott::gui::rect const& r)
    {
      x11::window::on_configure(r);
    }
};


int main()
{
  try {
    application app;
    pixelformat format;
    window a_window( app, rect(0,0,200,100),"Mein Titel", format ); 

    app.run();
  } catch( std:: runtime_error &e ){
    std::cout << e.what()<< std::endl;
  }
}

