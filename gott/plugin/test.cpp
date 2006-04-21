#include "metadata.hpp"
#include <iostream>
#include <algorithm>
#include <boost/lambda/lambda.hpp>

using namespace std;
using namespace boost::lambda;

int main() {
  gott::plugin::plugin_metadata x;
  cin >> x;

  cout << x.plugin_id << std::endl;
  cout << "Interfaces:\n";
  for_each(x.interfaces.begin(), x.interfaces.end(), cout << _1 << ' ');
  cout << endl;
}
