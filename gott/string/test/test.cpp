#include <gott/string/string.hpp>
#include <gott/string/buffer.hpp>
#include <gott/string/iterator.hpp>
#include <gott/string/atom.hpp>
#include <boost/algorithm/string.hpp>
#include <gott/range_algo.hpp>
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
  for (string::utf8_range r = n.as_utf8(); !r.empty(); ++r.begin())
    cerr << *r.begin();
#endif
}

int main() {
  cerr << "size:" << sizeof(string) << '\n';
  string n(zero_terminated("Hallo, string-Welt!\n"), ascii);
  string x(n); // fast!
  cerr << (x == n) << ' ';
  cerr << (x != string(L"muhkuh", wide)) << ' ';
  cerr << (x == string(L"Hallo, string-Welt!\n", wide)) << '\n';
  x = string(zero_terminated(L"xxxFoobar\n$"), wide);
  x = offset(x.as_utf32(), 3, -1);
  n = x + n;
  print(n);
  print(x);

  static char const ae_euro_lit[] = "\xc3\xa4 hohodu \xe2\x82\xac\n";
  string ae_euro(ae_euro_lit);
  string_buffer b(ae_euro);
  cerr << (b.size() == ae_euro.length()) << '\n';
  b[1] = utf32_t(L':');
  b.erase(range(b.begin() + 2, 4));
  range_t<wchar_t const *> r = zero_terminated(L"test ");
#ifdef NO_STDLIB
  range<wchar_t const *> o = b.insert(b.begin(), r.size());
  for (wchar_t const *it = r.begin(); it != r.end(); ++it)
    *o.Begin++ = *it;
#else
  copy(r, b.insert(b.begin(), r.size()));
#endif
  print(string(b));

  atom A(offset(n.as_utf32(), x.length()));
  atom B(string(zero_terminated("Hallo, string-Welt!\n").cast<utf8_t const *>(), string::literal));
  cerr << (A == B) << '\n';

  while (getline(std::cin, x))
    std::cout << '<' << x << '>' << std::endl;
}
