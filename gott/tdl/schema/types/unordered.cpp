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

#include "unordered.hpp"
#include <gott/range_algo.hpp>
#include "../rule_attr.hpp"

namespace schema = tdl::schema;
namespace ev = tdl::schema::ev;
using schema::item;
using schema::match_unordered;
using std::vector;

match_unordered::match_unordered(rule_attr_t const &a, vector<rule_t> const &r,
    match &m) 
: item(a, m), last(m.pos().current()), all_happy(true) {
  gott::copy(gott::range(r), std::back_inserter(children));

  pos = children.begin();

  if (pos != children.end())
    matcher().add(pos->generator);
}

match_unordered::~match_unordered() {
  matcher().pos().forget(last);
}

bool match_unordered::play(ev::child_succeed const &) {
  pos->slot.add();
  if (pos->slot.expectation() == item::nothing) 
    children.erase(pos);

  if (!children.empty()) {
    pos = children.begin();
    matcher().pos().forget(last);
    last = matcher().pos().current();
    matcher().add(pos->generator);
  }

  return true;
}

bool match_unordered::play(ev::child_fail const &) {
  matcher().pos().seek(last);
  bool happy = pos->slot.expectation() != need;
  if (++pos == children.end())
    if (happy) {
      children = list_t();
      return true;
    } else {
      all_happy = false;
      return false;
    }
  matcher().add(pos->generator);
  return true;
}

item::expect match_unordered::expectation() const {
  if (children.empty() && all_happy)
    return nothing;
  return need;
}

bool match_unordered::accept_empty(rule_attr_t const &, 
                                   std::vector<rule_t> const &children) {
  bool accept = true;
  for (std::vector<rule_t>::const_iterator it = children.begin(); 
       it != children.end(); ++it)
    accept &= it->attributes().outer().prefix_optional() 
              || it->accept_empty();
  return accept;
}

gott::string match_unordered::name() const {
  return gott::string("unordered");
}
