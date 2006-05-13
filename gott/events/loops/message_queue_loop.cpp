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

#include "message_queue_loop.hpp"
#include <gott/thread/message_queue.hpp>
#include <gott/xany/xany.hpp>
#include <time.h>

using gott::events::message_queue_loop;
using gott::xany::Xany;

class message_queue_loop::impl {
public:
  impl() : running(false), wait(0) {}

  bool running;
  gott::thread::message_queue<gott::xany::Xany> queue;
  sigc::signal1<void, gott::xany::Xany const &> on_receive_;
  sigc::signal0<void> on_idle_;
  int wait;
};

message_queue_loop::message_queue_loop() : p(new impl) {}

message_queue_loop::~message_queue_loop() {
  on_destroy().emit();
}

sigc::signal0<void> &message_queue_loop::on_idle() {
  return p->on_idle_;
}

sigc::signal1<void, gott::xany::Xany const &> &message_queue_loop::on_receive(){
  return p->on_receive_;
}

void message_queue_loop::add_waitable() {
  ++p->wait;
}

void message_queue_loop::remove_waitable() {
  --p->wait;
}

void message_queue_loop::quit_local() {
  p->running = false;
}

bool message_queue_loop::running() const {
  return p->running && p->wait > 0;
}

void message_queue_loop::run() {
  p->running = true;
  while (running()) {
    p->on_receive_.emit(p->queue.pop());
    p->on_idle_.emit();
  }
  p->running = false;
}

void message_queue_loop::send(Xany const &val) throw() {
  p->queue.push(val);
}

void *message_queue_loop::do_feature(gott::QID const &qid) {
  GOTT_EVENTS_FEATURE(qid,inprocess_message_manager);
  return 0;
}