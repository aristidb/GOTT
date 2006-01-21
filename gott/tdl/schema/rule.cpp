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
using sh::rule_attr_t;
using sh::item;
using std::vector;

class rule_t::impl {
public:
  struct immediate {
    abstract_rule abstract;
    rule_attr_t attr;
    vector<rule_t> children;

    immediate(abstract_rule const &ar, rule_attr_t const &a, 
        vector<rule_t> const &c)
    : abstract(ar), attr(a), children(c) {}

    immediate(immediate const &o)
    : abstract(o.abstract), attr(o.attr), children(o.children) {}
  };

  boost::variant<immediate, rule_t const *> data;

  impl(abstract_rule const &ar, rule_attr_t const &a, vector<rule_t> const &c)
  : data(immediate(ar, a, c)) {}

  impl(rule_t const *p)
  : data(p) {}
};

rule_t::rule_t() {}

rule_t::rule_t(abstract_rule const &ar, rule_attr_t const &a,
    vector<rule_t> const &c)
: p(new impl(ar, a, c)) {}

rule_t::rule_t(rule_t const &o) : p(o.p) {}

rule_t::rule_t(rule_t const *ptr) : p(new impl(ptr)) {}

rule_t::~rule_t() {}

void rule_t::operator=(rule_t const &o) {
  p = o.p;
}

item *rule_t::get(match &m) const {
  switch (p->data.which()) {
  case 0: // immediate
    { 
      impl::immediate const &im = boost::get<impl::immediate>(p->data);
      return im.abstract.constructor(im.attr, im.children ,m); 
    }
  case 1: // indirect
    return boost::get<rule_t const *>(p->data)->get(m);
  }
  throw 0;
}

rule_attr_t const &rule_t::attributes() const {
  switch (p->data.which()) {
  case 0: // immediate
    return boost::get<impl::immediate>(p->data).attr;
  case 1: // indirect
    return boost::get<rule_t const *>(p->data)->attributes();
  }
  throw 0;
}

bool rule_t::accept_empty() const {
  switch (p->data.which()) {
  case 0: // immediate
    { 
      impl::immediate const &imm = boost::get<impl::immediate>(p->data);
      return imm.abstract.accept_empty(imm.attr, imm.children);
    }
  case 1: // indirect
    return boost::get<rule_t const *>(p->data)->accept_empty();
  }
  throw 0;
}

rule_t sh::rule(string const &id, rule_attr_t const &a, vector<rule_t> const &c)
{
  return by_name().get(id, a, c);
}
