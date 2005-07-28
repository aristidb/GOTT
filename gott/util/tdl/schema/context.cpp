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
#include "by_name.hpp"

using boost::optional;
namespace schema = gott::util::tdl::schema;
namespace structure = gott::util::tdl::structure;
using schema::rule;
using schema::context;
using structure::repatch;

class context::IMPL {
public:
  IMPL() : first(0) {}
  ~IMPL();

  typedef std::vector<rule::factory *> container;
  container pool;
  rule::factory const *first;
  std::list<rule::factory *> org;

  void add_owning(rule::factory *f, optional<slotcfg> const &c, repatch *r);
  void add_child(rule::factory const *, boost::optional<slotcfg> const &);
  void add_slotted(rule::factory const *, slotcfg const &);
  void add_enc_slotted(rule::factory const *, slotcfg const &,
                       boost::optional<slotcfg> const & = boost::none);
  void add_owned(rule::factory *, boost::optional<slotcfg> const &);
};

context::context() : p(new IMPL) {}

context::~context() {}

void context::begin(unsigned i, rule::attributes const &a,
                    optional<slotcfg> const &c, repatch *r) {
  p->add_owning(get_factory(i, a), c, r);
}

void context::begin(std::wstring const &n, rule::attributes const &a,
                    optional<slotcfg> const &c, repatch *r) {
  begin(name_manager().get(n), a, c, r);
}

void context::end() {
  p->org.pop_back();
}

void context::ref(context const &other) {
  p->add_child(&other.get(), boost::none);
}

rule::factory const &context::get() const {
  return *p->first;
}

context::IMPL::~IMPL() {
  for (container::iterator it = pool.begin(); it != pool.end(); ++it)
    delete *it;
}

void context::IMPL::add_owning(rule::factory *f, optional<slotcfg> const &c, 
                               repatch *) {
  pool.push_back(f);
  add_owned(f, c);
}

void context::IMPL::add_owned(rule::factory *f, optional<slotcfg> const &c) {
  add_child(f, c);
  org.push_back(f);
}

void context::IMPL::add_child(rule::factory const *f, 
                              optional<slotcfg> const &c) {
  if (!org.empty())
    if (!c)
      org.back()->add(*f);
    else
      add_slotted(f, c.get());
  else
    first = f;
}

void context::IMPL::add_slotted(rule::factory const *f, slotcfg const &c) {
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

void context::IMPL::add_enc_slotted(rule::factory const *f, slotcfg const &c,
                              optional<slotcfg> const &e) {
  if (c.get_mode() == slotcfg::one)
    return org.back()->add(*f);

  static unsigned i = name_manager().get(L"list");
  add_owning(get_factory(i, rule::attributes(false)), e, 0);
    add_child(f, c);
  org.pop_back();
}
