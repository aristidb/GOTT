#include <cmath>
#include "fixed_vector.hpp"

namespace gott{namespace math{

template<typename T>
struct hyper_plane2
{
  typedef vector2<T> vector_type;
  vector_type normal;
  T projection;
  
  hyper_plane2(  ) {}
  hyper_plane2( vector_type const& n, T p ): normal(n), projection(p) {}
  hyper_plane2( vector_type const& a, vector_type const& b, vector_type const& test_point ) {
    normal = (a - b);
    swap(normal[0], normal[1]);
    T length = std::sqrt( normal[0]*normal[0] + normal[1]*normal[1] );
    normal[1] = -normal[1];
    normal /= length;
    projection = (a|normal);

    if( distance( test_point ) < 0 ) { projection=-projection; normal=-normal; }
  }

  T distance( vector_type const& p ) const { return (p|normal) + projection; }
};

}}

