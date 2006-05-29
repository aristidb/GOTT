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

#include "index.hpp"

using namespace boost::rel;
using namespace boost::mpl;

namespace gott { namespace metadata_db {

module_by_id_t &get_module_by_id() {
  static module_by_id_t obj = 
    key_index<vector1<module_id> >(get_module_table());
  return obj;
}

plugin_by_id_t &get_plugin_by_id() {
  static plugin_by_id_t obj = 
    key_index<vector1<plugin_id> >(get_plugin_table());
  return obj;
}

interface_by_id_t &get_interface_by_id() {
  static interface_by_id_t obj = 
    key_index<vector1<interface_id> >(get_interface_table());
  return obj;
}

plugin_with_interface_t &get_plugin_with_interface() {
  static plugin_with_interface_t obj =
    key_index<vector1<interface_handle> >(get_plugin_interfaces_table());
  return obj;
}

}}
