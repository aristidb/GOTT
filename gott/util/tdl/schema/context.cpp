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
#include "../structure/repatch.hpp"

using boost::optional;
using boost::shared_ptr;
namespace schema = gott::util::tdl::schema;
namespace structure = gott::util::tdl::structure;
using schema::rule;
using schema::context;
using structure::repatcher;

class context::IMPL {
public:
  IMPL() : first(0, 0) {}

  struct active_entry  {
    shared_ptr<rule::factory> factory;
    shared_ptr<structure::repatcher const> repatcher;
    active_entry(rule::factory *f, structure::repatcher const *rr) 
      : factory(f), repatcher(rr) {}

    operator entry() const { return entry(factory.get(), repatcher.get()); }
  };

  typedef std::vector<active_entry> container;
  container pool;
  entry first;
  std::list<active_entry> org;

  void add_owning(active_entry const &f, optional<slotcfg> const &);
  void add_child(entry const &, boost::optional<slotcfg> const &);
  void add_slotted(entry const &, slotcfg const &);
  void add_enc_slotted(entry const &, slotcfg const &,
                       boost::optional<slotcfg> const & = boost::none);
  void add_owned(active_entry const &, boost::optional<slotcfg> const &);
};

context::context() : p(new IMPL) {}

context::~context() {}

void context::begin(unsigned i, rule::attributes const &a,
                    optional<slotcfg> const &c, repatcher const *r) {
  p->add_owning(IMPL::active_entry(get_factory(i, a), r), c);
}

void context::begin(std::wstring const &n, rule::attributes const &a,
                    optional<slotcfg> const &c, repatcher const *r) {
  begin(name_manager().get(n), a, c, r);
}

void context::end() {
  p->org.pop_back();
}

void context::ref(context const &other) {
  p->add_child(other.get(), boost::none);
}

context::entry context::get() const {
  return p->first;
}

void context::IMPL::add_owning(active_entry const &f, 
                               optional<slotcfg> const &c) {
  pool.push_back(f);
  add_owned(f, c);
}

void context::IMPL::add_owned(active_entry const &f, 
                              optional<slotcfg> const &c) {
  add_child(f, c);
  org.push_back(f);
}

void context::IMPL::add_child(entry const &f, optional<slotcfg> const &c) {
  if (!org.empty())
    if (!c)
      org.back().factory->add(*f.factory);
    else
      add_slotted(f, c.get());
  else
    first = f;
}

void context::IMPL::add_slotted(entry const &f, slotcfg const &c) {
  rule::factory::with_slotcfg *w = org.back().factory->get_with_slotcfg();

  if (!w)
    return add_enc_slotted(f, c);

  if (w->supports(c))
    w->add(*f.factory, c);
  else if (c.prefix_optional() && (w->accepted_modes() & slotcfg::optional))
    add_enc_slotted(f, c.no_optional(), slotcfg(slotcfg::optional));
  else
    add_enc_slotted(f, c);
}

void context::IMPL::add_enc_slotted(entry const &f, slotcfg const &c,
                              optional<slotcfg> const &e) {
  if (c.get_mode() == slotcfg::one)
    return org.back().factory->add(*f.factory);

  static unsigned i = name_manager().get(L"list");
  add_owning(active_entry(get_factory(i, rule::attributes(false)), 0), e);
    add_child(f, c);
  org.pop_back();
}
