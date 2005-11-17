#include "verify.hpp"
#include "property.hpp"
#include "concrete_property.hpp"
#include "sigc_notification.hpp"
#include "external_storage.hpp"
#include "liaison.hpp"
#include "triggered_copy.hpp"
#include "translation_property.hpp"
#include <gott/string/string.hpp>
#include <gott/string/buffer.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/function.hpp>
#include <iostream>
#include <sigc++/bind.h>
#include <sigc++/hide.h>
#include <sigc++/connection.h>

using namespace gott;
using namespace gott::properties;
using namespace boost::lambda;
using boost::function;
using std::cout;
using std::cin;
using std::endl;

template<int i>
int add(int x) { return x + i; }

void test_observe(property<string_buffer> *p) {
  cout << "Changed: " << long(p);
  cout << " -> " << p->get() << endl;
}

class vobserve {
  vobserve(vobserve const &);
public:
  vobserve() {}
  template<class T>
  void notify(T *p) {
    cout << "Changed(ref): " << long(p);
    cout << " -> " << p->get() << endl;
  }
  enum { has_signal = false };
};

int read_fun() {
  int val;
  if (cin >> val)
    return val;
  throw 0;
}

struct my_conversion {
  string left_to_right(string const &left) {
    return "2/" + left;
  }

  string right_to_left(string const &right) {
    return offset(right.as_utf32(), 2);
  }
};

int main() {
  concrete_property<int> p;
  p.set(4);
  translation_property<int, double> xx(p);
  *xx.read_write() += 1.7;
  p.apply_change(add<2>);
  *p.read_write() -= 2;

  { // DIRRTY - undefined behaviour
    property<int>::read_write_reference r = p.read_write();
    p.set(10);
    *r -= 10;
  }
  
  concrete_property<concrete_property<int> > x(p);
  cout << x.read()->get() << endl;
  
  concrete_property<string_buffer, sigc_notification> w;
  sigc::connection c = w.on_change().connect(sigc::bind(&test_observe, &w));
  c.block();
  w.set(string("Hallo"));
  c.unblock();
  utf32_t const *add = (utf32_t const*)L" Welt!";
  std::copy(add, add + 6, w.read_write()->append(6).begin);
  cout << w.get() << endl;

  vobserve V;
  concrete_property<int, vobserve &> q(44, V);
  property<int> &ref = q;
  srand(time(NULL));
  ref.apply_write(rand);

  // Lambda is easy...
  typedef
  concrete_property<
    int, 
    no_notification, 
    external_storage<int>
  > 
  stream_property;

  function<int ()> read = read_fun;
  function<void (int)> write = cout << _1 << '\n';
  
  stream_property streamed(external_storage<int>(read, write));
  for (;;) {
    stream_property::read_write_reference r = streamed.read_write();
    *r *= 2;
    if (*r == 10)
      break;
  }
  streamed.set(7);

  // Liaisons and verifications
  concrete_property<string, sigc_notification> s1, s2;
  {
    liaison<string, my_conversion> l(s1, s2);
    s1.set("Hello, ");
    cout << s2.get();
    s2.set("2/world!\n");
  }
  s2.set("muhkuh");
  cout << s1.get() << s2.get() << endl;

  concrete_property<int, sigc_notification> sv;
  {
  verify<int, check_range<int>, enforce_value<int> > 
    ver(sv, check_range<int>(0, 10));
  sv.set(14);
  }
  cout << sv.get() << ' ';

  verify<int, check_range<int>, enforce_value<int>, 
      sigc::signal1<void, property<int> const &> > 
    ver(sv, check_range<int>(-5, 5));
  concrete_property<int> second;
  // Now this makes second be a copy of sv but always valid!
  ver.on_correct().connect(sigc::hide(triggered_copy<int>(sv, second)));
  sv.set(-22);
  cout << second.get() << endl;
}
