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

#ifndef GOTT_METADATA_INDEX_HPP
#define GOTT_METADATA_INDEX_HPP

#include "tables.hpp"

namespace gott { namespace metadata_db {

struct check_new_t {
  template<class T>
  bool operator() (T const &x) const {
    return !x[obsolete()];
  }
};

// Index tables by filtering obsolete

typedef
  rtl::selection_t<
    module_table_t,
    check_new_t
  >::type
  new_modules_t;

new_modules_t &get_new_modules();

typedef
  rtl::selection_t<
    plugin_table_t,
    check_new_t
  >::type
  new_plugins_t;

new_plugins_t &get_new_plugins();

typedef
  rtl::selection_t<
    interface_table_t,
    check_new_t
  >::type
  new_interfaces_t;

new_interfaces_t &get_new_interfaces();

// Index tables by IDs

typedef
  rtl::key_index_t<
    new_modules_t,
    mpl::vector1<module_id>
  >::type
  module_by_id_t;

module_by_id_t &get_module_by_id();

typedef
  rtl::key_index_t<
    new_plugins_t,
    mpl::vector1<plugin_id>
  >::type
  plugin_by_id_t;

plugin_by_id_t &get_plugin_by_id();

typedef
  rtl::key_index_t<
    new_interfaces_t,
    mpl::vector1<interface_id>
  >::type
  interface_by_id_t;

interface_by_id_t &get_interface_by_id();

// Reverse index

typedef
  rtl::key_index_t<
    plugin_interfaces_table_t,
    mpl::vector1<interface_handle>
  >::type
  plugin_with_interface_t;

plugin_with_interface_t &get_plugin_with_interface();

}}

#endif
