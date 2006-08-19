// vim:ts=2:sw=2:expandtab:autoindent:filetype=cpp:
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
#include "selector.hpp"
#include "descriptor.hpp"
#include "plugin_base.hpp"
#include "metadata_manager.hpp"
#include "error.hpp"

using namespace gott::plugin;

class plugin_handle_base::impl {
public:
  impl(plugin_descriptor const &desc)
  : mod(desc.enclosing_module), p(mod.load_plugin(desc)) {}

  module mod;
  boost::scoped_ptr<plugin_base> p;

  static bool init(boost::scoped_ptr<impl> &p, selector const &sel) {
    plugin_descriptor desc = sel.get_plugin();
    try {
      p.reset(new impl(desc));
    } catch (gott::system_error&) {
      // remove this plugin, it could not be loaded!
      // TODO: log?
      metadata_manager man;
      man.remove_plugin(desc);
      man.commit();
      return false;
    }
    return true;
  }
};

plugin_handle_base::plugin_handle_base(selector const &sel) {
  if (!impl::init(p, sel))
    try {
      while (!impl::init(p, sel))
        ;
    } catch (failed_load&) {
      throw failed_load("plugin", sel.to_string(), "invalid plugins only");
    }
}

plugin_handle_base::plugin_handle_base(plugin_descriptor const &desc)
try
: p(new impl(desc)) {}
catch (gott::system_error&) {
  throw failed_load("plugin", desc.to_string(),"invalid plugin");
}

plugin_handle_base::~plugin_handle_base() {}

plugin_base *plugin_handle_base::get_base() const {
  return p->p.get();
}

void plugin_handle_base::fail_interface(string const &which) {
  throw failed_load(
      "plugin",
      which,
      "plugin does not support requested interface");
}

