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

#include "module.hpp"
#include "plugin_base.hpp"
#include "descriptor.hpp"
#include "plugin_builder.hpp"
#include "metadata_manager.hpp"
#include <boost/scoped_array.hpp>
#include <gott/string/string.hpp>
#include <gott/exceptions.hpp>
#include <gott/syswrap/dl_unix.hpp>
#include <gott/syswrap/function_cast.hpp>
#include <vector>

using gott::plugin::module;
using gott::plugin::plugin_base;

class module::impl {
private:
  typedef std::vector<module> dep_list_t;
  dep_list_t dependencies;
  void *handle;
  
public:
  impl(module_descriptor const &which)
  : handle(0) {
    // load dependencies
    {
      metadata_manager man;
      module_information const &info = man.module_extra(which);
      std::vector<module_descriptor> const &d = info.dependencies;
      for (std::vector<module_descriptor>::const_iterator it = d.begin();
          it != d.end();
          ++it)
        dependencies.push_back(module(*it));
    }
    // load module
    handle = get_handle(which);
  }

  static void *get_handle(module_descriptor const &which) {
    return dlopen_unix(
        boost::scoped_array<char>(which.file_path.c_string_alloc()).get(),
        RTLD_LAZY | RTLD_GLOBAL);
  }

  ~impl() {
    if (handle)
      try {
        dlclose_unix(handle);
      } catch (...) {
        // TODO: log?
      }
  }

  void *entity(gott::string const &symbol) {
    return dlsym_unix(handle,
        boost::scoped_array<char>(symbol.c_string_alloc()).get());
  }
};

module::module(module_descriptor const &which) : p(new impl(which)) {}
module::~module() {}

void *module::entity(gott::string const &symbol) {
  return p->entity(symbol);
}

plugin_base *module::load_plugin(plugin_descriptor const &which) {
  plugin_builder *fun = function_cast<plugin_builder>(entity(which.symbol));
  return fun();
}
