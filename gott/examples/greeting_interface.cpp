#include "greeting_interface.hpp"

GOTT_PLUGIN_INTERFACE_BEGIN(greeting_interface, "example::greeting_interface")
  GOTT_PLUGIN_METHOD_ENTRY("example::greeting_interface::greet", 
      &greeting_interface::greet)
GOTT_PLUGIN_INTERFACE_END()
