#ifndef GOTT_GUI_UTILITY_HPP_INCLUDED
#define GOTT_GUI_UTILITY_HPP_INCLUDED
#include <boost/signals.hpp>
#include <utility>

namespace gott{ namespace gui{ 


struct vert 
{
  vert() : x(0), y(0), z(0) {}
  vert( double x_, double y_, double z_) : x(x_), y(y_), z(z_){}
  double x, y, z;
};
struct coord
{
  coord() : x(0), y(0) {}
  coord( int x_, int y_ ) : x(x_), y(y_){}
  int x, y;
  inline coord& operator +=(  coord const& r )  { x+=r.x;y+=r.y;return *this; }
};

inline coord operator +( coord const& l, coord const& r )  { coord t(l); t += r; return t; }

struct rect 
{
  long left, top; 
  std::size_t width, height;
  rect( long l, long t , std::size_t width, std::size_t height);
  rect();
};

struct window_flags
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

struct pixelformat
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

