#include <iostream>
#include <stdexcept>
#include <agg2/agg_basics.h>
#include <agg2/util/agg_color_conv_rgb8.h>
#include "antigrain.hpp"

namespace gott{namespace gui{namespace x11{ 

namespace detail {
template<class CopyRow> 
void sub_color_conv( gott::gui::rect const& region,
    agg::rendering_buffer* dst, 
    const agg::rendering_buffer* src,
    CopyRow copy_row_functor)
{
  if(region.width)
  {
    unsigned y;
    for(y = 0; y < region.height; y++)
    {
      copy_row_functor(dst->row(y), src->row(y + region.top) +region.left, region.width);
    }
  }
}
}

antigrain::antigrain()
  : display(0), vis(0), window(0), depth(32), byte_order(LSBFirst), sys_bpp(32), bpp(32), format(pix_format_rgba32), sys_format(pix_format_undefined)
  ,window_img(0), window_buffer(0), flip_y(false) {}
antigrain::~antigrain()
{ 
  close_renderer(display, screen, window );
}

Visual * antigrain::get_visual( ) {
  return vis;
}

int antigrain::get_depth() const {
  return depth;
}

void antigrain::prepare_visual( int screen, Display * display ) {
  this->screen = screen;
  this->display = display;

  // TODO make an excessive searc of the best? visual and/or add a parameter to antigrain that
  // reflects the users pixel wishes..
  depth = XDefaultDepth(display,screen);
  vis = XDefaultVisual(display,screen);
  unsigned long r_mask = vis->red_mask;
  unsigned long g_mask = vis->green_mask;
  unsigned long b_mask = vis->blue_mask;
  if( depth < 15 || r_mask == 0 || g_mask == 0 || b_mask == 0)
    throw std::runtime_error("failed to find a good visual");

  int t = 1;
  int hw_byte_order = LSBFirst;
  if(*(char*)&t == 0) hw_byte_order = MSBFirst;

  // Perceive SYS-format by mask
  switch(depth)
  {
    case 15:
      sys_bpp = 16;
      if(r_mask == 0x7C00 && g_mask == 0x3E0 && b_mask == 0x1F)
      {
        sys_format = pix_format_rgb555;
        byte_order = hw_byte_order;
      }
      break;

    case 16:
      sys_bpp = 16;
      if(r_mask == 0xF800 && g_mask == 0x7E0 && b_mask == 0x1F)
      {
        sys_format = pix_format_rgb565;
        byte_order = hw_byte_order;
      }
      break;

    case 24:
    case 32:
      sys_bpp = 32;
      if(g_mask == 0xFF00)
      {
        if(r_mask == 0xFF && b_mask == 0xFF0000)
        {
          switch(format)
          {
            case pix_format_rgba32:
              sys_format = pix_format_rgba32;
              byte_order = LSBFirst;
              break;

            case pix_format_abgr32:
              sys_format = pix_format_abgr32;
              byte_order = MSBFirst;
              break;

            default:                            
              byte_order = hw_byte_order;
              sys_format = 
                (hw_byte_order == LSBFirst) ?
                pix_format_rgba32 :
                pix_format_abgr32;
              break;
          }
        }

        if(r_mask == 0xFF0000 && b_mask == 0xFF)
        {
          switch(format)
          {
            case pix_format_argb32:
              sys_format = pix_format_argb32;
              byte_order = MSBFirst;
              break;

            case pix_format_bgra32:
              sys_format = pix_format_bgra32;
              byte_order = LSBFirst;
              break;

            default:                            
              byte_order = hw_byte_order;
              sys_format = 
                (hw_byte_order == MSBFirst) ?
                pix_format_argb32 :
                pix_format_bgra32;
              break;
          }
        }
      }
      break;
  }

  if(sys_format == pix_format_undefined)
    throw std::runtime_error("RGB masks are not compatible with AGG pixel formats");

}

void antigrain::setup_renderer( Display* , int , Drawable win , rect const& r ){
  window = win;
  gc = XCreateGC(display, window, 0, 0); 
  window_buffer = new unsigned char[r.width * r.height * (bpp / 8)];

  memset(window_buffer, 0, r.width * r.height * (bpp / 8));

  rbuffer.attach(window_buffer, r.width, r.height,
      flip_y ? -r.width * (bpp / 8) : r.width * (bpp / 8));

  window_img = XCreateImage(display,  vis, //CopyFromParent, 
        depth,       ZPixmap, 
        0,
        reinterpret_cast<char*>(window_buffer), 
        r.width,       r.height,        sys_bpp,
        r.width * (sys_bpp / 8));
  window_img->byte_order = byte_order;
}

void antigrain::resize_renderer( Display* , int , Drawable , rect const& r ){
  delete [] window_buffer;
  window_img->data = 0;
  XDestroyImage(window_img);
  window_buffer = new unsigned char[r.width * r.height * ( bpp / 8 )];
  rbuffer.attach( window_buffer, r.width, r.height, flip_y?-r.width*(bpp/8) : r.width*(bpp/8) );
  window_img = XCreateImage( display, vis, depth, ZPixmap, 0, reinterpret_cast<char*>(window_buffer), r.width, r.height, sys_bpp, r.width * (sys_bpp / 8));
  window_img->byte_order = byte_order;
}


void antigrain::close_renderer(Display* , int ,  Drawable ){
  delete [] window_buffer;
  window_buffer = 0;
  if( window_img ) {
    window_img->data = 0;
    XDestroyImage( window_img );
    window_img = 0;
  }
  if( gc ) {
    XFreeGC(display,  gc);
    gc = 0;
  }
}


void antigrain::update_rect( gott::gui::rect const& r ){
  if(window_img == 0) return;
  window_img->data = (char*)window_buffer;

  if( format == sys_format)
  {
    std::cout << "COPY" << std::endl;
    XPutImage( display, 
        window, 
        gc, 
        window_img, 
        r.left, r.top, r.left, r.top,
        r.width, 
        r.height);
  }
  else
  {
    int row_len = rbuffer.width() * sys_bpp / 8;
    unsigned char* buf_tmp = 
      new unsigned char[row_len * rbuffer.height()];

    agg::rendering_buffer rbuf_tmp;
    rbuf_tmp.attach(buf_tmp, 
        rbuffer.width(), 
        rbuffer.height(), 
        flip_y ? -row_len : row_len);

    using namespace agg;

    switch(sys_format)            
    {
      default: break;
      case pix_format_rgb555:
               switch(format)
               {
                 default: break;
                 case pix_format_rgb555: color_conv(&rbuf_tmp, &rbuffer, color_conv_rgb555_to_rgb555()); break;
                 case pix_format_rgb565: color_conv(&rbuf_tmp, &rbuffer, color_conv_rgb565_to_rgb555()); break;
                 case pix_format_rgb24:  color_conv(&rbuf_tmp, &rbuffer, color_conv_rgb24_to_rgb555());  break;
                 case pix_format_bgr24:  color_conv(&rbuf_tmp, &rbuffer, color_conv_bgr24_to_rgb555());  break;
                 case pix_format_rgba32: color_conv(&rbuf_tmp, &rbuffer, color_conv_rgba32_to_rgb555()); break;
                 case pix_format_argb32: color_conv(&rbuf_tmp, &rbuffer, color_conv_argb32_to_rgb555()); break;
                 case pix_format_bgra32: color_conv(&rbuf_tmp, &rbuffer, color_conv_bgra32_to_rgb555()); break;
                 case pix_format_abgr32: color_conv(&rbuf_tmp, &rbuffer, color_conv_abgr32_to_rgb555()); break;
               }
               break;

      case pix_format_rgb565:
               switch(format)
               {
                 default: break;
                 case pix_format_rgb555: color_conv(&rbuf_tmp, &rbuffer, color_conv_rgb555_to_rgb565()); break;
                 case pix_format_rgb565: color_conv(&rbuf_tmp, &rbuffer, color_conv_rgb565_to_rgb565()); break;
                 case pix_format_rgb24:  color_conv(&rbuf_tmp, &rbuffer, color_conv_rgb24_to_rgb565());  break;
                 case pix_format_bgr24:  color_conv(&rbuf_tmp, &rbuffer, color_conv_bgr24_to_rgb565());  break;
                 case pix_format_rgba32: color_conv(&rbuf_tmp, &rbuffer, color_conv_rgba32_to_rgb565()); break;
                 case pix_format_argb32: color_conv(&rbuf_tmp, &rbuffer, color_conv_argb32_to_rgb565()); break;
                 case pix_format_bgra32: color_conv(&rbuf_tmp, &rbuffer, color_conv_bgra32_to_rgb565()); break;
                 case pix_format_abgr32: color_conv(&rbuf_tmp, &rbuffer, color_conv_abgr32_to_rgb565()); break;
               }
               break;

      case pix_format_rgba32:
               switch(format)
               {
                 default: break;
                 case pix_format_rgb555: color_conv(&rbuf_tmp, &rbuffer, color_conv_rgb555_to_rgba32()); break;
                 case pix_format_rgb565: color_conv(&rbuf_tmp, &rbuffer, color_conv_rgb565_to_rgba32()); break;
                 case pix_format_rgb24:  color_conv(&rbuf_tmp, &rbuffer, color_conv_rgb24_to_rgba32());  break;
                 case pix_format_bgr24:  color_conv(&rbuf_tmp, &rbuffer, color_conv_bgr24_to_rgba32());  break;
                 case pix_format_rgba32: color_conv(&rbuf_tmp, &rbuffer, color_conv_rgba32_to_rgba32()); break;
                 case pix_format_argb32: color_conv(&rbuf_tmp, &rbuffer, color_conv_argb32_to_rgba32()); break;
                 case pix_format_bgra32: color_conv(&rbuf_tmp, &rbuffer, color_conv_bgra32_to_rgba32()); break;
                 case pix_format_abgr32: color_conv(&rbuf_tmp, &rbuffer, color_conv_abgr32_to_rgba32()); break;
               }
               break;

      case pix_format_abgr32:
               switch(format)
               {
                 default: break;
                 case pix_format_rgb555: color_conv(&rbuf_tmp, &rbuffer, color_conv_rgb555_to_abgr32()); break;
                 case pix_format_rgb565: color_conv(&rbuf_tmp, &rbuffer, color_conv_rgb565_to_abgr32()); break;
                 case pix_format_rgb24:  color_conv(&rbuf_tmp, &rbuffer, color_conv_rgb24_to_abgr32());  break;
                 case pix_format_bgr24:  color_conv(&rbuf_tmp, &rbuffer, color_conv_bgr24_to_abgr32());  break;
                 case pix_format_abgr32: color_conv(&rbuf_tmp, &rbuffer, color_conv_abgr32_to_abgr32()); break;
                 case pix_format_rgba32: color_conv(&rbuf_tmp, &rbuffer, color_conv_rgba32_to_abgr32()); break;
                 case pix_format_argb32: color_conv(&rbuf_tmp, &rbuffer, color_conv_argb32_to_abgr32()); break;
                 case pix_format_bgra32: color_conv(&rbuf_tmp, &rbuffer, color_conv_bgra32_to_abgr32()); break;
               }
               break;

      case pix_format_argb32:
               switch(format)
               {
                 default: break;
                 case pix_format_rgb555: color_conv(&rbuf_tmp, &rbuffer, color_conv_rgb555_to_argb32()); break;
                 case pix_format_rgb565: color_conv(&rbuf_tmp, &rbuffer, color_conv_rgb565_to_argb32()); break;
                 case pix_format_rgb24:  color_conv(&rbuf_tmp, &rbuffer, color_conv_rgb24_to_argb32());  break;
                 case pix_format_bgr24:  color_conv(&rbuf_tmp, &rbuffer, color_conv_bgr24_to_argb32());  break;
                 case pix_format_rgba32: color_conv(&rbuf_tmp, &rbuffer, color_conv_rgba32_to_argb32()); break;
                 case pix_format_argb32: color_conv(&rbuf_tmp, &rbuffer, color_conv_argb32_to_argb32()); break;
                 case pix_format_abgr32: color_conv(&rbuf_tmp, &rbuffer, color_conv_abgr32_to_argb32()); break;
                 case pix_format_bgra32: color_conv(&rbuf_tmp, &rbuffer, color_conv_bgra32_to_argb32()); break;
               }
               break;

      case pix_format_bgra32:
               switch(format)
               {
                 default: break;
                 case pix_format_rgb555: color_conv(&rbuf_tmp, &rbuffer, color_conv_rgb555_to_bgra32()); break;
                 case pix_format_rgb565: color_conv(&rbuf_tmp, &rbuffer, color_conv_rgb565_to_bgra32()); break;
                 case pix_format_rgb24:  color_conv(&rbuf_tmp, &rbuffer, color_conv_rgb24_to_bgra32());  break;
                 case pix_format_bgr24:  color_conv(&rbuf_tmp, &rbuffer, color_conv_bgr24_to_bgra32());  break;
                 case pix_format_rgba32: color_conv(&rbuf_tmp, &rbuffer, color_conv_rgba32_to_bgra32()); break;
                 case pix_format_argb32: color_conv(&rbuf_tmp, &rbuffer, color_conv_argb32_to_bgra32()); break;
                 case pix_format_abgr32: color_conv(&rbuf_tmp, &rbuffer, color_conv_abgr32_to_bgra32()); break;
                 case pix_format_bgra32: color_conv(&rbuf_tmp, &rbuffer, color_conv_bgra32_to_bgra32()); break;
               }
               break;
    }

    window_img->data = (char*)buf_tmp;
    XPutImage(display, 
        window, 
        gc, 
        window_img, 
        r.left, r.top, r.left, r.top,
        r.width, 
        r.height);

    delete [] buf_tmp;
  }
  /*{
    std::cout << "CONVERT from " << format << " to " << sys_format << std::endl;
    int row_len = r.width * sys_bpp / 8;
    unsigned char* buf_tmp = 
      new unsigned char[row_len * r.height];

    agg::rendering_buffer rbuf_tmp;
    rbuf_tmp.attach(buf_tmp, 
        r.width, 
        r.height, 
        flip_y ? -row_len : row_len);

    using namespace agg;

    switch(sys_format)            
    {
      default: 
        break;
      case pix_format_rgb555:
        switch(format)
        {
          default: break;
          case pix_format_rgb555: detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_rgb555_to_rgb555()); break;
          case pix_format_rgb565: detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_rgb565_to_rgb555()); break;
          case pix_format_rgb24:  detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_rgb24_to_rgb555());  break;
          case pix_format_bgr24:  detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_bgr24_to_rgb555());  break;
          case pix_format_rgba32: detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_rgba32_to_rgb555()); break;
          case pix_format_argb32: detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_argb32_to_rgb555()); break;
          case pix_format_bgra32: detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_bgra32_to_rgb555()); break;
          case pix_format_abgr32: detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_abgr32_to_rgb555()); break;
        }
        break;

      case pix_format_rgb565:
        switch(format)
        {
          default: break;
          case pix_format_rgb555: detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_rgb555_to_rgb565()); break;
          case pix_format_rgb565: detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_rgb565_to_rgb565()); break;
          case pix_format_rgb24:  detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_rgb24_to_rgb565());  break;
          case pix_format_bgr24:  detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_bgr24_to_rgb565());  break;
          case pix_format_rgba32: detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_rgba32_to_rgb565()); break;
          case pix_format_argb32: detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_argb32_to_rgb565()); break;
          case pix_format_bgra32: detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_bgra32_to_rgb565()); break;
          case pix_format_abgr32: detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_abgr32_to_rgb565()); break;
        }
        break;

      case pix_format_rgba32:
        switch(format)
        {
          default: break;
          case pix_format_rgb555: detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_rgb555_to_rgba32()); break;
          case pix_format_rgb565: detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_rgb565_to_rgba32()); break;
          case pix_format_rgb24:  detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_rgb24_to_rgba32());  break;
          case pix_format_bgr24:  detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_bgr24_to_rgba32());  break;
          case pix_format_rgba32: detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_rgba32_to_rgba32()); break;
          case pix_format_argb32: detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_argb32_to_rgba32()); break;
          case pix_format_bgra32: detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_bgra32_to_rgba32()); break;
          case pix_format_abgr32: detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_abgr32_to_rgba32()); break;
        }
        break;

      case pix_format_abgr32:
        switch(format)
        {
          default: break;
          case pix_format_rgb555: detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_rgb555_to_abgr32()); break;
          case pix_format_rgb565: detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_rgb565_to_abgr32()); break;
          case pix_format_rgb24:  detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_rgb24_to_abgr32());  break;
          case pix_format_bgr24:  detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_bgr24_to_abgr32());  break;
          case pix_format_abgr32: detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_abgr32_to_abgr32()); break;
          case pix_format_rgba32: detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_rgba32_to_abgr32()); break;
          case pix_format_argb32: detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_argb32_to_abgr32()); break;
          case pix_format_bgra32: detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_bgra32_to_abgr32()); break;
        }
        break;

      case pix_format_argb32:
        switch(format)
        {
          default: break;
          case pix_format_rgb555: detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_rgb555_to_argb32()); break;
          case pix_format_rgb565: detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_rgb565_to_argb32()); break;
          case pix_format_rgb24:  detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_rgb24_to_argb32());  break;
          case pix_format_bgr24:  detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_bgr24_to_argb32());  break;
          case pix_format_rgba32: detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_rgba32_to_argb32()); break;
          case pix_format_argb32: detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_argb32_to_argb32()); break;
          case pix_format_abgr32: detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_abgr32_to_argb32()); break;
          case pix_format_bgra32: detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_bgra32_to_argb32()); break;
        }
        break;

      case pix_format_bgra32:
        switch(format)
        {
          default: break;
          case pix_format_rgb555: detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_rgb555_to_bgra32()); break;
          case pix_format_rgb565: detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_rgb565_to_bgra32()); break;
          case pix_format_rgb24:  detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_rgb24_to_bgra32());  break;
          case pix_format_bgr24:  detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_bgr24_to_bgra32());  break;
          case pix_format_rgba32: detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_rgba32_to_bgra32()); break;
          case pix_format_argb32: detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_argb32_to_bgra32()); break;
          case pix_format_abgr32: detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_abgr32_to_bgra32()); break;
          case pix_format_bgra32: detail::sub_color_conv( r, &rbuf_tmp, &rbuffer, color_conv_bgra32_to_bgra32()); break;
        }
        break;
    }
    window_img->data = (char*)buf_tmp;
    XPutImage(display, 
        window, 
        gc, 
        window_img, 
        r.left, r.top, r.left, r.top,
        r.width, 
        r.height);
 */ /*   XImage * temp_image = XCreateImage(display, vis, depth, ZPixmap, 
        0, reinterpret_cast<char*>(buf_tmp),
        r.width, r.height, sys_bpp,
        r.width * (sys_bpp / 8));

    if( temp_image ){
      temp_image->byte_order = byte_order;
      XPutImage(display, window, gc, 
          temp_image, 0, 0, r.left, r.top,
          r.width, r.height);

      temp_image->data = 0;

      XDestroyImage( temp_image );
    }*/
/*
    delete [] buf_tmp;
  }*/

}
void antigrain::update_window() {
  if(window_img == 0) return;
  window_img->data = (char*)window_buffer;

  if( format == sys_format)
  {
    XPutImage( display, 
        window, 
        gc, 
        window_img, 
        0, 0, 0, 0,
        rbuffer.width(), 
        rbuffer.height());
  }
  else
  {
    int row_len = rbuffer.width() * sys_bpp / 8;
    unsigned char* buf_tmp = 
      new unsigned char[row_len * rbuffer.height()];

    agg::rendering_buffer rbuf_tmp;
    rbuf_tmp.attach(buf_tmp, 
        rbuffer.width(), 
        rbuffer.height(), 
        flip_y ? -row_len : row_len);

    using namespace agg;

    switch(sys_format)            
    {
      default: break;
      case pix_format_rgb555:
               switch(format)
               {
                 default: break;
                 case pix_format_rgb555: color_conv(&rbuf_tmp, &rbuffer, color_conv_rgb555_to_rgb555()); break;
                 case pix_format_rgb565: color_conv(&rbuf_tmp, &rbuffer, color_conv_rgb565_to_rgb555()); break;
                 case pix_format_rgb24:  color_conv(&rbuf_tmp, &rbuffer, color_conv_rgb24_to_rgb555());  break;
                 case pix_format_bgr24:  color_conv(&rbuf_tmp, &rbuffer, color_conv_bgr24_to_rgb555());  break;
                 case pix_format_rgba32: color_conv(&rbuf_tmp, &rbuffer, color_conv_rgba32_to_rgb555()); break;
                 case pix_format_argb32: color_conv(&rbuf_tmp, &rbuffer, color_conv_argb32_to_rgb555()); break;
                 case pix_format_bgra32: color_conv(&rbuf_tmp, &rbuffer, color_conv_bgra32_to_rgb555()); break;
                 case pix_format_abgr32: color_conv(&rbuf_tmp, &rbuffer, color_conv_abgr32_to_rgb555()); break;
               }
               break;

      case pix_format_rgb565:
               switch(format)
               {
                 default: break;
                 case pix_format_rgb555: color_conv(&rbuf_tmp, &rbuffer, color_conv_rgb555_to_rgb565()); break;
                 case pix_format_rgb565: color_conv(&rbuf_tmp, &rbuffer, color_conv_rgb565_to_rgb565()); break;
                 case pix_format_rgb24:  color_conv(&rbuf_tmp, &rbuffer, color_conv_rgb24_to_rgb565());  break;
                 case pix_format_bgr24:  color_conv(&rbuf_tmp, &rbuffer, color_conv_bgr24_to_rgb565());  break;
                 case pix_format_rgba32: color_conv(&rbuf_tmp, &rbuffer, color_conv_rgba32_to_rgb565()); break;
                 case pix_format_argb32: color_conv(&rbuf_tmp, &rbuffer, color_conv_argb32_to_rgb565()); break;
                 case pix_format_bgra32: color_conv(&rbuf_tmp, &rbuffer, color_conv_bgra32_to_rgb565()); break;
                 case pix_format_abgr32: color_conv(&rbuf_tmp, &rbuffer, color_conv_abgr32_to_rgb565()); break;
               }
               break;

      case pix_format_rgba32:
               switch(format)
               {
                 default: break;
                 case pix_format_rgb555: color_conv(&rbuf_tmp, &rbuffer, color_conv_rgb555_to_rgba32()); break;
                 case pix_format_rgb565: color_conv(&rbuf_tmp, &rbuffer, color_conv_rgb565_to_rgba32()); break;
                 case pix_format_rgb24:  color_conv(&rbuf_tmp, &rbuffer, color_conv_rgb24_to_rgba32());  break;
                 case pix_format_bgr24:  color_conv(&rbuf_tmp, &rbuffer, color_conv_bgr24_to_rgba32());  break;
                 case pix_format_rgba32: color_conv(&rbuf_tmp, &rbuffer, color_conv_rgba32_to_rgba32()); break;
                 case pix_format_argb32: color_conv(&rbuf_tmp, &rbuffer, color_conv_argb32_to_rgba32()); break;
                 case pix_format_bgra32: color_conv(&rbuf_tmp, &rbuffer, color_conv_bgra32_to_rgba32()); break;
                 case pix_format_abgr32: color_conv(&rbuf_tmp, &rbuffer, color_conv_abgr32_to_rgba32()); break;
               }
               break;

      case pix_format_abgr32:
               switch(format)
               {
                 default: break;
                 case pix_format_rgb555: color_conv(&rbuf_tmp, &rbuffer, color_conv_rgb555_to_abgr32()); break;
                 case pix_format_rgb565: color_conv(&rbuf_tmp, &rbuffer, color_conv_rgb565_to_abgr32()); break;
                 case pix_format_rgb24:  color_conv(&rbuf_tmp, &rbuffer, color_conv_rgb24_to_abgr32());  break;
                 case pix_format_bgr24:  color_conv(&rbuf_tmp, &rbuffer, color_conv_bgr24_to_abgr32());  break;
                 case pix_format_abgr32: color_conv(&rbuf_tmp, &rbuffer, color_conv_abgr32_to_abgr32()); break;
                 case pix_format_rgba32: color_conv(&rbuf_tmp, &rbuffer, color_conv_rgba32_to_abgr32()); break;
                 case pix_format_argb32: color_conv(&rbuf_tmp, &rbuffer, color_conv_argb32_to_abgr32()); break;
                 case pix_format_bgra32: color_conv(&rbuf_tmp, &rbuffer, color_conv_bgra32_to_abgr32()); break;
               }
               break;

      case pix_format_argb32:
               switch(format)
               {
                 default: break;
                 case pix_format_rgb555: color_conv(&rbuf_tmp, &rbuffer, color_conv_rgb555_to_argb32()); break;
                 case pix_format_rgb565: color_conv(&rbuf_tmp, &rbuffer, color_conv_rgb565_to_argb32()); break;
                 case pix_format_rgb24:  color_conv(&rbuf_tmp, &rbuffer, color_conv_rgb24_to_argb32());  break;
                 case pix_format_bgr24:  color_conv(&rbuf_tmp, &rbuffer, color_conv_bgr24_to_argb32());  break;
                 case pix_format_rgba32: color_conv(&rbuf_tmp, &rbuffer, color_conv_rgba32_to_argb32()); break;
                 case pix_format_argb32: color_conv(&rbuf_tmp, &rbuffer, color_conv_argb32_to_argb32()); break;
                 case pix_format_abgr32: color_conv(&rbuf_tmp, &rbuffer, color_conv_abgr32_to_argb32()); break;
                 case pix_format_bgra32: color_conv(&rbuf_tmp, &rbuffer, color_conv_bgra32_to_argb32()); break;
               }
               break;

      case pix_format_bgra32:
               switch(format)
               {
                 default: break;
                 case pix_format_rgb555: color_conv(&rbuf_tmp, &rbuffer, color_conv_rgb555_to_bgra32()); break;
                 case pix_format_rgb565: color_conv(&rbuf_tmp, &rbuffer, color_conv_rgb565_to_bgra32()); break;
                 case pix_format_rgb24:  color_conv(&rbuf_tmp, &rbuffer, color_conv_rgb24_to_bgra32());  break;
                 case pix_format_bgr24:  color_conv(&rbuf_tmp, &rbuffer, color_conv_bgr24_to_bgra32());  break;
                 case pix_format_rgba32: color_conv(&rbuf_tmp, &rbuffer, color_conv_rgba32_to_bgra32()); break;
                 case pix_format_argb32: color_conv(&rbuf_tmp, &rbuffer, color_conv_argb32_to_bgra32()); break;
                 case pix_format_abgr32: color_conv(&rbuf_tmp, &rbuffer, color_conv_abgr32_to_bgra32()); break;
                 case pix_format_bgra32: color_conv(&rbuf_tmp, &rbuffer, color_conv_bgra32_to_bgra32()); break;
               }
               break;
    }

    window_img->data = (char*)buf_tmp;
    XPutImage(display, 
        window, 
        gc, 
        window_img, 
        0, 0, 0, 0,
        rbuffer.width(), 
        rbuffer.height());

    delete [] buf_tmp;
  }

}


}}}
