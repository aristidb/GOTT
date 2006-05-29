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
#include "detail/validate.hpp"
#include "detail/database.hpp"
#include "detail/tables.hpp"
#include "detail/index.hpp"
#include <gott/plugin/module.hpp>
#include <gott/exceptions.hpp>

using namespace gott::metadata;
using namespace gott::metadata_db;
using namespace gott;

namespace {
template<class Ret, class T>
Ret get_attribute(handle_t const &handle, T const &attribute) {
  global_mutex::scoped_lock lock(get_global_lock());
  GOTT_AUTO(
      obj_sel,
      rtl::selection_eq(
        get_module_table(),
        rtl::row<mpl::vector1<module_handle> >(handle)));

  if (obj_sel.begin() == obj_sel.end() ||
      ++obj_sel.begin() != obj_sel.end())
    throw gott::internal_error(
        "metadata integrity: module query did not "
        "deliver exactly one item");

  GOTT_AUTO_CREF(obj, *obj_sel.begin());
  
  return obj[attribute];
}
}

QID module::module_id() const {
  return get_attribute<QID>(handle, metadata_db::module_id());
}

module::module_type_t module::module_type() const {
  return get_attribute<module_type_t>(handle, metadata_db::module_type());
}

string module::file_path() const {
  return get_attribute<string>(handle, metadata_db::file_path());
}

void module::enumerate_dependencies(
    boost::function<void (module const &)> const &fun) const {
  global_mutex::scoped_lock lock(get_global_lock());
  GOTT_AUTO(deps,
      rtl::projection<mpl::vector1<rtl::alias<module_handle> > >(
        rtl::selection_eq(
          get_module_dependencies_table(),
          rtl::row<mpl::vector1<module_handle> >(handle))));
  GOTT_FOREACH_RANGE(it, deps)
    fun(module(it.get(rtl::alias<module_handle>())));
}

bool module::is_valid() const {
  return detail::validate(*this);
}

boost::shared_ptr<gott::plugin::module> module::get_instance() const {
  boost::shared_ptr<gott::plugin::module> result(
      new gott::plugin::module(*this));
  return result;
}

namespace {
template<class T>
void enumerate_modules_internal(
    T const &rel,
    boost::function<void (module const &)> const &callback,
    bool cancel_early,
    bool validate) {
  GOTT_FOREACH_RANGE(it, rel) {
    if (it.get(obsolete()))
      continue;
    module current(it.get(module_handle()));
    if (validate && !current.is_valid())
      continue;
    callback(current);
    if (cancel_early)
      return;
  }
}
}

void gott::metadata::enumerate_modules_p(
    boost::function<void (module const &)> const &callback,
    boost::optional<QID const &> const &module_id,
    bool cancel_early,
    bool do_load_standard_metadata,
    bool validate) {
  if (do_load_standard_metadata)
    load_standard();
  metadata_db::global_mutex::scoped_lock lock(metadata_db::get_global_lock());
  if (module_id)
    return enumerate_modules_internal(
        rtl::selection_eq(
          get_module_by_id(),
          rtl::row<mpl::vector1<metadata_db::module_id> >(*module_id)),
        callback,
        cancel_early,
        validate);
  return enumerate_modules_internal(
      get_module_table(),
      callback,
      cancel_early,
      validate);
}

