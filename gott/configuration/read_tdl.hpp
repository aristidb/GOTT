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

#ifndef GOTT_BASE_CONFIG_READ_TDL_HPP
#define GOTT_BASE_CONFIG_READ_TDL_HPP

#include <gott/tdl/schema/item.hpp>
#include <gott/tdl/schema/rule.hpp>
#include <gott/tdl/schema/rule_attr.hpp>
#include <map>

namespace gott {
namespace config {

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

}}

#endif
