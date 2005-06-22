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

#include "context_template.hpp"
#include "by_name.hpp"

using std::wstring;
using std::vector;
using boost::static_visitor;
using gott::util::tdl::schema::context_template;
using gott::util::tdl::schema::rule;

class context_template::IMPL {
public:
  struct entry_begin {
    entry_begin(unsigned n, rule::attributes const &a) 
      : id(n), att(a) {}
    entry_begin(unsigned n, rule::attributes const &a, 
                slotcfg const &s)
      : id(n), att(a), scfg(s) {}

    unsigned id;
    rule::attributes att;
    boost::optional<slotcfg> scfg;
  };

  struct entry_end {};
  typedef boost::variant<eID, entry_begin, entry_end> job;
  struct visitor;
  typedef std::vector<job> book;

  book var;
};

context_template::context_template() : p(new IMPL) {}
context_template::~context_template() {}

void context_template::begin(wstring const &type, 
                             rule::attributes const &attr) {
  p->var.push_back(IMPL::entry_begin(name_manager().get(type), attr));
}

void context_template::begin(wstring const &type, rule::attributes const &attr,
                             slotcfg const &scfg) {
  p->var.push_back(IMPL::entry_begin(name_manager().get(type), attr, scfg));
}

void context_template::end() {
  p->var.push_back(IMPL::entry_end());
}

void context_template::param(eID x) {
  p->var.push_back(x);
}

struct context_template::IMPL::visitor : public static_visitor<>  {
  visitor(vector<context*> const &vv, context &oo) : v(vv), o(oo) {}

  vector<context*> const &v;
  context &o;

  void operator() (entry_begin const &b) {
    o.begin(b.id, b.att, b.scfg);
  }

  void operator() (entry_end const &) {
    o.end();
  }

  void operator() (eID i) {
    o.ref(*v[i]);
  }
};

void context_template::instantiate(vector<context*> const &v, context &c) {
  IMPL::visitor w(v, c);
  for_each(p->var.begin(), p->var.end(), apply_visitor(w));
}
