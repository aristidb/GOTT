// This program emits a number of animated bars on the console.
// The concept is built around the single "output line". Animation is done by
// redrawing as often as necessary.
// The "output line" consists of a number of bars which just are written one
// after each other. Note that there is no space between the bars but there
// is a special bar type for spacing. The bars are animated by timers managed by
// GOTT.
// This is an example and as such commented differently than say our APIs.

#include <gott/events/main_loop_factory.hpp>
#include <gott/events/loop_requirement.hpp>
#include <gott/events/main_loop.hpp>
#include <gott/events/timer_manager.hpp>
#include <gott/events/quit_manager.hpp>
#include <gott/plugin.hpp>
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

// OK here we have the abstract type for the "bar" concept. Not much spectacular
// stuff here.
class bar {
public:
  // Draw is invoked whenever a bar has to be drawn. Just print it to std::cout.
  virtual void draw() = 0;
  virtual ~bar() {}
};

// This class manages the single "output line". It keeps track of all bars and
// updates the output line when needed.
class bar_drawer {
public:
  bar_drawer() : valid(true) {}

  // Add a bar. This method is called by the constructor of the actual bar
  // implementation.
  void add(bar *b) {
    bars.push_back(b);
    invalidate();
  }

  // Invalidate the "output line". Called... when model and presentation are
  // expected not to match.
  void invalidate() {
    valid = false;
  }

  // Redraw the "output line" but only when invalidate() has been called before.
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

// The visible kind of bar. Two vertical walls, one token and much space. Oh and
// the token is either "*" (initially) or ".".
class pong_bar : public bar {
public:
  pong_bar(int width, bar_drawer &bd) 
  : width(width), pos(0), dx(1), token('*'), drawer(bd) {
    drawer.add(this);
  }

  // Let the token advance by one step and bounce off the walls.
  void pong_step() {
    if (pos + dx == -1 || pos + dx == width - 2)
      dx = -dx;
    pos += dx;
    drawer.invalidate();
  }

  // Let the token hop to a completely random position.
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
  int width;
  int pos;
  int dx;
  char token;
  bar_drawer &drawer;
};

// This is the invisible kind of bar. Just a number of spaces.
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

  pong_bar b1(30, d); //   visible, width 30
  space_bar s1(2, d); // invisible, width  2
  pong_bar b2(20, d); //   visible, width 20
  space_bar s2(2, d); // invisible, width  2
  pong_bar b3(26, d); //   visible, width 26
                      // -------------------
                      //              SUM 80 (standard terminal width)
  
  // Draw the initial state.
  d.redraw();
  
  // We need two features: timer_manager and quit_manager. Get a fitting
  // main_loop.
  main_loop_factory loop_gen;
  loop_gen.try_add(feature<timer_manager>() && feature<quit_manager>());

  // Main_loops are plugins, usually. And here: always.
  gott::plugin::plugin_handle<main_loop> loop(loop_gen.get());
  
  // With a timer manager, we can add timers.
  timer_manager &tm = loop->feature<timer_manager>();

  // (Visible) Bar 1: Let the token advance every 80 ms. The ", true" ensures
  // that the timer continues. This is "dirty" Boost.Lambda "magic".
  tm.add_periodic_timer(
      milliseconds(80),
      (bind(&pong_bar::pong_step, &b1), true));

  // All bars: Toggle the token every 0,4 s.
  tm.add_periodic_timer(
      milliseconds(400),
      (bind(&pong_bar::toggle_token, &b1),
       bind(&pong_bar::toggle_token, &b2),
       bind(&pong_bar::toggle_token, &b3),
       true));

  // (Visible) Bar 2: Let the token advance every 0,82 s.
  tm.add_periodic_timer(
      milliseconds(820),
      (bind(&pong_bar::pong_step, &b2), true));

  // (Visible) Bar 3: Let the token hop randomly every 0,3 s.
  tm.add_periodic_timer(
      milliseconds(300),
      (bind(&pong_bar::hop, &b3), true));

  // IMPORTANT: Redraw after all timers have been handled and before waiting
  // again. This ensures minimum system usage and maximum ... well, the bars
  // just have to actually float and on time.
  loop->on_idle().connect(bind(&bar_drawer::redraw, &d));

  // Also quite nice: With this code we let the loop check for conditions like
  // Strg-C or SIGTERM (Unix) and quit gracefully.
  loop->feature<quit_manager>().enable_master();

  // Do the actual work.
  // Yes, this method does the actual work and I'm adding senseless text to make
  // it more bold.
  loop->run();

  // Print a message to show that the application has actually quit gracefully.
  std::cout << "\nDone!" << std::endl;
}
