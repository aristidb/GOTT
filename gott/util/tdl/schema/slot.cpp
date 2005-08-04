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

#include "slot.hpp"
#include "rule.hpp"

using std::size_t;
using std::pair;
using std::make_pair;
using boost::get;
namespace schema = gott::tdl::schema;
using schema::rule;
using schema::rule_factory;
using schema::slotcfg;

class slotcfg::IMPL {
public:
  mode m;

  typedef
    boost::variant<
      std::size_t,
      std::pair<std::size_t, std::size_t>,
      callback
    >
  type_t;

  type_t type;

  enum { cancelled = size_t(-1) };
  size_t count;

  IMPL(mode mm, type_t tt = size_t(1), size_t cc = 0)
  : m(mm), type(tt), count(cc) {}
};

slotcfg::slotcfg() 
: p(new IMPL(one)) {}

slotcfg::slotcfg(slotcfg const &b)
: p(new IMPL(b.p->m, b.p->type, b.p->count)) {}

slotcfg::slotcfg(simple_mode mm) 
: p(new IMPL(mm)) {}

slotcfg::slotcfg(sized_mode mm, size_t s)
: p(new IMPL(mm, s)) {}

slotcfg::slotcfg(range_mode mm, size_t a, size_t b)
: p(new IMPL(mm, make_pair(a, b))) {}

slotcfg::slotcfg(function_mode mm, callback const &cb)
: p(new IMPL(mm, cb)) {}

slotcfg::~slotcfg() {}

void slotcfg::operator=(slotcfg const &b) {
  p.reset(new IMPL(b.p->m, b.p->type, b.p->count));
}

void slotcfg::add() {
  ++p->count;
}

slotcfg::mode slotcfg::get_mode() const {
  return p->m;
}

rule::expect slotcfg::expectation() const {
  if (p->count == IMPL::cancelled)
    return rule::nothing;
  
  switch (p->m) {
  case one:
    if (p->count == 1)
      return rule::nothing;
    else if (p->count == 0)
      return rule::need;
    else
      return rule::over_filled;

  case optional:
    if (p->count == 0)
      return rule::maybe;
    else if (p->count == 1)
      return rule::nothing;
    else
      return rule::over_filled;

  case list:
    return rule::maybe;

  case some:
    if (p->count > 0)
      return rule::maybe;
    else
      return rule::need;

  case exact:
    if (p->count < get<size_t>(p->type))
      return rule::need;
    else if (p->count == get<size_t>(p->type))
      return rule::nothing;
    else
      return rule::over_filled;

  case minimum:
    if (p->count >= get<size_t>(p->type))
      return rule::maybe;
    else
      return rule::need;

  case maximum:
    if (p->count < get<size_t>(p->type))
      return rule::maybe;
    else if (p->count == get<size_t>(p->type))
      return rule::nothing;
    else
      return rule::over_filled;

  case range:
    if (p->count < get<pair<size_t, size_t> >(p->type).first)
      return rule::need;
    if (p->count >= get<pair<size_t, size_t> >(p->type).first
        && p->count < get<pair<size_t, size_t> >(p->type).second)
      return rule::maybe;
    else if (p->count == get<pair<size_t, size_t> >(p->type).second)
      return rule::nothing;
    else
      return rule::over_filled;

  case function:
    return get<callback>(p->type)(p->count);
  }

  throw std::bad_exception();
}

void slotcfg::cancel() {
  p->count = IMPL::cancelled;
}

bool slotcfg::prefix_optional() const {
  switch (p->m) {
  case optional:
  case list:
  case maximum:
    return true;

  case minimum:
    return get<size_t>(p->type) == 0;

  case range:
    return get<pair<size_t, size_t> >(p->type).first == 0;

  case function:
    return get<callback>(p->type)(0) > 0;
  }

  return false;
}

namespace {
  struct denull {
    denull(slotcfg::callback const &c) : internal(c) {}
    slotcfg::callback internal;

    rule::expect operator() (size_t i) const {
      if (i == 0)
        return rule::need;
      return internal(i);
    }
  };
}

slotcfg slotcfg::no_optional() const {
  switch (p->m) {
  case optional:
    return slotcfg(one);
  case list:
    return slotcfg(some);
  case maximum:
    return slotcfg(range, 1, get<size_t>(p->type));

  case minimum:
    return slotcfg(minimum, !get<size_t>(p->type) ? 1 : get<size_t>(p->type));

  case range:
    if (get<pair<size_t, size_t> >(p->type).first == 0)
      return slotcfg(range, 1, get<pair<size_t, size_t> >(p->type).second);
    return *this;

  case function:
    return slotcfg(function, callback(denull(get<callback>(p->type))));
  }

  throw std::bad_exception();
}

void rule_factory::with_slotcfg::add(rule_factory const &child, unsigned, 
                                      slotcfg const &cfg) {
  add(child, cfg);
}

bool rule_factory::with_slotcfg::supports(slotcfg const &c) const {
  return bool(accepted_modes() & c.get_mode());
}

rule_factory::with_slotcfg::~with_slotcfg() {}
