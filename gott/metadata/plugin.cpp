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

#include "plugin.hpp"
#include "module.hpp"
#include "load.hpp"
#include "validate.hpp"
#include "database.hpp"
#include <gott/exceptions.hpp>
#include <gott/range_algo.hpp>
#include <boost/thread.hpp>
#include <map>
#include <boost/tuple/tuple.hpp>

using namespace gott::metadata;
using gott::QID;
using gott::string;

namespace {
  typedef boost::tuple<QID, QID, QID, string> desc_t;
  typedef std::map<handle_t, desc_t> 
    plugin_list_t;
  static plugin_list_t known_plugin;
  typedef std::multimap<gott::string, handle_t> res_map_t;
  static res_map_t resources;
}

QID plugin::plugin_id() const {
  return known_plugin[handle].get<0>();
}

QID plugin::supported_interface_id() const {
  return known_plugin[handle].get<1>();
}

bool plugin::supports_interface(QID const &x) const {
  return supported_interface_id() == x;
}

QID plugin::enclosing_module_id() const {
  return known_plugin[handle].get<2>();
}

string plugin::symbol() const {
  return known_plugin[handle].get<3>();
}

module plugin::enclosing_module(bool do_load_standard_metadata) const {
  boost::optional<module> res =
    find_module(
        enclosing_module_id(),
        tags::validate = false,
        tags::load_standard_metadata = do_load_standard_metadata);
  if (!res)
    throw system_error("module not found");
  return *res;
}

bool plugin::is_valid() const {
  return detail::validate(*this);
}

void gott::metadata::enumerate_plugins_p(
    boost::function<void (plugin const &)> const &function,
    boost::optional<QID const &> const &plugin_id,
    boost::optional<QID const &> const &interface_id,
    bool cancel_early,
    bool do_load_standard_metadata,
    bool validate) {
  if (do_load_standard_metadata)
    load_standard();
  metadata_db::global_mutex::scoped_lock lock(metadata_db::get_global_lock());
  plugin_list_t::const_iterator begin = known_plugin.begin();
  plugin_list_t::const_iterator end = known_plugin.end();
  for (plugin_list_t::const_iterator it = begin; it != end; ++it) {
    plugin current(it->first);
    if (plugin_id && current.plugin_id() != *plugin_id)
      continue;
    if (interface_id && current.supported_interface_id() != *interface_id)
      continue;
    if (validate && !current.is_valid())
      continue;
    function(current);
    if (cancel_early)
      break;
  }
}

void gott::metadata::detail::add_plugin(
    QID const &plugin_id,
    QID const &supported_interface,
    QID const &enclosing_module,
    string const &symbol,
    string const &resource) {
  handle_t h;
  known_plugin[h] = 
    desc_t(plugin_id, supported_interface, enclosing_module, symbol);
  resources.insert(std::make_pair(resource, h));
}

void gott::metadata::detail::remove_plugin_resource(string const &resource) {
  std::pair<res_map_t::iterator, res_map_t::iterator> rng
    = resources.equal_range(resource);
  for (res_map_t::iterator it = rng.first; it != rng.second; ++it)
    known_plugin.erase(it->second);
  resources.erase(rng.first, rng.second);
}
