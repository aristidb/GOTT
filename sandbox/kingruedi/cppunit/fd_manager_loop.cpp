#ifdef HAS_UNISTD_H

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <unistd.h>
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

class fd_manager_loop_test
  : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(fd_manager_loop_test);
    CPPUNIT_TEST(test_select_loop_read);
#ifdef BUILD_EPOLL
    CPPUNIT_TEST(test_epoll_loop);
#endif
#ifdef BUILD_KQUEUE
    CPPUNIT_TEST(test_kqueue_loop);
#endif
  CPPUNIT_TEST_SUITE_END();
public:
  void setUp() {
  }

  void tearDown() {
  }

  static void read_event(main_loop *ml, int fd, unsigned mask) {
    CPPUNIT_ASSERT(mask == fd_manager::read);
    char buffer[12];
    ssize_t n = read_unix(fd, buffer);
    CPPUNIT_ASSERT(n > 0);
    CPPUNIT_ASSERT_EQUAL(gott::string("Hallo Welt!"), gott::string(buffer));
    ml->quit_local();
  }

  static void write_thread(int fd) {
    write_unix(fd, "Hallo Welt!");
  }

  void test_read(main_loop *ploop) {
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
  }

  void test_select_loop_read() {
    test_read(new select_loop);
  }

  void test_epoll_loop() {
#ifdef BUILD_EPOLL
    test_read(new epoll_loop);
#endif
  }

  void test_kqueue_loop() {
#ifdef BUILD_KQUEUE
    test_read(new kqueue_loop);
#endif
  }
};

CPPUNIT_TEST_SUITE_REGISTRATION(fd_manager_loop_test);

#endif
