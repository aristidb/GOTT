#include <iostream>
#include <cmath>
#include <gott/graphics/transformations.hpp>

namespace g = gott::graphics;

double const pi = 3.14159265;

bool check(double isx, double isy, double expx, double expy, int test) {
  double const epsilon = 1.0E-5;
  double dx = std::fabs(isx - expx);
  double dy = std::fabs(isy - expy);
  if (dx < epsilon && dy < epsilon)
    return true;
  std::cerr << "Test failed: " << test << '\n';
  std::cerr << "Error x: " << dx << '\n';
  std::cerr << "Error y: " << dy << '\n';
  return false;
}

int main() {
  bool happy = true;

  double x = 7, y = -99;

  g::transformations t;
  t.apply(x, y);
  std::cout << x << ' ' << y << std::endl;
  happy &= check(x, y, 7, -99, 1);

  g::translate(5, 5).apply(x, y);
  std::cout << x << ' ' << y << std::endl;
  happy &= check(x, y, 12, -94, 2);

  g::scale(2, -0.2).apply(x, y);
  std::cout << x << ' ' << y << std::endl;
  happy &= check(x, y, 24, 18.8, 3);

  g::transformations t2 = g::rotate(pi);
  t2.apply(x, y);
  std::cout << x << ' ' << y << std::endl;
  happy &= check(x, y, -24, -18.8, 4);

  g::translate(25, 19.8).scale(8, 16).apply(x, y);
  std::cout << x << ' ' << y << std::endl;
  happy &= check(x, y, 8, 16, 5);

  g::scale(1.0 / 8, 1.0 / 16).rotate(pi / 2).apply(x, y);
  std::cout << x << ' ' << y << std::endl;
  happy &= check(x, y, -1.0, 1.0, 6);

  return happy ? 0 : 1;
}
