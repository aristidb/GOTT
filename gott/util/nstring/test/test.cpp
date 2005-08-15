#include <gott/util/nstring/nstring.hpp>
#include <gott/util/nstring/buffer.hpp>
#include <gott/util/nstring/iterator.hpp>
#include <boost/algorithm/string.hpp>
#include <gott/util/range_algo.hpp>
#include <iostream>
#include <locale>
#include <iomanip>

using namespace std;
using namespace gott;
using namespace boost;

int main() {
  nstring n(range("Hallo, nstring-Welt!\n"), ascii);
  nstring x(n); // fast!
  cerr << (x == n) << L' ';
  cerr << (x != range(L"muhkuh")) << L' ';
  cerr << (x == range("Hallo, nstring-Welt!\n")) << L'\n';
  x = zero_terminated(L"xxxFoobar\n$");
  x = offset(range(x), 3, -1);
  n = x + n;
  cerr << n << x;

  static char const ae_euro_lit[] = "\xc3\xa4 hohodu \xe2\x82\xac\n";
  nstring ae_euro(zero_terminated((utf8_t const *) ae_euro_lit), 
                  nstring::utf8_literal);
  nstring_buffer b(ae_euro);
  cerr << (b.size() == ae_euro.length()) << '\n';
  //to_upper(b);
  b[1] = utf32_t(L':');
  b.erase(range(b.begin() + 2, 4));
  range_t<wchar_t const *> r = zero_terminated(L"test ");
  copy(r, b.insert(b.begin(), r.size()));
  nstring a(b);
  cerr << a;
  cerr << "\n";
}
