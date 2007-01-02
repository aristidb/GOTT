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
 * The Original Code is An Event Handling Class Library.
 *
 * The Initial Developer of the Original Code is
 * Rüdiger Sonderfeld <kingruedi@c-plusplus.de>.
 * Portions created by the Initial Developer are Copyright (C) 2006, 2007
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *  Rüdiger Sonderfeld <kingruedi@c-plusplus.de>
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

#include "kqueue_loop.hpp"
#include <gott/syswrap/kqueue_bsd.hpp>
#include <gott/syswrap/scoped_unix_file.hpp>
#include <set>
#include <map>

#include <gott/notify_fs/event.hpp>

#include <gott/plugin/plugin_builder.hpp>
#include <gott/plugin/metadata.hpp>

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

GOTT_PLUGIN_MAKE_BUILDER_SIMPLE(plugin, gott::events::kqueue_loop)

GOTT_PLUGIN_METADATA(
"plugin gott::events::kqueue_loop\n"
"interface gott::events::main_loop\n"
"module-file ??FILE??\n"
"symbol plugin\n"
"priority high\n"
"feature gott::events::fd_manager\n"
"feature gott::events::timer_manager\n"
"feature gott::events::inprocess_message_manager\n"
"feature gott::events::signal_manager\n\n"
)

namespace gott {
namespace events {
  class kqueue_loop::impl {
  public:
    bool running;
    scoped_unix_file queue;

    // fd_manager
    struct callback {
      callback(unsigned mask_, boost::function<void (unsigned)> const &c,
	       bool wait_)
	: mask(mask_), call(c), wait(wait_)
      { }

      unsigned mask;
      boost::function<void (unsigned)> call;
      bool wait;
    };
    typedef std::map<int,callback> map_fd_cb;
    map_fd_cb fd_callbacks;

    unsigned wait;

    // signal_manager
    typedef std::map<int, boost::signal<void (int)> > map_sig_hnd;
    map_sig_hnd signals;

    // notify fs
    map_fd_cb notify_fs;

    // main_loop
    boost::signal<void ()> on_idle;

    impl()
      : running(false),
        queue(kqueue::create_bsd()),
	wait(0)
    {}
  };

  kqueue_loop::kqueue_loop()
    : standard_timer_manager(this),
      p(new impl),
      message_mgr(this)
  {}

  kqueue_loop::~kqueue_loop() {
    on_destroy()();
  }

  void *kqueue_loop::do_feature(gott::QID const &qid) {
    GOTT_EVENTS_FEATURE(qid, fd_manager);
    GOTT_EVENTS_FEATURE(qid, timer_manager);
    GOTT_EVENTS_FEATURE(qid, signal_manager);
    if (qid == inprocess_message_manager::qid)
      return &message_mgr;
    return 0;
  }

  boost::signal<void (int)> &kqueue_loop::on_signal(int sig) {
    impl::map_sig_hnd::iterator i = p->signals.find(sig);
    if (i == p->signals.end()) {
      struct kevent n;
      EV_SET(&n, sig, EVFILT_SIGNAL, EV_ADD | EV_ENABLE, 0, 0, 0);
      kqueue::event_bsd(p->queue.access(), &n, 1, 0, 0, 0);
      return p->signals[sig];
    }
    return i->second;
  }

  void kqueue_loop::add_waitable() {
    ++p->wait;
  }

  void kqueue_loop::remove_waitable() {
    assert(p->wait > 0);
    --p->wait;
  }

  void kqueue_loop::add_fd(int fd, unsigned mask, 
        		   boost::function<void (unsigned)> const &cb,
        		   bool wait)
  {
    if (mask & fd_manager::read || mask & fd_manager::exception) {
      struct kevent n;
      EV_SET(&n, fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
      kqueue::event_bsd(p->queue.access(), &n, 1, 0, 0, 0);
    }
    if (mask & fd_manager::write) {
      struct kevent n;
      EV_SET(&n, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, 0);
      kqueue::event_bsd(p->queue.access(), &n, 1, 0, 0, 0);
    }

    if (wait)
      add_waitable();

    p->fd_callbacks.insert(std::make_pair(fd, impl::callback(mask, cb, wait)));
  }

  void kqueue_loop::remove_fd(int fd) {
    impl::map_fd_cb::iterator i=p->fd_callbacks.find(fd);
    if (i == p->fd_callbacks.end())
      throw system_error("could not remove fd");

    if (i->second.mask & fd_manager::read ||
       i->second.mask & fd_manager::exception) {
      struct kevent n;
      EV_SET(&n, fd, EVFILT_READ, EV_DELETE, 0, 0, 0);
      kqueue::event_bsd(p->queue.access(), &n, 1, 0, 0, 0);
    }
    if (i->second.mask & fd_manager::write) {
      struct kevent n;
      EV_SET(&n, fd, EVFILT_WRITE, EV_DELETE, 0, 0, 0);
      kqueue::event_bsd(p->queue.access(), &n, 1, 0, 0, 0);
    }

    if (i->second.wait)
      remove_waitable();

    p->fd_callbacks.erase(i);
  }

  namespace {
    GOTT_LOCAL u_int ev_t2kqueue(unsigned mask) {
      u_int fflags = 0;
      if (mask & notify_fs::file_delete)
	fflags |= NOTE_DELETE;
      if (mask & notify_fs::file_move)
	fflags |= NOTE_RENAME;
      if (mask & notify_fs::file_modify)
	fflags |= NOTE_WRITE;
      if (mask & notify_fs::file_attrib)
	fflags |= NOTE_ATTRIB;
      if (mask & notify_fs::file_access_revoke)
	fflags |= NOTE_REVOKE;
      if (mask & notify_fs::file_link)
	fflags |= NOTE_LINK;
      if (mask & notify_fs::file_extend)
	fflags |= NOTE_EXTEND;
      return fflags;
    }
  }

  void kqueue_loop::watch_fd(int fd, unsigned mask,
			     boost::function<void (unsigned)> const &cb,
			     bool wait)
  {
    struct kevent e;
    EV_SET(&e, fd, EVFILT_VNODE, EV_ADD | EV_ENABLE |
	   (mask & notify_fs::flag_oneshot ? EV_ONESHOT : 0),
	   ev_t2kqueue(mask), 0, 0);

    p->notify_fs.insert(std::make_pair(fd, impl::callback(mask, cb, wait)));
    if (wait)
      add_waitable();

    kqueue::event_bsd(p->queue.access(), &e, 1, 0, 0, 0);
  }

  void kqueue_loop::unwatch_fd(int fd) {
    impl::map_fd_cb::iterator i = p->notify_fs.find(fd);
    if (i == p->notify_fs.end())
      return;

    struct kevent e;
    EV_SET(&e, fd, EVFILT_VNODE, EV_DELETE, ev_t2kqueue(i->second.mask), 0, 0);
    kqueue::event_bsd(p->queue.access(), &e, 1, 0, 0, 0);

    if (i->second.wait)
      remove_waitable();
    p->notify_fs.erase(i);
  }

  boost::signal<void ()> &kqueue_loop::on_idle() {
    return p->on_idle;
  }

  void kqueue_loop::quit_local() {
    p->running = false;
  }

  namespace {
    GOTT_LOCAL unsigned kqueue2ev_t(u_int mask) {
      unsigned ev_t_ = 0;
      if (mask & NOTE_DELETE)
	ev_t_ |= notify_fs::file_delete;
      if (mask & NOTE_RENAME)
	ev_t_ |= notify_fs::file_move;
      if (mask & NOTE_WRITE)
	ev_t_ |= notify_fs::file_modify;
      if (mask & NOTE_ATTRIB)
	ev_t_ |= notify_fs::file_attrib;
      if (mask & NOTE_REVOKE)
	ev_t_ |= notify_fs::file_access_revoke;
      if (mask & NOTE_LINK)
	ev_t_ |= notify_fs::file_link;
      if (mask & NOTE_EXTEND)
	ev_t_ |= notify_fs::file_extend;
      return ev_t_;
    }
  }

  bool kqueue_loop::running() const {
    return p->running && p->wait > 0;
  }

  void kqueue_loop::run() {
    p->running = true;
    enum { EVENTS_N=64 };
    struct kevent event_list[EVENTS_N];
    while(running()) {
      timespec tm;
      bool has_timers_mem = has_timers();
      if (has_timers_mem) {
        handle_pending_timers();
      	tm.tv_sec = 0;
      	tm.tv_nsec = int(std::min(time_left().total_nanoseconds(), 
        boost::int64_t(INT_MAX)));
      }

      p->on_idle.emit();
    
      if (!p->running || p->wait <= 0)
	break;

      int n=kqueue::event_bsd(p->queue.access(), 0, 0, event_list, EVENTS_N,
        		      has_timers_mem ? &tm : 0x0); //FIXME: EINTR?
      for(int i=0; i<n; ++i) {        
        if (event_list[i].filter == EVFILT_SIGNAL) {
          impl::map_sig_hnd::iterator j = p->signals.find(event_list[i].ident);
          if (j != p->signals.end())
	    j->second(event_list[i].ident);
        }
	else if (event_list[i].filter == EVFILT_VNODE) {
	  impl::map_fd_cb::iterator j=p->notify_fs.find(event_list[i].ident);
	  if (j == p->notify_fs.end())
	    continue;
	  j->second.call(kqueue2ev_t(event_list[i].fflags));
	}
	else {
	  impl::map_fd_cb::iterator j=p->fd_callbacks.find(event_list[i].ident);
	  if (j == p->fd_callbacks.end())
	    continue;
	  if (event_list[i].filter == EVFILT_READ) {
	     assert(j->second.mask & fd_manager::read ||
		    j->second.mask & fd_manager::exception);
	    j->second.call(fd_manager::read);
	  }
	  else if (event_list[i].filter == EVFILT_WRITE) {
	    assert(j->second.mask & fd_manager::write);
	    j->second.call(fd_manager::write);
	  }
	}
      }
    }
  }
}}
