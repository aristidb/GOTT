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

#include "parse.hpp"
#include "rule_factory.hpp"
#include "event.hpp"
#include "mismatch.hpp"
#include "position.hpp"
#include "parse_position.hpp"

using std::wstring;
using std::list;
using boost::shared_ptr;
namespace simple = gott::util::tdl::simple;
namespace structure = gott::util::tdl::structure;

using gott::util::tdl::schema::match;
using gott::util::tdl::schema::positioning;

class match::IMPL {
public:
  IMPL(structure::revocable_structure &x, match &r);

  void add(rule::factory const &);

  template<class T>
  void handle_token(T const &);

  void replay_buffer();

  void handle_tevent(ev::token_t const &);
  template<bool tok>
  void handle_event(ev::event const &);
  template<bool tok>
  bool handle_rule(ev::event const &);

  bool consume_event();
  bool pass_event();

  void succeed_rule();
  void fail_rule();

  typedef list<shared_ptr<rule> > Stack;
  Stack parse;

  structure::revocable_structure &struc;
  positioning pos;

  match &ref;
  detail::line_pos ln;
};

match::match(structure::revocable_structure &p) : pIMPL(new IMPL(p, *this)) {}

match::match(rule::factory const &f, structure::revocable_structure &p)
: pIMPL(new IMPL(p, *this)) {
  pIMPL->add(f);
}

match::~match() { delete pIMPL; }

// Forwarding

void match::add(rule::factory const &rf) {
  pIMPL->add(rf);
}

structure::revocable_structure &match::structure() const {
  return pIMPL->struc;
}

simple::line_logger *match::get_debug() const {
  return &pIMPL->ln;
}

positioning &match::pos() const {
  return pIMPL->pos;
}

// Parser forwarding

void match::begin_parse() {
  pIMPL->handle_token(ev::begin_parse());
}

void match::down() {
  pIMPL->handle_token(ev::down());
}

void match::node(wstring const &s) {
  pIMPL->handle_token(ev::node(s));
}

void match::up() {
  pIMPL->handle_token(ev::up());
}

void match::end_parse() {
  pIMPL->handle_token(ev::end_parse());
}

void match::comment(wstring const &, bool) {}

// Implementation

match::IMPL::IMPL(structure::revocable_structure &p, match &r)
  : struc(p), pos(struc), ref(r) {}

void match::IMPL::add(rule::factory const &f) {
  Stack::iterator it = --parse.end();
  shared_ptr<rule> x(f.get(ref));
  parse.insert(++it, x);
}

template<class T>
void match::IMPL::handle_token(T const &e) {
  pos.add(e);
  handle_event<true>(e);
  while (pos.want_replay())
    replay_buffer();
}

void match::IMPL::replay_buffer() {
  struct acc : positioning::acceptor {
    match::IMPL *tt;
    void operator() (ev::token const &t) {
      tt->handle_event<true>(t);
    }
    acc(match::IMPL *t) : tt(t) {}
  } a(this);
  pos.replay(a);
}

template<bool tok>
void match::IMPL::handle_event(ev::event const &e) {
  while (!parse.empty()) 
    if (handle_rule<tok>(e)) 
      break;
}

template<bool tok> 
bool match::IMPL::handle_rule(ev::event const &event) {
  if (event.play(*parse.back())) {
    if (tok) pos.consume();
    return consume_event();
  } else
    return pass_event();
}

bool match::IMPL::consume_event() {
  if (parse.back()->expects() == rule::nothing)
    succeed_rule();
  return true;
}

bool match::IMPL::pass_event() {
  if (parse.back()->expects() == rule::need) {
    fail_rule();
    return true;
  } else {
    succeed_rule();
    return pos.want_replay();
  }
}

void match::IMPL::succeed_rule() {
  parse.back()->finish();
  parse.pop_back();

  if (!parse.empty())
    handle_event<false>(ev::child_succeed());
}

void match::IMPL::fail_rule() {
  parse.pop_back();

  if (!parse.empty())
    handle_event<false>(ev::child_fail());
  else
    throw mismatch(ln);
}
