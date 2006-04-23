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
  enumerate_plugin_metadata(cout << _1 << "\n\n", tags::interface = "bar");
  cout << endl;
  enumerate_plugin_metadata(cout << _1 << "\n\n", tags::interface = "boo");
  if (find_plugin_metadata(tags::plugin_id = "foo", tags::interface = "bar"))
    cout << "Plugin foo supports interface bar!\n";
  if (!find_plugin_metadata(tags::plugin_id = "fox", tags::interface = "bar"))
    cout << "Plugin fox does NOT support interface bar!\n";
  cout << "First plugin to support interface boo: " <<
    find_plugin_metadata(tags::interface = "boo").get().plugin_id << ".\n";
}
