#include "metadata.hpp"
#include <iostream>
#include <algorithm>
#include <boost/lambda/lambda.hpp>

using namespace std;
using namespace boost::lambda;

void dump(gott::plugin::plugin_metadata const &x) {
  cout << x.plugin_id;
  if (!x.interfaces.empty()) {
    cout << " with interfaces: ";
    for_each(x.interfaces.begin(), x.interfaces.end(), cout << _1 << ' ');
  }
  cout << endl;
}

int main() {
  gott::plugin::extract_plugin_metadata(cin);
  gott::plugin::enumerate_plugin_metadata(&dump);
}
