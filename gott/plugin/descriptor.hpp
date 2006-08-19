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

#ifndef GOTT_PLUGIN_DESCRIPTOR_HPP
#define GOTT_PLUGIN_DESCRIPTOR_HPP

#include <gott/string/string.hpp>
#include <gott/string/qid.hpp>
#include <set>
#include <map>
#include <vector>

namespace gott { namespace plugin {

/**
 * A descriptor for modules. Uniquely identifies modules on the system.
 */
struct module_descriptor {
  /**
   * Default constructor. Creates an invalid descriptor.
   */
  module_descriptor() {}

  /**
   * Constructor.
   * \param file_path The path of the module file.
   */
  explicit module_descriptor(string const &file_path)
    : file_path(file_path) {}

  /// The path of the module file.
  string file_path;

  string to_string() const {
    return "module in " + file_path;
  }
};

/**
 * Check if two module_descriptors are equal.
 * \param l A module_descriptor to compare.
 * \param r The module_descriptor to compare with.
 */
inline bool operator==(module_descriptor const &l, module_descriptor const &r) {
  return l.file_path == r.file_path;
}

/**
 * A descriptor for plugins. Uniquely identifies plugins on the system.
 */
struct plugin_descriptor {
  /**
   * Default constructor. Creates an invalid descriptor.
   */
  plugin_descriptor() {}

  /**
   * Constructor.
   * \param symbol The symbol identifying the plugin inside its module.
   * \param mod The descriptor of the plugin's module.
   */
  explicit plugin_descriptor(string const &sym, module_descriptor const &mod)
    : symbol(sym), enclosing_module(mod) {}

  /// The symbol identifying the plugin inside its module.
  string symbol;

  /// The descriptor of the plugin's module.
  module_descriptor enclosing_module;

  string to_string() const {
    return "plugin with symbol " + symbol + 
      " in " + enclosing_module.to_string();
  }
};

/**
 * Check if two plugin_descriptors are equal.
 * \param l A plugin_descriptor to compare.
 * \param r The plugin_descriptor to compare with.
 */
inline bool operator==(plugin_descriptor const &l, plugin_descriptor const &r) {
  return l.symbol == r.symbol && l.enclosing_module == r.enclosing_module;
}

/**
 * Extra information for plugins.
 */
struct plugin_information {
  /// The priority of a plugin.
  enum priority_t { 
    /// Low priority.
    low,
    /// Normal priority.
    normal,
    /// High priority.
    high,
    /// A number 1 higher than the highest priority.
    N_PRIORITY };

  /// Default constructor.
  plugin_information() : priority(normal) {}

  /**
   * Constructor.
   * \param plugin_id The ID of the plugin.
   * \param enclosing_module The ID of the plugin's module.
   * \param interfaces The IDs of the plugin's supported interfaces.
   * \param features The IDs of the plugin's supported features.
   * \param priority The plugin's priority.
   */
  explicit plugin_information(
      QID const &plugin_id,
      QID const &enclosing_module,
      std::set<QID> const &interfaces,
      std::map<QID, bool> const &features,
      priority_t priority)
    : plugin_id(plugin_id),
    enclosing_module(enclosing_module),
    interfaces(interfaces),
    features(features),
    priority(priority) {}

  /// The ID of the plugin.
  QID plugin_id;

  /// The ID of the plugin's module.
  QID enclosing_module;

  ///  The IDs of the plugin's supported interfaces.
  std::set<QID> interfaces;

  /**
   * The IDs of the plugin's supported features.
   * The associated boolean indicates whether this is a native
   * or deduced feature.
   */
  std::map<QID, bool> features;

  /// The priority of the plugin.
  priority_t priority;
};

template<class T, class U>
bool feature_includes(T map1, T map2, U set1, U set2) {
  while (map1 != map2 && set1 != set2)
    if (*set1 < map1->first)
      return false;
    else if(map1->first < *set1)
      ++map1;
    else
      ++map1, ++set1;

  return set1 == set2;
}

/**
 * Extra information for modules.
 */
struct module_information {
  /// Default constructor.
  module_information() {}

  /**
   * Constructor.
   * \param module_id The ID of the module.
   */
  explicit module_information(
      QID const &module_id,
      std::vector<module_descriptor> const &dependencies =
        std::vector<module_descriptor>())
    : module_id(module_id),
    dependencies(dependencies) {}

  /// The ID of the module.
  QID module_id;

  /// The dependencies of the module.
  std::vector<module_descriptor> dependencies;
};

}}

#endif
