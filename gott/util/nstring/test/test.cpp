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
  wcerr.imbue(locale("POSIX"));
  nstring n("Hallo, nstring-Welt!\n", ascii);
  nstring x(n); // fast!
  wcerr << (x == n) << L' ';
  wcerr << (x != L"muhkuh") << L' ';
  wcerr << (x == "Hallo, nstring-Welt!\n") << L'\n';
  x = L"xxxFoobar\n$";
  x = offset(range(x), 3, -1);
  n = x + n;
  wcerr << n << x;

  nstring ae_euro((utf8_t const *)"\xc3\xa4 hohodu \xe2\x82\xac\n", 
                  nstring::utf8_literal);
  nstring_buffer b(ae_euro);
  wcerr << (b.size() == ae_euro.length()) << L'\n';
  //to_upper(b);
  b[1] = utf32_t(L':');
  b.erase(b.begin() + 2, b.begin() + 6);
  range_t<wchar_t const *> r = offset(range(L"test "), 0, -1);
  copy(r, b.insert(b.begin(), r.size()));
  nstring a(b);
  wcerr << a;
  wcerr << L"\n";
}
