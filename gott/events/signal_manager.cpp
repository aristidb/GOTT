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
 * Portions created by the Initial Developer are Copyright (C) 2005-2006
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

#include "signal_manager.hpp"
#include "main_loop.hpp"
#include "inprocess_message_manager.hpp"
#include <gott/exceptions.hpp>
#include <signal.h>
#include <map>

using gott::events::signal_manager;
using gott::xany::Xany;

signal_manager::signal_manager(sigsafe_message_manager *mm)
: message_manager(mm) {
  message_manager->on_receive().connect(
      sigc::mem_fun(this, &signal_manager::receive_message));
}
signal_manager::~signal_manager() {
  unregister_all(this);
}

gott::QID const signal_manager::qid("gott::events::signal_manager");

sigc::signal1<void, int> &signal_manager::on_signal(int sig) {
  std::map<int, sigc::signal1<void, int> >::iterator pos = handlers.find(sig);
  if (pos == handlers.end()) {
    register_signal(sig, this);
    return handlers[sig];
  }
  return pos->second;
}

namespace {
  typedef std::map<int, signal_manager *> handler_map;
  handler_map sys_handlers;
}

void signal_manager::register_signal(int sig, signal_manager *handler) {
  if (sys_handlers.find(sig) != sys_handlers.end())
    throw user_error(
        "cannot register more than one signal_manager per signal");
  sys_handlers[sig] = handler;
  signal(sig, signal_handler);
}

void signal_manager::unregister_all(signal_manager *handler) {
  handler_map::iterator it = sys_handlers.begin();
  while (it != sys_handlers.end())
    if (it->second == handler) {
      handler_map::iterator del = it++;
      sys_handlers.erase(del);
    } else
      ++it;
}

signal_manager *signal_manager::find(int sig) {
  return sys_handlers[sig];
}

void signal_manager::signal_handler(int sig) {
  find(sig)->immediate_action(sig);
}

namespace {
  struct signal_msg { 
    int sig; 
    bool operator==(signal_msg const &o) const { return sig == o.sig; };
  };
}

void signal_manager::immediate_action(int sig) {
  signal_msg s = { sig };
  message_manager->send(Xany(s));
}

void signal_manager::receive_message(Xany const &m) {
  if (m.compatible<signal_msg>()) {
    int sig = xany::Xany_cast<signal_msg>(m).sig;
    handlers[sig].emit(sig);
  }
}

void signal_manager::proxy_t::operator() (main_loop &m) const {
  signal_manager &s = m.feature<signal_manager>();
  for (std::map<int, sigc::signal1<void, int> >::const_iterator it = db.begin();
      it != db.end(); ++it)
    s.on_signal(it->first).connect(it->second);
}
