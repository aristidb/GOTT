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

#include "engine.hpp"
#include "../watch.hpp"
#include <gott/syswrap/read_write_unix.hpp>
#include <gott/syswrap/inotify_linux.hpp>
#include <gott/exceptions.hpp>
#include <algorithm>
#include <unistd.h>

#include <iostream>

using gott::string;
using gott::notify_fs::inotify_engine;
using gott::notify_fs::watch_implementation;
using gott::notify_fs::ev_t;
using gott::notify_fs::watch;

inotify_engine::inotify_engine() : conn(inotify_init_linux()) {
  std::cout << "Inotify up and running..." << std::endl;
}

inotify_engine::~inotify_engine() {
  std::cout << "Shut down Inotify." << std::endl;
}

typedef sigc::signal1<void, gott::notify_fs::event const &> sgnl;

boost::int32_t inotify_engine::get_watch(inotify_engine &eng, 
    string const &path, ev_t mask) {
  char *c_path = new char[path.size() + 1];
  c_path[path.size()] = '\0';
  std::copy(path.as_utf8().begin(), path.as_utf8().end(), c_path);
  boost::uint32_t result;
  try {
    result = gott::inotify_add_watch_linux(eng.conn.access(), c_path, mask);
  } catch (gott::system_error const &) {
    delete [] c_path;
    throw gott::notify_fs::watch_installation_failure(path);
  }
  delete [] c_path;
  return result;
}

struct inotify_engine::inotify_watch : watch_implementation {
  inotify_engine &eng;
  boost::int32_t wd;
  watch &context;

  inotify_watch(inotify_engine *e, string const &path, ev_t mask, watch &w)
  : eng(*e), wd(inotify_engine::get_watch(eng, path, mask)), context(w) {}

  ~inotify_watch() {
    std::cout << "Remove watch from " << &eng.watches << " : " << wd 
      << std::endl;
    eng.watches.erase(wd);
    gott::inotify_rm_watch_linux(eng.conn.access(), wd);
  }
};

watch_implementation *
inotify_engine::watch_alloc(string const &path, ev_t mask, watch *w) {
  inotify_watch *p = new inotify_watch(this, path, mask, *w);
  std::cout << "Add watch to " << &watches << " : " << p->wd << std::endl;
  watches[p->wd] = p;
  return p;
}

void inotify_engine::notify() {
  std::cout << "New events:" << std::endl;
  char buffer[16384];
  size_t r = read_unix(conn.access(), buffer, sizeof(buffer));
  for (size_t i = 0; i < size_t(r);) {
    inotify_event *pevent = reinterpret_cast<inotify_event *>(buffer + i);
    if (i + sizeof(inotify_event) >= sizeof(buffer) 
        || i + sizeof(inotify_event) + pevent->len >= sizeof(buffer)) {
      std::cout << "inotify: moveback" << std::endl;
      size_t rest = sizeof(buffer) - i;
      memmove(pevent, buffer, rest);
      i = 0;
      r = read_unix(conn.access(), buffer + rest, sizeof(buffer) - rest) + rest;
      continue;
    }
    i += sizeof(inotify_event) + pevent->len;
    if (pevent->mask & IN_Q_OVERFLOW)
      return;
    if (pevent->wd == -1) {
      std::cout << "inotify: Common event " << pevent->mask << std::endl;
      continue;
    }
    event ev = {
      static_cast<inotify_watch *>(watches[pevent->wd])->context,
      ev_t(pevent->mask),
      pevent->cookie,
      pevent->len ? gott::string(pevent->name, gott::utf8) : gott::string()
    };
    ev.context.on_fire().emit(ev);
  }
}

gott::QID inotify_engine::class_id() const {
  return "gott::notify_fs::inotify_engine";
}
