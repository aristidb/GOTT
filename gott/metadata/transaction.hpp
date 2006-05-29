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

#ifndef GOTT_METADATA_TRANSACTION_HPP
#define GOTT_METADATA_TRANSACTION_HPP

#include "module.hpp"
#include "plugin.hpp"
#include <boost/scoped_ptr.hpp>
#include <vector>
#include <iosfwd>

namespace gott {
  class string;

namespace metadata {

enum resource_kind { interface_resource, plugin_resource, module_resource };

class transaction {
public:
  GOTT_EXPORT transaction();
  GOTT_EXPORT ~transaction();
  GOTT_EXPORT void commit();

  GOTT_EXPORT void remove_resource(string const &resource);

  GOTT_EXPORT
  void update_resource(
      std::istream &stream,
      string const &resource,
      resource_kind kind);

public: //internal
  void add_module(
      QID const &module_id,
      module::module_type_t module_type,
      string const &file_path,
      std::vector<QID> const &dependencies,
      string const &resource);
  void add_plugin(
      QID const &plugin_id,
      std::vector<QID> const &supported_interfaces,
      QID const &enclosing_module,
      string const &symbol,
      plugin_priority priority,
      string const &resource);
  void add_interface(
      QID const &interface_id,
      string const &resource);

private:
  class impl;
  boost::scoped_ptr<impl> p;

  void commit_modules();
  void commit_plugins();
  void commit_interfaces();
};

}}

#endif
