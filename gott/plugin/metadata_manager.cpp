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
#include "descriptor.hpp"
#include <gott/exceptions.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/assign/list_of.hpp>
#include <vector>
#include <set>
#include <fstream>

using namespace gott::plugin;
using gott::string;
using std::vector;

namespace {
  boost::recursive_mutex mutex;

  struct whole_plugin {
    whole_plugin() {}

    whole_plugin(
        plugin_descriptor const &descriptor,
        plugin_information const &information,
        string const &resource)
      : descriptor(descriptor),
      information(information),
      resource(resource) {}

    plugin_descriptor descriptor;
    plugin_information information;
    string resource;
  };

  struct whole_module {
    whole_module() {}

    whole_module(
        module_descriptor const &descriptor,
        module_information const &information,
        string const &resource)
      : descriptor(descriptor),
      information(information),
      resource(resource) {}

    module_descriptor descriptor;
    module_information information;
    string resource;
  };

  //TODO safer, faster implementation
  vector<whole_plugin> plugins;
  vector<whole_module> modules;

  bool allow_recur = false;
  unsigned long recursion = 0;

  class scoped_allow_recur {
  public:
    scoped_allow_recur() : virgin(!allow_recur) { allow_recur = true; }
    ~scoped_allow_recur() { if (virgin) allow_recur = false; }
  private:
    bool virgin;
  };

#define ALLOW_RECUR scoped_allow_recur MAY_RECUR; (void)MAY_RECUR
}

class metadata_manager::impl {
public:
  impl() : lock(mutex) {}
  boost::recursive_mutex::scoped_lock lock;

  std::set<string> remove_resources;
  vector<plugin_descriptor> remove_plugins;
  vector<module_descriptor> remove_modules;
  vector<whole_plugin> add_plugins;
  vector<whole_module> add_modules;
};

metadata_manager::metadata_manager() : p(new impl) {
  if (!allow_recur && recursion > 0)
    throw gott::internal_error("multiple metadata_manager objects");
  ++recursion;
}
metadata_manager::~metadata_manager() { --recursion; }

void metadata_manager::commit() {
  for (vector<whole_plugin>::iterator it = plugins.begin();
      it != plugins.end();)
    if (p->remove_resources.count(it->resource) > 0)
      it = plugins.erase(it);
    else
      ++it;

  for (vector<whole_module>::iterator it = modules.begin();
      it != modules.end();)
    if (p->remove_resources.count(it->resource) > 0)
      it = modules.erase(it);
    else
      ++it;

  for (vector<module_descriptor>::iterator it = p->remove_modules.begin();
      it != p->remove_modules.end();
      ++it)
    for (vector<whole_module>::iterator jt = modules.begin();
        jt != modules.end();)
      if (*it == jt->descriptor) {
        for (vector<whole_plugin>::iterator yt = plugins.begin();
            yt != plugins.end();)
          if (yt->descriptor.enclosing_module == *it)
            yt = plugins.erase(yt);
          else
            ++yt;
        jt = modules.erase(jt);
      } else
        ++jt;

  for (vector<plugin_descriptor>::iterator it = p->remove_plugins.begin();
      it != p->remove_plugins.end();
      ++it)
    for (vector<whole_plugin>::iterator jt = plugins.begin();
        jt != plugins.end();)
      if (*it == jt->descriptor)
        jt = plugins.erase(jt);
      else
        ++jt;

  modules.insert(modules.end(), p->add_modules.begin(), p->add_modules.end());

  for (vector<whole_plugin>::iterator it = p->add_plugins.begin();
      it != p->add_plugins.end();
      ++it)
    for (vector<whole_module>::iterator jt = modules.begin();
        jt != modules.end();
        ++jt)
      if (jt->information.module_id == it->information.enclosing_module) {
        it->descriptor.enclosing_module = jt->descriptor;
        break;
      }
  
  plugins.insert(plugins.end(), p->add_plugins.begin(), p->add_plugins.end());

  p->remove_resources.clear();
  p->remove_plugins.clear();
  p->remove_modules.clear();
  p->add_plugins.clear();
  p->add_modules.clear();
}

void metadata_manager::remove_resource(string const &resource) {
  p->remove_resources.insert(resource);
}

void metadata_manager::update_resource(std::istream &s, string const &res) {
  ALLOW_RECUR;
  remove_resource(res);
  detail::load_tdl_resource(*this, s, res);
}

void metadata_manager::load_core() {
  ALLOW_RECUR;
  static bool loaded;

  if (!loaded) {
    module_descriptor tdl_builtins("tdl/libtdl_builtins.so");
    add_module(tdl_builtins, module_information("tdl::builtins"), "core");

    add_core_tdl_schema("any", tdl_builtins);
    add_core_tdl_schema("document", tdl_builtins);
    add_core_tdl_schema("follow", tdl_builtins);
    add_core_tdl_schema("list", tdl_builtins);
    add_core_tdl_schema("named", tdl_builtins);
    add_core_tdl_schema("ordered", tdl_builtins);
    add_core_tdl_schema("node", tdl_builtins);
    add_core_tdl_schema("unordered", tdl_builtins);
    add_core_tdl_schema("tree", tdl_builtins);
    loaded = true;
  }
}

void metadata_manager::add_core_tdl_schema(
    string const &type, module_descriptor const &tdl_builtins) {
  std::set<QID> interfaces;
  interfaces.insert("tdl::schema::type");

  std::set<QID> features;

  add_plugin(
      plugin_descriptor("plugin_schema_" + type, tdl_builtins),
      plugin_information(
        "tdl::schema::" + type,
        "tdl::builtins",
        interfaces,
        features,
        plugin_information::normal
        ),
      "core");
}

void metadata_manager::load_standard() {
  ALLOW_RECUR;

  metadata_manager man2;
  man2.load_core();
  man2.commit();

  static bool loaded;
  if (!loaded) {
    loaded = true;
    {
      std::ifstream plugins("./plugin_registry.tdl");
      update_resource(plugins, "./plugin_registry.tdl");
    }
    {
      std::ifstream modules("./module_registry.tdl");
      update_resource(modules, "./module_registry.tdl");
    }
  }
}

bool metadata_manager::enum_plugins(
    boost::function<
      bool (
        plugin_descriptor const &plugin_descriptor,
        plugin_information const &info
      ) > const &callback,
      boost::optional<QID> const &/*plugin_id*/,
      boost::optional<QID> const &/*interface_id*/,
      std::set<QID> const &/*features*/,
      plugin_information::priority_t prio) const {
  for (vector<whole_plugin>::iterator it = plugins.begin();
      it != plugins.end();
      ++it) {
    if (it->information.priority == prio)
      continue;
    if (!callback(it->descriptor, it->information))
      return true;
  }
  return false;
}

bool metadata_manager::enum_modules(
    boost::function<
      bool (
        module_descriptor const &descriptor,
        module_information const &info
      )
    > const &callback,
    boost::optional<QID> const &/*module_id*/) const {
  for (vector<whole_module>::iterator it = modules.begin();
      it != modules.end();
      ++it)
    if (!callback(it->descriptor, it->information))
      return true;
  return false;
}

void metadata_manager::remove_plugin(plugin_descriptor const &descriptor) {
  p->remove_plugins.push_back(descriptor);
}

void metadata_manager::remove_module(module_descriptor const &descriptor) {
  p->remove_modules.push_back(descriptor);
}

void metadata_manager::add_plugin(
    plugin_descriptor const &desc,
    plugin_information const &info,
    string const &resource) {
  p->add_plugins.push_back(whole_plugin(desc, info, resource));
}

void metadata_manager::add_module(
    module_descriptor const &desc,
    module_information const &info,
    string const &resource) {
  p->add_modules.push_back(whole_module(desc, info, resource));
}
