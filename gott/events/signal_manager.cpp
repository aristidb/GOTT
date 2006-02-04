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
#include <stdexcept>
#include <signal.h>
#include <map>

using gott::events::signal_manager;

signal_manager::signal_manager() {}
signal_manager::~signal_manager() {}

gott::QID const signal_manager::qid("gott::events::signal_manager");

namespace {
  typedef std::map<int, signal_manager *> handler_map;
  handler_map handlers;
}

void signal_manager::register_signal(int sig, signal_manager *handler) {
  if (handlers.find(sig) != handlers.end())
    throw std::runtime_error(
        "cannot register more than one signal_manager per signal");
  handlers[sig] = handler;
  signal(sig, signal_handler);
}

void signal_manager::unregister_all(signal_manager *handler) {
  handler_map::iterator it = handlers.begin();
  while (it != handlers.end())
    if (it->second == handler) {
      handler_map::iterator del = it++;
      handlers.erase(del);
    } else
      ++it;
}

signal_manager *signal_manager::find(int sig) {
  return handlers[sig];
}

void signal_manager::signal_handler(int sig) {
  find(sig)->immediate_action(sig);
}
