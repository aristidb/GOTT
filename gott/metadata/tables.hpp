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

#ifndef GOTT_METADATA_TABLES_HPP
#define GOTT_METADATA_TABLES_HPP

#include "handle.hpp"
#include "module.hpp"
#include <gott/string/string.hpp>
#include <gott/string/qid.hpp>
#include <boost/rtl/rtl.hpp>

namespace gott { namespace metadata_db {

namespace rtl = boost::rel;
namespace mpl = boost::mpl;

// Columns
BOOST_RTL_DEFINE_COLUMN(string, resource)

BOOST_RTL_DEFINE_COLUMN(metadata::handle_t, module_handle)
BOOST_RTL_DEFINE_COLUMN(QID, module_id)
BOOST_RTL_DEFINE_COLUMN(metadata::module::module_type_t, module_type)
BOOST_RTL_DEFINE_COLUMN(string, file_path)

BOOST_RTL_DEFINE_COLUMN(metadata::handle_t, plugin_handle)
BOOST_RTL_DEFINE_COLUMN(QID, plugin_id)
BOOST_RTL_DEFINE_COLUMN(string, symbol)

BOOST_RTL_DEFINE_COLUMN(metadata::handle_t, interface_handle)
BOOST_RTL_DEFINE_COLUMN(QID, interface_id)

// Field lists
typedef 
  mpl::vector<module_handle>
  module_sort_list;
typedef
  mpl::vector<module_handle, module_id, module_type, file_path>
  module_field_list;

typedef
  mpl::vector<plugin_handle>
  plugin_sort_list;
typedef
  mpl::vector<plugin_handle, plugin_id, symbol, module_handle>
  plugin_field_list;

typedef
  mpl::vector<interface_handle>
  interface_sort_list;
typedef
  mpl::vector<interface_handle, interface_id>
  interface_field_list;

typedef
  mpl::vector<plugin_handle, interface_handle>
  plugin_interfaces_field_list;
typedef
  plugin_interfaces_field_list
  plugin_interfaces_sort_list;

typedef
  mpl::vector<module_handle, rtl::alias<module_handle> >
  module_dependencies_field_list;
typedef
  module_dependencies_field_list
  module_dependencies_sort_list;

// Table information
struct module_info
  : rtl::table_info<module_field_list, module_sort_list> {};
struct plugin_info
  : rtl::table_info<plugin_field_list, plugin_sort_list> {};
struct interface_info
  : rtl::table_info<interface_field_list, interface_sort_list> {};
struct plugin_interfaces_info
  : rtl::table_info<plugin_interfaces_field_list,
                    plugin_interfaces_sort_list> {};
struct module_dependencies_info
  : rtl::table_info<module_dependencies_field_list,
                    module_dependencies_sort_list> {};

typedef rtl::table<module_info> module_table_t;
typedef rtl::table<plugin_info> plugin_table_t;
typedef rtl::table<interface_info> interface_table_t;
typedef rtl::table<plugin_interfaces_info> plugin_interfaces_table_t;
typedef rtl::table<module_dependencies_info> module_dependencies_table_t;

module_table_t &get_module_table();
plugin_table_t &get_plugin_table();
interface_table_t &get_interface_table();
plugin_interfaces_table_t &get_plugin_interfaces_table();
module_dependencies_table_t &get_module_dependencies_table();

}}

#endif
