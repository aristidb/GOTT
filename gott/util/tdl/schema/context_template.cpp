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

using std::vector;
using boost::static_visitor;
using gott::util::tdl::schema::context_template;
using gott::util::tdl::schema::rule;

context_template::context_template() {}
context_template::~context_template() {}

void context_template::begin(unsigned type, rule::attributes const &attr) {
  var.push_back(entry_begin(type, attr));
}

void context_template::begin(unsigned type, rule::attributes const &attr,
                             slotcfg const &scfg) {
  var.push_back(entry_begin(type, attr, scfg));
}

void context_template::end() {
  var.push_back(entry_end());
}

void context_template::param(eID x) {
  var.push_back(x);
}

struct context_template::visitor : public static_visitor<>  {
  visitor(vector<context*> const &vv, context &oo) : v(vv), o(oo) {}
  
  vector<context*> const &v;
  context &o;
  
  void operator() (context_template::entry_begin const &b) {
    o.begin(b.id, b.att, b.scfg);
  }

  void operator() (context_template::entry_end const &) {
    o.end();
  }

  void operator() (eID i) {
    o.ref(*v[i]);
  }
};

void context_template::instantiate(vector<context*> const &v, context &c) {
  visitor w(v, c);
  for_each(var.begin(), var.end(), apply_visitor(w));
}
