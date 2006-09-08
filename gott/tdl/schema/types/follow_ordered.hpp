// vim:ts=2:sw=2:expandtab:autoindent:filetype=cpp:
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

#ifndef GOTT_TDL_SCHEMA_FOLLOW_ORDERED_HPP
#define GOTT_TDL_SCHEMA_FOLLOW_ORDERED_HPP

#include "../match.hpp"
#include "../rule.hpp"
#include "../slot.hpp"
#include "../parse_position.hpp"
#include "../rule_attr.hpp"
#include <gott/string/atom.hpp>

namespace tdl {
namespace schema {

class match_follow_ordered : public item {
public:
  match_follow_ordered(rule_attr_t const &, std::vector<rule_t> const &,match&);
  ~match_follow_ordered();

  static bool accept_empty(rule_attr_t const &, std::vector<rule_t> const &);

  static gott::atom const id;
  
private:
  struct active_element {
    rule_t generator;
    slotcfg slot;
    bool accept_empty, rest_accept_empty;
    
    active_element(rule_t const &e) 
    : generator(e), slot(e.attributes().outer()) {}
  };

  void init_accept_empty();
  bool search_insertible() const;

  typedef std::vector<active_element> container;
  container children;
  mutable container::iterator pos;
  int opened;
  bool saw_up;
  positioning::id last;
  bool unhappy;
  bool no_req;

  expect expectation() const;
  bool play(ev::child_succeed const &);
  bool play(ev::child_fail const &);
  bool play(ev::down const &);
  bool play(ev::up const &);
  bool miss_events(ev::event const &, unsigned);
  gott::string name() const;
};

}}

#endif
