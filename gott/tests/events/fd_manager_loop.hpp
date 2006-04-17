#include <cxxtest/TestSuite.h>

#ifdef HAS_UNISTD_H
#include <unistd.h>
#endif
#include <iostream>
#include <cstring>

#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include <gott/syswrap/pipe_unix.hpp>
#include <gott/syswrap/read_write_unix.hpp>

#include <gott/events/select_loop.hpp>
#include <gott/events/kqueue_loop.hpp>
#include <gott/events/epoll_loop.hpp>

using namespace gott;
using namespace gott::events;

class fd_manager_loop_test : public CxxTest::TestSuite
{
public:
  static void read_event(main_loop *ml, int fd, unsigned mask) {
    TS_ASSERT(mask == fd_manager::read);
    char buffer[12];
    ssize_t n = read_unix(fd, buffer);
    TS_ASSERT(n > 0);
    TS_ASSERT(gott::string("Hallo Welt!") == gott::string(buffer));
    ml->quit_local();
  }

  static void write_thread(int fd) {
    write_unix(fd, "Hallo Welt!");
  }

  void reader(main_loop *ploop) {
#ifdef HAS_UNISTD_H
    boost::scoped_ptr<main_loop> loop(ploop);

    int pipes[2];
    pipe_unix(pipes);

    boost::thread thrd(boost::bind(fd_manager_loop_test::write_thread,
        			   pipes[1]));

    loop->feature<fd_manager>().add_fd
      (pipes[0], fd_manager::read, boost::bind
       (fd_manager_loop_test::read_event, loop.get(), pipes[0], _1));
    loop->run();

    thrd.join();
#endif
  }

  void test_select_loop_read() {
    reader(new select_loop);
  }

  void test_epoll_loop() {
#ifdef BUILD_EPOLL
    reader(new epoll_loop);
#endif
  }

  void test_kqueue_loop() {
#ifdef BUILD_KQUEUE
    reader(new kqueue_loop);
#endif
  }
};

