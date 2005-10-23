#include <stdexcept>
#include <iostream>
#include <boost/timer.hpp>
#include <agg_pixfmt_rgba.h>
#include <agg_renderer_base.h>
#include <agg_renderer_scanline.h>
#include <agg_scanline_u.h>
#include <agg_span_allocator.h>
#include <agg_rasterizer_scanline_aa.h>
#include "agg_renderer_primitives.h"
#include "agg_conv_curve.h"
#include "agg_conv_contour.h"
#include <agg_ellipse.h>
#include <agg_span_gradient.h>
#include <agg_span_interpolator_linear.h>
#include <agg_span_allocator.h>
#include <agg_font_freetype.h>
#include "utility.hpp"
#include "input.hpp"
#include "x11/application.hpp"
#include "x11/window.hpp"
#include "handle.hpp"
#include "simple_handle.hpp"
#include "vector_obj.hpp"
#include "rounded_rect.hpp"
// #pragma GCC visibility push(default)
#include <boost/bind.hpp>
// #pragma GCC visibility pop

using namespace std;
using namespace gott::gui;
using gott::gui::x11::application;

template<typename T>
inline T sqr(T const a ) {return a*a;}
std::string find_font_file( std::string const& ) {
  return "/usr/share/fonts/corefonts/verdana.ttf";
}



class window : public x11::window
{
  private:
    boost::timer t;
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
                               color_array_type> span_gradient_type;
    // The gradient scanline renderer type
    //-----------------
    typedef agg::renderer_scanline_aa<renderer_base_type, span_allocator_type, span_gradient_type > renderer_gradient_type;
    typedef agg::renderer_scanline_aa_solid<renderer_base_type> renderer_solid;
    typedef agg::renderer_scanline_bin_solid<renderer_base_type> renderer_bin;
    typedef agg::font_engine_freetype_int32 font_engine_type;
    typedef agg::font_cache_manager<font_engine_type> font_manager_type;
    font_engine_type font_engine;
    font_manager_type font_manager;

    bool mouse_clicked, dragging;
    gott::gui::coord click_point;
    std::list<boost::shared_ptr<designer::vector_obj> > objects;
    std::list<boost::weak_ptr<designer::vector_obj> > drawables;
    std::list<boost::weak_ptr<designer::handle> > handles;

  public:
    window( application& app, rect const& r, std::string const& title, pixelformat const& p )
      : x11::window( app, r, title, p, window_flags::Defaults )
      , font_manager(font_engine)
      , mouse_clicked(false)
      , dragging(false)
      {
        if( ! font_engine.load_font( find_font_file("verdana").c_str(), 0,agg::glyph_ren_agg_gray8 ) ) 
        {
          throw runtime_error("could not find verdana font");
        }
        set_on_key(boost::bind(&window::on_key, this, _1));
        set_on_mouse(boost::bind(&window::on_mouse, this, _1));
        set_on_redraw(boost::bind(&window::on_redraw, this));
        set_on_configure(boost::bind(&window::on_configure, this, _1));

        designer::rounded_rect *rr  = new  designer::rounded_rect(rect( 40,20, 100, 200 ) );

        objects.push_back( boost::shared_ptr<designer::vector_obj>( rr ) );
        drawables.push_back( objects.back() );

        designer::rounded_rect::handle_list l (rr->get_handles() );

        for(  designer::rounded_rect::handle_list::const_iterator it = l.begin(), e=l.end(); it != e; ++it ) {
          handles.push_back( *it );
          drawables.push_back( *it );
        }
      }
    window( rect const& r, std::string const& title, pixelformat const& p )
      : x11::window( r, title, p, window_flags::Defaults )
      , font_manager(font_engine)
      , mouse_clicked(false)
      , dragging(false)
      {
        if( ! font_engine.load_font( find_font_file("verdana").c_str(), 0, agg::glyph_ren_agg_gray8) ) 
        {
          throw runtime_error("could not find verdana font");
        }
        set_on_key(boost::bind(&window::on_key, this, _1));
        set_on_mouse(boost::bind(&window::on_mouse, this, _1));
        set_on_redraw(boost::bind(&window::on_redraw, this));
        set_on_configure(boost::bind(&window::on_configure, this, _1));
      }

    void drag_begin( coord const& start_pos, coord const& current_pos, size_t button ) {  //button  mask?
      std::cout << "Beginning to drag:" << start_pos.x << "  " << start_pos.y << std::endl;
    }

    void drag_on( coord const& pos ) {
      std::cout << "continuing to drag:" << pos.x << "  " << pos.y << std::endl;
    }

    void drag_end( coord const& end ) {
      std::cout << "ending to drag:" << end.x << "  " << end.y << std::endl;
    }

    void on_click( coord const& pos, size_t button ) {
      std::cout << "just clicked :" << pos.x << "  " << pos.y  << " B: " << button << std::endl;
    }

    void on_mouse( gott::gui::mouse_event const& ev )
    {
      coord  current_point = ev.primary;
      using namespace gott::gui;
      if( mouse_clicked ) {
        if( dragging ) {
          switch( ev.type ) {
            case mouse_event::Move:  
              drag_on( current_point ); 
              break;
            case mouse_event::Release:  
              drag_end( current_point );
              dragging = false;
              mouse_clicked = false;
              break;
            default: break;
          }
        }
        else {
          switch( ev.type ) {
            case mouse_event::Move: 
              {
                int sum = sqr( click_point.x - current_point.x ) + sqr( click_point.y -current_point.y );
                if(  sum > 25 )  {
                  dragging = true;
                  drag_begin( click_point, current_point, ev.button_index );
                }

                break;
              }
            case mouse_event::Release: 
              mouse_clicked = false;
              on_click( current_point, ev.button_index );
              dragging = false;
              break;
            case mouse_event::Press: 
              click_point = current_point;
              mouse_clicked = true;
              break;
            default: break;
          };

        }
      }
      else {
        switch( ev.type ) {
          case mouse_event::Move: 
            break;
          case mouse_event::Release: 
            break;
          case mouse_event::Press: 
            std::cout << " Just pressed " << std::endl;
            click_point = current_point;
            mouse_clicked = true;
            break;
          default: break;
        };

      }
    }

    void on_key( gott::gui::key_event const& ev )
    {
      using namespace gott::gui;
      if(ev.type == gott::gui::key_event::Press )
      {
        switch(ev.code)
        {
          case KeyR:  break;
          case KeyRight: break;
          case KeyUp:  break;
          case KeyDown:  break;
          default: break;
        }
      }
    }
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
    template<typename Array>
    void fill_color_array( Array& array, std::vector<std::pair<float,agg::rgba8> > const& colours ) {
      for( std::size_t i = 0; i < colours.size()-1; ++i ) {
        unsigned bpos = unsigned(float(array.size()) * colours[i].first);
        unsigned epos = unsigned(float(array.size()) * colours[i+1].first);
        for( std::size_t j = bpos; j != epos; ++j ){
          array[j] = colours[i].second.gradient(colours[i+1].second, (j-bpos)/double(epos-bpos) );
        }
      }
    }
    void on_redraw() 
    {
      std::cout << "Time passed since last frame: " << t.elapsed() << std::endl; 
      t = boost::timer();
      set_render_context();
      x11::window::on_redraw();
      agg::rendering_buffer & rbuf =  get_render_buffer();
      // Pixel format and basic renderers.
      //-----------------
      // Common declarations (pixel format and basic renderer).
      //----------------
      pixfmt_type pixf(rbuf);
      renderer_base_type rbase(pixf);
      rbase.clear(agg::rgba(1,1,1));


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
      rect g = get_rect();
      span_gradient_type span_gradient(span_interpolator, 
          gradient_func, 
          color_array, 
          0, g.width);

      // The gradient renderer
      //----------------
      renderer_gradient_type ren_gradient(rbase, span_allocator, span_gradient);


      // The rasterizing/scanline stuff
      //----------------
      agg::rasterizer_scanline_aa<> ras;
      agg::scanline_u8 sl;


      // Finally we can draw a circle.
      //----------------
      // rbase.clear(agg::rgba8(255, 255, 255));

      std::vector<std::pair<float, agg::rgba8> > colours;
      colours.push_back( std::make_pair( 0.0f, agg::rgba8(255, 0, 0, 178)));
      colours.push_back( std::make_pair( 0.2f, agg::rgba8(204, 51, 0, 127)));
      colours.push_back( std::make_pair( 0.4f, agg::rgba8(153, 51, 51, 127)));
      colours.push_back( std::make_pair( 0.6f, agg::rgba8(102, 102, 51, 127)));
      colours.push_back( std::make_pair( 0.8f, agg::rgba8(51, 102, 102, 76)));
      colours.push_back( std::make_pair( 1.0f, agg::rgba8(0, 153, 102, 51)));

      fill_color_array( color_array, colours );

      /*    fill_color_array(color_array, 
            agg::rgba8(0,50,50), 
            agg::rgba8(240, 255, 100), 
            agg::rgba8(80, 0, 0));*/

      agg::ellipse ell(g.width / 2, g.height/2, g.width/2, g.height/2, 100);
      ras.add_path(ell);

      agg::render_scanlines(ras, sl, ren_gradient);

      size_t hir= 0;

      for( std::list< boost::weak_ptr<designer::vector_obj> >::iterator it = drawables.begin(); it != drawables.end(); ++it )
      {
        if(boost::shared_ptr<designer::vector_obj> obj = it->lock())
        {
          std::cout << ++hir << std::endl;
          obj->draw( rbuf );
        }
      }

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

