#ifndef GOTT_EXAMPLES_GREETING_INTERFACE_HPP
#define GOTT_EXAMPLES_GREETING_INTERFACE_HPP

#include <gott/plugin/plugin.hpp>
#include <iosfwd>

GOTT_PLUGIN_INTERFACE_DECLARE_BEGIN(greeting_interface)
  virtual void greet(std::ostream *) = 0;
GOTT_PLUGIN_INTERFACE_DECLARE_END()

#endif
