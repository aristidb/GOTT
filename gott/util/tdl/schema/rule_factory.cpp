// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL Schema engine
// Authors: Aristid Breitkreuz
//
// This File is part of the Gott Project (http://gott.sf.net)
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

#include "rule_factory.hpp"
#include "slot.hpp"

using std::vector;
using gott::util::tdl::schema::rule;
namespace factory_template = gott::util::tdl::schema::factory_template;

void rule::factory::add(rule::factory const &f, unsigned) {
  add(f);
}

rule::factory::~factory() {}

rule::factory::with_slotcfg *rule::factory::get_with_slotcfg() {
  return dynamic_cast<with_slotcfg *>(this);
}

typedef vector<factory_template::rule_factory_builder> vbd_t;
static vbd_t &builders() {
  static vbd_t v;
  return v;
}

namespace gott {
namespace util {
namespace tdl {
namespace schema {

unsigned add_factory(factory_template::rule_factory_builder x) {
  builders().push_back(x);
  return builders().size() - 1;
}

rule::factory *get_factory(unsigned x, rule::attributes const &a, unsigned l) {
  return builders().at(x)(a, l);
}

}}}}
