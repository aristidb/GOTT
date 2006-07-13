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
 * The Original Code is A Plugin / Loadable Module Engine.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2006
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

#include "metadata_manager.hpp"
#include <gott/tdl/schema/match.hpp>
#include <gott/tdl/schema/rule.hpp>
#include <gott/tdl/schema/slot.hpp>
#include <gott/tdl/structure/revocable_adapter.hpp>
#include <gott/tdl/structure/print.hpp>//FIXME
#include <gott/tdl/structure/container.hpp>//FIXME
#include<iostream>//FIXME
#include <gott/tdl/structure/repatchers/enumeration.hpp>
#include <gott/tdl/exceptions.hpp>
#include <gott/string/qid.hpp>
#include <boost/assign/list_of.hpp>
#include <vector>

using std::vector;
using namespace tdl::schema;
using namespace tdl::structure;

void gott::plugin::detail::load_tdl_resource(
    metadata_manager &manager,
    std::istream &stream,
    string const &resource) {
  /*
  ordered
    named (plugin-id), node
    unordered
      :list named (has-interface), node
      named (enclosing-module), node
      named (symbol), node
      named (priority), enumeration $ low, normal, high
   */
  rule_t const plugin_schema =
    rule("tdl::schema::ordered", rule_attr(tag = "plugin", outer = list()),
        boost::assign::list_of
        (rule_one("tdl::schema::named", rule_attr(tag = "plugin-id"),
                  rule("tdl::schema::node")))
        (rule("tdl::schema::unordered", rule_attr(coat = false),
          boost::assign::list_of
          (rule_one("tdl::schema::named",
                    rule_attr(tag = "has-interface"),
                    rule("tdl::schema::node")))
          (rule_one("tdl::schema::named", rule_attr(tag = "enclosing-module"),
                    rule("tdl::schema::node")))
          (rule_one("tdl::schema::named", rule_attr(tag = "symbol"),
                    rule("tdl::schema::node")))
          (rule_one("tdl::schema::named",
                    rule_attr(tag = "priority", outer = optional()),
                    rule("tdl::schema::node",
                      rule_attr(
                        tdl::schema::repatcher = new repatch_enumeration(
                          boost::assign::list_of
                          (string("low"))
                          (string("normal"))
                          (string("high"))
                          .operator vector<string>()))))))));

  /*
  ordered
    named (module-id), node
    unordered
      named (file-path), node
      named (module-type), enumeration $ dynamic-native
      :list named (depend-on), node
   */
  rule_t const module_schema =
    rule("tdl::schema::ordered", rule_attr(tag = "module", outer = list()),
        boost::assign::list_of
        (rule_one("tdl::schema::named", rule_attr(tag = "module-id"),
                  rule("tdl::schema::node")))
        (rule("tdl::schema::unordered", rule_attr(coat = false),
          boost::assign::list_of
          (rule_one("tdl::schema::named", rule_attr(tag = "file-path"),
                    rule("tdl::schema::node")))
          (rule_one("tdl::schema::named", rule_attr(tag = "module-type"),
                    rule("tdl::schema::node", rule_attr(
                      tdl::schema::repatcher = new repatch_enumeration(
                        boost::assign::list_of
                        (string("dynamic-native"))
                        .operator std::vector<string>())))))
          (rule_one("tdl::schema::named",
                    rule_attr(tag = "depend-on", outer = list()),
                    rule("tdl::schema::node"))))));

  rule_t const document_schema =
    rule_one("tdl::schema::document", rule_attr(),
        rule("tdl::schema::unordered", rule_attr(coat = false),
          boost::assign::list_of(module_schema) (plugin_schema)));

  container out;
  revocable_adapter adapter(out);

  try {
    match(document_schema, adapter).parse(stream);
  } catch (tdl::tdl_error &) {
    throw gott::system_error("could not load resource " + resource);
  }

  std::cout << out << std::endl;
}
