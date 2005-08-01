#include <gott/util/nstring/nstring.hpp>
#include <iostream>

int main() {
  gott::nstring n("Hallo, nstring-Welt", gott::ascii);
  gott::nstring x(n); // fast!
  for (gott::utf8_iterator it = n.begin(); it != n.end(); ++it)
    std::wcout << wchar_t(*it);
  std::wcout << std::endl;
}
