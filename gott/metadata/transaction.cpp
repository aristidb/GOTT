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
#include "interface.hpp"
#include "database.hpp"
#include "tables.hpp"
#include "index.hpp"
#include <gott/range.hpp>
#include <gott/exceptions.hpp>
#include <vector>
#include <boost/tuple/tuple.hpp>
#include <boost/rtl/key_index_delta.hpp>
#include <boost/rtl/table_delta.hpp>
#include <boost/rtl/lambda_support.hpp>
#include<iostream>//FIXME

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

struct print_ftor {//FIXME
  void operator() (gott::metadata::interface const &x) const {
    std::cout << x.interface_id().get_string() << std::endl;
  }
};

void transaction::commit() {
  metadata_db::global_mutex::scoped_lock lock(metadata_db::get_global_lock());
  GOTT_FOREACH_RANGE(it, p->remove_resources) {
    detail::remove_module_resource(*it);
    detail::remove_plugin_resource(*it);
  }
  GOTT_FOREACH_RANGE(it, p->insert_plugins) {
    detail::add_plugin(
        it->get<0>(),
        it->get<1>(),
        it->get<2>(),
        it->get<3>(),
        it->get<4>());
  }
  GOTT_FOREACH_RANGE(it, p->insert_modules) {
    detail::add_module(
        it->get<0>(),
        it->get<1>(),
        it->get<2>(),
        it->get<3>(),
        it->get<4>());
  }

  using namespace metadata_db;
  using namespace boost::lambda;
  rtl::transaction tr;

  // mark obsolete resources' metadata
  GOTT_FOREACH_RANGE(it, p->remove_resources) {
    std::cout << "$obsoleting resource... " << *it << std::endl;
    // mark interfaces from obsolete resource
    GOTT_AUTO(obsolete_interfaces,
        rtl::selection(
          get_interface_table(),
          _1[metadata_db::resource()] == *it));
    GOTT_FOREACH_RANGE(jt, obsolete_interfaces) {
      interface_table_t::value_type x = *jt;
      std::cout << "obsoleting interface " << x[interface_id()].get_string()
        << std::endl;
      x[obsolete()] = true;
      tr.update(get_interface_table(), x);
    }

    // mark modules from obsolete resource
    GOTT_AUTO(obsolete_modules,
        rtl::selection(
          get_module_table(),
          _1[metadata_db::resource()] == *it));
    GOTT_FOREACH_RANGE(jt, obsolete_modules) {
      module_table_t::value_type x = *jt;
      std::cout << "obsoleting module " << x[module_id()].get_string()
        << std::endl;
      x[obsolete()] = true;
      tr.update(get_module_table(), x);
    }

    // mark plugins from obsolete resource
    GOTT_AUTO(obsolete_plugins,
        rtl::selection(
          get_plugin_table(),
          _1[metadata_db::resource()] == *it));
    GOTT_FOREACH_RANGE(jt, obsolete_plugins) {
      plugin_table_t::value_type x = *jt;
      std::cout << "obsoleting plugin " << x[plugin_id()].get_string()
        << std::endl;
      x[obsolete()] = true;
      tr.update(get_plugin_table(), x);
    }
  }

  // add fresh interfaces
  GOTT_FOREACH_RANGE(it, p->insert_interfaces) {
    GOTT_AUTO_CREF(new_id, it->get<0>());
    GOTT_AUTO_CREF(new_resource, it->get<1>());

    GOTT_AUTO(candidates,
        selection_eq(
          get_interface_by_id(),
          rtl::row<mpl::vector<interface_id> >(new_id)));
    GOTT_AUTO_CREF(duplicates, candidates); // interface-id is the only data
    if (duplicates.begin() == duplicates.end()) {
      std::cout << "inserting ";
      // ... no duplicate => insert
      tr.insert(get_interface_table(), interface_table_t::value_type(
            handle_t(),
            new_id,
            new_resource,
            false));
    } else {
      std::cout << "updating ";
      // ... duplicate => reuse handle
      tr.update(get_interface_table(), interface_table_t::value_type(
            duplicates.begin().get(interface_handle()),
            new_id,
            new_resource,
            false));
    }
    std::cout << new_id.get_string() << std::endl;
  }

  std::vector<std::pair<handle_t, impl::mod_lst::iterator> > added_new_modules;

  // add fresh modules (ignore dependencies for now)
  GOTT_FOREACH_RANGE(it, p->insert_modules) {
    GOTT_AUTO_CREF(new_id, it->get<0>());
    GOTT_AUTO_CREF(new_type, it->get<1>());
    GOTT_AUTO_CREF(new_file, it->get<2>());
    GOTT_AUTO_CREF(new_resource, it->get<4>());

    GOTT_AUTO(candidates,
        selection_eq(
          get_module_by_id(),
          rtl::row<mpl::vector<module_id> >(new_id)));
    // FIXME: check whether dependencies are the same
    GOTT_AUTO(duplicates,
        selection(
          candidates,
          _1[module_type()] == new_type &&
          _1[file_path()] == new_file));
    if (duplicates.begin() == duplicates.end()) {
      std::cout << "inserting module ";
      // ... no duplicate => insert
      handle_t handle;
      tr.insert(get_module_table(), module_table_t::value_type(
            handle,
            new_id,
            new_type,
            new_file,
            new_resource,
            false));
      added_new_modules.push_back(std::make_pair(handle, it));
    } else {
      std::cout << "updating module ";
      // ... duplicate => reuse handle
      tr.update(get_module_table(), module_table_t::value_type(
            duplicates.begin().get(module_handle()),
            new_id,
            new_type,
            new_file,
            new_resource,
            false));
    }
    std::cout << new_id.get_string() << std::endl;
  }

  // now care about their dependencies
  GOTT_FOREACH_RANGE(it, added_new_modules) {
    GOTT_AUTO_CREF(deps, it->second->get<3>());
    GOTT_FOREACH_RANGE(jt, deps) {
      std::cout << "+dep " << it->second->get<0>().get_string()
        << " on " << jt->get_string() << std::endl;
      GOTT_AUTO(exact_dependency_list,
          rtl::selection_eq(
            rtl::modified(get_module_by_id(), tr),
            rtl::row<mpl::vector<module_id> >(*jt)));
      if (exact_dependency_list.begin() == exact_dependency_list.end())
        throw gott::system_error("module dependency not found");
      handle_t dep_handle =
        exact_dependency_list.begin().get(module_handle());
      
      tr.insert(get_module_dependencies_table(),
          module_dependencies_table_t::value_type(
            it->first, dep_handle));
    }
  }

  // add fresh plugins
  GOTT_FOREACH_RANGE(it, p->insert_plugins) {
    GOTT_AUTO_CREF(new_id, it->get<0>());
    std::vector<QID> supported_interfaces(1, it->get<1>());
    GOTT_AUTO_CREF(new_module_id, it->get<2>());
    GOTT_AUTO_CREF(new_symbol, it->get<3>());
    GOTT_AUTO_CREF(new_resource, it->get<4>());

    // search enclosing module first
    GOTT_AUTO(enclosing_module_list,
        rtl::selection_eq(
          rtl::modified(get_module_by_id(), tr),
          rtl::row<mpl::vector<module_id> >(new_module_id)));
    if (enclosing_module_list.begin() == enclosing_module_list.end())
      throw gott::system_error("enclosing module not found");
    handle_t enclosing_module_handle =
        enclosing_module_list.begin().get(module_handle());

    GOTT_AUTO(candidates,
        selection_eq(
          get_plugin_by_id(),
          rtl::row<mpl::vector<plugin_id> >(new_id)));
    // FIXME: check whether supported interfaces are the same
    GOTT_AUTO(duplicates,
        selection(
          candidates,
          _1[symbol()] == new_symbol &&
          _1[module_handle()] == enclosing_module_handle));
    if (duplicates.begin() == duplicates.end()) {
      std::cout << "inserting plugin (";
      // ... no duplicate => insert
      handle_t handle;
      tr.insert(get_plugin_table(), plugin_table_t::value_type(
            handle,
            new_id,
            new_symbol,
            enclosing_module_handle,
            new_resource,
            false));

      // add supported interfaces
      GOTT_FOREACH_RANGE(jt, supported_interfaces) {
        std::cout << jt->get_string() << ' ' << std::flush;
        // search the actual interface first
        GOTT_AUTO(supported_interface_list,
            rtl::selection_eq(
              rtl::modified(get_interface_by_id(), tr),
              rtl::row<mpl::vector<interface_id> >(*jt)));
        if (supported_interface_list.begin() == supported_interface_list.end())
          throw gott::system_error("supported interface not found");
        handle_t the_interface_handle =
          supported_interface_list.begin().get(interface_handle());

        tr.insert(get_plugin_interfaces_table(),
            plugin_interfaces_table_t::value_type(
              handle,
              the_interface_handle));
      }
      std::cout << ')';
    } else {
      std::cout << "updating plugin ";
      // ... duplicate => just update
      tr.update(get_plugin_table(), plugin_table_t::value_type(
            duplicates.begin().get(plugin_handle()),
            new_id,
            new_symbol,
            enclosing_module_handle,
            new_resource,
            false));
    }
    std::cout << new_id.get_string() << std::endl;
  }

  // update indexes and commit
  rtl::expression_registry exprs;
  exprs.add(get_module_by_id());
  exprs.add(get_plugin_by_id());
  exprs.add(get_interface_by_id());
  tr.commit(exprs);

  std::cout << "all interfaces:" << std::endl;
  enumerate_interfaces(print_ftor(), tags::load_standard_metadata = false);
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
