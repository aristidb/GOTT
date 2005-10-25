#include "property.hpp"
#include <gott/util/string/string.hpp>
#include <gott/util/string/buffer.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/function.hpp>
#include <iostream>
#include <sigc++/bind.h>

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
  sigc::signal0<void> *get_on_change(void*) { return 0; }
};

int read_fun() {
  int val;
  if (cin >> val)
    return val;
  throw 0;
}

int main() {
  concrete_property<int> p;
  p.set(4);
  translation_property<int, double> xx(p);
  *xx.read_write() += 1.7;
  p.apply_change(add<2>);
  *p.read_write() -= 2;

  { // DIRRTY
    property<int>::read_write_reference r = p.read_write();
    p.set(10);
    *r -= 10;
  }
  
  concrete_property<concrete_property<int> > x(p);
  cout << x.read()->get() << endl;
  
  concrete_property<string_buffer, sigc_notification> w(string("Hallo"));
  w.on_change().connect(sigc::bind(&test_observe, &w));
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
}
