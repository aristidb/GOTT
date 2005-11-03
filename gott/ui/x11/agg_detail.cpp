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

#include <X.h>
#include <utility>
#include <gott/ui/x11/agg_detail.hpp>
#include <agg2/agg_basics.h>
#include <agg2/util/agg_color_conv_rgb8.h>

namespace gott{namespace ui{namespace x11{namespace detail{

agg_buffer::agg_buffer( Display* d, int s, Window h, Visual* v, int de )
  : display(d),window_buffer(0), vis(v), handle(h), gc(0), window_img(0), depth(de), screen(s)  {

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

    window_img->data = reinterpret_cast<char*>(buf_tmp);
    XPutImage(display, 
        window, 
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
    window_img->data = reinterpret_cast<char*>(buf_tmp);
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


void agg_buffer::update_window() {
  if(window_img == 0) return;
  window_img->data = reinterpret_cast<char*>(window_buffer);

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

    window_img->data = reinterpret_cast<char*>(buf_tmp);
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


void agg_buffer::blit_buffer( coord const& destination, agg_rendering_buffer const& buffer, pix_format_e buf_format ){
  // display buffer on window 
}
void agg_buffer::blit_rect( rect const& source, coord const& destination, agg_rendering_buffer const& buffer, pix_format_e buf_format ) {
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


XVisualInfo* pick_visual( Display* display,  int screen ) 
{
  // Direct Color True Color ?? 
  XVisualInfo templ;
  templ.depth = 32;
  templ.red_mask = 0xFF;
  templ.green_mask = 0xFF00;
  templ.blue_mask = 0xFF0000;
  long mask = VisualRedMaskMask | VisualBlueMaskMask | VisualGreenMaskMask | VisualDepthMask;

  int num_ret;
  XVisualInfo* ret = XGetVisualInfo( display, mask, &temp, &num_ret );
  if( ret == 0 )  {
    templ.depth = 24;
    XVisualInfo* ret = XGetVisualInfo( display, mask, &temp, &num_ret );
  }
  if( ret == 0 )
    return std::make_pair( XDefaultVisual(display,screen), XDefaultDepth(display,screen) );
  else 
    return std::make_pair( ret->visual,  ret->depth );
}

}}}}
