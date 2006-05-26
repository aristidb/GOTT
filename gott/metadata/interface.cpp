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
 * The Original Code is Metadata for GOTT.
 *
 * The Initial Developer of the Original Code is Metadata for GOTT
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

#include "interface.hpp"
#include "database.hpp"
#include "load.hpp"
#include <gott/exceptions.hpp>

using gott::QID;
using namespace gott::metadata;
using namespace gott::metadata_db;

namespace {
template<class Ret, class T>
Ret get_attribute(handle_t const &handle, T const &attribute) {
  global_mutex::scoped_lock lock(get_global_lock());
  GOTT_AUTO(
      obj_sel,
      rtl::selection_eq(
        get_interface_table(),
        rtl::row<mpl::vector<interface_handle> >(handle)));

  if (obj_sel.begin() == obj_sel.end() ||
      ++obj_sel.begin() != obj_sel.end())
    throw gott::internal_error(
        "metadata integrity: interface query did not "
        "deliver exactly one item");

  GOTT_AUTO_CREF(obj, *obj_sel.begin());
  
  return obj[attribute];
}
}

QID interface::interface_id() const {
  return get_attribute<QID>(handle, metadata_db::interface_id());
}

namespace {
template<class T>
void enumerate_internal(
    T const &rel,
    boost::function<void (interface const &)> const &callback,
    bool cancel_early,
    bool validate) {
  for (typename T::const_iterator it = rel.begin(); it != rel.end(); ++it) {
    callback(interface(it.get(interface_handle())));
    if (cancel_early)
      break;
  }
}
}

void gott::metadata::enumerate_interfaces_p(
    boost::function<void (interface const &)> const &callback,
    boost::optional<QID const &> const &interface_id,
    bool cancel_early,
    bool load_standard_metadata,
    bool validate) {
  if (load_standard_metadata)
    load_standard();
  global_mutex::scoped_lock lock(get_global_lock());
  if (!interface_id)
    return enumerate_internal(
        get_interface_table(),
        callback,
        cancel_early,
        validate);
  return enumerate_internal(
      rtl::selection_eq(
        get_interface_by_id(),
        rtl::row<mpl::vector<metadata_db::interface_id> >(interface_id.get())),
      callback,
      cancel_early,
      validate);
}
