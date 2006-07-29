// vim:ts=2:sw=2:expandtab:autoindent:filetype=cpp:
#ifndef GOTT_UI_X11_AGG_INIT_HPP_INCLUDED
#define GOTT_UI_X11_AGG_INIT_HPP_INCLUDED

/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is a user interface abstraction library.
 *
 * The Initial Developer of the Original Code is
 * Andreas Pokorny (andreas.pokorny@gmail.com)
 * Portions created by the Initial Developer are Copyright (C) 2005
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *  Andreas Pokorny (andreas.pokorny@gmail.com)
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <agg_basics.h>
#include <agg_rendering_buffer.h>
#include <gott/ui/window_base.hpp>
#include <gott/graphics/geometry.hpp>

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

