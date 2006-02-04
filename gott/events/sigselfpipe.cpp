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
 * ***** END LICENSE BLOCK ***** *//

#include "sigselfpipe.hpp"
#include <boost/bind.hpp>
#include <unistd.h>
#include <signal.h>
#include <gott/range_algo.hpp>
#include <boost/optional.hpp>
#include <boost/none.hpp>
#include <exception>

using gott::events::signal_manager;
using gott::events::sigselfpipe;

typedef sigc::signal1<void, int> signl_t;

sigselfpipe::sigselfpipe(fd_manager *fdm) {
  if (pipe(selfpipe) == -1)
    throw std::runtime_error("pipe() failed");
  fdm->add_fd(selfpipe[0], fd_manager::read, 
      boost::bind(&sigselfpipe::notify_in, this), false);
}

sigselfpipe::~sigselfpipe() {
  signal_manager::unregister_all(this);
  close(selfpipe[0]);
  close(selfpipe[1]);
}

signl_t &sigselfpipe::on_signal(int sig) {
  std::map<int, signl_t>::iterator pos = handlers.find(sig);
  if (pos == handlers.end()) {
    signal_manager::register_signal(sig, this);
    return handlers[sig];
  }
  return pos->second;
}

void sigselfpipe::immediate_action(int sig) {
  write(selfpipe[1], &sig, sizeof(sig));
}

void sigselfpipe::notify_in() {
  int sig;
  read(selfpipe[0], &sig, sizeof(sig));
  handlers[sig].emit(sig);
}
