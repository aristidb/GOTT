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

#include "transaction.hpp"
#include "plugin.hpp"
#include "module.hpp"
#include "database.hpp"
#include <vector>
#include <boost/tuple/tuple.hpp>

using gott::metadata::transaction;

class transaction::impl {
public:
  typedef std::vector<string> res_lst;
  res_lst remove_resources;
  typedef
    std::vector<
      boost::tuple<QID, QID, QID, string, string> >
    plg_lst;
  plg_lst insert_plugins;
  typedef
    std::vector<
      boost::tuple<
        QID,
        module::module_type_t,
        string,
        std::vector<QID>,
        string
        > >
    mod_lst;
  mod_lst insert_modules;
  typedef
    std::vector<
      boost::tuple<
        QID,
        string
        > >
    if_lst;
  if_lst insert_interfaces;
};

transaction::transaction() : p(new impl) {}
transaction::~transaction() {}

void transaction::commit() {
  metadata_db::global_mutex::scoped_lock lock(metadata_db::get_global_lock());
  for (impl::res_lst::iterator it = p->remove_resources.begin();
      it != p->remove_resources.end();
      ++it) {
    detail::remove_module_resource(*it);
    detail::remove_plugin_resource(*it);
  }
  for (impl::plg_lst::iterator it = p->insert_plugins.begin();
      it != p->insert_plugins.end();
      ++it)
    detail::add_plugin(
        it->get<0>(),
        it->get<1>(),
        it->get<2>(),
        it->get<3>(),
        it->get<4>());
  for (impl::mod_lst::iterator it = p->insert_modules.begin();
      it != p->insert_modules.end();
      ++it)
    detail::add_module(
        it->get<0>(),
        it->get<1>(),
        it->get<2>(),
        it->get<3>(),
        it->get<4>());
}

void transaction::remove_resource(string const &r) {
  p->remove_resources.push_back(r);
}

void transaction::add_plugin(
    QID const &plugin_id,
    QID const &supported_interface,
    QID const &enclosing_module,
    string const &symbol,
    string const &resource) {
  p->insert_plugins.push_back(impl::plg_lst::value_type(
        plugin_id, supported_interface, enclosing_module, symbol, resource));
}

void transaction::add_module(
    QID const &module_id,
    module::module_type_t module_type,
    string const &file_path,
    std::vector<QID> const &dependencies,
    string const &resource) {
  p->insert_modules.push_back(impl::mod_lst::value_type(
        module_id, module_type, file_path, dependencies, resource));
}

void transaction::add_interface(
    QID const &interface_id,
    string const &resource) {
  p->insert_interfaces.push_back(impl::if_lst::value_type(
        interface_id, resource));
}
