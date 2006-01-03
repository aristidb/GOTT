// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
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

#include "plugin_configuration.hpp"
#include <gott/string/qid.hpp>
#include <boost/variant.hpp>
#include <gott/xany/xany.hpp>

using gott::plugin::plugin_configuration;
using gott::plugin::hook;
using gott::QID;
using gott::xany::Xany;
using boost::variant;

class plugin_configuration::IMPL {
public:
  typedef variant<Xany, hook const *> entry;
  typedef VectorMap<QID, entry> mapping;

  mapping parameters;
};

NTL_MOVEABLE(plugin_configuration::IMPL::entry);

plugin_configuration::plugin_configuration() : p(new IMPL) {}

plugin_configuration::~plugin_configuration() {}

void plugin_configuration::add_hook(QID const &id, hook const &h) {
  p->parameters.Add(id, &h);
}

hook const &plugin_configuration::find_hook(QID const &id) const {
  return *boost::get<hook const *>(p->parameters.Get(id));
}

void plugin_configuration::add_param(QID const &id, Xany const &pp) {
  p->parameters.Add(id, pp);
}

Xany const &plugin_configuration::find_param(QID const &id) const {
  return boost::get<Xany>(p->parameters.Get(id));
}
