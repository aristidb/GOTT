#include "property.hpp"
#include <iostream>
#include <gott/util/autoconv.hpp>

using namespace std;
using namespace gott::properties;

template<int i>
int add(int x) { return x + i; }

struct test_observe {
  template<class T>
  void notify(T *p) {
    cout << "Changed: " << print_ptr(p);
    cout << " -> " << p->get() << endl;
  }
};

class vobserve {
  vobserve(vobserve const &);
public:
  vobserve() {}
  template<class T>
  void notify(T *p) {
    cout << "Changed(ref): " << print_ptr(p);
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
  
  concrete_property<string, test_observe> w("Hallo");
  w.read_write()->append(" Welt!");
  cout << w.get() << endl;

  vobserve V;
  concrete_property<int, vobserve &> q(44, V);
  property<int> &ref = q;
  srand(time(NULL));
  ref.apply_write(rand);
}
