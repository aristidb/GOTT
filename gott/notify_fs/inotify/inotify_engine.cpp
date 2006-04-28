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

#include "../notification_engine.hpp"
#include "../watch.hpp"
#include "../engine_factory.hpp"
#include <gott/events/main_loop.hpp>
#include <gott/events/fd_manager.hpp>
#include <gott/syswrap/read_write_unix.hpp>
#include <gott/syswrap/inotify_linux.hpp>
#include <gott/syswrap/scoped_unix_file.hpp>
#include <gott/exceptions.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/scoped_array.hpp>
#include <algorithm>
#include <map>
#include <unistd.h>

using gott::string;
using namespace gott::notify_fs;
using gott::events::main_loop;

namespace gott {
namespace notify_fs {

class inotify_engine : public notification_engine {
public:
  inotify_engine(main_loop &r_loop) 
    : loop(&r_loop),
      fdm(&loop->feature<gott::events::fd_manager>()),
      conn(inotify_init_linux()) {
    using namespace boost::lambda;
    fdm->add_fd(
        conn.access(),
        gott::events::fd_manager::read,
        bind(&inotify_engine:: notify, this),
        false);    
  }

  ~inotify_engine() {
    fdm->remove_fd(conn.access());
  }
 
private:
  bool support_event(ev_t) const { return true; }

  watch_implementation *watch_alloc(string const &, ev_t, watch *, bool);

private:
  main_loop *loop;
  gott::events::fd_manager *fdm;
  scoped_unix_file conn;
  std::map<boost::uint32_t, watch_implementation *> watches;

  void notify();

  static boost::int32_t get_watch(inotify_engine &eng, string const &path, 
    ev_t mask);
  struct inotify_watch;
};

}}

namespace {
struct inotify_factory : engine_factory {
  notification_engine *alloc(main_loop &loop) const {
    return new inotify_engine(loop);
  }
};
}

extern "C"
GOTT_EXPORT 
inotify_factory *inotify_plugin() { return new inotify_factory; }

typedef sigc::signal1<void, gott::notify_fs::event const &> sgnl;

boost::int32_t inotify_engine::get_watch(
    inotify_engine &eng, 
    string const &path,
    ev_t mask) {
  boost::scoped_array<char> c_path(new char[path.size() + 1]);
  c_path.get()[path.size()] = '\0';
  std::copy(path.as_utf8().begin(), path.as_utf8().end(), c_path.get());
  boost::uint32_t result;
  try {
    result = 
      gott::inotify_add_watch_linux(eng.conn.access(), c_path.get(), mask);
  } catch (gott::system_error const &) {
    throw gott::notify_fs::watch_installation_failure(path);
  }
  return result;
}

struct inotify_engine::inotify_watch : watch_implementation {
  inotify_engine &eng;
  boost::int32_t wd;
  watch &context;
  bool wait;

  inotify_watch(
      inotify_engine *e,
      string const &path,
      ev_t mask,
      watch &w,
      bool wait)
  : eng(*e),
    wd(inotify_engine::get_watch(eng, path, mask)),
    context(w),
    wait(wait) {
    if (wait)
      eng.loop->add_waitable();
  }

  ~inotify_watch() {
    if (wait)
      eng.loop->remove_waitable();
    eng.watches.erase(wd);
    gott::inotify_rm_watch_linux(eng.conn.access(), wd);
  }
};

watch_implementation *
inotify_engine::watch_alloc(string const &path, ev_t mask, watch *w, bool wait){
  inotify_watch *pp = new inotify_watch(this, path, mask, *w, wait);
  watches[pp->wd] = pp;
  return pp;
}

void inotify_engine::notify() {
  char buffer[16384];
  size_t r = read_unix(conn.access(), buffer, sizeof(buffer));
  for (size_t i = 0; i < size_t(r);) {
    inotify_event *pevent = reinterpret_cast<inotify_event *>(buffer + i);
    if (i + sizeof(inotify_event) >= sizeof(buffer) 
        || i + sizeof(inotify_event) + pevent->len >= sizeof(buffer)) {
      size_t rest = sizeof(buffer) - i;
      memmove(pevent, buffer, rest);
      i = 0;
      r = read_unix(conn.access(), buffer + rest, sizeof(buffer) - rest)
        + rest;
      continue;
    }
    i += sizeof(inotify_event) + pevent->len;
    if (pevent->mask & IN_Q_OVERFLOW)
      return;
    if (pevent->wd == -1)
      continue;
    event ev = {
      static_cast<inotify_watch *>(watches[pevent->wd])->context,
      ev_t(pevent->mask),
      pevent->cookie,
      pevent->len ? gott::string(pevent->name, gott::utf8) : gott::string()
    };
    ev.context.on_fire().emit(ev);
  }
}

