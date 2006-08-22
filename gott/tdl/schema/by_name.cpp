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
#include <gott/tdl/exceptions.hpp>
#include <gott/string/string.hpp>
#include <gott/string/atom.hpp>
#include <gott/plugin.hpp>
#include <gott/plugin/lazy_handle.hpp>
#include <gott/plugin/metadata_manager.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/once.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility/in_place_factory.hpp>
#include <map>

using namespace tdl::schema;
using namespace gott::plugin;
using gott::string;
using std::vector;

namespace {
  // those are NEVER unloaded because they depend on "us" and this leads to a
  // deadly circular reference or something
  typedef std::map<gott::atom, lazy_handle<type> > map_t;
  map_t all_handles;
  boost::optional<multi_plugin> book_keeper;
  boost::mutex mutex;
  boost::once_flag init_flag = BOOST_ONCE_INIT;

  void do_add(plugin_descriptor const &desc) {
    boost::mutex::scoped_lock lock(mutex);
    metadata_manager man;
    all_handles.insert(map_t::value_type(
          man.plugin_extra(desc).plugin_id,
          lazy_handle<type>(desc)));
  }

  void init() {
    book_keeper = boost::in_place(
        with_interface<type>(),
        multi_plugin::add_callback_t(&do_add));
  }
}

rule_t tdl::schema::get_by_name(string const &s, rule_attr_t const &a,
    vector<rule_t> const &c) {
  boost::mutex::scoped_lock lock(mutex);

  map_t::iterator it = all_handles.find(s);

  if (it == all_handles.end()) {
    lock.unlock();

    metadata_manager man;
    man.load_standard();
    man.commit();

    boost::call_once(&init, init_flag);

    lock.lock();

    it = all_handles.find(s);
    if (it == all_handles.end())
      throw tdl::tdl_error("TDL Schema", "type not found");
  }

  lazy_handle<type> &handle = it->second;
  abstract_rule abstract = handle->get_abstract();
  lock.unlock();

  return rule_t(abstract, a, c);
}
