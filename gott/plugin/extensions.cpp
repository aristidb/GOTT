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

#include "extensions.hpp"
#include <boost/thread/mutex.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/none.hpp>

using gott::QID;
using gott::plugin::extension_point;
using namespace boost::multi_index;

namespace {
  typedef boost::mutex mutex_t;
  typedef mutex_t::scoped_lock scoped_lock;
  mutex_t mutex;

  typedef multi_index_container<
    extension_point *,
    indexed_by<
      hashed_unique<
        member<extension_point, QID const, &extension_point::id>
      >
    >
  > set_t;
  set_t set;
}

extension_point::extension_point(QID const &id) : id(id) {
  scoped_lock lock(mutex);
  set.insert(this);
}

extension_point::~extension_point() {
  scoped_lock lock(mutex);
  set.erase(id);
}

boost::optional<extension_point &> extension_point::find(QID const &id) {
  scoped_lock lock(mutex);
  set_t::const_iterator it = set.find(id);
  if (it == set.end())
    return boost::none;
  return **it;
}
