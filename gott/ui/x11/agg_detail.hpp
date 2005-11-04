#ifndef GOTT_UI_X11_AGG_INIT_HPP_INCLUDED
#define GOTT_UI_X11_AGG_INIT_HPP_INCLUDED

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
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <agg_basics.h>
#include <agg_rendering_buffer.h>
#include <gott/ui/window_base.hpp>
#include <gott/util/geometry.hpp>

namespace gott{namespace ui{namespace x11{namespace detail{

/**
 * \brief broken partial converter.
 * Fix that code!
 * \param[in] region region to convert in pixels
 * \param[in] dst the destination buffer
 * \param[in] src the source buffer 
 * \param[in] copy_row_functor the converting functor, works on a row as a whole
 */
template<class CopyRow> 
void sub_color_conv( rect const& region,
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

/**
 * \brief Little agg_buffer represents the buffer of the window
 * able to blit buffers onto screen. 
 * The current code thinks that rgba32 it the right pixel format!
 * This is a major design problem, and should be handled by our
 * coming high level render system!
 */
struct GOTT_LOCAL agg_buffer {

  Display *display;
  agg::rendering_buffer buffer;
  unsigned char* window_buffer;
  Visual *vis;
  Window handle;
  GC gc;
  XImage* window_img;
  int depth, byte_order, sys_bpp, bpp, screen;
  pixel_format::type format, sys_format;


  agg_buffer( Display* d, int screen, Window handle, Visual* vis, int depth );
  /**
   * \brief called by x11::window whenever the screen size changes.
   */
  void resize_buffer( rect const& new_size );
  /**
   * \brief commits the contents of the rendering_buffer to the window
   */
  void update_window();
  /**
   * \brief commits the region defined by r from the rendering_buffer onto the window
   */
  void update_rect( rect const& r );

  /**
   * \brief copy the contents of the buffer onto screen 
   */
  void blit_buffer( coord const& destination, agg::rendering_buffer const& buffer, pixel_format::type buf_format = pixel_format::rgba32 );

  /**
   * \brief copy the contents described by source, from buffer to screen.
   */
  void blit_rect( rect const& source, coord const& destination, agg::rendering_buffer const& buffer, pixel_format::type buf_format = pixel_format::rgba32 );
  ~agg_buffer();
};

/**
 * \brief Sweet little visual selector.
 * Tries to select a visual with a pixelformat that matches rgba32. 
 * Otherwise the default visual is returned, this function should not 
 * fail.
 */
std::pair<Visual*,int> pick_visual( Display* display, int screen );

}}}}


#endif

