#ifndef GOTT_GUI_PIXELFORMAT_HPP_INCLUDED
#define GOTT_GUI_PIXELFORMAT_HPP_INCLUDED

#include <utility>

namespace gott{namespace gui{
  
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
  buffer_bit_type color, depth, stencil, samples;

  pixelformat();

};

}}

#endif

