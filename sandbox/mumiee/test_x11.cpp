#include <stdexcept>
#include <iostream>
#ifdef USE_ANTIGRAIN
#include "agg_pixfmt_rgba.h"
#include "agg_renderer_base.h"
#include "agg_renderer_scanline.h"
#include "agg_scanline_u.h"
#include "agg_rasterizer_scanline_aa.h"
#include "agg_ellipse.h"
#include "agg_span_gradient.h"
#include "agg_span_interpolator_linear.h"
#else
//#include <cairo.h>
#endif
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
#ifdef USE_ANTIGRAIN
  template<class Array>
  void fill_color_array(Array& array, 
                        agg::rgba8 begin, 
                        agg::rgba8 middle, 
                        agg::rgba8 end)
  {
      unsigned i;
      unsigned half_size = array.size() / 2;
      for(i = 0; i < half_size; ++i)
      {
          array[i] = begin.gradient(middle, i / double(half_size));
      }
      for(; i < array.size(); ++i)
      {
          array[i] = middle.gradient(end, (i - half_size) / double(half_size));
      }
  }
#endif
    void on_redraw() 
    {
      set_render_context();
      x11::window::on_redraw();
#ifdef USE_ANTIGRAIN
    agg::rendering_buffer & rbuf =  get_render_buffer();
  // Pixel format and basic renderers.
    //-----------------
    typedef agg::pixfmt_rgba32 pixfmt_type;
    typedef agg::renderer_base<pixfmt_type> renderer_base_type;

    typedef agg::pod_auto_array<agg::rgba8, 256> color_array_type;


    // Gradient shape function (linear, radial, custom, etc)
    //-----------------
    typedef agg::gradient_x gradient_func_type;   


    // Span interpolator. This object is used in all span generators 
    // that operate with transformations during iterating of the spans,
    // for example, image transformers use the interpolator too.
    //-----------------
    typedef agg::span_interpolator_linear<> interpolator_type;


    // Span allocator is an object that allocates memory for 
    // the array of colors that will be used to render the 
    // color spans. One object can be shared between different 
    // span generators.
    //-----------------
    typedef agg::span_allocator<agg::rgba8> span_allocator_type;


    // Finally, the gradient span generator working with the agg::rgba8 
    // color type. 
    // The 4-th argument is the color function that should have 
    // the [] operator returning the color in range of [0...255].
    // In our case it will be a simple look-up table of 256 colors.
    //-----------------
    typedef agg::span_gradient<agg::rgba8, 
                               interpolator_type, 
                               gradient_func_type, 
                               color_array_type,
                               span_allocator_type> span_gradient_type;


    // The gradient scanline renderer type
    //-----------------
    typedef agg::renderer_scanline_aa<renderer_base_type, 
                                      span_gradient_type> renderer_gradient_type;


    // Common declarations (pixel format and basic renderer).
    //----------------
    pixfmt_type pixf(rbuf);
    renderer_base_type rbase(pixf);


    // The gradient objects declarations
    //----------------
    gradient_func_type  gradient_func;                   // The gradient function
    agg::trans_affine   gradient_mtx;                    // Affine transformer
    interpolator_type   span_interpolator(gradient_mtx); // Span interpolator
    span_allocator_type span_allocator;                  // Span Allocator
    color_array_type    color_array;                     // Gradient colors


    // Declare the gradient span itself. 
    // The last two arguments are so called "d1" and "d2" 
    // defining two distances in pixels, where the gradient starts
    // and where it ends. The actual meaning of "d1" and "d2" depands
    // on the gradient function.
    //----------------
    span_gradient_type span_gradient(span_allocator, 
                                     span_interpolator, 
                                     gradient_func, 
                                     color_array, 
                                     0, 100);

    // The gradient renderer
    //----------------
    renderer_gradient_type ren_gradient(rbase, span_gradient);


    // The rasterizing/scanline stuff
    //----------------
    agg::rasterizer_scanline_aa<> ras;
    agg::scanline_u8 sl;


    // Finally we can draw a circle.
    //----------------
    rbase.clear(agg::rgba8(255, 255, 255));

    fill_color_array(color_array, 
                     agg::rgba8(0,50,50), 
                     agg::rgba8(240, 255, 100), 
                     agg::rgba8(80, 0, 0));

    agg::ellipse ell(50, 50, 50, 50, 100);
    ras.add_path(ell);

    agg::render_scanlines(ras, sl, ren_gradient);
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

