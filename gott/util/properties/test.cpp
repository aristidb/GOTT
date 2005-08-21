#include "property.hpp"
#include <gott/util/string/string.hpp>
#include <gott/util/string/buffer.hpp>
#include <iostream>

using namespace gott;
using namespace gott::properties;
using std::cout;
using std::endl;

template<int i>
int add(int x) { return x + i; }

struct test_observe {
  template<class T>
  void notify(T *p) {
    cout << "Changed: " << long(p);
    cout << " -> " << p->get() << endl;
  }
};

class vobserve {
  vobserve(vobserve const &);
public:
  vobserve() {}
  template<class T>
  void notify(T *p) {
    cout << "Changed(ref): " << long(p);
    cout << " -> " << p->get() << endl;
  }
};

int main() {
  concrete_property<int> p;
  p.set(4);
  p.apply_change(add<2>);
  *p.read_write() -= 2;
  
  concrete_property<concrete_property<int> > x(p);
  cout << x.read()->get() << endl;
  
  concrete_property<string_buffer, test_observe> w(string("Hallo"));
  utf32_t const *add = (utf32_t const*)" Welt!";
  std::copy(add, add + 6, w.read_write()->append(6).begin);
  cout << w.get() << endl;

  vobserve V;
  concrete_property<int, vobserve &> q(44, V);
  property<int> &ref = q;
  srand(time(NULL));
  ref.apply_write(rand);
}
