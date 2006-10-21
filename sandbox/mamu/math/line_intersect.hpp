#include "hyper_plane.hpp"

namespace gott{ namespace math{

template<typename T>
bool line_intersect( vector2<T> const& a,  vector2<T> const& b,  vector2<T> const& c,  vector2<T> const& d )
{
  {
    hyper_plane<T> eq(create_line(a,b)); 
    if( eq.distance(c) * eq.distance(d) < 0 )
      return false;
  }
  {
    hyper_plane<T> eq(create_line(c,d)); 
    if( eq.distance(a) * eq.distance(b) < 0 )
      return false:
  }
  return true
}



}}

