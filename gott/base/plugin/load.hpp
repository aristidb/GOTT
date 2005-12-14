// Copyright (C) 2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: GOTT Plugin/Loadable Module Engine
// Authors: Aristid Breitkreuz
//
// This file is part of the Gott Project (http://gott.sf.net)
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#ifndef GOTT_BASE_PLUGIN_LOAD_HPP
#define GOTT_BASE_PLUGIN_LOAD_HPP

#include "plugin.hpp"

namespace gott {
class string;

namespace plugin {

GOTT_EXPORT
plugin_base *load_plugin(
    QID const &name, 
    string const &version,
    system_configuration &sysc,
    plugin_configuration const &plgc);

GOTT_EXPORT
void unload_plugin(plugin_base *plugin);

template<class ConcretePlugin>
class plugin_handle {
public:
  plugin_handle(
      QID const &name, 
      string const &version,
      system_configuration &sysc, 
      plugin_configuration const &plgc)
  : p(
      dynamic_cast<ConcretePlugin *>(
        load_plugin(name, version, sysc, plgc)))
  {}
  ~plugin_handle() { unload_plugin(p); }

  ConcretePlugin &operator*() { return *p; }
  ConcretePlugin const &operator*() const { return *p; }
  ConcretePlugin *operator->() { return p; }
  ConcretePlugin const *operator->() const { return p; }
  ConcretePlugin *get() { return p; }
  ConcretePlugin const *get() const { return p; }
private:
  ConcretePlugin *p;
};

}}

#endif
