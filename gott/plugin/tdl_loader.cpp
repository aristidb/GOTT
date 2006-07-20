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
#include <gott/tdl/structure/repatchers/enumeration.hpp>
#include <gott/tdl/exceptions.hpp>
#include <gott/string/qid.hpp>
#include <boost/assign/list_of.hpp>
#include <vector>
#include <set>
#include <stack>

//#define VERBOSE

#ifdef VERBOSE
#include <gott/tdl/structure/container.hpp>
#include <gott/tdl/structure/print.hpp>
#include <iostream>
#endif

using std::vector;
using namespace tdl::schema;
using namespace tdl::structure;
using namespace gott::plugin;


namespace {
// this class makes a hell lot of assumptions:
// - there must not be more than one tag per level
// - the parser must work and find all invalid data
struct adapter : writable_structure {
  adapter(metadata_manager &manager, gott::string const &resource) 
  : manager(manager), resource(resource) {} 

  metadata_manager &manager;
  gott::string resource;

  gott::Xany Data;
  gott::string Tag;
  std::stack<gott::string> higher_tags;

  plugin_descriptor plugin_desc;
  plugin_information plugin_info;

  module_descriptor module_desc;
  module_information module_info;
  std::vector<gott::QID> module_dep;

  gott::QID feature_desc;
  std::vector<std::set<gott::QID> > feature_info;

  void begin(tdl::source_position const &) {
  }

  void end() {
    if (Tag == "$module") {
      manager.add_module(module_desc, module_info, module_dep, resource);
      module_desc = module_descriptor();
      module_info = module_information();
      module_dep.clear();
    } else if (Tag == "$plugin") {
      manager.add_plugin(plugin_desc, plugin_info, resource);
      plugin_desc = plugin_descriptor();
      plugin_info = plugin_information();
    } else if (Tag == "$deduced-feature") {
      manager.add_deduced_feature(feature_desc, feature_info, resource);
      feature_desc = gott::QID();
      feature_info.clear();
    } else if (Tag == "plugin")
      plugin_info.plugin_id = _str();
    else if (Tag == "enclosing-module")
      plugin_info.enclosing_module = _str();
    else if (Tag == "interface")
      plugin_info.interfaces.insert(_str());
    else if (Tag == "feature")
      plugin_info.features.insert(std::pair<gott::QID, bool>(_str(), false));
    else if (Tag == "priority")
      plugin_info.priority =
        gott::Xany_cast<plugin_information::priority_t>(Data);
    else if (Tag == "symbol")
      plugin_desc.symbol = _str();
    else if (Tag == "module")
      module_info.module_id = _str();
    else if (Tag == "depend-on")
      module_dep.push_back(_str());
    else if (Tag == "file-path")
      module_desc.file_path = _str();
    else if (Tag == "deduced-feature")
      feature_desc = _str();
    else if (Tag == "feature-set-item")
      feature_info.back().insert(_str());
    // TODO/missing:
    // - module-type
    if (!higher_tags.empty()) {
      Tag = higher_tags.top();
      higher_tags.pop();
    } else
      Tag = gott::string();
    Data = gott::Xany();
  }

  void data(gott::Xany const &x) {
    Data = x;
  }

  void add_tag(gott::string const &x) {
    higher_tags.push(Tag);
    Tag = x;
    if (Tag == "feature-set")
      feature_info.push_back(std::set<gott::QID>());
  }

  gott::string _str() const {
    return gott::Xany_cast<gott::string>(Data);
  }
};
}

void gott::plugin::detail::load_tdl_resource(
    metadata_manager &manager,
    std::istream &stream,
    string const &resource) {

  rule_t const inode = rule("tdl::schema::node", rule_attr(coat = false));
  
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
    rule("tdl::schema::ordered", rule_attr(tag = "$plugin", outer = list()),
        boost::assign::list_of
        (rule_one("tdl::schema::named", rule_attr(tag = "plugin"),
                  inode))
        (rule("tdl::schema::unordered", rule_attr(coat = false),
          boost::assign::list_of
          (rule_one("tdl::schema::named",
                    rule_attr(tag = "interface"),
                    inode))
          (rule_one("tdl::schema::named", rule_attr(tag = "enclosing-module"),
                    inode))
          (rule_one("tdl::schema::named", rule_attr(tag = "symbol"),
                    inode))
          (rule_one("tdl::schema::named",
                    rule_attr(tag = "priority", outer = optional()),
                    rule("tdl::schema::node",
                      rule_attr(
                        coat = false,
                        tdl::schema::repatcher = new repatch_enumeration(
                          boost::assign::list_of
                          (string("low"))
                          (string("normal"))
                          (string("high"))
                          .operator vector<string>())))))
          
          (rule_one("tdl::schema::named",
                    rule_attr(tag = "feature", outer = list()),
                    inode)))));

  /*
  ordered
    named (module-id), node
    unordered
      named (file-path), node
      named (module-type), enumeration $ dynamic-native
      :list named (depend-on), node
   */
  rule_t const module_schema =
    rule("tdl::schema::ordered", rule_attr(tag = "$module", outer = list()),
        boost::assign::list_of
        (rule_one("tdl::schema::named", rule_attr(tag = "module"),
                  inode))
        (rule("tdl::schema::unordered", rule_attr(coat = false),
          boost::assign::list_of
          (rule_one("tdl::schema::named", rule_attr(tag = "file-path"),
                    inode))
          (rule_one("tdl::schema::named", rule_attr(tag = "module-type"),
                    rule("tdl::schema::node", rule_attr(
                      coat = false,
                      tdl::schema::repatcher = new repatch_enumeration(
                        boost::assign::list_of
                        (string("dynamic-native"))
                        .operator std::vector<string>())))))
          (rule_one("tdl::schema::named",
                    rule_attr(tag = "depend-on", outer = list()),
                    inode)))));

  rule_t const deduced_feature_schema =
    rule("tdl::schema::ordered",
        rule_attr(tag = "$deduced-feature", outer = list()),
        boost::assign::list_of
        (rule_one("tdl::schema::named", rule_attr(tag = "deduced-feature"),
                  inode))
        (rule_one("tdl::schema::named",
                  rule_attr(
                    tag = "feature-set",
                    user = Xany("-"),
                    outer = list()),
                  rule_one("tdl::schema::list", rule_attr(coat = false),
                    rule("tdl::schema::node",
                      rule_attr(tag = "feature-set-item", outer = list()))))));

  rule_t const document_schema =
    rule_one("tdl::schema::document", rule_attr(coat = false),
        rule("tdl::schema::unordered", rule_attr(coat = false),
          boost::assign::list_of
          (module_schema)
          (plugin_schema)
          (deduced_feature_schema)));

  adapter out(manager, resource);
#ifndef VERBOSE
  revocable_adapter adapter(out);
#else
  container cont;
  revocable_adapter adapter(cont);
#endif

  try {
    match(document_schema, adapter).parse(stream);
#ifdef VERBOSE
    std::cout << resource << ":\n" << cont << std::endl;
    cont.copy_to(out);
#endif
  } catch (tdl::tdl_error &) {
    throw gott::system_error("could not load resource " + resource);
  }
}
