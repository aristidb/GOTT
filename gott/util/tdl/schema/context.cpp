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

#include "context.hpp"
#include "slot.hpp"
#include "by_name.hpp"
#include "rule_factory.hpp"
#include <gott/util/range_algo.hpp>
#include <boost/optional.hpp>
#include <gott/util/nstring/nstring.hpp>

using boost::optional;
using gott::tdl::schema::context;
using gott::tdl::schema::rule;
using gott::tdl::schema::rule_factory;

class context::IMPL {
public:
  typedef std::vector<rule_factory *> container;
  container pool;
  rule_factory const *first;
  std::list<rule_factory *> org;

  void begin(unsigned, rule_attr const &, optional<slotcfg> const &);
  void end();
  void ref(context const &);

  void add_child(rule_factory const *, boost::optional<slotcfg> const &);
  void add_slotted(rule_factory const *, slotcfg const &);
  void add_enc_slotted(rule_factory const *, slotcfg const &,
                       boost::optional<slotcfg> const & = boost::none);
  void add_owned(rule_factory *, boost::optional<slotcfg> const &);

  IMPL() : first(0) {}
  ~IMPL() { 
    for_each(range(pool), boost::checked_delete<rule_factory>); 
  }
};

context::context() : p(new IMPL) {}
context::~context() {}

void context::begin(nstring const &n, rule_attr const &a,
                    optional<slotcfg> const &c) {
  begin(name_manager().get(n), a, c);
}

void context::begin(unsigned i, rule_attr const &a, 
                    optional<slotcfg> const &c) {
  p->begin(i, a, c);
}

void context::end() {
  p->end();
}

void context::ref(context const &other) {
  p->ref(other);
}

rule_factory const &context::get() const {
  return *p->first;
}

void context::IMPL::begin(unsigned i, rule_attr const &a,
                          optional<slotcfg> const &c) {
  rule_factory *f = get_factory(i, a);
  pool.push_back(f);
  add_owned(f, c);
}

void context::IMPL::end() {
  org.pop_back();
}

void context::IMPL::ref(context const &other) {
  add_child(&other.get(), boost::none);
}

void context::IMPL::add_owned(rule_factory *f, optional<slotcfg> const &c) {
  add_child(f, c);
  org.push_back(f);
}

void context::IMPL::add_child(rule_factory const *f, 
                              optional<slotcfg> const &c) {
  if (!org.empty())
    if (!c)
      org.back()->add(*f);
    else
      add_slotted(f, c.get());
  else
    first = f;
}

void context::IMPL::add_slotted(rule_factory const *f, slotcfg const &c) {
  rule_factory::with_slotcfg *w = org.back()->get_with_slotcfg();

  if (!w)
    return add_enc_slotted(f, c);

  if (w->supports(c))
    w->add(*f, c);
  else if (c.prefix_optional() && (w->accepted_modes() & slotcfg::optional))
    add_enc_slotted(f, c.no_optional(), slotcfg(slotcfg::optional));
  else
    add_enc_slotted(f, c);
}

void context::IMPL::add_enc_slotted(rule_factory const *f, slotcfg const &c,
                                    optional<slotcfg> const &e) {
  if (c.get_mode() == slotcfg::one)
    return org.back()->add(*f);

  static unsigned const list_id = name_manager().get("list");
  begin(list_id, rule_attr(rule_attr::simple, false), e);
    add_child(f, c);
  end();
}
