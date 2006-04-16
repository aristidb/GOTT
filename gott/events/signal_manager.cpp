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

using namespace gott::events;
using gott::xany::Xany;

gott::QID const signal_manager::qid(
    "gott::events::signal_manager");

signal_manager::signal_manager() {}
signal_manager::~signal_manager() {}

standard_signal_manager::standard_signal_manager(sigsafe_message_manager *mm)
: message_manager(mm) {
  message_manager->on_receive().connect(
      sigc::mem_fun(this, &standard_signal_manager::receive_message));
}

standard_signal_manager::~standard_signal_manager() {
  unregister_all(this);
}

sigc::signal1<void, int> &standard_signal_manager::on_signal(int sig) {
  handler_map_t::iterator pos = handlers.find(sig);
  if (pos == handlers.end()) {
    register_signal(sig, this);
    return handlers[sig];
  }
  return pos->second;
}

namespace {
  typedef std::map<int, std::pair<standard_signal_manager *, void (*)(int)> > 
    sys_handler_map_t;
  sys_handler_map_t sys_handlers;
}

void standard_signal_manager::register_signal(
    int sig, standard_signal_manager *handler) {
  if (sys_handlers.find(sig) != sys_handlers.end())
    throw user_error(
        "cannot register more than one standard_signal_manager per signal");
  sys_handlers[sig] =
    std::make_pair(
        handler,
        signal(sig, signal_handler));
}

void standard_signal_manager::unregister_all(standard_signal_manager *handler) {
  sys_handler_map_t::iterator it = sys_handlers.begin();
  while (it != sys_handlers.end())
    if (it->second.first == handler) {
      signal(it->first, it->second.second); // reinstall old handler
      sys_handler_map_t::iterator del = it++;
      sys_handlers.erase(del);
    } else
      ++it;
}

standard_signal_manager *standard_signal_manager::find(int sig) {
  return sys_handlers[sig].first;
}

void standard_signal_manager::signal_handler(int sig) {
  find(sig)->immediate_action(sig);
}

namespace {
  struct signal_msg { 
    int sig;
    typedef boost::mpl::false_ equality_comparable;
    typedef boost::mpl::false_ printable;
  };
}

void standard_signal_manager::immediate_action(int sig) {
  signal_msg s = { sig };
  message_manager->send(Xany(s));
}

void standard_signal_manager::receive_message(Xany const &m) {
  if (m.compatible<signal_msg>()) {
    int sig = xany::Xany_cast<signal_msg>(m).sig;
    handlers[sig].emit(sig);
  }
}
