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

#include "any.hpp"
#include "../type.hpp"
#include <gott/plugin/plugin_builder.hpp>

namespace schema = tdl::schema;
namespace ev = schema::ev;
using schema::match_any;
using schema::item;

GOTT_PLUGIN_MAKE_BUILDER_SIMPLE(
    plugin_schema_any,
    schema::concrete_type<match_any>)

match_any::match_any(rule_attr_t const &a, std::vector<rule_t> const &vv, match &m) 
: happy_once(a, m), v(vv), pos(v.begin()) {
  if (pos != v.end()) {
    begin = matcher().pos().current();
    matcher().add(*pos);
  }
}

match_any::~match_any() {
  if (expectation() != nothing)
    matcher().pos().forget(begin);
}

bool match_any::play(ev::child_fail const &) {
  matcher().pos().seek(begin);
  
  if (++pos != v.end()) {
    matcher().add(*pos);
    return true;
  }

  return false;
}

bool match_any::play(ev::child_succeed const &) {
  matcher().pos().forget(begin);
  be_happy();
  return true;
}

bool match_any::accept_empty(rule_attr_t const &, std::vector<rule_t> const &choices) {
  for (std::vector<rule_t>::const_iterator it = choices.begin(); 
       it != choices.end(); ++it)
    if (it->accept_empty())
      return true;
  return false;
}

gott::string match_any::name() const {
  return gott::string("tdl::schema::any");
}
