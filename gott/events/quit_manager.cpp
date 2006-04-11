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
 * The Original Code is An Event Handling Class Library.
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

#include "quit_manager.hpp"
#include "signal_manager.hpp"
#include <boost/lambda/bind.hpp>
#include <signal.h>

using namespace boost::lambda;
using namespace gott::events;

gott::QID const quit_manager::qid("gott::events::quit_manager");

quit_manager::quit_manager(main_loop &loop) : ref_loop(loop) {}
quit_manager::~quit_manager() {}

quit_manager *quit_manager::get_for(main_loop &loop) {
  void *&slot = loop.feature_data(qid);
  if (slot)
    return static_cast<quit_manager *>(slot);
  quit_manager *man = new quit_manager(loop);
  slot = man;
  return man;
}

void quit_manager::enable_master() {
  ref_loop.feature<signal_manager>().on_signal(SIGINT).connect(
      bind(&quit_manager::quit_event, this));
  ref_loop.feature<signal_manager>().on_signal(SIGTERM).connect(
      bind(&quit_manager::quit_event, this));
  ref_loop.feature<signal_manager>().on_signal(SIGPIPE).connect(
      bind(&quit_manager::quit_event, this));
}

void quit_manager::quit_event() {
  ref_loop.quit_local();
}
