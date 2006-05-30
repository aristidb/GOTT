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
#include "load.hpp"
#include "detail/database.hpp"
#include "detail/tables.hpp"
#include "detail/index.hpp"
#include <gott/range.hpp>
#include <gott/exceptions.hpp>
#include <gott/auto.hpp>
#include <boost/rtl/lambda_support.hpp>
#include <boost/rtl/key_index_delta.hpp>
#include <boost/rtl/selection_delta.hpp>
#include <boost/rtl/table_delta.hpp>
#include <vector>

using gott::metadata::transaction;

class transaction::impl {
public:
  typedef std::vector<string> res_lst;
  res_lst remove_resources;

  struct plugin_info {
    QID plugin_id;
    std::vector<QID> supported_interfaces;
    QID enclosing_module;
    string symbol;
    plugin_priority priority;
    string resource;
  };

  typedef std::vector<plugin_info> plg_lst;

  plg_lst insert_plugins;

  struct module_info {
    QID module_id;
    module::module_type_t module_type;
    string file_path;
    std::vector<QID> dependencies;
    string resource;
  };
  
  typedef std::vector<module_info> mod_lst;

  mod_lst insert_modules;

  struct interface_info {
    QID interface_id;
    string resource;
  };

  typedef std::vector<interface_info> if_lst;
  if_lst insert_interfaces;
};

transaction::transaction() : p(new impl) {}
transaction::~transaction() {}

void transaction::commit() {
  metadata_db::global_mutex::scoped_lock lock(metadata_db::get_global_lock());
  commit_modules();
  commit_interfaces();
  commit_plugins();
}

void transaction::commit_interfaces() {
  using namespace gott::metadata_db;
  using namespace boost::lambda;

  rtl::transaction tr;

  GOTT_FOREACH_RANGE(it, p->remove_resources) {
    // mark interfaces from obsolete resource
    GOTT_AUTO(obsolete_interfaces,
        rtl::selection(
          get_interface_table(),
          _1[metadata_db::resource()] == *it));
    GOTT_FOREACH_RANGE(jt, obsolete_interfaces) {
      interface_table_t::value_type x = *jt;
      x[obsolete()] = true;
      tr.update(get_interface_table(), x);
    }
  }

  // add fresh interfaces
  GOTT_FOREACH_RANGE(it, p->insert_interfaces) {
    GOTT_AUTO(candidates,
        rtl::selection_eq(
          get_interface_by_id(),
          rtl::row<mpl::vector1<interface_id> >(it->interface_id)));
    GOTT_AUTO_CREF(duplicates, candidates); // interface-id is the only data
    if (duplicates.begin() == duplicates.end()) {
      // ... no duplicate => insert
      tr.insert(get_interface_table(), interface_table_t::value_type(
            handle_t(),
            it->interface_id,
            it->resource,
            false));
    } else {
      // ... duplicate => reuse handle
      tr.update(get_interface_table(), interface_table_t::value_type(
            duplicates.begin().get(interface_handle()),
            it->interface_id,
            it->resource,
            false));
    }
  }

  // commit and update indexes
  rtl::expression_registry exprs;
  exprs.add(get_new_interfaces());
  exprs.add(get_interface_by_id());
  tr.commit(exprs);
}

void transaction::commit_modules() {
  using namespace gott::metadata_db;
  using namespace boost::lambda;
  
  std::vector<std::pair<handle_t, impl::mod_lst::iterator> > added;
  added.reserve(p->insert_modules.size());

  {
    rtl::transaction tr;

    GOTT_FOREACH_RANGE(it, p->remove_resources) {
      // mark modules from obsolete resource
      GOTT_AUTO(obsolete_modules,
          rtl::selection(
            get_module_table(),
            _1[metadata_db::resource()] == *it));
      GOTT_FOREACH_RANGE(jt, obsolete_modules) {
        module_table_t::value_type x = *jt;
        x[obsolete()] = true;
        tr.update(get_module_table(), x);
      }
    }

    // add fresh modules (ignore dependencies for now)
    GOTT_FOREACH_RANGE(it, p->insert_modules) {
      GOTT_AUTO(candidates,
          rtl::selection_eq(
            get_module_by_id(),
            rtl::row<mpl::vector1<module_id> >(it->module_id)));
      // FIXME: check whether dependencies are the same
      GOTT_AUTO(duplicates,
          selection(
            candidates,
            _1[module_type()] == it->module_type &&
            _1[file_path()] == it->file_path));
      if (duplicates.begin() == duplicates.end()) {
        // ... no duplicate => insert
        handle_t handle;
        tr.insert(get_module_table(), module_table_t::value_type(
              handle,
              it->module_id,
              it->module_type,
              it->file_path,
              it->resource,
              false));
        // remember, for dependency injection later
        added.push_back(std::make_pair(handle, it));
      } else {
        // ... duplicate => reuse handle
        tr.update(get_module_table(), module_table_t::value_type(
              duplicates.begin().get(module_handle()),
              it->module_id,
              it->module_type,
              it->file_path,
              it->resource,
              false));
      }
    }

    rtl::expression_registry exprs;
    exprs.add(get_new_modules());
    exprs.add(get_module_by_id());
    tr.commit(exprs);
  }

  {
    rtl::transaction tr;

    // now care about their dependencies
    GOTT_FOREACH_RANGE(it, added) {
      GOTT_FOREACH_RANGE(jt, it->second->dependencies) {
        boost::optional<module> dep = 
          find_module(
              tags::module_id = *jt,
              tags::load_standard_metadata = false,
              tags::validate = false);
        if (!dep)
          throw gott::system_error("module dependency not found");
        handle_t dep_handle = dep->get_handle();
      
        tr.insert(get_module_dependencies_table(),
            module_dependencies_table_t::value_type(
              it->first, dep_handle));
      }
    }
    
    rtl::expression_registry exprs;
    tr.commit(exprs);
  }
}

void transaction::commit_plugins() {
  using namespace gott::metadata_db;
  using namespace boost::lambda;

  rtl::transaction tr;

  GOTT_FOREACH_RANGE(it, p->remove_resources) {
    // mark plugins from obsolete resource
    GOTT_AUTO(obsolete_plugins,
        rtl::selection(
          get_plugin_table(),
          _1[metadata_db::resource()] == *it));
    GOTT_FOREACH_RANGE(jt, obsolete_plugins) {
      plugin_table_t::value_type x = *jt;
      x[obsolete()] = true;
      tr.update(get_plugin_table(), x);
    }
  }

  // add fresh plugins
  GOTT_FOREACH_RANGE(it, p->insert_plugins) {
    boost::optional<module> enclosing_module =
      find_module(
          tags::module_id = it->enclosing_module,
          tags::validate = false,
          tags::load_standard_metadata = false);
    if (!enclosing_module)
      throw gott::system_error("enclosing module not found");
    handle_t enclosing_module_handle = enclosing_module->get_handle();

    GOTT_AUTO(candidates,
        rtl::selection_eq(
          get_plugin_by_id(),
          rtl::row<mpl::vector1<plugin_id> >(it->plugin_id)));
    // FIXME: check whether supported interfaces are the same
    GOTT_AUTO(duplicates,
        selection(
          candidates,
          _1[symbol()] == it->symbol &&
          _1[module_handle()] == enclosing_module_handle));
    if (duplicates.begin() == duplicates.end()) {
      // ... no duplicate => insert
      handle_t handle;
      tr.insert(get_plugin_table(), plugin_table_t::value_type(
            handle,
            it->priority,
            it->plugin_id,
            it->symbol,
            enclosing_module_handle,
            it->resource,
            false));

      // add supported interfaces
      GOTT_FOREACH_RANGE(jt, it->supported_interfaces) {
        boost::optional<interface> supported_interface =
          find_interface(
              tags::interface_id = *jt,
              tags::validate = false,
              tags::load_standard_metadata = false);
        if (!supported_interface)
          throw gott::system_error("supported interface not found");
        handle_t the_interface_handle = supported_interface->get_handle();

        tr.insert(get_plugin_interfaces_table(),
            plugin_interfaces_table_t::value_type(
              handle,
              the_interface_handle));
      }
    } else {
      // ... duplicate => just update
      tr.update(get_plugin_table(), plugin_table_t::value_type(
            duplicates.begin().get(plugin_handle()),
            it->priority,
            it->plugin_id,
            it->symbol,
            enclosing_module_handle,
            it->resource,
            false));
    }
  }

  // update indexes and commit
  rtl::expression_registry exprs;
  exprs.add(get_new_plugins());
  exprs.add(get_plugin_by_id());
  exprs.add(get_plugin_with_interface());
  tr.commit(exprs);
}

void transaction::remove_resource(string const &r) {
  p->remove_resources.push_back(r);
}

void transaction::update_resource(
    std::istream &stream,
    string const &resource,
    resource_kind kind) {
  switch (kind) {
  case interface_resource:
    detail::update_interface_resource(stream, resource, *this);
    break;
  case module_resource:
    detail::update_module_resource(stream, resource, *this);
    break;
  case plugin_resource:
    detail::update_plugin_resource(stream, resource, *this);
    break;
  }
}

void transaction::add_plugin(
    QID const &plugin_id,
    std::vector<QID> const &supported_interfaces,
    QID const &enclosing_module,
    string const &symbol,
    plugin_priority priority,
    string const &resource) {
  impl::plugin_info x =
    { 
      plugin_id,
      supported_interfaces,
      enclosing_module,
      symbol,
      priority,
      resource
    };
  p->insert_plugins.push_back(x);
}

void transaction::add_module(
    QID const &module_id,
    module::module_type_t module_type,
    string const &file_path,
    std::vector<QID> const &dependencies,
    string const &resource) {
  impl::module_info x =
    {
      module_id,
      module_type,
      file_path,
      dependencies,
      resource
    };
  p->insert_modules.push_back(x);
}

void transaction::add_interface(
    QID const &interface_id,
    string const &resource) {
  impl::interface_info x =
    {
      interface_id,
      resource
    };
  p->insert_interfaces.push_back(x);
}
