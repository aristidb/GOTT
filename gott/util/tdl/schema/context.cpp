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

#include "context.hpp"
#include "slot.hpp"
#include "types/list.hpp"

using boost::optional;
using gott::util::tdl::schema::rule;
using gott::util::tdl::schema::context;

context::context() : first(0) {}

context::~context() {
  for (container::iterator it = pool.begin(); it != pool.end(); ++it)
    delete *it;
}

void context::begin(unsigned i, rule::attributes const &a,
                    optional<slotcfg> const &c, unsigned l) {
  rule::factory *f = get_factory(i, a, l);

  pool.push_back(f);

  add_owned(f, c);
}

void context::end() {
  org.pop_back();
}

void context::ref(context const &other) {
  add_child(&other.get(), boost::none);
}

rule::factory const &context::get() const {
return *first;
}

void context::add_owned(rule::factory *f, optional<slotcfg> const &c) {
  add_child(f, c);
  org.push_back(f);
}

void context::add_child(rule::factory const *f, optional<slotcfg> const &c) {
  if (!org.empty())
    if (!c)
      org.back()->add(*f);
    else
      add_slotted(f, c.get());
  else
    first = f;
}

void context::add_slotted(rule::factory const *f, slotcfg const &c) {
  rule::factory::with_slotcfg *w = org.back()->get_with_slotcfg();

  if (!w)
    return add_enc_slotted(f, c);

  if (w->supports(c))
    w->add(*f, c);
  else if (c.prefix_optional() && (w->accepted_modes() & slotcfg::optional))
    add_enc_slotted(f, c.no_optional(), slotcfg(slotcfg::optional));
  else
    add_enc_slotted(f, c);
}

void context::add_enc_slotted(rule::factory const *f, slotcfg const &c,
                              optional<slotcfg> const &e) {
  if (c.get_mode() == slotcfg::one)
    return org.back()->add(*f);

  begin(match_list::factory::index(), rule::attributes(false), e, 1);
    add_child(f, c);
  end();
}
