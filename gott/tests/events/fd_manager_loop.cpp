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
    CPPUNIT_TEST(test_epoll_loop);
    CPPUNIT_TEST(test_kqueue_loop);
  CPPUNIT_TEST_SUITE_END();
public:
  void setUp() {
  }

  void tearDown() {
  }

  static void read_event(main_loop *ml, int fd, unsigned mask) {
    CPPUNIT_ASSERT(mask == fd_manager::read);
    char buffer[12];
    ssize_t n = read_unix(fd, buffer, 12);
    CPPUNIT_ASSERT(n > 0);
    CPPUNIT_ASSERT(std::memcmp(buffer, "Hallo Welt!", 12) == 0);
    ml->quit_local();
  }

  static void write_thread(int fd) {
    write_unix(fd, "Hallo Welt!", 12);
  }

  void test_select_loop_read() {
    int pipes[2];
    pipe_unix(pipes);

    boost::thread thrd(boost::bind(fd_manager_loop_test::write_thread,
				   pipes[1]));

    boost::scoped_ptr<main_loop> loop(new select_loop);
    loop->feature<fd_manager>().add_fd
      (pipes[0], fd_manager::read, boost::bind
       (fd_manager_loop_test::read_event, loop.get(), pipes[0], _1));
    loop->run();

    thrd.join();
  }

  void test_epoll_loop() {
  }

  void test_kqueue_loop() {
  }
};
CPPUNIT_TEST_SUITE_REGISTRATION(fd_manager_loop_test);

#endif
