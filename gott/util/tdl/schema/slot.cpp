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

#include "slot.hpp"

using std::size_t;
using std::pair;
using std::make_pair;
using boost::get;
using gott::util::tdl::schema::rule;
using gott::util::tdl::schema::slotcfg;

slotcfg::slotcfg() : m(one), count(0) {}

slotcfg::slotcfg(slotcfg const &b) : m(b.m), type(b.type), count(b.count) {}

slotcfg::slotcfg(simple_mode mm) 
: m(mm), count(0) {}

slotcfg::slotcfg(sized_mode mm, size_t s)
: m(mm), type(s), count(0) {}

slotcfg::slotcfg(range_mode mm, size_t a, size_t b)
: m(mm), type(make_pair(a, b)), count(0) {}

slotcfg::slotcfg(function_mode mm, callback const &cb)
: m(mm), type(cb), count(0) {}

slotcfg::~slotcfg() {}

void slotcfg::add() {
  ++count;
}

rule::expect slotcfg::expectation() const {
  if (count == cancelled)
    return rule::nothing;
  
  switch (m) {
  case one:
    if (count == 1)
      return rule::nothing;
    else if (count == 0)
      return rule::need;
    else
      return rule::over_filled;

  case optional:
    if (count == 0)
      return rule::maybe;
    else if (count == 1)
      return rule::nothing;
    else
      return rule::over_filled;

  case list:
    return rule::maybe;

  case some:
    if (count > 0)
      return rule::maybe;
    else
      return rule::need;

  case exact:
    if (count < get<size_t>(type))
      return rule::need;
    else if (count == get<size_t>(type))
      return rule::nothing;
    else
      return rule::over_filled;

  case minimum:
    if (count >= get<size_t>(type))
      return rule::maybe;
    else
      return rule::need;

  case maximum:
    if (count < get<size_t>(type))
      return rule::maybe;
    else if (count == get<size_t>(type))
      return rule::nothing;
    else
      return rule::over_filled;

  case range:
    if (count < get<pair<size_t, size_t> >(type).first)
      return rule::need;
    if (count >= get<pair<size_t, size_t> >(type).first
        && count < get<pair<size_t, size_t> >(type).second)
      return rule::maybe;
    else if (count == get<pair<size_t, size_t> >(type).second)
      return rule::nothing;
    else
      return rule::over_filled;

  case function:
    return get<callback>(type)(count);
  }

  throw std::bad_exception();
}

void slotcfg::cancel() {
  count = cancelled;
}

bool slotcfg::prefix_optional() const {
  switch (m) {
  case optional:
  case list:
  case maximum:
    return true;

  case minimum:
    return get<size_t>(type) == 0;

  case range:
    return get<pair<size_t, size_t> >(type).first == 0;

  case function:
    return get<callback>(type)(0) > 0;
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
  switch (m) {
  case optional:
    return slotcfg(one);
  case list:
    return slotcfg(some);
  case maximum:
    return slotcfg(range, 1, get<size_t>(type));

  case minimum:
    return slotcfg(minimum, !get<size_t>(type) ? 1 : get<size_t>(type));

  case range:
    if (get<pair<size_t, size_t> >(type).first == 0)
      return slotcfg(range, 1, get<pair<size_t, size_t> >(type).second);
    return *this;

  case function:
    return slotcfg(function, callback(denull(get<callback>(type))));
  }
  throw std::bad_exception();
}


void rule::factory::with_slotcfg::add(rule::factory const &child,
                                      unsigned, slotcfg const &cfg) {
  add(child, cfg);
}

bool rule::factory::with_slotcfg::supports(slotcfg const &c) const {
  return bool(accepted_modes() & c.get_mode());
}

rule::factory::with_slotcfg::~with_slotcfg() {}
