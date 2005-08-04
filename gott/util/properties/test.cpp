#include "property.hpp"

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
  property<int> p;
  p.set(4);
  p.apply_change(add<2>);
  *p.read_write() -= 2;
  
  property<property<int> > x(p);
  cout << x.read()->get() << endl;
  
  property<string, test_observe> w("Hallo");
  w.read_write()->append(" Welt!");
  cout << w.get() << endl;

  vobserve V;
  property<int, vobserve &> q(44, V);
  srand(time(NULL));
  q.apply_write(rand);
}
