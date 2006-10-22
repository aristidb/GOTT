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

#include <X11/Xutil.h>
#include "cairo.hpp"

namespace gott{namespace gui{namespace x11{
cairo::cairo()
  : display(0), vis(0), window(0), screen(0), depth(0), surface(0), context(0) {}
cairo::~cairo()
{ 
  close_renderer(display, screen, window );
}

Visual * cairo::get_visual( ) {
  return vis;
}

int cairo::get_depth() const {
  return depth;
}

void cairo::prepare_visual( int screen, Display * display ) {
  this->screen = screen;
  this->display = display;

  XVisualInfo vinfo;
  if(  XMatchVisualInfo( display, screen, 24, TrueColor, &vinfo) == 0 ) {
    depth = XDefaultDepth(display,screen);
    vis = XDefaultVisual(display,screen);
  }
  else {
    vis = vinfo.visual;
    depth = vinfo.depth;
  }
}

void cairo::setup_renderer( Display* , int , Drawable win , rect const& r ){
  window = win;
  surface =  cairo_xlib_surface_create(display, window, vis, r.width, r.height);
  context = cairo_create(surface);
}

void cairo::resize_renderer( Display* , int , Drawable w, rect const& r ){
  window = w;
  cairo_xlib_surface_set_size(surface, r.width, r.height);
}


void cairo::close_renderer(Display* , int ,  Drawable ){
if( surface )
      cairo_surface_destroy( surface );
    if( context )
      cairo_destroy( context );
    surface = 0;
    context = 0;

}


void cairo::update_window() {

}


cairo_surface_t* cairo::get_surface() { return surface; };
cairo_t* cairo::get_context() { return context;}
}}}


