#include <gott/util/nstring/nstring.hpp>
#include <iostream>

using namespace std;
using namespace gott;

int main() {
  nstring n("Hallo, nstring-Welt!\n", ascii);
  nstring x(n); // fast!
  x = L"Foobar\n";
  wcerr << n << x;
  // Print: ae, euro
  nstring ae_euro("\xc3\xa4 \xe2\x82\xac\n", utf8);
  wcerr << hex;
  for (utf8_t const *it = ae_euro.data(); *it; ++it)
    wcerr << *it << L' ';
  wcerr << L'\n';
  for (utf8_iterator it = ae_euro.begin(); it != ae_euro.end(); ++it)
    wcerr << *it << L' ';
  wcerr << L'\n';
  wcerr << ae_euro;// << x_ae << ae_euro2;
}
