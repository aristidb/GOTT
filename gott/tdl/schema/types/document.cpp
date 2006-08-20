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

#include "document.hpp"
#include "../type.hpp"
#include <gott/plugin/plugin_builder.hpp>

namespace schema = tdl::schema;
namespace ev = tdl::schema::ev;
using schema::item;
using schema::match_document;

GOTT_PLUGIN_MAKE_BUILDER_SIMPLE(
    plugin_schema_document,
    schema::concrete_type<match_document>)

match_document::match_document(rule_attr_t const &a, 
    std::vector<rule_t> const &sr, match &m)
: happy_once(a, m), sub(deflatten(sr)), state(first) {}

schema::rule_t match_document::deflatten(std::vector<rule_t> const &children) {
  if (children.size() == 1)
    if (children[0].attributes().outer().get_mode() == slotcfg::one)
      return children[0];
    else
      return rule("tdl::schema::list", rule_attr(coat = false), children);
  return rule("tdl::schema::ordered", rule_attr(coat = false), children);
}

bool match_document::play(ev::begin_parse const &) {
  state = begun_parse;
  return true;
}

bool match_document::play(ev::down const &) { 
  if (state != begun_parse)
    return false;

  state = opened;
  matcher().add(sub);
  return true;
}

bool match_document::play(ev::up const &) {
  if (state != opened)
    return false;

  state = closed;
  return true;
}

bool match_document::play(ev::end_parse const &) {
  if (state == closed) {
    be_happy();
    return true;
  }

  return false;
}

bool match_document::play(ev::child_succeed const &) {
  return true;
}

gott::string match_document::name() const {
  return gott::string("tdl::schema::document");
}
