#include <gott/notify_fs/kqueue/knotify_engine.hpp>
#include <gott/notify_fs/watch.hpp>
#include <gott/notify_fs/event.hpp>
#include <gott/events/kqueue_loop.hpp>
#include <gott/syswrap/scoped_unix_file.hpp>
#include <gott/syswrap/open_unix.hpp>
#include <boost/scoped_array.hpp>
#include <boost/bind.hpp>

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

    if(event & file_access)
      return false;
    if(event & file_close_write)
      return false;
    if(event & file_close_nowrite)
      return false;
    if(event & file_close)
      return false;
    if(event & file_open)
      return false;
    /*
FIXME: file_move is supported which is a composition of
      file_moved_from = 0x40,
      file_moved_to = 0x80,
which are unsupported
    */
    if(event & subfile_create)
      return false;
    if(event & subfile_delete)
      return false;
    if(event & file_delete)
      return false;
    if(event & extra_unmount)
      return false;
    if(event & extra_queue_overflow)
      return false;
    if(event & exta_ignored)
      return false;
    if(event & flag_directory)
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

      fd.reset(open_unix(c_path.get(), O_RDONLY));

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
