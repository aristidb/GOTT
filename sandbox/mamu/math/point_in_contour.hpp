
#include <utility>
#include "fixed_vector.hpp"

namespace gott{namespace math{

namespace detail{
  // quadrant_number :
  //   1 | 0
  //   --+--->
  //   3 | 2
  template<typename T>
  inline unsigned char get_quadrant_number( vector2<T> const& p ) { return ( p[0] < 0 ) + ( p[1] < 0 )*2; }

  template<typename T>
  inline signed int handle_double_switch( vector2<T> const& prev_point, vector2<T> const& point )
  {
    // difference vector:
    vector2<T> v = point - prev_point;
    // create normal of hyper_plane
    std::swap( v[0], v[1] );
    v[0]  = -v[0];

    // get step by calculateing projection onto one of the points
    return ( v*point < 0 )?-2:2;
  }


}

template<typename InputPointIterator, typename T>
bool point_in_contour( InputPointIterator begin_contour, InputPointIterator end_contour, vector2<T> const& point )
{
  signed int winding_counter = 0;
  InputPointIterator prev_it = begin_contour;
  InputPointIterator it = begin_contour;
  vector2<T> prev_pos = *prev_it - point;
  unsigned char quadrant = detail::get_quadrant_number( prev_pos );

  signed char array[4][4] = {
    { 0,-1, 1, 2}
  , { 1, 0, 2,-1}
  , {-1, 2, 0, 1}
  , { 2, 1,-1, 0}
  };

  std::cout << "start position " << prev_pos[0] << "," << prev_pos[1] << " in quadrant " << int(quadrant) << std::endl;
  while( ++it != end_contour )
  {
    vector2<T> pos = *it - point;
    unsigned char next_quadrant(detail::get_quadrant_number(pos));
    std::cout << "next position " << pos[0] << "," << pos[1] << " in quadrant "<< int(next_quadrant) << std::endl;

    switch( array[quadrant][next_quadrant] ) {
      case 1:  ++winding_counter; break;
      case -1: --winding_counter; break;
      case 2: winding_counter += detail::handle_double_switch( prev_pos, pos ); break;
      default: break;
    }

    quadrant = next_quadrant;
    prev_it = it;
    prev_pos = pos;

    std::cout << "winding_counter: " << winding_counter << std::endl;
    
  }

  // handle final edge to start point:
  vector2<T> pos = *begin_contour - point;
  unsigned char next_quadrant(detail::get_quadrant_number(pos));
  std::cout << "next position " << pos[0] << "," << pos[1]<< " in quadrant " << int(next_quadrant) << std::endl;
  switch( array[quadrant][next_quadrant] ) {
    case 1:  ++winding_counter; break;
    case -1: --winding_counter; break;
    case 2: winding_counter += detail::handle_double_switch( prev_pos, pos ); break;
    default: break;
  }

  std::cout << "winding_counter: " << winding_counter << std::endl;
  return winding_counter == 4 || winding_counter == -4;
}
 
}}
