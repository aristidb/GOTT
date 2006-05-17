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

#include "plugin_handle.hpp"
#include <gott/metadata/plugin.hpp>
#include <gott/metadata/module.hpp>
#include "plugin_base.hpp"
#include <gott/exceptions.hpp>

using namespace gott::plugin;

class plugin_handle_base::impl {
public:
  impl(plugin_metadata const &which)
    : mod(which.enclosing_module_metadata().get_instance()),
      p(mod->load_plugin(which)) {}

  ~impl() { delete p; }

  boost::shared_ptr<module> mod;
  plugin_base *p;
};

plugin_handle_base::plugin_handle_base(plugin_metadata const &which)
: p(new impl(which)) {}

namespace {
plugin_metadata const &unwrap(
    boost::optional<plugin_metadata const &> const &which) {
  if (!which)
    throw gott::system_error("plugin not found");
  return which.get();
}
}

plugin_handle_base::plugin_handle_base(
    boost::optional<plugin_metadata const &> const &which)
: p(new impl(unwrap(which))) {}

plugin_handle_base::~plugin_handle_base() {}

plugin_base *plugin_handle_base::get_base() const {
  return p->p;
}

void plugin_handle_base::fail_interface(plugin_metadata const &which) {
  throw gott::system_error(
      which.plugin_id.get_string()
      + " : plugin does not support requested interface");
}

