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
#include <gott/metadata/load.hpp>
#include "validate.hpp"
#include <gott/plugin/module.hpp>
#include <gott/exceptions.hpp>
#include <boost/thread.hpp>
#include <boost/weak_ptr.hpp>
#include <vector>

using namespace gott::metadata;
using namespace gott;

class module::impl {
public:
  boost::weak_ptr<gott::plugin::module> instance;
  boost::optional<bool> validation;
  boost::mutex mutex;
};

module::module() 
  : module_type(dynamic_native),
    p(new impl) {}

module::module(module const &o) 
  : module_id(o.module_id),
    module_type(o.module_type),
    file_path(o.file_path),
    dependencies(o.dependencies),
    p(new impl) {}

void module::operator=(module const &o) {
  module_id = o.module_id;
  module_type = o.module_type;
  file_path = o.file_path;
  dependencies = o.dependencies;
  p.reset(new impl);
}

module::~module() {}

bool module::is_valid() const {
  boost::mutex::scoped_lock lock(p->mutex);
  if (!p->validation)
    p->validation = detail::validate(*this);
  return *p->validation;
}

boost::shared_ptr<gott::plugin::module> module::get_instance() const {
  boost::mutex::scoped_lock lock(p->mutex);
  if (!p->instance.expired())
    return p->instance.lock();
  boost::shared_ptr<gott::plugin::module> result(
      new gott::plugin::module(*this));
  p->instance = result;
  return result;
}

namespace {
  static boost::recursive_mutex metadata_biglock;
  #define BIGLOCK boost::recursive_mutex::scoped_lock B_lock(metadata_biglock)

  typedef std::vector<module> module_list_t;
  static module_list_t known_module;
  static bool enabled = true;
  static module_list_t core_module;
}

void gott::metadata::enumerate_modules_p(
    boost::function<void (module const &)> const &callback,
    boost::optional<QID const &> const &module_id,
    bool cancel_early,
    bool do_load_standard_metadata,
    bool validate) {
  if (do_load_standard_metadata)
    load_standard();
  BIGLOCK;
  module_list_t::iterator begin = known_module.begin();
  module_list_t::iterator end = known_module.end();
  if (!enabled || begin == end) {
    begin = core_module.begin();
    end = core_module.end();
  }
  for (module_list_t::iterator it = begin; it != end; ++it) {
    if (module_id && it->module_id != *module_id)
      continue;
    if (validate && !it->is_valid())
      continue;
    callback(*it);
    if (cancel_early)
      return;
  }
}

void gott::metadata::add_module(
    module const &metadata,
    bool core) {
  BIGLOCK;
  if (core)
    core_module.push_back(metadata);
  else
    known_module.push_back(metadata);
}

void gott::metadata::clear_module() {
  BIGLOCK;
  module_list_t().swap(known_module);
}

void gott::metadata::disable_module() {
  BIGLOCK;
  enabled = false;
}

void gott::metadata::enable_module() {
  BIGLOCK;
  enabled = true;
}
