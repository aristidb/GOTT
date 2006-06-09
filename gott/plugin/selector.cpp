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
#include <gott/exceptions.hpp>
#include <boost/optional/optional.hpp>
#include <gott/metadata/module.hpp>
#include <gott/metadata/plugin.hpp>
#include <algorithm>
#include <vector>

using gott::plugin::selector;

class selector::impl {
public:
  boost::optional<QID> plugin_id;
  boost::optional<QID> interface_id;
  boost::optional<QID> module_id;
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
  return selector(n);
}

namespace {
template<class T>
struct push_back_t {
  push_back_t(T &container) : container(container) {}
  T &container;
  void operator() (typename T::value_type const &x) {
    container.push_back(x);
  }
};

template<class T>
push_back_t<T> push_back(T &container) {
  return push_back_t<T>(container);
}
}

std::pair<
  gott::plugin::plugin_descriptor,
  gott::plugin::module_descriptor
>
selector::get() const {
  using namespace gott::metadata;
  using std::vector;

  vector<metadata::plugin> plugins;
  if (p->plugin_id)
    if (p->interface_id)
      enumerate_plugins(
          push_back(plugins),
          tags::plugin_id = p->plugin_id.get(),
          tags::interface_id = p->interface_id.get());
    else
      enumerate_plugins(
          push_back(plugins),
          tags::plugin_id = p->plugin_id.get());
  else if (p->interface_id)
    enumerate_plugins(
        push_back(plugins),
        tags::interface_id = p->interface_id.get());
  else
    enumerate_plugins(push_back(plugins));

  vector<metadata::module> modules;
  if (p->module_id) {
    enumerate_modules(
        push_back(modules),
        tags::module_id = p->module_id.get());
    if (p->plugin_id || p->module_id) { // filter
      vector<metadata::module> old_modules;
      old_modules.swap(modules);
      vector<metadata::module> plugin_modules;
      for (vector<metadata::plugin>::const_iterator it = plugins.begin();
          it != plugins.end();
          ++it)
        plugin_modules.push_back(it->enclosing_module());
      for (vector<metadata::module>::const_iterator it = plugin_modules.begin();
          it != plugin_modules.end();
          ++it)
        for (vector<metadata::module>::const_iterator jt = old_modules.begin();
            jt != old_modules.end();
            ++jt)
          if (it->get_handle() == jt->get_handle())
            modules.push_back(*it);
    }
  } else {
    for (vector<metadata::plugin>::const_iterator it = plugins.begin();
        it != plugins.end();
        ++it)
      modules.push_back(it->enclosing_module());
  }

  return
    std::make_pair(
        plugin_descriptor(
          plugins.empty() ? string() : plugins[0].symbol()
        ),
        module_descriptor(
          modules.empty() ? string() : modules[0].file_path()
        ));
}
