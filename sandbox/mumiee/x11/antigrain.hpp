// Copyright (C) 2004-2005 by Andreas Pokorny andreas.pokorny@gmail.com
// Content: GOTT Antigrain support in x11 code
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


#ifndef GOTT_GUI_X11_ANTIGRAIN_HPP_INCLUDED
#define GOTT_GUI_X11_ANTIGRAIN_HPP_INCLUDED

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "../utility.hpp"
#include <agg_basics.h>
#include <agg_rendering_buffer.h>

namespace gott{namespace gui{namespace x11{ 

struct antigrain 
{
  private:
    enum pix_format_e
    {
        pix_format_undefined = 0,  // By default. No conversions are applied 
        pix_format_bw,             // 1 bit per color B/W
        pix_format_gray8,          // Simple 256 level grayscale
        pix_format_gray16,         // Simple 65535 level grayscale
        pix_format_rgb555,         // 15 bit rgb. Depends on the byte ordering!
        pix_format_rgb565,         // 16 bit rgb. Depends on the byte ordering!
        pix_format_rgbAAA,         // 30 bit rgb. Depends on the byte ordering!
        pix_format_rgbBBA,         // 32 bit rgb. Depends on the byte ordering!
        pix_format_bgrAAA,         // 30 bit bgr. Depends on the byte ordering!
        pix_format_bgrABB,         // 32 bit bgr. Depends on the byte ordering!
        pix_format_rgb24,          // R-G-B, one byte per color component
        pix_format_bgr24,          // B-G-R, native win32 BMP format.
        pix_format_rgba32,         // R-G-B-A, one byte per color component
        pix_format_argb32,         // A-R-G-B, native MAC format
        pix_format_abgr32,         // A-B-G-R, one byte per color component
        pix_format_bgra32,         // B-G-R-A, native win32 BMP format
        pix_format_rgb48,          // R-G-B, 16 bits per color component
        pix_format_bgr48,          // B-G-R, native win32 BMP format.
        pix_format_rgba64,         // R-G-B-A, 16 bits byte per color component
        pix_format_argb64,         // A-R-G-B, native MAC format
        pix_format_abgr64,         // A-B-G-R, one byte per color component
        pix_format_bgra64,         // B-G-R-A, native win32 BMP format
  
        end_of_pix_formats
    };

    Display *display;
    Visual* vis;
    Drawable window;
    int depth, byte_order, sys_bpp, bpp, screen;
    pix_format_e format, sys_format;
    GC gc;
    XImage* window_img;
    unsigned char* window_buffer;
    bool flip_y;
    agg::rendering_buffer rbuffer;
  protected:
    void prepare_visual( int screen, Display * display );
    Visual * get_visual();
    int get_depth() const;
    void setup_renderer( Display* display, int screen, Drawable window, rect const& r );
    void resize_renderer( Display* display, int screen, Drawable window, rect const& r );
    void close_renderer( Display* display, int screen, Drawable window );


  public: 
    antigrain();
    ~antigrain();
    void update_window();
    void update_rect( gott::gui::rect const& r );
    agg::rendering_buffer& get_render_buffer() { return rbuffer; }

//    void put_image(const rendering_buffer* src ... position  ...);
    // accessor to redraw blit and stuff..
};

}}}

#endif
