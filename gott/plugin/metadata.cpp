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

#include "metadata.hpp"
#include <gott/metadata/plugin.hpp>
#include <gott/metadata/module.hpp>
#include "module.hpp"
#include <gott/syswrap/function_cast.hpp>
#include <gott/exceptions.hpp>
#include <boost/thread/once.hpp>
#include <fstream>

using namespace gott::plugin;

namespace {
boost::once_flag once_standard = BOOST_ONCE_INIT;
boost::once_flag once_core = BOOST_ONCE_INIT;

void noop() {}

void do_load_standard() {
  load_core_metadata();

  disable_plugin_metadata();
  {
    std::ifstream in("plugin_registry.tdl");
    extract_plugin_metadata(in);
  }
  enable_plugin_metadata();

  disable_module_metadata();
  {
    std::ifstream in("module_registry.tdl");
    extract_module_metadata(in);
  }
  enable_module_metadata();
}

void do_load_core() {
  // Load metadata for TDL, necessary for reading in the metadata.
  // Yes, this is what they call "boot-strapping".
  {
    module_metadata builtins;
    builtins.module_id = "tdl::builtins";
    builtins.file_path = "tdl/libtdl_builtins.so";
    add_module_metadata(builtins, true);
  }
  struct schema_type_adder {
    void operator()(gott::string const &s) {
      plugin_metadata schema_type;
      schema_type.plugin_id = "tdl::schema::" + s;
      schema_type.interfaces.push_back("tdl::schema::type");
      schema_type.enclosing_module = "tdl::builtins";
      schema_type.symbol = "plugin_schema_" + s;
      add_plugin_metadata(schema_type, true);
    }
  } add_schema_type;
  add_schema_type("any");
  add_schema_type("document");
  add_schema_type("follow");
  add_schema_type("list");
  add_schema_type("named");
  add_schema_type("ordered");
  add_schema_type("unordered");
  add_schema_type("tree");
  add_schema_type("node");
  // The TDL metadata loader
  {
    module_metadata loader;
    loader.module_id = "gott::plugin::tdl_metadata";
    loader.file_path = "plugin/libplugin_tdl.so";
    loader.dependencies.push_back("tdl::library");
    add_module_metadata(loader, true);
  }
}
}

void gott::plugin::load_standard_metadata() {
  static bool first = true; // protect against recursion
  if (!first)
    return;
  first = false;
  boost::call_once(&do_load_standard, once_standard);
}

void gott::plugin::load_core_metadata() {
  boost::call_once(&do_load_core, once_core);
}
