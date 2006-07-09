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

#include "metadata_manager.hpp"
#include <gott/exceptions.hpp>
#include <boost/thread/recursive_mutex.hpp>

using gott::plugin::metadata_manager;
using gott::string;

namespace {
  boost::recursive_mutex mutex;
  bool allow_recur = false;
  unsigned long recursion = 0;

  class scoped_allow_recur {
  public:
    scoped_allow_recur() : virgin(!allow_recur) { allow_recur = true; }
    ~scoped_allow_recur() { if (virgin) allow_recur = false; }
  private:
    bool virgin;
  };

#define ALLOW_RECUR scoped_allow_recur MAY_RECUR; (void)MAY_RECUR
}

class metadata_manager::impl {
public:
  impl() : lock(mutex) {}
  boost::recursive_mutex::scoped_lock lock;
};

metadata_manager::metadata_manager() : p(new impl) {
  if (!allow_recur && recursion > 0)
    throw gott::internal_error("multiple metadata_manager objects");
  ++recursion;
}
metadata_manager::~metadata_manager() { --recursion; }

void metadata_manager::commit() {
  ALLOW_RECUR;
}

void metadata_manager::remove_resource(string const &) {
  ALLOW_RECUR;
}

void metadata_manager::update_resource(std::istream &, string const &) {
  ALLOW_RECUR;
}

void metadata_manager::load_core() {
  ALLOW_RECUR;
}

void metadata_manager::load_standard() {
  ALLOW_RECUR;
}
