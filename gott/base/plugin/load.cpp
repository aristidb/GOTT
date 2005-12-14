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

#include "load.hpp"

using gott::string;
using gott::plugin::plugin_base;
using gott::plugin::QID;
using gott::plugin::system_configuration;
using gott::plugin::plugin_configuration;

plugin_base *gott::plugin::load_plugin(
    QID const &name,
    string const &version,
    system_configuration &sysc,
    plugin_configuration const &plgc) {
  //TODO implement me
}

void gott::plugin::unload_plugin(plugin_base *p) {
  //TODO implement me
}
