// Copyright (C) 2004-2005 by Andreas Pokorny andreas.pokorny@gmail.com
// Content: GOTT Misc. Utility Code
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




#ifndef GOTT_GUI_UTILITY_HPP_INCLUDED
#define GOTT_GUI_UTILITY_HPP_INCLUDED
// #pragma GCC visibility push(default) 
#include <boost/signals.hpp>
// #pragma GCC visibility pop
#include <utility>
#include <gott/util/visibility.hpp>

namespace gott{ namespace gui{ 

std::string find_font_file( std::string const& path_name, std::string const& name );
  // throw away!

struct GOTT_EXPORT vert 
{
  vert() : x(0), y(0), z(0) {}
  vert( double x_, double y_, double z_) : x(x_), y(y_), z(z_){}
  double x, y, z;
};
struct GOTT_EXPORT coord
{
  coord() : x(0), y(0) {}
  coord( int x_, int y_ ) : x(x_), y(y_){}
  int x, y;
  inline coord& operator +=(  coord const& r )  { x+=r.x;y+=r.y;return *this; }
};

inline coord operator +( coord const& l, coord const& r )  { coord t(l); t += r; return t; }

struct GOTT_EXPORT rect 
{
  long left, top; 
  std::size_t width, height;
  rect( long l, long t , std::size_t width, std::size_t height);
  rect();
  bool is_inside( coord const& c ) const;
};

struct GOTT_EXPORT window_flags
{
  enum {
    Clear = 0
      , Decoration 
      , Fullscreen
      , Visible = 4
      , KeyEvents = 1<<3
      , MouseEvents = 1<<4
      , Menu = 1<<5
      , Toolbar = 1<<6
      , Utility = 1<<7
      , Dialog = 1<<8
      , Splash = 1<<9
      , Normal = 1<<10
      , Dock = 1<<11
      , ToolTip = 1<<12
      , Open = 1<<13
      , Defaults = (MouseEvents | KeyEvents | Visible | Decoration )

  };
};

struct GOTT_EXPORT pixelformat
{
  enum Flags{
    Clear = 0
      , DrawToWindow 
      , DrawToPBuffer 
      , DoubleBuffer = 4
  };
  Flags flags;
  typedef std::pair<bool, short> buffer_bit_type; // rename!
  buffer_bit_type color, depth, samples;

  pixelformat();
};


}}

#endif

