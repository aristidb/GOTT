#include <cmath>
#include <utility>
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

  T distance( vector_type const& p ) const { return (p|normal) + projection; }
};

template<typename T>
inline hyper_plane2<T> create_oriented_hesse_line( vector2<T> const& a, vector2<T> const& b, vector2<T> const& test_point )
{
  vector2<T> normal = (a - b);
  swap(normal[0], normal[1]);
  T length = std::sqrt( normal[0]*normal[0] + normal[1]*normal[1] );
  normal[1] = -normal[1];
  normal /= length;
  T projection = (a|normal);

  if( (test_point|normal) + projection < 0 ) { projection=-projection; normal=-normal; }

  return hyper_plane2<T>( normal, projection );
}

template<typename T>
inline hyper_plane2<T> create_hesse_line( vector2<T> const& a, vector2<T> const& b )
{
  vector2<T> normal = (a - b);
  swap(normal[0], normal[1]);
  T length = std::sqrt( normal[0]*normal[0] + normal[1]*normal[1] );
  normal[1] = -normal[1];
  normal /= length;
  T projection = (a|normal);

  return hyper_plane2<T>( normal, projection );
}

template<typename T>
inline hyper_plane2<T> create_line(vector2<T> const& a, vector2<T> const& b )
{
  vector2<T> normal = (a - b);
  std::swap(normal[0], normal[1]);
  normal[1] = -normal[1];
  T projection = (a|normal);
  return hyper_plane2<T>( normal, projection );
}
// TODO Add factory functions .. the current constructors are not descritpive enough!


}}

