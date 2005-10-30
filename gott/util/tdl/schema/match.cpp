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

#include "match.hpp"
#include "rule.hpp"
#include "item.hpp"
#include "rule_attr.hpp"
#include "event.hpp"
#include "../exceptions.hpp"
#include "stream_position.hpp"
#include "parse_position.hpp"
#include "../structure/repatch.hpp"
#include <gott/util/range_algo.hpp>
#include <gott/util/string/stl.hpp>
#include <gott/util/debug/assert.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

//#define VERBOSE

using gott::string;
using boost::shared_ptr;
namespace simple = gott::tdl::simple;
namespace structure = gott::tdl::structure;
using gott::tdl::schema::match;
using gott::tdl::schema::positioning;
using structure::writable_structure;

class match::IMPL {
public:
  IMPL(structure::revocable_structure &x, match &r);

  void add(rule_t const &);

  template<class T>
  void handle_token(T const &);

  void replay_buffer();

  void handle_tevent(ev::token_t const &);
  void handle_event(ev::event const &, bool token);
  bool try_play(ev::event const &, item &r);
  bool handle_item(ev::event const &, bool token);

  bool consume_event(bool token);
  bool pass_event(bool token);

  void succeed_item();
  void fail_item();

  void fail_all();

  void parental_requirement(ev::event const &, unsigned);
  void real_parental_requirement();

  struct deferred_miss {
    deferred_miss(ev::event const &e, unsigned c) 
    : event(e.clone()), count(c) {}

    deferred_miss(deferred_miss pick_ &o)
    : event(o.event), count(o.count) {
      const_cast<deferred_miss &>(o).event = 0;
    }

    ~deferred_miss() { delete event; }

    ev::event *event;
    unsigned count;
  };

  ::boost::optional<deferred_miss> miss;

  structure::revocable_structure &base_struc;

  shared_ptr<writable_structure> direct_structure_non_base();
  
  positioning pos;

  match &ref;
  detail::stream_position ln;

  struct entry : Moveable<entry> {
    item *the_item;
    shared_ptr<writable_structure> structure;

    entry() {}

    entry(entry pick_ &o) : the_item(o.the_item), structure(o.structure) {
      entry &x = const_cast<entry &>(o);
      x.the_item = 0;
      x.structure.reset();
    }

    ~entry() {
      delete the_item;
    }

    entry(item *r) : the_item(r) {}

    entry(item *r, shared_ptr<writable_structure> const &s) 
    : the_item(r), structure(s) {}
  };

  typedef Vector<entry> Stack;
  Stack parse;
  Vector<string> shadow_names;

  static string get_name(item const &);
};

match::match(structure::revocable_structure &p) 
: parser(0), pIMPL(new IMPL(p, *this)) {
  set_line_logger(get_debug());
}

match::match(rule_t const &rf, structure::revocable_structure &p)
: parser(0), pIMPL(new IMPL(p, *this)) {
  set_line_logger(get_debug());
  pIMPL->add(rf);
}

match::~match() {}

void match::add(rule_t const &rf) {
  pIMPL->add(rf);
}

structure::revocable_structure &match::revocable_structure() const {
  return pIMPL->base_struc;
}

structure::writable_structure &match::direct_structure() const {
  shared_ptr<writable_structure> s = pIMPL->direct_structure_non_base();
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

void match::parental_requirement(ev::event const &event, unsigned count) {
  pIMPL->parental_requirement(event, count);
}

// Parser forwarding

void match::begin_parse() {
  pIMPL->handle_token(ev::begin_parse());
}

void match::down() {
  pIMPL->handle_token(ev::down());
}

void match::node(string const &s) {
  pIMPL->handle_token(ev::node(s));
}

void match::up() {
  pIMPL->handle_token(ev::up());
}

void match::end_parse() {
  pIMPL->handle_token(ev::end_parse());
}

void match::comment(string const &, bool) {}

// Implementation

match::IMPL::IMPL(structure::revocable_structure &p, match &r)
: base_struc(p), pos(base_struc), ref(r) {}

shared_ptr<writable_structure> match::IMPL::direct_structure_non_base() {
  if (parse.IsEmpty()) 
    return shared_ptr<writable_structure>();
  return parse.back().structure;
}

void match::IMPL::add(rule_t const &f) {
  shared_ptr<writable_structure> struc = direct_structure_non_base();

  if (structure::repatcher const *r = f.attributes().repatcher())
    struc.reset(r->deferred_write(struc ? *struc : base_struc));

  int current = parse.GetCount();
  parse.Add().structure = struc;

  item *the_item = f.get(ref);
  GOTT_ASSERT_2(the_item,(item*)0,std::not_equal_to<item*>(),"Acquired rule_t");
  GOTT_ASSERT_2(the_item->attributes(), f.attributes(), 
      std::equal_to<rule_attr_t>(), "Rule attributes");

  parse[current].the_item = the_item;
}

template<class T>
void match::IMPL::handle_token(T const &e) {
  shadow_names.clear();
  range_t<Stack::iterator> in = range(parse);
  while (!in.empty())
    shadow_names.Add(get_name(*in.begin++->the_item));

  if (miss) {    
    real_parental_requirement();
    miss.reset();
  }
  
  pos.add(e);
  handle_event(e, true);
  while (pos.want_replay())
    replay_buffer();
}

void match::IMPL::replay_buffer() {
  struct acc : positioning::acceptor {
    match::IMPL *tt;
    void operator() (ev::token const &t) {
      tt->handle_event(t, true);
    }
    acc(match::IMPL *t) : tt(t) {}
  } a(this);
  pos.replay(a);
}

void match::IMPL::handle_event(ev::event const &event, bool token) {
#ifdef VERBOSE
  std::cout << event << '{' << std::endl;
  struct close { ~close() { std::cout << '}' << std::endl; } } x; (void)x;
#endif
  if (parse.IsEmpty()) 
    fail_all();
  while (!parse.IsEmpty()) 
    if (handle_item(event, token)) 
      break;
}

bool match::IMPL::handle_item(ev::event const &event, bool token) {
#ifdef VERBOSE
  std::cout << get_name(*parse.back().the_item) << '{' << std::endl;
  struct close { ~close() { std::cout << '}' << std::endl; } } x; (void)x;
#endif
  if (try_play(event, *parse.back().the_item)) {
    if (token) pos.consume();
    return consume_event(token);
  } else {
    if (token) pos.pass();
    return pass_event(token);
  }
}

bool match::IMPL::try_play(ev::event const &event, item &current) {
  try {
    return event.play(current);
  } catch (structure::failed_repatch &) {
    return false;
  }
}

bool match::IMPL::consume_event(bool) {
#ifdef VERBOSE
  std::cout << "consume" << std::endl;
#endif
  if (parse.back().the_item->expectation() == item::nothing)
    succeed_item();
  return true;
}

bool match::IMPL::pass_event(bool) {
#ifdef VERBOSE
  std::cout << "pass" << std::endl;
#endif
  if (parse.back().the_item->expectation() == item::need) {
    fail_item();
    return true;
  } else {
    succeed_item();
    return pos.want_replay();
  }
}

void match::IMPL::succeed_item() {
  parse.back().the_item->finish();
  parse.pop_back();

  if (!parse.IsEmpty())
    handle_event(ev::child_succeed(), false);
}

void match::IMPL::fail_item() {
  parse.pop_back();

  if (!parse.IsEmpty())
    handle_event(ev::child_fail(), false);
  else
    fail_all();
}

void match::IMPL::fail_all() {
  throw mismatch(ln, shadow_names);
}

void match::IMPL::parental_requirement(ev::event const &event, unsigned count) {
  miss = deferred_miss(event, count);
}

void match::IMPL::real_parental_requirement() {
  Stack::iterator it = parse.end();
  if (it == parse.begin())
    fail_all();
  while (--it != parse.begin())
    if (it->the_item->miss_events(*miss->event, miss->count))
      return;
  fail_all();
}

string match::IMPL::get_name(item const &rl) {
  static string const s_open("("), s_close(")"), sep(",");
  Vector<string> out;
  out.Reserve(1 + 
      (rl.attributes().tags().GetCount() > 0 
       ? 1 + rl.attributes().tags().GetCount() * 2
       : 0));
  out.Add(rl.name());
  if (rl.attributes().tags().GetCount() > 0) {
    out.Add(s_open);
    range_t<string const *> r = range(rl.attributes().tags());
    out.Add(*r.begin);
    for (string const *it = r.begin + 1; it != r.end; ++it) {
      out.Add(sep);
      out.Add(*it);
    }
    out.Add(s_close);
  }
  return string(range(out).cast<string const *>(), string::concatenate);
}
