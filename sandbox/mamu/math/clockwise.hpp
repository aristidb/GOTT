#include <list>
#include "fixed_vector.hpp"

namespace gott{ namespace math{
  
enum order { Undefined, Clockwise, CounterClockwise };

template<typename T>
inline T get_normal( vector2<T> const& prev, vector2<T> const& current, vector2<T> const&  next )
{
  /*
   a = prev - current
   b = next - current
   /a.x\   /b.x\
 ( |a.y| x |b.y| ).z =  a.x*b.y - a.y*b-x
   \a.z/   \b.z/
   */
  return ( prev[0] - current[0] )*( next[1] - current[1] ) - ( prev[1] - current[1] )*( next[0] - current[0] );
}

template<typename T>
struct clock_order {
  std::list<vector2<T> > points;
  vector2<T> first, second;
  int counter;
//  T acc;

  clock_order ()
    : counter(0)/*,acc(0)*/{}

  void init() 
  { points.clear(); /*acc =*/first[0] = first[1] = second[0] = second[1] = counter = 0;}   

  inline order get_ordering() const
  {
    typename std::list<vector2<T> >::const_iterator c = points.begin();
    ++c;
    typename std::list<vector2<T> >::const_iterator p = c++;
    T t1 = get_normal( *p, *c, first );
    T t2 = get_normal( *c, first, second );

    return ((counter - (t1<0) + (t1>0) + (t2>0) - (t2<0)) > 0)?Clockwise:CounterClockwise;
  }
  inline void operator()( vector2<T> const& p ) {

    points.push_back(p);
    switch( points.size() ) {
      case 1: first = p; break;
      case 2: second = p; break;
      case 4: points.pop_front(); // fall through intended
      case 3: 
              typename std::list<vector2<T> >::const_iterator l = points.begin();
              typename std::list<vector2<T> >::const_iterator p = l++;
              typename std::list<vector2<T> >::const_iterator c = l++;
              T t = get_normal( *p, *c, *l );
              if( t > 0 ) ++counter;
              else --counter;
              //acc += t;
    }
  }
};

template<typename T>
struct clock_order_frontend {
  clock_order<T> & d;
  clock_order_frontend( clock_order<float> & data ) : d(data) { d.init(); }
  inline void operator()( vector2<T> const& p ) { d(p); }
  inline order get_ordering() const { return d.get_ordering(); }
};

}}

