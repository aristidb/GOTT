#include <list>
#include <iostream>
#include "point_in_contour.hpp"

int main()
{
  using namespace gott::math;
  typedef vector2<float> vec;
  std::list<vec> polygon;
  polygon.push_back( vec(1.0f,0.5f));
  polygon.push_back( vec(-0.5f,-2));
  polygon.push_back( vec(2,-2));
  polygon.push_back( vec(3,2));
  polygon.push_back( vec(2,1));
  polygon.push_back( vec(2,4));
  polygon.push_back( vec(1,1));
  polygon.push_back( vec(1,4));
  polygon.push_back( vec(-1,4));
  polygon.push_back( vec(-1.5,0.5));

  std::cout << "0,0 is " << (point_in_contour(polygon.begin(), polygon.end(), vec(0,0) )?"in":"out") << std::endl;
  std::cout << "0,1 is " << (point_in_contour(polygon.begin(), polygon.end(), vec(0,1) )?"in":"out") << std::endl;
  std::cout << "-2,-1 is " << (point_in_contour(polygon.begin(), polygon.end(), vec(-2,-1) )?"in":"out") << std::endl;
}

