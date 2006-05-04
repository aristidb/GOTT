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
 * The Original Code is A General Purpose Configuration Library.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2005-2006
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

#include <gott/tdl/schema/item.hpp>
#include <gott/tdl/schema/rule.hpp>
#include <gott/tdl/schema/rule_attr.hpp>
#include <gott/tdl/schema/by_name.hpp>
#include <gott/tdl/schema/event.hpp>
#include <gott/tdl/schema/match.hpp>
#include <boost/algorithm/string.hpp>
#include <gott/range.hpp>
#include <map>

using gott::string;
namespace schema = tdl::schema;
namespace ev = schema::ev;

namespace {

/**
 * Matcher for a single config item.
 * Matches one TDL item (which may configure more than one item).
 * Tag is key and key is tag. 
 */
class match_config_tdl : public tdl::schema::item {
public:
  match_config_tdl(tdl::schema::rule_attr_t const &, 
      std::vector<tdl::schema::rule_t> const &, 
      tdl::schema::match &);

  static bool accept_empty(
      tdl::schema::rule_attr_t const &, 
      std::vector<tdl::schema::rule_t> const &)
  { return true; }

private:
  bool play(tdl::schema::ev::down const &);
  bool play(tdl::schema::ev::up const &);
  bool play(tdl::schema::ev::node const &);
  bool play(tdl::schema::ev::child_succeed const &);

  expect expectation() const;

  string name() const;

  typedef std::map<string, tdl::schema::rule_t> children_t;
  children_t children;
  string current_id;
  std::vector<int> add_len;
  children_t::const_iterator next_child;
  bool dirty, peer, may_leave;
};

}

match_config_tdl::match_config_tdl(schema::rule_attr_t const &ra, 
    std::vector<schema::rule_t> const &o_children, 
    schema::match &m) 
: schema::item(ra, m), dirty(false), peer(false), may_leave(false) {
  for (std::vector<schema::rule_t>::const_iterator it = o_children.begin(); 
      it != o_children.end(); ++it)
    children.insert(std::make_pair(it->attributes().tags()[0], *it));
  next_child = children.end();
}

bool match_config_tdl::play(ev::down const &) {
  peer = false;

  if (next_child != children.end() && !dirty)
    matcher().add(next_child->second);
  else {
    add_len.back() += 2;
    current_id = current_id + "::";
  }

  return true;
}

bool match_config_tdl::play(ev::up const &) {
  if (!may_leave)
    return false;

  if (next_child == children.end())
    return false;

  dirty = false;
  peer = false;

  current_id = offset(current_id.as_utf8(), 0, -add_len.back());
  add_len.pop_back();
  
  return true;
}

bool match_config_tdl::play(ev::node const &n) {
  if (peer)
    return false;

  peer = true;
  may_leave = false;
  
  add_len.push_back(n.get_data().size());
  if (current_id == string()) 
    current_id = n.get_data();
  else
    current_id = current_id + n.get_data();

  next_child = children.find(current_id);

  return true;
}

bool match_config_tdl::play(ev::child_succeed const &) {
  dirty = true;
  peer = true;
  may_leave = true;
  return true;
}

schema::item::expect match_config_tdl::expectation() const {
  if (next_child == children.end())
    if (current_id == string())
      return maybe;
    else
      return need;
  if (current_id == string())
    return nothing;
  return maybe;
}

string match_config_tdl::name() const {
  return gott::string("gott::tdl_config");
}
