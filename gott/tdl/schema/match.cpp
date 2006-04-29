/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is A C++ Library for Dealing with the TDL Language.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2004-2006
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *  Aristid Breitkreuz (aribrei@arcor.de)
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include "match.hpp"
#include "rule.hpp"
#include "item.hpp"
#include "rule_attr.hpp"
#include "event.hpp"
#include "../exceptions.hpp"
#include "parse_position.hpp"
#include "../structure/repatch.hpp"
#include <gott/range_algo.hpp>
#include <gott/string/stl.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <cassert>

//#define VERBOSE

using gott::string;
using boost::shared_ptr;
namespace structure = tdl::structure;
using tdl::schema::match;
using tdl::schema::positioning;
using tdl::source_position;
using structure::writable_structure;

class match::impl {
public:
  impl(structure::revocable_structure &p, match &r)
  : base_struc(p), pos(base_struc), ref(r) {}

  template<class T>
  void handle_token(T const &);

  void replay_buffer();

  void handle_tevent(ev::token_t const &);
  void handle_event(ev::event const &, bool token);
  bool try_play(ev::event const &, item &r);
  bool handle_item(ev::event const &, bool token);
  void inject_item(ev::token const &, source_position const &);

  bool consume_event(bool token);
  bool pass_event(bool token);

  void succeed_item();
  void fail_item();

  void fail_all();

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

  ::boost::optional<source_position> overwrite_where;
};

match::match(structure::revocable_structure &p) 
: p(new impl(p, *this)) {}

match::match(rule_t const &rf, structure::revocable_structure &p)
: p(new impl(p, *this)) {
  add(rf);
}

match::~match() {}

void match::add(rule_t const &f) {
  shared_ptr<writable_structure> struc = p->direct_structure_non_base();

  if (structure::repatcher const *r = f.attributes().repatcher())
    struc.reset(r->deferred_write(struc ? *struc : p->base_struc));

  int current = p->parse.size();
  p->parse.push_back(impl::entry());
  p->parse[current].structure = struc;

  item *the_item = f.get(*this);
  assert(the_item != 0);
  assert(the_item->attributes() == f.attributes());

  p->parse[current].the_item.reset(the_item);
}

structure::revocable_structure &match::revocable_structure() const {
  return p->base_struc;
}

structure::writable_structure &match::direct_structure() const {
  shared_ptr<writable_structure> s = p->direct_structure_non_base();
  if (s)
    return *s;
  return p->base_struc;
}

positioning &match::pos() const {
  return p->pos;
}

source_position const &match::where_out() const {
  if (p->overwrite_where)
    return p->overwrite_where.get();
  return where();
}

void match::parental_requirement(ev::event const &event, unsigned count) {
  p->miss = impl::deferred_miss(event, count);
}

// Parser forwarding

void match::begin_parse() {
  p->handle_token(ev::begin_parse());
}

void match::down() {
  p->handle_token(ev::down());
}

void match::node(string const &s) {
  p->handle_token(ev::node(s));
}

void match::up() {
  p->handle_token(ev::up());
}

void match::end_parse() {
  p->handle_token(ev::end_parse());
}

void match::comment(string const &, bool) {}

// Implementation

shared_ptr<writable_structure> match::impl::direct_structure_non_base() {
  if (parse.empty()) 
    return shared_ptr<writable_structure>();
  return parse.back().structure;
}

template<class T>
void match::impl::handle_token(T const &e) {
  if (miss) {    
    real_parental_requirement();
    miss.reset();
  }
  
  pos.add(e, ref.where_out());
  handle_event(e, true);
  while (pos.want_replay())
    replay_buffer();
}

void match::impl::replay_buffer() {
  pos.replay(boost::bind(&impl::inject_item, boost::ref(*this), _1, _2));
}

void match::impl::inject_item(ev::token const &t, source_position const &w) {
  overwrite_where = w;
  handle_event(t, true);
  overwrite_where.reset();
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
  } catch (tdl_error const &e) {
    if (e.module() != "TDL Structure repatcher")
      throw;
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
  throw tdl_error("TDL Schema matcher", "failed to match schema", 
      ref.where().token_position());
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

