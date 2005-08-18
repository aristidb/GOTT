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

#include "event.hpp"
#include "match.hpp"
#include "rule.hpp"

namespace ev = gott::tdl::schema::ev;
using gott::tdl::schema::rule;
using std::ostream;
using boost::static_visitor;

ev::event::~event() {}

bool ev::begin_parse::play(rule &r) const {
  return r.play(*this);
}

bool ev::node::play(rule &r) const {
  return r.play(*this);
}

bool ev::down::play(rule &r) const {
  return r.play(*this);
}

bool ev::up::play(rule &r) const { 
  return r.play(*this);
}

bool ev::end_parse::play(rule &r) const {
  return r.play(*this);
}

bool ev::child_succeed::play(rule &r) const {
  return r.play(*this);
}

bool ev::child_fail::play(rule &r) const {
  return r.play(*this);
}

void ev::begin_parse::print(ostream &s) const {
  s << "token:begin_parse";
}

void ev::node::print(ostream &s) const {
  s << "token:node{" << data << "}";
}

void ev::down::print(ostream &s) const {
  s << "token:down";
}

void ev::up::print(ostream &s) const {
  s << "token:up";
}

void ev::end_parse::print(ostream &s) const {
  s << "token:end_parse";
}

void ev::child_succeed::print(ostream &s) const {
  s << "notify:child_succeed";
}

void ev::child_fail::print(ostream &s) const {
  s << "notify:child_fail";
}

ev::node::node(string const &s) : data(s) {}

gott::string const &ev::node::get_data() const { return data; }

namespace {
  struct token_visitor : public static_visitor<ev::token const &> {
    ev::token const &operator() (ev::token const &t) const {
      return t;
    }
  };
}

ev::token const &ev::get(ev::token_t const &t) {
  return apply_visitor(token_visitor(), t);
};
