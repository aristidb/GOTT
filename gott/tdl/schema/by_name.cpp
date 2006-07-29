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
 * The Original Code is A C++ Library for Dealing with the TDL Language.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2004-2006
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

#include "by_name.hpp"
#include "rule.hpp"
#include "type.hpp"
#include <gott/string/string.hpp>
#include <gott/string/atom.hpp>
#include <gott/plugin.hpp>
#include <gott/plugin/metadata_manager.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <map>

using namespace tdl::schema;
using namespace gott::plugin;
using gott::string;
using std::vector;

namespace {
  // those are NEVER unloaded because they depend on "us" and this leads to a
  // deadly circular reference or something
  typedef std::map<gott::atom, plugin_handle<type> * > map_t;
  map_t all_handles;
  boost::mutex mutex;
}

rule_t tdl::schema::get_by_name(string const &s, rule_attr_t const &a,
    vector<rule_t> const &c) {
  {
    gott::plugin::metadata_manager man;
    man.load_standard();
    man.commit();
  }

  boost::mutex::scoped_lock lock(mutex);

  if (all_handles.count(s) == 0) {
    plugin_handle<type> * handle(new plugin_handle<type>(with_plugin_id(s)));

    all_handles.insert(map_t::value_type(s, handle));
  }

  plugin_handle<type> * handle = all_handles.find(s)->second;
  type &type = *handle->get();
  abstract_rule abstract = type.get_abstract();

  return rule_t(abstract, a, c);
}
