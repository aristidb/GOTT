#include <gott/util/nstring/nstring.hpp>
#include <gott/util/nstring/buffer.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <locale>

using namespace std;
using namespace gott;
using namespace boost;

int main() {
  nstring n("Hallo, nstring-Welt!\n", ascii);
  nstring x(n); // fast!
  x = L"Foobar\n";
  n = x + n;
  wcerr << n << x;
  // Print: ae, euro
  nstring ae_euro((utf8_t const *)"\xc3\xa4 \xe2\x82\xac\n", 
                  nstring::utf8_literal);
  wcerr << hex;
  for (utf8_t const *it = ae_euro.data(); *it; ++it)
    wcerr << *it << L' ';
  wcerr << L'\n';
  for (utf8_iterator it = ae_euro.begin(); it != ae_euro.end(); ++it)
    wcerr << *it << L' ';
  wcerr << L'\n';
  nstring_buffer b(ae_euro);
  //to_upper(b);
  b.begin()[1] = utf32_t(L':');
  nstring a(b);
  wcerr << a;
  wcerr << L"done\n";
}
