// Copyright (C) 2004-2005 by Andreas Pokorny andreas.pokorny@gmail.com
// Content: GOTT internal antigrain handling
// Authors: Andreas Pokorny
//
// This file is part of the Gott Project (http://gott.sf.net)
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include <X11/X.h>
#include <utility>
#include <iostream>
#include <gott/ui/x11/agg_detail.hpp>
#include <agg2/agg_basics.h>
#include <agg2/util/agg_color_conv_rgb8.h>

namespace gott{namespace ui{namespace x11{namespace detail{

agg_buffer::agg_buffer( Display* d, int s, Window h, Visual* v, int de )
  : display(d),window_buffer(0), vis(v), handle(h), gc(0), window_img(0), depth(de), bpp(32), screen(s)  {

  unsigned long r_mask = vis->red_mask;
  unsigned long g_mask = vis->green_mask;
  unsigned long b_mask = vis->blue_mask;
  if( depth < 15 || r_mask == 0 || g_mask == 0 || b_mask == 0)
    throw std::runtime_error("failed to find a good visual");

  int t = 1;
  int hw_byte_order = LSBFirst;
  if(* reinterpret_cast<char*>(&t) == 0) hw_byte_order = MSBFirst;

  // Perceive SYS-format by mask
  switch(depth)
  {
    case 15:
      sys_bpp = 16;
      if(r_mask == 0x7C00 && g_mask == 0x3E0 && b_mask == 0x1F)
      {
        sys_format = pixel_format::rgb555;
        byte_order = hw_byte_order;
      }
      break;

    case 16:
      sys_bpp = 16;
      if(r_mask == 0xF800 && g_mask == 0x7E0 && b_mask == 0x1F)
      {
        sys_format = pixel_format::rgb565;
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
            case pixel_format::rgba32:
              sys_format = pixel_format::rgba32;
              byte_order = LSBFirst;
              break;

            case pixel_format::abgr32:
              sys_format = pixel_format::abgr32;
              byte_order = MSBFirst;
              break;

            default:                            
              byte_order = hw_byte_order;
              sys_format = 
                (hw_byte_order == LSBFirst) ?
                pixel_format::rgba32 :
                pixel_format::abgr32;
              break;
          }
        }

        if(r_mask == 0xFF0000 && b_mask == 0xFF)
        {
          switch(format)
          {
            case pixel_format::argb32:
              sys_format = pixel_format::argb32;
              byte_order = MSBFirst;
              break;

            case pixel_format::bgra32:
              sys_format = pixel_format::bgra32;
              byte_order = LSBFirst;
              break;

            default:                            
              byte_order = hw_byte_order;
              sys_format = 
                (hw_byte_order == MSBFirst) ?
                pixel_format::argb32 :
                pixel_format::bgra32;
              break;
          }
        }
      }
      break;
  }

  if(sys_format == pixel_format::undefined)
    throw std::runtime_error("RGB masks are not compatible with AGG pixel formats");

  gc = XCreateGC(display, handle, 0, 0); 
 }

void agg_buffer::resize_buffer( rect const& new_size ){
  if( new_size.width != buffer.width()  
      || new_size.height != buffer.height() ){

    delete [] window_buffer;

    if( window_img ) {
      window_img->data = 0;
      XDestroyImage(window_img);
    }

    window_buffer = new unsigned char[new_size.width * new_size.height * ( bpp / 8 )];

    buffer.attach( window_buffer, new_size.width, new_size.height,  new_size.width*(bpp/8) );
    window_img = XCreateImage( display, vis, depth, ZPixmap, 0, reinterpret_cast<char*>(window_buffer)
        , new_size.width, new_size.height, sys_bpp, new_size.width * (sys_bpp / 8));
    window_img->byte_order = byte_order;
  }

}

void agg_buffer::update_rect( rect const& r )  {
  if(window_img == 0) 
    return;

  window_img->data = reinterpret_cast<char*>(window_buffer);

  if( format == sys_format)
  {
    std::cout << "COPY" << std::endl;
    XPutImage( display, 
        handle, 
        gc, 
        window_img, 
        r.left, r.top, r.left, r.top,
        r.width, 
        r.height);
  }
  else
  {
    int row_len = buffer.width() * sys_bpp / 8;
    unsigned char* buf_tmp = 
      new unsigned char[row_len * buffer.height()];

    agg::rendering_buffer rbuf_tmp;
    rbuf_tmp.attach(buf_tmp, 
        buffer.width(), 
        buffer.height(), 
        row_len);

    using namespace agg;

    switch(sys_format)            
    {
      default: break;
      case pixel_format::rgb555:
               switch(format)
               {
                 default: break;
                 case pixel_format::rgb555: color_conv(&rbuf_tmp, &buffer, color_conv_rgb555_to_rgb555()); break;
                 case pixel_format::rgb565: color_conv(&rbuf_tmp, &buffer, color_conv_rgb565_to_rgb555()); break;
                 case pixel_format::rgb24:  color_conv(&rbuf_tmp, &buffer, color_conv_rgb24_to_rgb555());  break;
                 case pixel_format::bgr24:  color_conv(&rbuf_tmp, &buffer, color_conv_bgr24_to_rgb555());  break;
                 case pixel_format::rgba32: color_conv(&rbuf_tmp, &buffer, color_conv_rgba32_to_rgb555()); break;
                 case pixel_format::argb32: color_conv(&rbuf_tmp, &buffer, color_conv_argb32_to_rgb555()); break;
                 case pixel_format::bgra32: color_conv(&rbuf_tmp, &buffer, color_conv_bgra32_to_rgb555()); break;
                 case pixel_format::abgr32: color_conv(&rbuf_tmp, &buffer, color_conv_abgr32_to_rgb555()); break;
               }
               break;

      case pixel_format::rgb565:
               switch(format)
               {
                 default: break;
                 case pixel_format::rgb555: color_conv(&rbuf_tmp, &buffer, color_conv_rgb555_to_rgb565()); break;
                 case pixel_format::rgb565: color_conv(&rbuf_tmp, &buffer, color_conv_rgb565_to_rgb565()); break;
                 case pixel_format::rgb24:  color_conv(&rbuf_tmp, &buffer, color_conv_rgb24_to_rgb565());  break;
                 case pixel_format::bgr24:  color_conv(&rbuf_tmp, &buffer, color_conv_bgr24_to_rgb565());  break;
                 case pixel_format::rgba32: color_conv(&rbuf_tmp, &buffer, color_conv_rgba32_to_rgb565()); break;
                 case pixel_format::argb32: color_conv(&rbuf_tmp, &buffer, color_conv_argb32_to_rgb565()); break;
                 case pixel_format::bgra32: color_conv(&rbuf_tmp, &buffer, color_conv_bgra32_to_rgb565()); break;
                 case pixel_format::abgr32: color_conv(&rbuf_tmp, &buffer, color_conv_abgr32_to_rgb565()); break;
               }
               break;

      case pixel_format::rgba32:
               switch(format)
               {
                 default: break;
                 case pixel_format::rgb555: color_conv(&rbuf_tmp, &buffer, color_conv_rgb555_to_rgba32()); break;
                 case pixel_format::rgb565: color_conv(&rbuf_tmp, &buffer, color_conv_rgb565_to_rgba32()); break;
                 case pixel_format::rgb24:  color_conv(&rbuf_tmp, &buffer, color_conv_rgb24_to_rgba32());  break;
                 case pixel_format::bgr24:  color_conv(&rbuf_tmp, &buffer, color_conv_bgr24_to_rgba32());  break;
                 case pixel_format::rgba32: color_conv(&rbuf_tmp, &buffer, color_conv_rgba32_to_rgba32()); break;
                 case pixel_format::argb32: color_conv(&rbuf_tmp, &buffer, color_conv_argb32_to_rgba32()); break;
                 case pixel_format::bgra32: color_conv(&rbuf_tmp, &buffer, color_conv_bgra32_to_rgba32()); break;
                 case pixel_format::abgr32: color_conv(&rbuf_tmp, &buffer, color_conv_abgr32_to_rgba32()); break;
               }
               break;

      case pixel_format::abgr32:
               switch(format)
               {
                 default: break;
                 case pixel_format::rgb555: color_conv(&rbuf_tmp, &buffer, color_conv_rgb555_to_abgr32()); break;
                 case pixel_format::rgb565: color_conv(&rbuf_tmp, &buffer, color_conv_rgb565_to_abgr32()); break;
                 case pixel_format::rgb24:  color_conv(&rbuf_tmp, &buffer, color_conv_rgb24_to_abgr32());  break;
                 case pixel_format::bgr24:  color_conv(&rbuf_tmp, &buffer, color_conv_bgr24_to_abgr32());  break;
                 case pixel_format::abgr32: color_conv(&rbuf_tmp, &buffer, color_conv_abgr32_to_abgr32()); break;
                 case pixel_format::rgba32: color_conv(&rbuf_tmp, &buffer, color_conv_rgba32_to_abgr32()); break;
                 case pixel_format::argb32: color_conv(&rbuf_tmp, &buffer, color_conv_argb32_to_abgr32()); break;
                 case pixel_format::bgra32: color_conv(&rbuf_tmp, &buffer, color_conv_bgra32_to_abgr32()); break;
               }
               break;

      case pixel_format::argb32:
               switch(format)
               {
                 default: break;
                 case pixel_format::rgb555: color_conv(&rbuf_tmp, &buffer, color_conv_rgb555_to_argb32()); break;
                 case pixel_format::rgb565: color_conv(&rbuf_tmp, &buffer, color_conv_rgb565_to_argb32()); break;
                 case pixel_format::rgb24:  color_conv(&rbuf_tmp, &buffer, color_conv_rgb24_to_argb32());  break;
                 case pixel_format::bgr24:  color_conv(&rbuf_tmp, &buffer, color_conv_bgr24_to_argb32());  break;
                 case pixel_format::rgba32: color_conv(&rbuf_tmp, &buffer, color_conv_rgba32_to_argb32()); break;
                 case pixel_format::argb32: color_conv(&rbuf_tmp, &buffer, color_conv_argb32_to_argb32()); break;
                 case pixel_format::abgr32: color_conv(&rbuf_tmp, &buffer, color_conv_abgr32_to_argb32()); break;
                 case pixel_format::bgra32: color_conv(&rbuf_tmp, &buffer, color_conv_bgra32_to_argb32()); break;
               }
               break;

      case pixel_format::bgra32:
               switch(format)
               {
                 default: break;
                 case pixel_format::rgb555: color_conv(&rbuf_tmp, &buffer, color_conv_rgb555_to_bgra32()); break;
                 case pixel_format::rgb565: color_conv(&rbuf_tmp, &buffer, color_conv_rgb565_to_bgra32()); break;
                 case pixel_format::rgb24:  color_conv(&rbuf_tmp, &buffer, color_conv_rgb24_to_bgra32());  break;
                 case pixel_format::bgr24:  color_conv(&rbuf_tmp, &buffer, color_conv_bgr24_to_bgra32());  break;
                 case pixel_format::rgba32: color_conv(&rbuf_tmp, &buffer, color_conv_rgba32_to_bgra32()); break;
                 case pixel_format::argb32: color_conv(&rbuf_tmp, &buffer, color_conv_argb32_to_bgra32()); break;
                 case pixel_format::abgr32: color_conv(&rbuf_tmp, &buffer, color_conv_abgr32_to_bgra32()); break;
                 case pixel_format::bgra32: color_conv(&rbuf_tmp, &buffer, color_conv_bgra32_to_bgra32()); break;
               }
               break;
    }

    window_img->data = reinterpret_cast<char*>(buf_tmp);
    XPutImage(display, 
        handle, 
        gc, 
        window_img, 
        r.left, r.top, r.left, r.top,
        r.width, 
        r.height);

    delete [] buf_tmp;
  }
  /*
   * TODO FIXME!!
   *
   * {
    std::cout << "CONVERT from " << format << " to " << sys_format << std::endl;
    int row_len = r.width * sys_bpp / 8;
    unsigned char* buf_tmp = 
      new unsigned char[row_len * r.height];

    agg::rendering_buffer rbuf_tmp;
    rbuf_tmp.attach(buf_tmp, 
        r.width, 
        r.height, 
         row_len);

    using namespace agg;

    switch(sys_format)            
    {
      default: 
        break;
      case pixel_format::rgb555:
        switch(format)
        {
          default: break;
          case pixel_format::rgb555: detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_rgb555_to_rgb555()); break;
          case pixel_format::rgb565: detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_rgb565_to_rgb555()); break;
          case pixel_format::rgb24:  detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_rgb24_to_rgb555());  break;
          case pixel_format::bgr24:  detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_bgr24_to_rgb555());  break;
          case pixel_format::rgba32: detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_rgba32_to_rgb555()); break;
          case pixel_format::argb32: detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_argb32_to_rgb555()); break;
          case pixel_format::bgra32: detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_bgra32_to_rgb555()); break;
          case pixel_format::abgr32: detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_abgr32_to_rgb555()); break;
        }
        break;

      case pixel_format::rgb565:
        switch(format)
        {
          default: break;
          case pixel_format::rgb555: detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_rgb555_to_rgb565()); break;
          case pixel_format::rgb565: detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_rgb565_to_rgb565()); break;
          case pixel_format::rgb24:  detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_rgb24_to_rgb565());  break;
          case pixel_format::bgr24:  detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_bgr24_to_rgb565());  break;
          case pixel_format::rgba32: detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_rgba32_to_rgb565()); break;
          case pixel_format::argb32: detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_argb32_to_rgb565()); break;
          case pixel_format::bgra32: detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_bgra32_to_rgb565()); break;
          case pixel_format::abgr32: detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_abgr32_to_rgb565()); break;
        }
        break;

      case pixel_format::rgba32:
        switch(format)
        {
          default: break;
          case pixel_format::rgb555: detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_rgb555_to_rgba32()); break;
          case pixel_format::rgb565: detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_rgb565_to_rgba32()); break;
          case pixel_format::rgb24:  detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_rgb24_to_rgba32());  break;
          case pixel_format::bgr24:  detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_bgr24_to_rgba32());  break;
          case pixel_format::rgba32: detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_rgba32_to_rgba32()); break;
          case pixel_format::argb32: detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_argb32_to_rgba32()); break;
          case pixel_format::bgra32: detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_bgra32_to_rgba32()); break;
          case pixel_format::abgr32: detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_abgr32_to_rgba32()); break;
        }
        break;

      case pixel_format::abgr32:
        switch(format)
        {
          default: break;
          case pixel_format::rgb555: detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_rgb555_to_abgr32()); break;
          case pixel_format::rgb565: detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_rgb565_to_abgr32()); break;
          case pixel_format::rgb24:  detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_rgb24_to_abgr32());  break;
          case pixel_format::bgr24:  detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_bgr24_to_abgr32());  break;
          case pixel_format::abgr32: detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_abgr32_to_abgr32()); break;
          case pixel_format::rgba32: detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_rgba32_to_abgr32()); break;
          case pixel_format::argb32: detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_argb32_to_abgr32()); break;
          case pixel_format::bgra32: detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_bgra32_to_abgr32()); break;
        }
        break;

      case pixel_format::argb32:
        switch(format)
        {
          default: break;
          case pixel_format::rgb555: detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_rgb555_to_argb32()); break;
          case pixel_format::rgb565: detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_rgb565_to_argb32()); break;
          case pixel_format::rgb24:  detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_rgb24_to_argb32());  break;
          case pixel_format::bgr24:  detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_bgr24_to_argb32());  break;
          case pixel_format::rgba32: detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_rgba32_to_argb32()); break;
          case pixel_format::argb32: detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_argb32_to_argb32()); break;
          case pixel_format::abgr32: detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_abgr32_to_argb32()); break;
          case pixel_format::bgra32: detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_bgra32_to_argb32()); break;
        }
        break;

      case pixel_format::bgra32:
        switch(format)
        {
          default: break;
          case pixel_format::rgb555: detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_rgb555_to_bgra32()); break;
          case pixel_format::rgb565: detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_rgb565_to_bgra32()); break;
          case pixel_format::rgb24:  detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_rgb24_to_bgra32());  break;
          case pixel_format::bgr24:  detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_bgr24_to_bgra32());  break;
          case pixel_format::rgba32: detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_rgba32_to_bgra32()); break;
          case pixel_format::argb32: detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_argb32_to_bgra32()); break;
          case pixel_format::abgr32: detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_abgr32_to_bgra32()); break;
          case pixel_format::bgra32: detail::sub_color_conv( r, &rbuf_tmp, &buffer, color_conv_bgra32_to_bgra32()); break;
        }
        break;
    }
    window_img->data = reinterpret_cast<char*>(buf_tmp);
    XPutImage(display, 
        handle, 
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
      XPutImage(display, handle, gc, 
          temp_image, 0, 0, r.left, r.top,
          r.width, r.height);

      temp_image->data = 0;

      XDestroyImage( temp_image );
    }*/
/*
    delete [] buf_tmp;
  }*/
}


void agg_buffer::update_window() {
  if(window_img == 0) return;
  window_img->data = reinterpret_cast<char*>(window_buffer);

  if( format == sys_format)
  {
    XPutImage( display, 
        handle, 
        gc, 
        window_img, 
        0, 0, 0, 0,
        buffer.width(), 
        buffer.height());
  }
  else
  {
    int row_len = buffer.width() * sys_bpp / 8;
    unsigned char* buf_tmp = 
      new unsigned char[row_len * buffer.height()];

    agg::rendering_buffer rbuf_tmp;
    rbuf_tmp.attach(buf_tmp, 
        buffer.width(), 
        buffer.height(), 
         row_len);

    using namespace agg;

    switch(sys_format)            
    {
      default: break;
      case pixel_format::rgb555:
               switch(format)
               {
                 default: break;
                 case pixel_format::rgb555: color_conv(&rbuf_tmp, &buffer, color_conv_rgb555_to_rgb555()); break;
                 case pixel_format::rgb565: color_conv(&rbuf_tmp, &buffer, color_conv_rgb565_to_rgb555()); break;
                 case pixel_format::rgb24:  color_conv(&rbuf_tmp, &buffer, color_conv_rgb24_to_rgb555());  break;
                 case pixel_format::bgr24:  color_conv(&rbuf_tmp, &buffer, color_conv_bgr24_to_rgb555());  break;
                 case pixel_format::rgba32: color_conv(&rbuf_tmp, &buffer, color_conv_rgba32_to_rgb555()); break;
                 case pixel_format::argb32: color_conv(&rbuf_tmp, &buffer, color_conv_argb32_to_rgb555()); break;
                 case pixel_format::bgra32: color_conv(&rbuf_tmp, &buffer, color_conv_bgra32_to_rgb555()); break;
                 case pixel_format::abgr32: color_conv(&rbuf_tmp, &buffer, color_conv_abgr32_to_rgb555()); break;
               }
               break;

      case pixel_format::rgb565:
               switch(format)
               {
                 default: break;
                 case pixel_format::rgb555: color_conv(&rbuf_tmp, &buffer, color_conv_rgb555_to_rgb565()); break;
                 case pixel_format::rgb565: color_conv(&rbuf_tmp, &buffer, color_conv_rgb565_to_rgb565()); break;
                 case pixel_format::rgb24:  color_conv(&rbuf_tmp, &buffer, color_conv_rgb24_to_rgb565());  break;
                 case pixel_format::bgr24:  color_conv(&rbuf_tmp, &buffer, color_conv_bgr24_to_rgb565());  break;
                 case pixel_format::rgba32: color_conv(&rbuf_tmp, &buffer, color_conv_rgba32_to_rgb565()); break;
                 case pixel_format::argb32: color_conv(&rbuf_tmp, &buffer, color_conv_argb32_to_rgb565()); break;
                 case pixel_format::bgra32: color_conv(&rbuf_tmp, &buffer, color_conv_bgra32_to_rgb565()); break;
                 case pixel_format::abgr32: color_conv(&rbuf_tmp, &buffer, color_conv_abgr32_to_rgb565()); break;
               }
               break;

      case pixel_format::rgba32:
               switch(format)
               {
                 default: break;
                 case pixel_format::rgb555: color_conv(&rbuf_tmp, &buffer, color_conv_rgb555_to_rgba32()); break;
                 case pixel_format::rgb565: color_conv(&rbuf_tmp, &buffer, color_conv_rgb565_to_rgba32()); break;
                 case pixel_format::rgb24:  color_conv(&rbuf_tmp, &buffer, color_conv_rgb24_to_rgba32());  break;
                 case pixel_format::bgr24:  color_conv(&rbuf_tmp, &buffer, color_conv_bgr24_to_rgba32());  break;
                 case pixel_format::rgba32: color_conv(&rbuf_tmp, &buffer, color_conv_rgba32_to_rgba32()); break;
                 case pixel_format::argb32: color_conv(&rbuf_tmp, &buffer, color_conv_argb32_to_rgba32()); break;
                 case pixel_format::bgra32: color_conv(&rbuf_tmp, &buffer, color_conv_bgra32_to_rgba32()); break;
                 case pixel_format::abgr32: color_conv(&rbuf_tmp, &buffer, color_conv_abgr32_to_rgba32()); break;
               }
               break;

      case pixel_format::abgr32:
               switch(format)
               {
                 default: break;
                 case pixel_format::rgb555: color_conv(&rbuf_tmp, &buffer, color_conv_rgb555_to_abgr32()); break;
                 case pixel_format::rgb565: color_conv(&rbuf_tmp, &buffer, color_conv_rgb565_to_abgr32()); break;
                 case pixel_format::rgb24:  color_conv(&rbuf_tmp, &buffer, color_conv_rgb24_to_abgr32());  break;
                 case pixel_format::bgr24:  color_conv(&rbuf_tmp, &buffer, color_conv_bgr24_to_abgr32());  break;
                 case pixel_format::abgr32: color_conv(&rbuf_tmp, &buffer, color_conv_abgr32_to_abgr32()); break;
                 case pixel_format::rgba32: color_conv(&rbuf_tmp, &buffer, color_conv_rgba32_to_abgr32()); break;
                 case pixel_format::argb32: color_conv(&rbuf_tmp, &buffer, color_conv_argb32_to_abgr32()); break;
                 case pixel_format::bgra32: color_conv(&rbuf_tmp, &buffer, color_conv_bgra32_to_abgr32()); break;
               }
               break;

      case pixel_format::argb32:
               switch(format)
               {
                 default: break;
                 case pixel_format::rgb555: color_conv(&rbuf_tmp, &buffer, color_conv_rgb555_to_argb32()); break;
                 case pixel_format::rgb565: color_conv(&rbuf_tmp, &buffer, color_conv_rgb565_to_argb32()); break;
                 case pixel_format::rgb24:  color_conv(&rbuf_tmp, &buffer, color_conv_rgb24_to_argb32());  break;
                 case pixel_format::bgr24:  color_conv(&rbuf_tmp, &buffer, color_conv_bgr24_to_argb32());  break;
                 case pixel_format::rgba32: color_conv(&rbuf_tmp, &buffer, color_conv_rgba32_to_argb32()); break;
                 case pixel_format::argb32: color_conv(&rbuf_tmp, &buffer, color_conv_argb32_to_argb32()); break;
                 case pixel_format::abgr32: color_conv(&rbuf_tmp, &buffer, color_conv_abgr32_to_argb32()); break;
                 case pixel_format::bgra32: color_conv(&rbuf_tmp, &buffer, color_conv_bgra32_to_argb32()); break;
               }
               break;

      case pixel_format::bgra32:
               switch(format)
               {
                 default: break;
                 case pixel_format::rgb555: color_conv(&rbuf_tmp, &buffer, color_conv_rgb555_to_bgra32()); break;
                 case pixel_format::rgb565: color_conv(&rbuf_tmp, &buffer, color_conv_rgb565_to_bgra32()); break;
                 case pixel_format::rgb24:  color_conv(&rbuf_tmp, &buffer, color_conv_rgb24_to_bgra32());  break;
                 case pixel_format::bgr24:  color_conv(&rbuf_tmp, &buffer, color_conv_bgr24_to_bgra32());  break;
                 case pixel_format::rgba32: color_conv(&rbuf_tmp, &buffer, color_conv_rgba32_to_bgra32()); break;
                 case pixel_format::argb32: color_conv(&rbuf_tmp, &buffer, color_conv_argb32_to_bgra32()); break;
                 case pixel_format::abgr32: color_conv(&rbuf_tmp, &buffer, color_conv_abgr32_to_bgra32()); break;
                 case pixel_format::bgra32: color_conv(&rbuf_tmp, &buffer, color_conv_bgra32_to_bgra32()); break;
               }
               break;
    }

    window_img->data = reinterpret_cast<char*>(buf_tmp);
    XPutImage(display, 
        handle, 
        gc, 
        window_img, 
        0, 0, 0, 0,
        buffer.width(), 
        buffer.height());

    delete [] buf_tmp;
  }

}


void agg_buffer::blit_buffer( coord const& , agg::rendering_buffer const& , pixel_format::type ){
  // display buffer on window 
}
void agg_buffer::blit_rect( rect const& , coord const& , agg::rendering_buffer const& , pixel_format::type ) {
  // partially display buffer on window 
}
agg_buffer::~agg_buffer(){
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


std::pair<Visual*,int> pick_visual( Display* display, int screen )
{
  // Direct Color True Color ?? 
  XVisualInfo templ;
  templ.depth = 32;
  templ.red_mask = 0xFF;
  templ.green_mask = 0xFF00;
  templ.blue_mask = 0xFF0000;
  long mask = VisualRedMaskMask | VisualBlueMaskMask | VisualGreenMaskMask | VisualDepthMask;

  int num_ret;
  XVisualInfo* ret = XGetVisualInfo( display, mask, &templ, &num_ret );
  if( ret == 0 )  {
    templ.depth = 24;
    ret = XGetVisualInfo( display, mask, &templ, &num_ret );
  }
  if( ret == 0 )
    return std::make_pair( XDefaultVisual(display,screen), XDefaultDepth(display,screen) );
  else 
    return std::make_pair( ret->visual,  ret->depth );
}

}}}}
