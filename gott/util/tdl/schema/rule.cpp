// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL Schema engine
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

#include "rule.hpp"
#include "rule_attr.hpp"
#include "by_name.hpp"
#include "item.hpp"
#include "match.hpp"
#include <boost/variant.hpp>

namespace sh = gott::tdl::schema;
using sh::rule_t;
using sh::rule_attr;
using sh::item;

class rule_t::IMPL {
public:
  struct immediate {
    item_constructor con;
    rule_attr attr;
    Vector<rule_t> children;

    immediate(item_constructor t, rule_attr const &a, Vector<rule_t> pick_ &c)
    : con(t), attr(a), children(c) {}

    immediate(immediate const &o)
    : con(o.con), attr(o.attr), children(o.children, 1) {}
  };

  boost::variant<immediate, rule_t const *> data;

  IMPL(item_constructor t, rule_attr const &a, Vector<rule_t> pick_ &c)
  : data(immediate(t, a, c)) {}

  IMPL(rule_t const *p)
  : data(p) {}
};

rule_t::rule_t(item_constructor t, rule_attr const &a, Vector<rule_t> pick_ &c)
: p(new IMPL(t, a, c)) {}

rule_t::rule_t(rule_t const &o) : p(o.p) {}

rule_t::rule_t(rule_t const *ptr) : p(new IMPL(ptr)) {}

rule_t::~rule_t() {}

void rule_t::operator=(rule_t const &o) {
  p = o.p;
}

item *rule_t::get(match &m) const {
  switch (p->data.which()) {
  case 0: // immediate
    { IMPL::immediate const &imm = boost::get<IMPL::immediate>(p->data);
      return imm.con(imm.attr, imm.children, m); }
  case 1: // indirect
    return boost::get<rule_t *>(p->data)->get(m);
  }
  throw (void*)0; // never happen
}

rule_t sh::rule(string const &id, rule_attr const &a, Vector<rule_t> pick_ &c) {
  return by_name().get(id, a, c);
}

rule_attr const &rule_t::attributes() const {
  switch (p->data.which()) {
  case 0: // immediate
    return boost::get<IMPL::immediate>(p->data).attr;
  case 1: // indirect
    return boost::get<rule_t *>(p->data)->attributes();
  };
  throw (void*)0; // never happen
}
