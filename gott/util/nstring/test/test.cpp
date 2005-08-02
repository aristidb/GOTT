#include <gott/util/nstring/nstring.hpp>
#include <gott/util/nstring/buffer.hpp>
#include <boost/algorithm/string.hpp>
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
  x = L"Foobar\n";
  n = x + n;
  wcerr << n << x;

  nstring ae_euro((utf8_t const *)"\xc3\xa4 hohodu \xe2\x82\xac\n", 
                  nstring::utf8_literal);
  nstring_buffer b(ae_euro);
  //to_upper(b);
  b[1] = utf32_t(L':');
  b.erase(b.begin() + 2, b.begin() + 6);
  b.insert(b.begin(), 5);
  wchar_t const *X = L"test ";
  copy(X, X + 5, b.begin());
  nstring a(b);
  wcerr << a;
  wcerr << L"\n";
}
