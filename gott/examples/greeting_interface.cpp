#include "greeting_interface.hpp"

GOTT_PLUGIN_INTERFACE_IMPL_BEGIN2(examples, greeting_interface)
  GOTT_PLUGIN_METHOD_ENTRY2(examples::greeting_interface::greet)
GOTT_PLUGIN_INTERFACE_IMPL_END()
