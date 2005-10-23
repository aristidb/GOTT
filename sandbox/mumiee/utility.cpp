#include "utility.hpp"
#include <iostream>

namespace gott{ namespace gui{ 


rect::rect( long l, long t, std::size_t w, std::size_t h)
  : left(l), top(t), width(w), height(h)
{
}
rect::rect()
  : left(0), top(0), width(1), height(1)
{}

bool rect::is_inside( coord const& c  ) const {
  return c.x >= left && c.x <= left + long(width)  &&  c.y >= top && c.y <= top + long(height);
}
  
pixelformat::pixelformat()
  : flags( DoubleBuffer ), color(false,1), depth(false,1), samples(false,0)
{
}



}}
