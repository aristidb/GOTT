#include "metadata.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <boost/lambda/lambda.hpp>

using namespace std;
using namespace boost::lambda;
using namespace gott::plugin;

int main() {
  ifstream f("testregistry.tdl");
  if (!f) f.open("plugin/testregistry.tdl");
  extract_plugin_metadata(f);
  enumerate_plugin_metadata(cout << _1 << "\n\n");
  cout << endl;
  enumerate_plugin_metadata_with_interface("bar", cout << _1 << "\n\n");
  cout << endl;
  enumerate_plugin_metadata_with_interface("boo", cout << _1 << "\n\n");
}
