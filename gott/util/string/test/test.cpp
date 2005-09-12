#include <gott/util/string/string.hpp>
#include <gott/util/string/buffer.hpp>
#include <gott/util/string/iterator.hpp>
#include <boost/algorithm/string.hpp>
#include <gott/util/range_algo.hpp>
#include <iostream>
#include <locale>
#include <iomanip>

using std::cerr;

using namespace gott;
using namespace boost;

void print(string const &n) {
#ifndef NO_STDLIB
  cerr << n;
#else
  for (string::utf8_range r = n.as_utf8(); !r.empty(); ++r.begin)
    std::cerr << *r.begin;
#endif
}

int main() {
  string n(range("Hallo, string-Welt!\n"), ascii);
  string x(n); // fast!
  cerr << (x == n) << ' ';
  cerr << (x != range(L"muhkuh")) << ' ';
  cerr << (x == range("Hallo, string-Welt!\n")) << '\n';
  x = zero_terminated(L"xxxFoobar\n$");
  x = offset(x.as_utf32(), 3, -1);
  n = x + n;
  print(n);
  print(x);

  static char const ae_euro_lit[] = "\xc3\xa4 hohodu \xe2\x82\xac\n";
  string ae_euro(zero_terminated((utf8_t const *) ae_euro_lit), 
                  string::utf8_literal);
  string_buffer b(ae_euro);
  cerr << (b.size() == ae_euro.length()) << '\n';
  b[1] = utf32_t(L':');
  b.erase(range(b.begin() + 2, 4));
  range_t<wchar_t const *> r = zero_terminated(L"test ");
  copy(r, b.insert(b.begin(), r.size()));
  print(string(b));
}
