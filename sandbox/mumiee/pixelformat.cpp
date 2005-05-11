#include "pixelformat.hpp"

namespace gott{namespace gui{
pixelformat::pixelformat()
  : flags(Clear), color(false,32), depth(false,8), stencil(false,8), samples(false,0)
{
}


}}
