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

#include "selector.hpp"
#include "descriptor.hpp"
#include "metadata_manager.hpp"
#include <gott/exceptions.hpp>
#include <boost/optional/optional.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/if.hpp>
#include <boost/lambda/bind.hpp>
#include <set>
#include <algorithm>

using gott::plugin::selector;

class selector::impl {
public:
  boost::optional<QID> plugin_id;
  boost::optional<QID> interface_id;
  boost::optional<QID> module_id;
  std::set<QID> features;

  bool check_plugin(plugin_information const &x) const {
    if (plugin_id && x.plugin_id != plugin_id.get())
      return false;
    if (interface_id && x.interfaces.count(interface_id.get()) == 0)
      return false;
    if (module_id && x.enclosing_module != module_id.get())
      return false;
    if (!std::includes(x.features.begin(), x.features.end(),
          features.begin(), features.end()))
      return false;
    return true;
  }

  bool check_module (module_information const &x) const {
    if (module_id && x.module_id != module_id.get())
      return false;
    return true;
  }
};

selector::selector(impl *p) : p(p) {}
selector::~selector() {}

selector selector::with_plugin_id(QID const &plugin_id) {
  impl *p = new impl;
  p->plugin_id = plugin_id;
  return selector(p);
};

selector selector::with_interface_id(QID const &interface_id) {
  impl *p = new impl;
  p->interface_id = interface_id;
  return selector(p);
}

selector selector::with_module_id(QID const &module_id) {
  impl *p = new impl;
  p->module_id = module_id;
  return selector(p);
}

selector selector::with_feature_id(QID const &feature_id) {
  impl *p = new impl;
  p->features.insert(feature_id);
  return selector(p);
}

selector selector::operator&&(selector const &o) const {
  impl *n = new impl(*p);
  if (o.p->plugin_id) { 
    if (n->plugin_id && n->plugin_id.get() != o.p->plugin_id.get())
      throw user_error("contradictory selector");
    n->plugin_id = o.p->plugin_id;
  }
  if (o.p->interface_id) {
    if (n->interface_id && n->interface_id.get() != o.p->interface_id.get())
      throw user_error("contradictory selector");
    n->interface_id = o.p->interface_id;
  }
  if (o.p->module_id) {
    if (n->module_id && n->module_id.get() != o.p->module_id.get())
      throw user_error("contradictory selector");
    n->module_id = o.p->module_id;
  }
  n->features.insert(o.p->features.begin(), o.p->features.end());
  return selector(n);
}

gott::plugin::plugin_descriptor
selector::get_plugin() const {
  if (p->module_id)
    throw gott::system_error(
        "loading plugins by module-id is not supported");

  metadata_manager man;
  plugin_descriptor result;

  using namespace boost::lambda;

  man.enum_plugins(
      if_then_else_return(
        bind(&impl::check_plugin, p.get(), _2),
        (var(result) = _1, false),
        true),
      p->plugin_id,
      p->interface_id,
      p->features);

  return result;
}

gott::plugin::module_descriptor selector::get_module() const {
  if (p->plugin_id || p->interface_id || !p->features.empty())
    throw gott::system_error(
        "loading modules by plugin-id, interface-id or feature "
        "is not supported");

  metadata_manager man;
  module_descriptor result;

  using namespace boost::lambda;

  man.enum_modules(
      if_then_else_return(
        bind(&impl::check_module, p.get(), _2),
        (var(result) = _1, false),
        true),
      p->module_id);

  return result;
}
