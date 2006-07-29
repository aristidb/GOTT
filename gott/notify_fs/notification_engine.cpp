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
 * The Original Code is A Filesystem Notification Library.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2005
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *  Aristid Breitkreuz (aribrei@arcor.de)
 *  Rüdiger Sonderfeld <kingruedi@c-plusplus.de>
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

#include "notification_engine.hpp"
#include "engine_factory.hpp"
#include <gott/plugin.hpp>
#include <gott/events/main_loop.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/construct.hpp>
#include <boost/thread/once.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/optional.hpp>
#include <boost/utility/in_place_factory.hpp>

using gott::notify_fs::notification_engine;
using gott::notify_fs::engine_factory;
using gott::events::main_loop;
using gott::plugin::plugin_handle;
using namespace boost::lambda;

gott::QID const notification_engine::qid(
    "gott::notify_fs::notification_engine");

notification_engine::~notification_engine() {}

namespace {
  boost::scoped_ptr<plugin_handle<engine_factory> > handle;
  boost::mutex plugin_mutex;
}

notification_engine *notification_engine::get_for(main_loop &m) {
  void *&slot = m.feature_data(qid);
  if (slot)
    return static_cast<notification_engine *>(slot);

  notification_engine *eng;
  {
  boost::mutex::scoped_lock lock(plugin_mutex);
  using namespace gott::plugin;
  if (!handle)
    handle.reset(new plugin_handle<engine_factory>(
          with_interface_id("gott::notify_fs::engine_factory")));
  eng = handle->get()->alloc(m);
  }
  
  slot = eng;
  m.on_destroy().connect(bind(delete_ptr(), eng));
  return eng;
}
