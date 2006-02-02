#include <iostream>
#include <gott/graphics/transformations.hpp>

namespace g = gott::graphics;

int main() {
  g::transformations t;
  double x = 7, y = -99;
  t.apply(x, y);
  std::cout << x << ' ' << y << std::endl;
  g::translate(5, 5).apply(x, y);
  std::cout << x << ' ' << y << std::endl;
  g::scale(2, -0.2).apply(x, y);
  std::cout << x << ' ' << y << std::endl;
  g::rotate(3.14159265).apply(x, y); // invert sign
  std::cout << x << ' ' << y << std::endl;
  g::translate(25, 19.8).scale(8, 16).apply(x, y);
  std::cout << x << ' ' << y << std::endl;

}
