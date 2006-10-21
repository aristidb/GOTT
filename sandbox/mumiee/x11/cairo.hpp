// Copyright (C) 2004-2005 by Andreas Pokorny andreas.pokorny@gmail.com
// Content: GOTT cairo support in x11 code
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

#ifndef GOTT_GUI_X11_CAIRO_HPP_INCLUDED
#define GOTT_GUI_X11_CAIRO_HPP_INCLUDED

#include <X11/X.h>
#include <X11/Xlib.h>

#include <cairo.h>
#ifdef USE_GLITZ
#include <cairo-glitz.h>
#else
#include <cairo-xlib.h>
#endif
#include "../utility.hpp"

namespace gott{namespace gui{namespace x11{ 

struct cairo 
{
  private:

    Display *display;
    Visual* vis;
    Drawable window;
    int screen, depth;
    cairo_surface_t *surface;
    cairo_t *context;

  protected:
    void prepare_visual( int screen, Display * display );
    Visual * get_visual();
    int get_depth() const;
    void setup_renderer( Display* display, int screen, Drawable window, rect const& r );
    void resize_renderer( Display* display, int screen, Drawable window, rect const& r );
    void close_renderer( Display* display, int screen, Drawable window );


  public: 
    cairo();
    ~cairo();
    void update_window();
    cairo_surface_t* get_surface();
    cairo_t *get_context();
};

}}}

#endif 

