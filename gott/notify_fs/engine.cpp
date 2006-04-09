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

#include "engine.hpp"
#include "inotify/engine.hpp"
#include <gott/events/main_loop.hpp>
#include <gott/events/fd_manager.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/construct.hpp>

using gott::notify_fs::notification_engine;
using gott::events::main_loop;
using gott::events::fd_manager;
using namespace boost::lambda;

gott::QID const notification_engine::qid(
    "gott::notify_fs::notification_engine");

notification_engine::~notification_engine() {}

notification_engine *notification_engine::get_for(main_loop &m) {
  void *&slot = m.feature_data(qid);
  inotify_engine *eng = new inotify_engine;
  slot = eng;
  m.on_destroy().connect(bind(delete_ptr(), eng));
  m.feature<fd_manager>().add_fd(
      eng->conn.access(),
      fd_manager::read,
      bind(&inotify_engine::notify, eng));
  return eng;
}
