#include <gott/events/main_loop_factory.hpp>
#include <gott/events/loop_requirement.hpp>
#include <gott/events/main_loop.hpp>
#include <gott/events/timer_manager.hpp>
#include <gott/events/quit_manager.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>

using namespace gott::events;
using namespace boost::lambda;
using namespace boost::posix_time;

class bar {
public:
  virtual void draw() = 0;
  virtual ~bar() {}
};

class bar_drawer {
public:
  void add(bar *b) {
    bars.push_back(b);
    invalidate();
  }

  void invalidate() {
    valid = false;
  }

  void redraw() {
    if (!valid) {
      std::cout << '\r';
      std::for_each(bars.begin(), bars.end(), bind(&bar::draw, _1));
      std::cout << std::flush;
      valid = true;
    }
  }

private:
  std::vector<bar *> bars;
  bool valid;
};

class pong_bar : public bar {
public:
  pong_bar(int width, bar_drawer &bd) 
  : width(width), pos(0), dx(1), token('*'), drawer(bd) {
    drawer.add(this);
  }

  void pong_step() {
    advance();
    drawer.invalidate();
  }

  void hop() {
    pos = (std::rand() % (width - 2));
    drawer.invalidate();
  }

  void toggle_token() {
    if (token == '*')
      token = '.';
    else
      token = '*';
    drawer.invalidate();
  }

  void draw() {
    std::cout << '|';
    for (int i = 0; i < width - 2; ++i)
      if (i == pos)
        std::cout << token;
      else
        std::cout << ' ';
    std::cout << '|';
  }

private:
  void advance() {
    if (pos + dx == -1 || pos + dx == width - 2)
      dx = - dx;
    pos += dx;
  }
  
private:
  int width;
  int pos;
  int dx;
  char token;
  bar_drawer &drawer;
};

class space_bar : public bar {
public:
  space_bar(int width, bar_drawer &d) : width(width) {
    d.add(this);
  }
  
  void draw() {
    for (int i = 0; i < width; ++i)
      std::cout << ' ';
  }

private:
  int width;
};

int main() {
  std::srand(std::time(0));
  bar_drawer d;
  pong_bar b1(30, d);
  space_bar s1(2, d);
  pong_bar b2(20, d);
  space_bar s2(2, d);
  pong_bar b3(26, d);
  d.redraw();

  
  main_loop_factory loop_gen;
  loop_gen.try_add(feature<timer_manager>() && feature<quit_manager>());
  boost::scoped_ptr<main_loop> loop(loop_gen.get_alloc());
  
  timer_manager &tm = loop->feature<timer_manager>();
  tm.add_periodic_timer(
      milliseconds(80),
      (bind(&pong_bar::pong_step, &b1), true));
  tm.add_periodic_timer(
      milliseconds(400),
      (bind(&pong_bar::toggle_token, &b1),
       bind(&pong_bar::toggle_token, &b2),
       bind(&pong_bar::toggle_token, &b3),
       true));
  tm.add_periodic_timer(
      milliseconds(820),
      (bind(&pong_bar::pong_step, &b2), true));
  tm.add_periodic_timer(
      milliseconds(300),
      (bind(&pong_bar::hop, &b3), true));

  loop->on_idle().connect(bind(&bar_drawer::redraw, &d));
  loop->feature<quit_manager>().enable_master();

  loop->run();
  std::cout << "\nDone!" << std::endl;
}
