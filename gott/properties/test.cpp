// vim:ts=2:sw=2:expandtab:autoindent:filetype=cpp:
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is A Properties Library.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2005-2006
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *  Aristid Breitkreuz (aribrei@arcor.de)
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include "translation_property.hpp"
#include "verify.hpp"
#include "property.hpp"
#include "concrete_property.hpp"
#include "external_storage.hpp"
#include "liaison.hpp"
#include "triggered_copy.hpp"
#include "signal_notification.hpp"
#include <gott/string/string.hpp>
#include <gott/string/buffer.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/function.hpp>
#include <iostream>
#include <boost/lambda/bind.hpp>
#include <boost/signals/connection.hpp>

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
  p.set(3);
  p.apply_change_ref(++_1 - 7);
  translation_property<int, double> xx(p);
  *xx.change() += 1.7;
  p.apply_change(add<2>);
  *p.change() -= 2;

  cout << p() << endl;

  { // DIRRTY - undefined behaviour
    change_reference<int> r(p.change());
    p.set(10);
    *r -= 10;
  }
  
  concrete_property<concrete_property<int> > x(p);
  cout << x.read()->get() << endl;
  
  concrete_property<string_buffer, signal_notification> w;
  boost::signals::connection c = w.on_change().connect(bind(&test_observe, &w));
  c.block();
  w.set(string("Hallo"));
  c.unblock();
  utf32_t const *add = (utf32_t const*)L" Welt!";
  std::copy(add, add + 6, w.change()->append(6).begin());
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
    change_reference<int> r = streamed.change();
    *r *= 2;
    if (*r == 10)
      break;
  }
  streamed.set(7);

  // Liaisons and verifications
  concrete_property<string, signal_notification> s1, s2;
  {
    liaison<string, my_conversion> l(s1, s2);
    s1.set("Hello, ");
    cout << s2.get();
    s2.set("2/world!\n");
  }
  s2.set("muhkuh");
  cout << s1.get() << s2.get() << endl;

  concrete_property<int, signal_notification> sv;
  {
  verify<int, check_range<int>, enforce_value<int> > 
    ver(sv, check_range<int>(0, 10));
  sv.set(14);
  }
  cout << sv.get() << ' ';

  verify<int, check_range<int>, enforce_value<int>, 
      boost::signal<void (property<int> const &)> > 
    ver(sv, check_range<int>(-5, 5));
  concrete_property<int> second;
  // Now this makes second be a copy of sv but always valid!
  ver.on_correct().connect(bind(triggered_copy<int>(sv, second)));
  sv.set(-22);
  cout << second.get() << endl;
}
