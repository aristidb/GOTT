#include <cxxtest/TestSuite.h>

#ifdef HAS_UNISTD_H
#include <unistd.h>
#endif
#include <iostream>
#include <set>

#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/assign/list_of.hpp>

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

static char const helloworld_lit[] = "Hallo Welt!";

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
  void read_event(
      main_loop *ml,
      std::set<int> repeaters,
      int cancel,
      int quit,
      unsigned mask) {
    ++reads;
    TS_ASSERT_EQUALS(mask, unsigned(fd_manager::read));
    char buffer[12];
    ssize_t n = read_unix(pipes[0], buffer);
    TS_ASSERT_LESS_THAN(0, n);
    TS_ASSERT(gott::string(helloworld_lit) == buffer);
    if (reads == quit)
      ml->quit_local();
    if (reads == cancel)
      ml->feature<fd_manager>().remove_fd(pipes[0]);
    if (repeaters.count(reads) > 0)
      write_unix(pipes[1], helloworld_lit);
  }

  void write_thread(int t, int n) {
    if (t >= 0) sleep(t);
    for (int i = 0; i < n; ++i)
      write_unix(pipes[1], helloworld_lit);
  }
#endif

  void meta_reader(
      main_loop *ploop,
      int presleep_time,
      int no_initial,
      std::set<int> repeaters,
      int cancel,
      int quit = -1) {
#ifdef HAS_UNISTD_H
    boost::scoped_ptr<main_loop> loop(ploop);

    loop->feature<fd_manager>().add_fd(
        pipes[0],
        fd_manager::read,
        boost::bind(
          &fd_manager_loop_test::read_event,
          this,
          loop.get(),
          repeaters,
          cancel,
          quit,
          _1));

    boost::thread thrd(boost::bind(
          &fd_manager_loop_test::write_thread,
          this,
          presleep_time,
          no_initial));

    loop->run();

    int expected_total_count = std::min(cancel, quit);
    if (cancel == -1) expected_total_count = quit;
    if (quit == -1) expected_total_count = cancel;
    TS_ASSERT_EQUALS(reads, expected_total_count);

    thrd.join();
#endif
  }

  void test_read1_select_loop() {
#ifdef HAS_UNISTD_H
    meta_reader(new select_loop, 1, 2, boost::assign::list_of(2), 3);
#endif
  }

  void test_read1_epoll_loop() {
#ifdef BUILD_EPOLL
    meta_reader(new epoll_loop, 1, 2, boost::assign::list_of(2), 3);
#endif
  }

  void test_read1_kqueue_loop() {
#ifdef BUILD_KQUEUE
    meta_reader(new kqueue_loop, 1, 2, boost::assign::list_of(2), 3);
#endif
  }

  void test_read2_select_loop() {
#ifdef HAS_UNISTD_H
    meta_reader(new select_loop, -1, 2, boost::assign::list_of(2), 3);
#endif
  }

  void test_read2_epoll_loop() {
#ifdef BUILD_EPOLL
    meta_reader(new epoll_loop, -1, 2, boost::assign::list_of(2), 3);
#endif
  }

  void test_read2_kqueue_loop() {
#ifdef BUILD_KQUEUE
    meta_reader(new kqueue_loop, -1, 2, boost::assign::list_of(2), 3);
#endif
  }

  void test_read3_select_loop() {
#ifdef HAS_UNISTD_H
    meta_reader(new select_loop, 0, 2, boost::assign::list_of(2), -1, 3);
#endif
  }

  void test_read3_epoll_loop() {
#ifdef BUILD_EPOLL
    meta_reader(new epoll_loop, 0, 2, boost::assign::list_of(2), -1, 3);
#endif
  }

  void test_read3_kqueue_loop() {
#ifdef BUILD_KQUEUE
    meta_reader(new kqueue_loop, 0, 2, boost::assign::list_of(2), -1, 3);
#endif
  }

  void test_read4_select_loop() {
#ifdef HAS_UNISTD_H
    meta_reader(new select_loop, 0, 3, boost::assign::list_of(2)(3), 5);
#endif
  }

  void test_read4_epoll_loop() {
#ifdef BUILD_EPOLL
    meta_reader(new epoll_loop, -1, 3, boost::assign::list_of(2)(3), 5);
#endif
  }

  void test_read4_kqueue_loop() {
#ifdef BUILD_KQUEUE
    meta_reader(new kqueue_loop, -1, 3, boost::assign::list_of(2)(3), 5);
#endif
  }
};
