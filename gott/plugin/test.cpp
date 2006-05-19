#include "plugin.hpp"
#include "module_metadata.hpp"
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
  extract_plugin(f);
  extract_module_metadata(f);
  enumerate_plugin(
      cout << _1 << "\n\n",
      tags::validate = false, tags::load_standard_metadata = false);

  cout << endl << "(bar)\n";
  enumerate_plugin(
      cout << _1 << "\n\n", 
      tags::interface = "bar",
      tags::validate = false, tags::load_standard_metadata = false);

  cout << endl << "(boo)\n";
  enumerate_plugin(
      cout << _1 << "\n\n",
      tags::interface = "boo",
      tags::validate = false, tags::load_standard_metadata = false);

  if (find_plugin(
        tags::plugin_id = "foo",
        tags::interface = "bar",
        tags::validate = false, tags::load_standard_metadata = false))
    cout << "Plugin foo supports interface bar!\n";

  if (!find_plugin(
        tags::plugin_id = "fox",
        tags::interface = "bar",
        tags::validate = false, tags::load_standard_metadata = false))
    cout << "Plugin fox does NOT support interface bar!\n";

  boost::optional<plugin const &> x =
    find_plugin(
        tags::interface = "boo",
        tags::validate = false, tags::load_standard_metadata = false);
  if (!x)
    cout << "There is no plugin to support interface boo?!\n";
  else
    cout << "First plugin to support interface boo: "
      << x.get().plugin_id.get_string() << ".\n";

  enumerate_module_metadata(cout << _1 << "\n\n",
      tags::validate = false, tags::load_standard_metadata = false);
}
