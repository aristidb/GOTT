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
 * Rüdiger Sonderfeld <kingruedi@c-plusplus.de>
 * Portions created by the Initial Developer are Copyright (C) 2005
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s): Rüdiger Sonderfeld <kingruedi@c-plusplus.de>
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
#include <gott/notify_fs/kqueue/knotify_engine.hpp>
#include <gott/notify_fs/engine_factory.hpp>
#include <gott/notify_fs/notification_engine.hpp>
#include <gott/notify_fs/watch.hpp>
#include <gott/notify_fs/event.hpp>
#include <gott/events/loops/kqueue_loop.hpp>
#include <gott/syswrap/scoped_unix_file.hpp>
#include <gott/syswrap/open_unix.hpp>
#include <boost/scoped_array.hpp>
#include <boost/bind.hpp>
#include <gott/plugin.hpp>

namespace {
  struct knotify_factory : gott::notify_fs::engine_factory {
    gott::notify_fs::notification_engine *alloc(gott::events::main_loop &loop) const {
      return new gott::notify_fs::knotify_engine(loop);
    }
  };
}

GOTT_PLUGIN_MAKE_BUILDER_SIMPLE(kqueue_plugin, knotify_factory)


namespace gott {
namespace notify_fs {
  knotify_engine::knotify_engine(gott::events::main_loop &m)
    : kq(&dynamic_cast<gott::events::kqueue_loop&>(m))
  { }

  knotify_engine::~knotify_engine() { }

  bool knotify_engine::support_event(ev_t event) const {
    // if you want to change the mapping and add events, see
    // ev_t2kqueue and kqueue2ev_t functions in gott/events/kqueue_loop.cpp

    // Supported:
    // file_delete -> NOTE_DELETE
    // file_move -> NOTE_RENAME
    // file_modify -> NOTE_WRITE
    // file_attrib -> NOTE_ATTRIB
    // file_access_revoke -> NOTE_REVOKE
    // file_link -> NOTE_LINK
    // file_extend -> NOTE_EXTEND

    if (event & file_access)
      return false;
    if (event & file_close_write)
      return false;
    if (event & file_close_nowrite)
      return false;
    if (event & file_close)
      return false;
    if (event & file_open)
      return false;
    /*
FIXME: file_move is supported which is a composition of
      file_moved_from = 0x40,
      file_moved_to = 0x80,
which are unsupported
    */
    if (event & subfile_create)
      return false;
    if (event & subfile_delete)
      return false;
    if (event & file_delete)
      return false;
    if (event & extra_unmount)
      return false;
    if (event & extra_queue_overflow)
      return false;
    if (event & exta_ignored)
      return false;
    if (event & flag_directory)
      return false;
    return true;
  }

  struct knotify_engine::kqueue_watch : public watch_implementation {
    string path;
    scoped_unix_file fd;
    events::kqueue_loop *kq;
    watch *w;

    kqueue_watch(string const &str, ev_t events, watch *this_w, bool wait,
		 events::kqueue_loop *k)
      : path(str),
	kq(k),
	w(this_w)
    {
      boost::scoped_array<char> c_path(new char[path.size() + 1]);
      c_path.get()[path.size()] = '\0';
      std::copy(path.as_utf8().begin(), path.as_utf8().end(), c_path.get());

      try {
	fd.reset(open_unix(c_path.get(), O_RDONLY));
      }
      catch(gott::system_error const&) {
	throw gott::notify_fs::watch_installation_failure(path);
      }

      kq->watch_fd(fd.access(), events,
		   boost::bind(&knotify_engine::kqueue_watch::notify, this,
			       _1),
		   wait);
    }

    // move this into kqueue_loop to reduce indirections?
    void notify(unsigned mask) {
      event e = {
	*w, (ev_t)mask, 0, path
      };
      w->on_fire().emit(e);
    }

    ~kqueue_watch() {
      kq->unwatch_fd(fd.access());
    }
  };

  watch_implementation *knotify_engine::watch_alloc(string const &path,
						    ev_t events,
						    watch *this_w,
						    bool wait)
  {
    return new kqueue_watch(path, events, this_w, wait, kq);
  }
}}
