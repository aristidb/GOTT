#include <cstdlib>
#include <iostream>
#include <ctime>
#include <utility>
#include <boost/bind.hpp>
#include <gott/ui/window_base.hpp>
#include <gott/ui/x11/window.hpp>
#include <gott/ui/x11/uicontext.hpp>
#include <gott/util/string/string.hpp>
#include <gott/base/events/select_loop.hpp>
#include <agg_pixfmt_rgba.h>
#include <agg_renderer_base.h>
#include <agg_renderer_scanline.h>
#include <agg_scanline_u.h>
#include <agg_span_allocator.h>
#include <agg_rasterizer_scanline_aa.h>
#include <agg_renderer_primitives.h>
#include <agg_conv_curve.h>
#include <agg_conv_contour.h>
#include <agg_ellipse.h>
#include <agg_span_gradient.h>
#include <agg_span_interpolator_linear.h>
#include <agg_span_allocator.h>

using namespace gott::ui::x11;
using namespace gott::events;
using namespace boost::posix_time;

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

void draw(agg::rendering_buffer& buf, gott::rect const& r )
{
  std::cout << "Drawing (" << r.left << "," << r.top << ")(" << r.width << "," << r.height  << ")" << std::endl;
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

  pixfmt_type pixf(buf);
  renderer_base_type rbase(pixf);
  rbase.clear(agg::rgba(1,1,1,0));


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
  span_gradient_type span_gradient(span_interpolator, 
      gradient_func, 
      color_array, 
      0, r.width);

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

  agg::ellipse ell(r.width / 2, r.height/2, r.width/2, r.height/2, 100);
  ras.add_path(ell);

  agg::render_scanlines(ras, sl, ren_gradient);
}

deadline_timer move_window( window *win ) {
  gott::rect reg = win->region().get();
  std::cout <<"move_window from (" << reg.left << "," << reg.top << ")(" << reg.width << "," << reg.height << ")";
  reg.left += 80 - rand()%180;
  reg.top += 80 - rand()%180;
  reg.left = std::max( 0L, reg.left );
  reg.top = std::max( 0L, reg.top);
  std::cout <<"  to (" << reg.left << "," << reg.top << ")(" << reg.width << "," << reg.height<<  ")" << std::endl;
  win->region().set(reg);
  return deadline_timer( microsec_clock::local_time() + seconds(6), boost::bind( &move_window, win) );
}

deadline_timer resize_window( window *win ) {
  gott::rect reg = win->region().get();
  std::cout <<"resize_window (" << reg.left << "," << reg.top << ")(" << reg.width << "," << reg.height  << ")";
  reg.width = 10 + rand()%180;
  reg.height= 10 + rand()%180;
  std::cout <<"  to (" << reg.left << "," << reg.top << ")(" << reg.width << "," << reg.height << ")" << std::endl;
  win->region().set(reg);
  return deadline_timer( microsec_clock::local_time() + seconds(10), boost::bind( &resize_window, win) );
}


int main(int, char **){
  std::srand(std::time(0));
  select_loop loop;
  uicontext con;
  {
    window win( con, gott::rect( 30,30,123,230), gott::string("simple_x11.cpp title"), size_t(gott::ui::window_flags::Defaults) );
    win.on_draw().connect( &draw );
    loop.add_read_fd( con.get_descriptor(), boost::bind( &uicontext::process_read, &con ) );
    loop.add_timer( deadline_timer( microsec_clock::local_time() + seconds(10), boost::bind( &resize_window, &win ) ) );
    loop.add_timer( deadline_timer( microsec_clock::local_time() + seconds(7), boost::bind( &move_window, &win ) ) );
    loop.run();
  }
}
