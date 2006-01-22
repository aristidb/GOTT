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
#include <gott/range_algo.hpp>
#include <gott/string/stl.hpp>
#include <gott/debug/assert.hpp>
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

class match::impl {
public:
  impl(structure::revocable_structure &x, match &r);

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

    deferred_miss(deferred_miss const &o)
    : event(o.event->clone()), count(o.count) {}

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

  struct entry {
    shared_ptr<item> the_item;
    shared_ptr<writable_structure> structure;

    entry() {}
    entry(item *r) : the_item(r) {}

    entry(item *r, shared_ptr<writable_structure> const &s) 
    : the_item(r), structure(s) {}
  };

  typedef std::vector<entry> Stack;
  Stack parse;
  std::vector<string> shadow_names;

  static string get_name(item const &);
};

match::match(structure::revocable_structure &p) 
: parser(0), pimpl(new impl(p, *this)) {
  set_line_logger(get_debug());
}

match::match(rule_t const &rf, structure::revocable_structure &p)
: parser(0), pimpl(new impl(p, *this)) {
  set_line_logger(get_debug());
  pimpl->add(rf);
}

match::~match() {}

void match::add(rule_t const &rf) {
  pimpl->add(rf);
}

structure::revocable_structure &match::revocable_structure() const {
  return pimpl->base_struc;
}

structure::writable_structure &match::direct_structure() const {
  shared_ptr<writable_structure> s = pimpl->direct_structure_non_base();
  if (s)
    return *s;
  return pimpl->base_struc;
}

simple::line_logger *match::get_debug() const {
  return &pimpl->ln;
}

positioning &match::pos() const {
  return pimpl->pos;
}

void match::parental_requirement(ev::event const &event, unsigned count) {
  pimpl->parental_requirement(event, count);
}

// Parser forwarding

void match::begin_parse() {
  pimpl->handle_token(ev::begin_parse());
}

void match::down() {
  pimpl->handle_token(ev::down());
}

void match::node(string const &s) {
  pimpl->handle_token(ev::node(s));
}

void match::up() {
  pimpl->handle_token(ev::up());
}

void match::end_parse() {
  pimpl->handle_token(ev::end_parse());
}

void match::comment(string const &, bool) {}

// Implementation

match::impl::impl(structure::revocable_structure &p, match &r)
: base_struc(p), pos(base_struc), ref(r) {}

shared_ptr<writable_structure> match::impl::direct_structure_non_base() {
  if (parse.empty()) 
    return shared_ptr<writable_structure>();
  return parse.back().structure;
}

void match::impl::add(rule_t const &f) {
  shared_ptr<writable_structure> struc = direct_structure_non_base();

  if (structure::repatcher const *r = f.attributes().repatcher())
    struc.reset(r->deferred_write(struc ? *struc : base_struc));

  int current = parse.size();
  parse.push_back(entry());
  parse[current].structure = struc;

  item *the_item = f.get(ref);
  GOTT_ASSERT_2(the_item,(item*)0,std::not_equal_to<item*>(),"Acquired rule_t");
  GOTT_ASSERT_2(the_item->attributes(), f.attributes(), 
      std::equal_to<rule_attr_t>(), "Rule attributes");

  parse[current].the_item.reset(the_item);
}

template<class T>
void match::impl::handle_token(T const &e) {
  shadow_names.clear();
  range_t<Stack::iterator> in = range(parse);
  while (!in.empty())
    shadow_names.push_back(get_name(*in.Begin++->the_item));

  if (miss) {    
    real_parental_requirement();
    miss.reset();
  }
  
  pos.add(e);
  handle_event(e, true);
  while (pos.want_replay())
    replay_buffer();
}

void match::impl::replay_buffer() {
  struct acc : positioning::acceptor {
    match::impl *tt;
    void operator() (ev::token const &t) {
      tt->handle_event(t, true);
    }
    acc(match::impl *t) : tt(t) {}
  } a(this);
  pos.replay(a);
}

void match::impl::handle_event(ev::event const &event, bool token) {
#ifdef VERBOSE
  std::cout << event << '{' << std::endl;
  struct close { ~close() { std::cout << '}' << std::endl; } } x; (void)x;
#endif
  if (parse.empty()) 
    fail_all();
  while (!parse.empty()) 
    if (handle_item(event, token)) 
      break;
}

bool match::impl::handle_item(ev::event const &event, bool token) {
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

bool match::impl::try_play(ev::event const &event, item &current) {
  try {
    return event.play(current);
  } catch (structure::failed_repatch &) {
    return false;
  }
}

bool match::impl::consume_event(bool) {
#ifdef VERBOSE
  std::cout << "consume" << std::endl;
#endif
  if (parse.back().the_item->expectation() == item::nothing)
    succeed_item();
  return true;
}

bool match::impl::pass_event(bool) {
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

void match::impl::succeed_item() {
  parse.back().the_item->finish();
  parse.pop_back();

  if (!parse.empty())
    handle_event(ev::child_succeed(), false);
}

void match::impl::fail_item() {
  parse.pop_back();

  if (!parse.empty())
    handle_event(ev::child_fail(), false);
  else
    fail_all();
}

void match::impl::fail_all() {
  throw mismatch(ln, shadow_names);
}

void match::impl::parental_requirement(ev::event const &event, unsigned count) {
  miss = deferred_miss(event, count);
}

void match::impl::real_parental_requirement() {
  Stack::iterator it = parse.end();
  if (it == parse.begin())
    fail_all();
  while (--it != parse.begin())
    if (it->the_item->miss_events(*miss->event, miss->count))
      return;
  fail_all();
}

string match::impl::get_name(item const &rl) {
  static string const s_open("("), s_close(")"), sep(",");
  std::vector<string> out;
  std::vector<string> const &tags = rl.attributes().tags();
  out.reserve(1 + 
      (tags.size() > 0 
       ? 1 + tags.size() * 2
       : 0));
  out.push_back(rl.name());
  if (tags.size() > 0) {
    out.push_back(s_open);
    range_t<std::vector<string>::const_iterator> r = range(tags);
    out.push_back(*r.begin());
    for (std::vector<string>::const_iterator it = r.begin() + 1; 
        it != r.end(); ++it) {
      out.push_back(sep);
      out.push_back(*it);
    }
    out.push_back(s_close);
  }
  return string(range(out), string::concatenate);
}
