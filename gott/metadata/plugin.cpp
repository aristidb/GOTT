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
#include "interface.hpp"
#include "load.hpp"
#include "detail/validate.hpp"
#include "detail/database.hpp"
#include "detail/tables.hpp"
#include "detail/index.hpp"
#include <gott/exceptions.hpp>
#include <gott/auto.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/if.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/construct.hpp>
#include <boost/rtl/lambda_support.hpp>
#include <boost/rtl/invert.hpp>

using namespace gott::metadata;
using namespace gott::metadata_db;
using gott::QID;
using gott::string;

namespace {
template<class Ret, class T>
Ret get_attribute(handle_t const &handle, T const &attribute) {
  global_mutex::scoped_lock lock(get_global_lock());
  GOTT_AUTO(
      obj_sel,
      rtl::selection_eq(
        get_plugin_table(),
        rtl::row<mpl::vector1<plugin_handle> >(handle)));

  if (obj_sel.begin() == obj_sel.end() ||
      ++obj_sel.begin() != obj_sel.end())
    throw gott::internal_error(
        "metadata integrity: plugin query did not "
        "deliver exactly one item");

  GOTT_AUTO_CREF(obj, *obj_sel.begin());
  
  return obj[attribute];
}
}

QID plugin::plugin_id() const {
  return get_attribute<QID>(handle, metadata_db::plugin_id());
}

bool plugin::supports_interface(interface const &x) const {
  global_mutex::scoped_lock lock(get_global_lock());
  GOTT_AUTO(sel,
      rtl::selection_eq(
        get_plugin_interfaces_table(),
        plugin_interfaces_table_t::value_type(handle, x.get_handle())));
  return sel.begin() != sel.end();
}

void plugin::enumerate_supported_interfaces(
    boost::function<void (interface const &)> const &callback) const {
  global_mutex::scoped_lock lock(get_global_lock());
  GOTT_AUTO(sel,
      rtl::selection_eq(
        get_plugin_interfaces_table(),
        rtl::row<mpl::vector1<plugin_handle> >(handle)));
  GOTT_FOREACH_RANGE(it, sel)
    callback(interface(it.get(interface_handle())));
}

bool plugin::supports_interface_id(QID const &id) const {
  using namespace boost::lambda;
  bool value = false;
  enumerate_supported_interfaces(
      if_then(
        id == bind(&interface::interface_id, _1),
        var(value) = true));
  return value;
}

string plugin::symbol() const {
  return get_attribute<string>(handle, metadata_db::symbol());
}

plugin_priority plugin::priority() const {
  return get_attribute<plugin_priority>(handle, metadata_db::priority());
}

module plugin::enclosing_module() const {
  return module(get_attribute<handle_t>(handle, metadata_db::module_handle()));
}

bool plugin::is_valid() const {
  //TODO: do some caching
  return detail::validate(*this);
}

namespace {
bool enumerate_plugins_internal2(
    std::vector<plugin> const &plugins,
    boost::function<void (plugin const &)> const &function,
    bool cancel_early) {
  GOTT_FOREACH_RANGE(it, plugins) {
    function(*it);
    if (cancel_early)
      return true;
  }
  return false;
}

template<class T>
void enumerate_plugins_internal(
    T const &rel,
    boost::function<void (plugin const &)> const &function,
    bool cancel_early,
    bool validate) {
  std::vector<plugin> by_priority[INVALID_PRIORITY];
  GOTT_FOREACH_RANGE(it, rel) {
    plugin current(it.get(plugin_handle()));
    if (validate && !current.is_valid())
      continue;
    by_priority[current.priority()].push_back(current);
  }
  for (int i = 0; i < INVALID_PRIORITY; ++i)
    if (enumerate_plugins_internal2(by_priority[i], function, cancel_early))
      break;
}

bool check_interface(QID const &ifc, handle_t const &handle) {
  return plugin(handle).supports_interface_id(ifc);
}
}

void gott::metadata::enumerate_plugins_p(
    boost::function<void (plugin const &)> const &function,
    boost::optional<QID const &> const &the_plugin_id,
    boost::optional<QID const &> const &the_interface_id,
    bool cancel_early,
    bool do_load_standard_metadata,
    bool validate) {
  if (do_load_standard_metadata)
    load_standard();
  global_mutex::scoped_lock lock(get_global_lock());
  using namespace boost::lambda;
  if (the_plugin_id) {
    GOTT_AUTO(
        sel,
        rtl::selection_eq(
          get_plugin_by_id(),
          rtl::row<mpl::vector1<plugin_id> >(*the_plugin_id)));
    if (!the_interface_id)
      return enumerate_plugins_internal(sel, function, cancel_early, validate);
    return enumerate_plugins_internal(
        rtl::selection(
          rtl::join_eq<mpl::vector1<plugin_handle> >(
            sel,
            get_plugin_interfaces_table()),
          bind(
            &check_interface,
            *the_interface_id,
            _1[plugin_handle()])),
        function, cancel_early, validate);
  }
  if (the_interface_id) {
    GOTT_AUTO(
        interfaces,
        rtl::selection_eq(
          get_interface_by_id(),
          rtl::row<mpl::vector1<interface_id> >(*the_interface_id))
        );
    GOTT_AUTO(
        sel,
        rtl::join_eq<mpl::vector1<interface_handle> >(
          interfaces,
          get_plugin_with_interface()));
    return enumerate_plugins_internal(sel, function, cancel_early, validate);
  }
  GOTT_AUTO_CREF(sel, get_new_plugins());
  return enumerate_plugins_internal(sel, function, cancel_early, validate);
}

