#include <list>
#include <iostream>
#include <algorithm>
#include "clockwise.hpp"

int main()
{
  using namespace gott::math;


  clock_order<float> data;
  typedef vector2<float> vec;
  {
    std::list<vec> polygon;

    polygon.push_back(vec(0,6));
    polygon.push_back(vec(4,0));
    polygon.push_back(vec(0,-6));
    polygon.push_back(vec(-4,-2));
    polygon.push_back(vec(-1,-4));
    polygon.push_back(vec(0,-4));
    polygon.push_back(vec(2,-1));
    polygon.push_back(vec(2,1));
    polygon.push_back(vec(0,4));
    polygon.push_back(vec(1,4));
    polygon.push_back(vec(-4,2));

    {
      std::for_each(polygon.begin(), polygon.end(),  clock_order_frontend<float>( data ) );
      std::cout << ((data.get_ordering()==clock_order<float>::CounterClockwise)?"CounterClockwise":"Clockwise") << std::endl;
    }

    {
      std::for_each(polygon.rbegin(), polygon.rend(),  clock_order_frontend<float>( data ) );
      std::cout << ((data.get_ordering()==clock_order<float>::CounterClockwise)?"CounterClockwise":"Clockwise") << std::endl;
    }
  }

  {
    std::list<vec> polygon;

    polygon.push_back(vec(0,6));
    polygon.push_back(vec(4,0));
    polygon.push_back(vec(0,-6));
    polygon.push_back(vec(-4,-2));
    polygon.push_back(vec(-4,2));

    {
      std::for_each(polygon.begin(), polygon.end(),  clock_order_frontend<float>( data ) );
      std::cout << ((data.get_ordering()==clock_order<float>::CounterClockwise)?"CounterClockwise":"Clockwise") << std::endl;
    }

    {
      std::for_each(polygon.rbegin(), polygon.rend(),  clock_order_frontend<float>( data ) );
      std::cout << ((data.get_ordering()==clock_order<float>::CounterClockwise)?"CounterClockwise":"Clockwise") << std::endl;
    }
  }
}

