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

#include "list.hpp"
#include "../rule_attr.hpp"
#include "../type.hpp"
#include <gott/plugin/plugin_builder.hpp>
#include <cassert>

namespace schema = tdl::schema;
namespace ev = tdl::schema::ev;
using schema::item;
using schema::slotcfg;
using schema::match_list;

GOTT_PLUGIN_MAKE_BUILDER_SIMPLE(
    plugin_schema_list,
    schema::concrete_type<match_list>)

gott::atom const match_list::id("list");

match_list::match_list(rule_attr_t const &a, std::vector<rule_t> const &r, match &m)
: item(a, m), sub(r[0]), cfg(r[0].attributes().outer()), cancelled(false) {
  assert(r.size() == 1);
  last = m.pos().current();
  if (accept_more(expectation()))
    matcher().add(sub);
}

match_list::~match_list() {
  if (expectation() != nothing || cancelled)
    matcher().pos().forget(last);
}

bool match_list::play(ev::child_fail const &) {
  matcher().pos().seek(last);
  return true;
}

bool match_list::play(ev::child_succeed const &) {
  if (matcher().pos().proceeded(last))
    return full();
  else
    return empty();
}

bool match_list::full() {
  cfg.add();

  matcher().pos().forget(last);
  if (expectation() != nothing) {
    last = matcher().pos().current();
    matcher().add(sub);
  }

  return true;
}

bool match_list::empty() {
  matcher().pos().seek(last);
  if (expectation() != need) {
    cfg.cancel();
    cancelled = true;
  }

  return true;
}

item::expect match_list::expectation() const {
  return cfg.expectation();
}

gott::string match_list::name() const {
  return gott::string("list");
}

bool match_list::accept_empty(rule_attr_t const &, std::vector<rule_t> const &c) {
  return c[0].attributes().outer().prefix_optional();
}
