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

#include "load.hpp"
#include "transaction.hpp"
#include "plugin.hpp"
#include "module.hpp"
#include <gott/plugin/module.hpp>
#include <gott/syswrap/function_cast.hpp>
#include <gott/exceptions.hpp>
#include <boost/thread/once.hpp>
#include <fstream>

using namespace gott::metadata;

namespace {
boost::once_flag once_standard = BOOST_ONCE_INIT;
boost::once_flag once_core = BOOST_ONCE_INIT;

void do_load_standard() {
  load_core();

  transaction tr;
  tr.remove_resource("core");
  {
    std::ifstream in("./plugin_registry.tdl");
    tr.update_resource(in, "./plugin_registry.tdl", plugin_resource);
  }

  {
    std::ifstream in("./module_registry.tdl");
    tr.update_resource(in, "./module_registry.tdl", module_resource);
  }

  {
    std::ifstream in("./interface_registry.tdl");
    tr.update_resource(in, "./interface_registry.tdl", interface_resource);
  }
  tr.commit();
}

void do_load_core() {
  // Load metadata for TDL, necessary for reading in the metadata.
  // Yes, this is what they call "boot-strapping".
  transaction tr;
  tr.add_module(
        "tdl::builtins",
        module::dynamic_native,
        "tdl/libtdl_builtins.so",
        std::vector<gott::QID>(),
        "core");
  struct schema_type_adder {
    void operator()(gott::string const &s, transaction &tr) {
      tr.add_plugin(
          "tdl::schema::" + s,
          std::vector<gott::QID>(1, "tdl::schema::type"),
          "tdl::builtins",
          "plugin_schema_" + s,
          normal_priority,
          "core");
    }
  } add_schema_type;
  add_schema_type("any", tr);
  add_schema_type("document", tr);
  add_schema_type("follow", tr);
  add_schema_type("list", tr);
  add_schema_type("named", tr);
  add_schema_type("ordered", tr);
  add_schema_type("unordered", tr);
  add_schema_type("tree", tr);
  add_schema_type("node", tr);
  tr.add_interface("tdl::schema::type", "core");
  tr.commit();
}
}

void gott::metadata::load_standard() {
  static bool first = true; // protect against recursion
  if (!first)
    return;
  first = false;
  boost::call_once(&do_load_standard, once_standard);
}

void gott::metadata::load_core() {
  boost::call_once(&do_load_core, once_core);
}
