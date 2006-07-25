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
#include <map>
#include <fstream>
#include <algorithm>

using namespace gott::plugin;
using gott::string;
using gott::QID;
using std::vector;
using std::map;
using std::set;

namespace {
  // protect all data
  boost::recursive_mutex mutex;

  // need to store plugin_descriptor, plugin_information and the resource
  // for each plugin
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

  // need to store module_descriptor, module_information and the resource
  // for each module
  struct whole_module {
    whole_module() {}

    whole_module(
        module_descriptor const &descriptor,
        module_information const &information,
        string const &resource,
        vector<QID> const &raw_dependencies = vector<QID>())
      : descriptor(descriptor),
      information(information),
      resource(resource),
      raw_dependencies(raw_dependencies) {}

    module_descriptor descriptor;
    module_information information;
    string resource;
    vector<QID> raw_dependencies;
  };

  // just store all plugins and modules
  //TODO safer, faster implementation
  vector<whole_plugin> plugins;
  vector<whole_module> modules;

  // some features are artificial... keep track
  struct deduced_feature {
    typedef vector<set<QID> > fsets_t;

    deduced_feature(
        QID const &descriptor,
        fsets_t const &feature_sets,
        string const &resource)
      : descriptor(descriptor),
      feature_sets(feature_sets),
      resource(resource) {}
    
    QID descriptor;
    fsets_t feature_sets;
    string resource;
  };
  vector<deduced_feature> deduced_features;
}

class metadata_manager::impl {
public:
  impl() : lock(mutex) {}
  boost::recursive_mutex::scoped_lock lock;

  // task list for next commit
  set<string> remove_resources;
  vector<plugin_descriptor> remove_plugins;
  vector<module_descriptor> remove_modules;
  vector<whole_plugin> add_plugins;
  vector<whole_module> add_modules;
  vector<deduced_feature> add_features;
};

metadata_manager::metadata_manager() : p(new impl) {}
metadata_manager::~metadata_manager() {}

void metadata_manager::commit() {
  // removing resources:
  // - removing plugins from removed resources
  for (vector<whole_plugin>::iterator it = plugins.begin();
      it != plugins.end();)
    if (p->remove_resources.count(it->resource) > 0)
      it = plugins.erase(it);
    else
      ++it;

  // - removing modules from removed resources
  for (vector<whole_module>::iterator it = modules.begin();
      it != modules.end();)
    if (p->remove_resources.count(it->resource) > 0)
      it = modules.erase(it);
    else
      ++it;

  // - removing features from removed resources
  for (vector<deduced_feature>::iterator it = deduced_features.begin();
      it != deduced_features.end();)
    if (p->remove_resources.count(it->resource) > 0)
      it = deduced_features.erase(it);
    else
      ++it;

  // directly removing modules
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

  // directly removing plugins
  for (vector<plugin_descriptor>::iterator it = p->remove_plugins.begin();
      it != p->remove_plugins.end();
      ++it)
    for (vector<whole_plugin>::iterator jt = plugins.begin();
        jt != plugins.end();)
      if (*it == jt->descriptor)
        jt = plugins.erase(jt);
      else
        ++jt;

  // adding new features
  deduced_features.insert(deduced_features.end(), p->add_features.begin(),
      p->add_features.end());

  // adding new modules
  modules.insert(modules.end(), p->add_modules.begin(), p->add_modules.end());

  // patching modules' dependencies
  for (vector<whole_module>::iterator it = modules.begin();
      it != modules.end();
      ++it) {
    for (vector<QID>::iterator jt = it->raw_dependencies.begin();
        jt != it->raw_dependencies.end();
        ++jt)
      for (vector<whole_module>::iterator xt = modules.begin();
          xt != modules.end();
          ++xt)
        if (*jt == xt->information.module_id) {
          it->information.dependencies.push_back(xt->descriptor);
          break;
        }
    vector<QID>().swap(it->raw_dependencies);
  }

  // patching new plugins' module_descriptor entries
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
  
  // adding new and patched plugins
  plugins.insert(plugins.end(), p->add_plugins.begin(), p->add_plugins.end());

  // remove all deduced features from all plugins
  for (vector<whole_plugin>::iterator it = plugins.begin(); it != plugins.end();
      ++it) {
    map<QID, bool> &features = it->information.features;
    map<QID, bool>::iterator jt = features.begin();
    while (jt != features.end())
      if (jt->second)
        features.erase(jt++);
      else
        ++jt;
  }

  // now add deduced features to all plugins
  bool done_something = true;
  while (done_something) {
    done_something = false;
    for (vector<deduced_feature>::iterator it = deduced_features.begin();
        it != deduced_features.end();
        ++it)
      for (vector<whole_plugin>::iterator xt = plugins.begin();
          xt != plugins.end();
          ++xt) {
        if (xt->information.features.count(it->descriptor) > 0)
          continue;
        for (vector<set<QID> >::iterator jt = it->feature_sets.begin();
            jt != it->feature_sets.end();
            ++jt)
          if (feature_includes(
                xt->information.features.begin(),
                xt->information.features.end(),
                jt->begin(), jt->end())) {
            xt->information.features.insert(
                map<QID, bool>::value_type(it->descriptor, true));
            done_something = true;
          }
      }
  }

  // clear task lists
  p->remove_resources.clear();
  p->remove_plugins.clear();
  p->remove_modules.clear();
  p->add_plugins.clear();
  p->add_modules.clear();
  p->add_features.clear();
}

void metadata_manager::remove_resource(string const &resource) {
  p->remove_resources.insert(resource);
}

void metadata_manager::update_resource(std::istream &s, string const &res) {
  remove_resource(res);
  detail::load_tdl_resource(*this, s, res);
}

void metadata_manager::load_core() {
  static bool loaded;

  if (!loaded) {
    // just add the metadata necessary for reading metadata from files
    module_descriptor tdl_builtins("tdl/libtdl_builtins.plugin");
    add_module(
        tdl_builtins,
        module_information("tdl::builtins"),
        vector<QID>(),
        "core");

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
  set<QID> interfaces;
  interfaces.insert("tdl::schema::type");

  map<QID, bool> features;

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
  // ensure we CAN read the metadata from files
  metadata_manager man2;
  man2.load_core();
  man2.commit();

  static bool loaded;
  if (!loaded) {
    loaded = true;
    // now read them
    {
      std::ifstream plugins("./plugin_registry.tdl");
      update_resource(plugins, "./plugin_registry.tdl");
    }
    {
      std::ifstream modules("./module_registry.tdl");
      update_resource(modules, "./module_registry.tdl");
    }
    {
      std::ifstream deduced_features("./deduced_features.tdl");
      update_resource(deduced_features, "./deduced_features.tdl");
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
      set<QID> const &/*features*/,
      plugin_information::priority_t prio) const {
  // send all (yeah that's ok) plugins with the correct priority to the callback
  // - until the callback returns false
  for (vector<whole_plugin>::iterator it = plugins.begin();
      it != plugins.end();
      ++it) {
    if (it->information.priority != prio)
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
  // send all (still ok) modules to the callback - until the callback returns
  // false
  for (vector<whole_module>::iterator it = modules.begin();
      it != modules.end();
      ++it)
    if (!callback(it->descriptor, it->information))
      return true;
  return false;
}

module_information const &metadata_manager::module_extra(
    module_descriptor const &descriptor) const {
  for (vector<whole_module>::iterator it = modules.begin(); it != modules.end();
      ++it)
    if (it->descriptor == descriptor)
      return it->information;
  throw gott::system_error("module information not found");
}

plugin_information const &metadata_manager::plugin_extra(
    plugin_descriptor const &descriptor) const {
  for (vector<whole_plugin>::iterator it = plugins.begin(); it != plugins.end();
      ++it)
    if (it->descriptor == descriptor)
      return it->information;
  throw gott::system_error("module information not found");
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
    vector<QID> const &dependencies,
    string const &resource) {
  p->add_modules.push_back(whole_module(desc, info, resource, dependencies));
}

void metadata_manager::add_deduced_feature(
    QID const &desc,
    vector<set<QID> > const &info,
    string const &resource) {
  p->add_features.push_back(deduced_feature(desc, info, resource));
}

