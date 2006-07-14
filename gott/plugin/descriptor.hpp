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

namespace gott { namespace plugin {

struct module_descriptor {
  module_descriptor() {}

  explicit module_descriptor(string const &file_path)
    : file_path(file_path) {}
  string file_path;
};

inline bool operator==(module_descriptor const &l, module_descriptor const &r) {
  return l.file_path == r.file_path;
}

struct plugin_descriptor {
  plugin_descriptor() {}

  explicit plugin_descriptor(string const &sym, module_descriptor const &mod)
    : symbol(sym), enclosing_module(mod) {}
  string symbol;
  module_descriptor enclosing_module;
};

inline bool operator==(plugin_descriptor const &l, plugin_descriptor const &r) {
  return l.symbol == r.symbol && l.enclosing_module == r.enclosing_module;
}

struct plugin_information {
  enum priority_t { low, normal, high, N_PRIORITY };

  plugin_information() : priority(normal) {}

  explicit plugin_information(
      QID const &plugin_id,
      QID const &enclosing_module,
      std::set<QID> const &interfaces,
      std::set<QID> const &features,
      priority_t priority)
    : plugin_id(plugin_id),
    enclosing_module(enclosing_module),
    interfaces(interfaces),
    features(features),
    priority(priority) {}

  QID plugin_id;
  QID enclosing_module;
  std::set<QID> interfaces;
  std::set<QID> features;
  priority_t priority;
};

struct module_information {
  module_information() {}

  explicit module_information(QID const &module_id)
    : module_id(module_id) {}

  QID module_id;
};

}}

#endif
