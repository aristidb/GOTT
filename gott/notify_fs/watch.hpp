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
 * The Original Code is A Filesystem Notification Library.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2005
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

#ifndef GOTT_NOTIFY_FS_WATCH_HPP
#define GOTT_NOTIFY_FS_WATCH_HPP

#include "event.hpp"
#include <gott/visibility.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <sigc++/signal.h>
#include <stdexcept>

namespace gott {
namespace notify_fs {

class watch_implementation;

class watch : boost::noncopyable {
public:
  GOTT_EXPORT watch(gott::string const &path, ev_t mask);
  ~watch() {}
  sigc::signal1<void, event const &> on_fire() const {
    return fire;
  }

private:
  sigc::signal1<void, event const &> fire;
  boost::scoped_ptr<watch_implementation> p;
};

class GOTT_EXPORT watch_installation_failure : public std::runtime_error {
public:
  watch_installation_failure(gott::string const &file);
  ~watch_installation_failure() throw();
};

class GOTT_EXPORT watch_implementation {
public:
  virtual ~watch_implementation() = 0;
};

}}

#endif
