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

#include "resource.hpp"
#include "exceptions.hpp"
#include <gott/plugin/metadata_manager.hpp>
#include <boost/thread/once.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/mem_fun.hpp>

using tdl::resource;
using tdl::detail::generic_callback;
using gott::atom;
using namespace boost::multi_index;

namespace {
  typedef boost::mutex mutex_t;
  typedef mutex_t::scoped_lock scoped_lock;
  mutex_t mutex;

  struct by_id {};
  struct by_kind {};

  typedef multi_index_container<
    resource *,
    indexed_by<
      sequenced<>,
      hashed_unique<
        tag<by_id>,
        const_mem_fun<resource, atom, &resource::get_id>
      >,
      hashed_non_unique<
        tag<by_kind>,
        const_mem_fun<resource, atom, &resource::get_kind> 
      >
    >
  > resource_set;

  resource_set all_resources;

  boost::once_flag init_flag = BOOST_ONCE_INIT;

  void init2();
  
  void init(scoped_lock &lock) {
    lock.unlock();

    gott::plugin::metadata_manager man;
    man.load_standard();
    man.commit();

    boost::call_once(&init2, init_flag);

    lock.lock();
  }
}

resource::~resource() {}

void resource::list_impl(atom const &kind, generic_callback const &callback) {
  scoped_lock lock(mutex);
  typedef resource_set::index<by_kind>::type idx_t;
  idx_t &idx = all_resources.get<by_kind>();
  std::pair<idx_t::iterator, idx_t::iterator> range = idx.equal_range(kind);
  if (range.first == range.second) {
    init(lock);
    range = idx.equal_range(kind);
  }
  for (; range.first != range.second; ++range.first)
    callback(*range.first);
}

void resource::find_impl(
    atom const &id,
    atom const &kind,
    generic_callback const &callback) {
  scoped_lock lock(mutex);
  typedef resource_set::index<by_id>::type idx_t;
  idx_t &idx = all_resources.get<by_id>();
  idx_t::iterator it = idx.find(id);
  if (it == idx.end()) {
    init(lock);
    it = idx.find(id);
    if (it == idx.end())
      throw tdl_error("TDL resource loader", "resource not found");
  }
  resource *res = *it;
  if (res->get_kind() != kind)
    throw tdl_error("TDL resource loader", "resource has wrong type");
  callback(res);
}

void resource::add(resource *res) {
  scoped_lock lock(mutex);
  all_resources.push_back(res);
}

namespace {
  void init2() {
  }
}
