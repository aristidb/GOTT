#include <stdexcept>
#include <iostream>
//#include <cairo.h>
#include "utility.hpp"
#include "input.hpp"
#include "x11/application.hpp"
#include "x11/window.hpp"
#pragma GCC visibility push(default)
#include <boost/bind.hpp>
#pragma GCC visibility pop

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
#ifdef USE_ANTIGRAIN
//      agg::rendering_buffer & buf =  get_render_buffer();
#else
      
      cairo_t * cr = this->get_context();
      rect g = get_rect();

      cairo_pattern_t *pattern=cairo_pattern_create_linear(0.0, 0.0,
          g.width,g.height);

      cairo_pattern_set_extend(pattern, CAIRO_EXTEND_REPEAT);
      cairo_pattern_set_filter(pattern, CAIRO_FILTER_GOOD);
      cairo_pattern_add_color_stop_rgba(pattern, 0,
          1.0, 0.0, 0.0, 0.7);
      cairo_pattern_add_color_stop_rgba(pattern, 0.2,
          0.8, 0.2, 0, 0.5);
      cairo_pattern_add_color_stop_rgba(pattern, 0.4,
          0.6, 0.2, 0.2, 0.5);
      cairo_pattern_add_color_stop_rgba(pattern, 0.6,
          0.4, 0.4, 0.2, 0.5);
      cairo_pattern_add_color_stop_rgba(pattern, 0.8,
          0.2, 0.4, 0.4, 0.3);
      cairo_pattern_add_color_stop_rgba(pattern, 1.0,
          0.0, 0.6, 0.4, 0.2);
      cairo_set_source(cr, pattern);
      cairo_rectangle(cr, 0.0, 0.0, g.width,g.height);

      cairo_fill(cr);

      cairo_save(cr);
      cairo_set_source_rgba(cr, 0.0, 0.2, 0.1, 0.6);
      cairo_move_to(cr, 100, 100);
      cairo_set_font_size(cr, 20);
      cairo_show_text(cr, "Hallo Muster!");
      cairo_restore(cr);

      cairo_pattern_destroy(pattern);
#endif 
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

