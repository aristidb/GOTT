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

#include "plugin_metadata.hpp"
#include "module_metadata.hpp"
#include "metadata.hpp"
#include "validate.hpp"
#include <gott/exceptions.hpp>
#include <gott/range_algo.hpp>
#include <boost/thread.hpp>

using namespace gott::plugin;
using namespace gott;

class plugin_metadata::impl {
public:
  boost::optional<bool> validation;
  boost::mutex mutex;
};

plugin_metadata::plugin_metadata() : p(new impl) {}
plugin_metadata::~plugin_metadata() {}
plugin_metadata::plugin_metadata(plugin_metadata const &o)
  : plugin_id(o.plugin_id),
    interfaces(o.interfaces),
    enclosing_module(o.enclosing_module),
    symbol(o.symbol),
    p(new impl) {}

void plugin_metadata::operator=(plugin_metadata const &o) {
  plugin_id = o.plugin_id;
  interfaces = o.interfaces;
  enclosing_module = o.enclosing_module;
  symbol = o.symbol;
  p.reset(new impl);
}

module_metadata const &plugin_metadata::enclosing_module_metadata(
    bool do_load_standard_metadata) const {
  boost::optional<module_metadata const &> res =
    find_module_metadata(
        enclosing_module,
        tags::validate = false,
        tags::load_standard_metadata = do_load_standard_metadata);
  if (!res)
    throw system_error("module not found");
  return *res;
}

bool plugin_metadata::is_valid() const {
  boost::mutex::scoped_lock lock(p->mutex);
  if (!p->validation)
    p->validation = detail::validate_metadata(*this);
  return *p->validation;
}

namespace {
  static boost::recursive_mutex metadata_biglock;
  #define BIGLOCK boost::recursive_mutex::scoped_lock B_lock(metadata_biglock)

  typedef std::vector<plugin_metadata> plugin_metadata_list_t;
  static plugin_metadata_list_t known_plugin_metadata;
  static bool enabled = true;
  static plugin_metadata_list_t core_plugin_metadata;
}

void gott::plugin::enumerate_plugin_metadata_p(
    boost::function<void (plugin_metadata const &)> const &function,
    boost::optional<QID const &> const &plugin_id,
    boost::optional<QID const &> const &interface_id,
    bool cancel_early,
    bool do_load_standard_metadata,
    bool validate) {
  if (do_load_standard_metadata)
    load_standard_metadata();
  BIGLOCK;
  plugin_metadata_list_t::iterator begin = known_plugin_metadata.begin();
  plugin_metadata_list_t::iterator end = known_plugin_metadata.end();
  if (!enabled || begin == end) {
    begin = core_plugin_metadata.begin();
    end = core_plugin_metadata.end();
  }
  for (plugin_metadata_list_t::iterator it = begin; it != end; ++it) {
    if (plugin_id && it->plugin_id != *plugin_id)
      continue;
    if (interface_id && 
        find(range(it->interfaces), *interface_id) == it->interfaces.end())
      continue;
    if (validate && !it->is_valid())
      continue;
    function(*it);
    if (cancel_early)
      break;
  }
}

void gott::plugin::add_plugin_metadata(
    plugin_metadata const &metadata,
    bool core) {
  BIGLOCK;
  if (core)
    core_plugin_metadata.push_back(metadata);
  else
    known_plugin_metadata.push_back(metadata);
}

void gott::plugin::clear_plugin_metadata() {
  BIGLOCK;
  plugin_metadata_list_t().swap(known_plugin_metadata);
}

void gott::plugin::disable_plugin_metadata() {
  BIGLOCK;
  enabled = false;
}

void gott::plugin::enable_plugin_metadata() {
  BIGLOCK;
  enabled = true;
}
