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

#include "module.hpp"
#include "load.hpp"
#include "validate.hpp"
#include "database.hpp"
#include "tables.hpp"
#include <gott/plugin/module.hpp>
#include <gott/exceptions.hpp>
#include <boost/thread.hpp>
#include <boost/weak_ptr.hpp>
#include <vector>
#include <map>

using namespace gott::metadata;
using namespace gott;

namespace {
  typedef
    boost::tuple<
      QID,
      module::module_type_t,
      string,
      std::vector<QID>
    >
    desc_t;
  typedef std::map<handle_t, desc_t> module_list_t;
  static module_list_t known_module;
  typedef std::multimap<string, handle_t> res_map_t;
  static res_map_t resources;
}

QID const &module::module_id() const {
  return known_module[handle].get<0>();
}

module::module_type_t module::module_type() const {
  return known_module[handle].get<1>();
}

string const &module::file_path() const {
  return known_module[handle].get<2>();
}

void module::enumerate_dependencies(
    boost::function<void (module const &)> const &fun) const {
  std::vector<QID> const &v = known_module[handle].get<3>();
  for (std::vector<QID>::const_iterator it = v.begin(); it != v.end(); ++it) {
    boost::optional<module> mod = find_module(*it);
    if (!mod)
      throw gott::system_error("module dependency not found");
    fun(*mod);
  }
}

bool module::is_valid() const {
  return detail::validate(*this);
}

boost::shared_ptr<gott::plugin::module> module::get_instance() const {
  boost::shared_ptr<gott::plugin::module> result(
      new gott::plugin::module(*this));
  return result;
}

void gott::metadata::enumerate_modules_p(
    boost::function<void (module const &)> const &callback,
    boost::optional<QID const &> const &module_id,
    bool cancel_early,
    bool do_load_standard_metadata,
    bool validate) {
  if (do_load_standard_metadata)
    load_standard();
  global_mutex::scoped_lock lock(get_global_lock());
  module_list_t::const_iterator begin = known_module.begin();
  module_list_t::const_iterator end = known_module.end();
  for (module_list_t::const_iterator it = begin; it != end; ++it) {
    module current(it->first);
    if (module_id && current.module_id() != *module_id)
      continue;
    if (validate && !current.is_valid())
      continue;
    callback(current);
    if (cancel_early)
      return;
  }
}

void gott::metadata::detail::add_module(
    QID const &module_id,
    module::module_type_t module_type,
    string const &file_path,
    std::vector<QID> const &dependencies,
    string const &resource) {
  handle_t h;
  known_module[h] = desc_t(
        module_id, module_type, file_path, dependencies);
  resources.insert(std::make_pair(resource, h));
}

void gott::metadata::detail::remove_module_resource(string const &resource) {
  std::pair<res_map_t::iterator, res_map_t::iterator> rng
    = resources.equal_range(resource);
  for (res_map_t::iterator it = rng.first; it != rng.second; ++it)
    known_module.erase(it->second);
  resources.erase(rng.first, rng.second);
}
