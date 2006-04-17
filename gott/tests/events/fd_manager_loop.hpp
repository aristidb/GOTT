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
#ifdef BUILD_KQUEUE
#include <gott/events/kqueue_loop.hpp>
#endif
#ifdef BUILD_EPOLL
#include <gott/events/epoll_loop.hpp>
#endif

using namespace gott;
using namespace gott::events;

class fd_manager_loop_test : public CxxTest::TestSuite
{
  int pipes[2];
  int reads;

public:
  void setUp() {
    reads = 0;
#ifdef HAS_UNISTD_H
    pipe_unix(pipes);
#endif
  }

  void tearDown() {
#ifdef HAS_UNISTD_H
    close(pipes[0]);
    close(pipes[1]);
    pipes[0] = pipes[1] = -1;
#endif
  }

#ifdef HAS_UNISTD_H
  void read_event(main_loop *ml, unsigned mask) {
    ++reads;
    TS_ASSERT_EQUALS(mask, unsigned(fd_manager::read));
    char buffer[12];
    ssize_t n = read_unix(pipes[0], buffer);
    TS_ASSERT_LESS_THAN(0, n);
    TS_ASSERT(gott::string("Hallo Welt!") == buffer);
    switch (reads) {
    case 2:
      write_unix(pipes[1], "Hallo Welt!");
      break;
    case 3:
      ml->feature<fd_manager>().remove_fd(pipes[0]);
      break;
    }
  }

  void write_thread() {
    sleep(1);
    write_unix(pipes[1], "Hallo Welt!\0Hallo Welt!");
  }
#endif

  void meta_reader(main_loop *ploop) {
#ifdef HAS_UNISTD_H
    boost::scoped_ptr<main_loop> loop(ploop);

    loop->feature<fd_manager>().add_fd
      (pipes[0], fd_manager::read, boost::bind
       (&fd_manager_loop_test::read_event, this, loop.get(), _1));

    boost::thread thrd(boost::bind(&fd_manager_loop_test::write_thread, this));

    loop->run();

    TS_ASSERT_EQUALS(reads, 3);

    thrd.join();
#endif
  }

  void test_read__select_loop() {
#ifdef HAS_UNISTD_H
    meta_reader(new select_loop);
#endif
  }

  void test_read__epoll_loop() {
#ifdef BUILD_EPOLL
    meta_reader(new epoll_loop);
#endif
  }

  void test_read__kqueue_loop() {
#ifdef BUILD_KQUEUE
    meta_reader(new kqueue_loop);
#endif
  }
};

