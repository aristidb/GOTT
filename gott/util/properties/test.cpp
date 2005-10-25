#include "property.hpp"
#include <gott/util/string/string.hpp>
#include <gott/util/string/buffer.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/function.hpp>
#include <iostream>

using namespace gott;
using namespace gott::properties;
using namespace boost::lambda;
using boost::function;
using std::cout;
using std::cin;
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
  
  concrete_property<concrete_property<int> > x(p);
  cout << x.read()->get() << endl;
  
  concrete_property<string_buffer, test_observe> w(string("Hallo"));
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
  external_storage<
    int,
    function<int ()>,
    function<void (int)> 
  > 
  stream_storage;

  typedef
  concrete_property<
    int, 
    no_notification, 
    stream_storage
  > 
  stream_property;

  function<int ()> read = read_fun;
  function<void (int)> write = cout << _1 << '\n';
  
  stream_property streamed(stream_storage(read, write));
  *streamed.read_write() *= 2;
}
