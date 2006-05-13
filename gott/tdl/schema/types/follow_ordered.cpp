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

#include "follow_ordered.hpp"
#include "../event.hpp"
#include "../type.hpp"
#include <gott/plugin/plugin_builder.hpp>

namespace schema = tdl::schema;
namespace ev = tdl::schema::ev;
using schema::item;
using schema::match_follow_ordered;

GOTT_PLUGIN_MAKE_BUILDER_SIMPLE(
    plugin_schema_follow,
    schema::concrete_type<match_follow_ordered>)

match_follow_ordered::match_follow_ordered(rule_attr_t const &a, 
                                           std::vector<rule_t> const &c, 
                                           match &m)
: item(a, m), opened(0), saw_up(false), last(m.pos().current()), 
    unhappy(false), no_req(false) {
  for (std::vector<rule_t>::const_iterator it = c.begin(); it != c.end(); ++it)
    children.push_back(*it);
  pos = children.begin();
  init_accept_empty();
  if (expectation() == nothing)
    return;
  last = matcher().pos().current();
  matcher().add(pos->generator);
}

void match_follow_ordered::init_accept_empty() {
  bool rest_accept_empty = true;
  for (int i = children.size() - 1; i >= 0; --i) {
    active_element &e = children[i];
    e.rest_accept_empty = rest_accept_empty;
    e.accept_empty = e.generator.accept_empty();
    rest_accept_empty &= (e.slot.prefix_optional() || e.accept_empty);
  }
}

bool match_follow_ordered::play(ev::child_succeed const &) {
  if (matcher().pos().proceeded(last)) {
    pos->slot.add();
    matcher().pos().forget(last);
    last = matcher().pos().current();
  } else {
    ++pos; // don't repeat empty
    if (!search_insertible())
      return true;
    matcher().pos().forget(last);
    last = matcher().pos().current();
    matcher().add(pos->generator);
  }
  return true;
}

bool match_follow_ordered::play(ev::child_fail const &) {
  bool this_flee = !pos->accept_empty;
  unhappy |= this_flee && pos->slot.expectation() == need;
  ++pos;
  if (unhappy || !search_insertible()) {
    if (!this_flee)
      return true;
    matcher().pos().seek(last);
    no_req = true;
    return false;
  }
  matcher().pos().seek(last);
  if (--opened < 0) {
    opened = 0;
    matcher().add(pos->generator);
  }
  return true;
}

bool match_follow_ordered::play(ev::down const &) {
  if (saw_up || !search_insertible())
    return false;
  ++opened;
  matcher().add(pos->generator);
  return true;
}

bool match_follow_ordered::play(ev::up const &) {
  saw_up = true;
  if (--opened < 0) {
    opened = 0;
    return false;
  }
  return true;
}

bool match_follow_ordered::search_insertible() const {
  while (pos != children.end() && !accept_more(pos->slot.expectation()))
    ++pos;
  return pos != children.end();
}

item::expect match_follow_ordered::expectation() const {
  if (unhappy) 
    return need;
  if (!search_insertible()) 
    return opened > 0 ? maybe : nothing;
  if (pos->rest_accept_empty)
    if (pos->slot.expectation() != need || pos->accept_empty) 
      return maybe;
  return need;
}

bool match_follow_ordered::accept_empty(rule_attr_t const &,
    std::vector<rule_t> const &children) {
  bool accept = true;
  for (std::vector<rule_t>::const_iterator it = children.begin(); 
       it != children.end(); ++it)
    accept &= it->attributes().outer().prefix_optional() || it->accept_empty();
  return accept;
}

gott::string match_follow_ordered::name() const {
  return gott::string("tdl::schema::follow");
}

match_follow_ordered::~match_follow_ordered() {
  matcher().pos().forget(last);
  if (!no_req && expectation() != need && opened > 0) 
    matcher().parental_requirement(ev::up(), opened);
}

bool match_follow_ordered::miss_events(ev::event const &event, unsigned count) {
  if (typeid(event) != typeid(ev::up))
    return false;
  opened += count;
  return true;
}
