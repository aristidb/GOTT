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

#include "parse.hpp"
#include "rule_factory.hpp"
#include "event.hpp"
#include "../exceptions.hpp"
#include "stream_position.hpp"
#include "parse_position.hpp"
#include "../structure/repatch.hpp"
#include <gott/util/range_algo.hpp>
#include <gott/util/autoconv.hpp>
#include <gott/util/nstring/stl.hpp>
#include <gott/util/debug/assert.hpp>

#include <iostream> //DEBUG

using std::list;
using boost::shared_ptr;
using gott::nstring;
namespace simple = gott::tdl::simple;
namespace structure = gott::tdl::structure;

using gott::tdl::schema::match;
using gott::tdl::schema::positioning;
using structure::writable_structure;

class match::IMPL {
public:
  IMPL(structure::revocable_structure &x, match &r);

  void add(rule_factory const &);

  template<class T>
  void handle_token(T const &);

  void replay_buffer();

  void handle_tevent(ev::token_t const &);
  template<bool tok>
  void handle_event(ev::event const &);
  bool try_play(ev::event const &, rule &r);
  template<bool tok>
  bool handle_rule(ev::event const &);

  bool consume_event();
  bool pass_event();

  void succeed_rule();
  void fail_rule();

  void fail_all();

  structure::revocable_structure &base_struc;

  shared_ptr<writable_structure> const &direct_structure_non_base();
  
  positioning pos;

  match &ref;
  detail::stream_position ln;

  struct entry : Moveable<entry> {
    shared_ptr<rule> the_rule;
    shared_ptr<writable_structure> structure;

    entry() {}

    entry(shared_ptr<rule> const &r, shared_ptr<writable_structure> s = 0)
      : the_rule(r), structure(s) {}

    operator shared_ptr<rule>() { return the_rule; }
  };

  typedef Vector<entry> Stack;
  Stack parse;
  Vector<shared_ptr<rule> > shadow;

  static nstring get_name(shared_ptr<rule> const &);
};

NTL_MOVEABLE(shared_ptr<gott::tdl::schema::rule>);

match::match(structure::revocable_structure &p) : pIMPL(new IMPL(p, *this)) {}

match::match(rule_factory const &f, structure::revocable_structure &p)
: pIMPL(new IMPL(p, *this)) {
  pIMPL->add(f);
}

match::~match() {}

void match::add(rule_factory const &rf) {
  pIMPL->add(rf);
}

structure::revocable_structure &match::revocable_structure() const {
  return pIMPL->base_struc;
}

structure::writable_structure &match::direct_structure() const {
  shared_ptr<writable_structure> const &s = pIMPL->direct_structure_non_base();
  if (s)
    return *s;
  return pIMPL->base_struc;
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

void match::node(nstring const &s) {
  pIMPL->handle_token(ev::node(s));
}

void match::up() {
  pIMPL->handle_token(ev::up());
}

void match::end_parse() {
  pIMPL->handle_token(ev::end_parse());
}

void match::comment(nstring const &, bool) {}

// Implementation

match::IMPL::IMPL(structure::revocable_structure &p, match &r)
  : base_struc(p), pos(base_struc), ref(r) {}

shared_ptr<writable_structure> const&match::IMPL::direct_structure_non_base() {
  if (parse.IsEmpty()) {
    static shared_ptr<writable_structure> nothing;
    return nothing;
  }
  return parse.back().structure;
}

void match::IMPL::add(rule_factory const &f) {
  shared_ptr<writable_structure> struc = direct_structure_non_base();
  int current = parse.GetCount();
  parse.Add();
  shared_ptr<rule> the_rule(f.get(ref));
  if (structure::repatcher const *r = the_rule->attributes().repatcher())
    struc.reset(r->deferred_write(struc ? *struc : base_struc));
  parse[current] = entry(the_rule, struc);
  GOTT_ASSERT_1(parse[current].the_rule, nonnull(), "Acquired rule");
}

template<class T>
void match::IMPL::handle_token(T const &e) {
  shadow.clear();
  copy(range(parse), std::back_inserter(shadow));
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
  if (parse.IsEmpty()) 
    fail_all();
  while (!parse.IsEmpty()) 
    if (handle_rule<tok>(e)) 
      break;
}

template<bool tok> 
bool match::IMPL::handle_rule(ev::event const &event) {
  if (try_play(event, *parse.back().the_rule)) {
    if (tok) pos.consume();
    return consume_event();
  } else {
    if (tok) pos.pass();
    return pass_event();
  }
}

bool match::IMPL::try_play(ev::event const &event, rule &current) {
  try {
    return event.play(current);
  } catch (structure::failed_repatch &) {
    return false;
  }
}

bool match::IMPL::consume_event() {
  if (parse.back().the_rule->expectation() == rule::nothing)
    succeed_rule();
  return true;
}

bool match::IMPL::pass_event() {
  if (parse.back().the_rule->expectation() == rule::need) {
    fail_rule();
    return true;
  } else {
    succeed_rule();
    return pos.want_replay();
  }
}

void match::IMPL::succeed_rule() {
  parse.back().the_rule->finish();
  parse.pop_back();

  if (!parse.IsEmpty())
    handle_event<false>(ev::child_succeed());
}

void match::IMPL::fail_rule() {
  parse.pop_back();

  if (!parse.IsEmpty())
    handle_event<false>(ev::child_fail());
  else
    fail_all();
}

void match::IMPL::fail_all() {
  Vector<nstring> names;
  names.AddN(shadow.GetCount());
  transform(range(shadow), range(names), get_name);
  throw mismatch(ln, names);
}

nstring match::IMPL::get_name(shared_ptr<rule> const &rp) {
  static nstring const s_open("("), s_close(")"), sep(",");
  Vector<nstring> out;
  out.Add(rp->name());
  if (rp->attributes().tags().GetCount() > 0) {
    out.Add(s_open);
    range_t<nstring const *> r = range(rp->attributes().tags());
    out.Add(*r.begin);
    for (nstring const *it = r.begin + 1; it != r.end; ++it) {
      out.Add(sep);
      out.Add(*it);
    }
    out.Add(s_close);
  }
  return nstring(range(out).cast<nstring const *>());
}
