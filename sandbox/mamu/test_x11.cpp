#include <stdexcept>
#include <iostream>
#include "utility.hpp"
#include "input.hpp"
#include "x11/application.hpp"
#include "x11/window.hpp"

using namespace gott::gui;
using namespace gott::gui::x11;

class MyWindow : public gott::gui::x11::Window
{
  public:
    MyWindow( Application& app, rect const& r, std::string const& title, pixelformat const& p )
      : gott::gui::x11::Window( app, r, title, p, WindowFlags::Defaults ) {}
    MyWindow( rect const& r, std::string const& title, pixelformat const& p )
      : gott::gui::x11::Window( r, title, p, WindowFlags::Defaults ) {}

    void on_redraw() 
    {
      set_render_context();
      Window::on_redraw();
      glClearColor( 0.4,0.2,0.8, 1.0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      swap_buffer();
    }
    void on_configure( gott::gui::rect const& r)
    {
      Window::on_configure(r);
      set_render_context();
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glOrtho( r.left, r.left+ r.width, r.top, r.top + r.height, 0.1, 2.0 );
    }
};


int main()
{
  try {
    Application app;
    pixelformat format;
    MyWindow a_window( app, rect(0,0,200,100),"Mein Titel", format ); 
    MyWindow b_window( app, rect(300,50,200,100),"Ein anderer Titel", format); 

    app.run();
  } catch( std:: runtime_error &e ){
    std::cout << e.what()<< std::endl;
  }
}

