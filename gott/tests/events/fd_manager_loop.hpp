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
 * The Original Code is GOTT Tests.
 *
 * The Initial Developer of the Original Code is
 * Ruediger Sonderfeld (kingruedi@c-plusplus.de).
 * Portions created by the Initial Developer are Copyright (C) 2006
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *  Ruediger Sonderfeld (kingruedi@c-plusplus.de)
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

#include <gott/plugin.hpp>
#include <gott/events/main_loop.hpp>
#include <gott/events/fd_manager.hpp>

#include <boost/utility/in_place_factory.hpp>

using namespace gott;
using namespace gott::events;

static char const helloworld_lit[] = "Hallo Welt!";

class fd_manager_loop_test : public CxxTest::TestSuite
{
  int pipes[2];
  int reads;
  boost::optional<gott::plugin::plugin_handle<main_loop> > ploop;

public:
  void setUp() {
    reads = 0;
#ifdef HAS_UNISTD_H
    pipe_unix(pipes);
#endif
  }

  void tearDown() {
#ifdef HAS_UNISTD_H
    ploop = boost::none;
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
      int presleep_time,
      int no_initial,
      std::set<int> repeaters,
      int cancel,
      int quit = -1) {
#ifdef HAS_UNISTD_H
    TS_ASSERT(ploop);

    main_loop *loop = ploop->get();

    loop->feature<fd_manager>().add_fd(
        pipes[0],
        fd_manager::read,
        boost::bind(
          &fd_manager_loop_test::read_event,
          this,
          loop,
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

  void create_loop(QID const &which) {
    TS_ASSERT_THROWS_NOTHING(
        ploop = boost::in_place(gott::metadata::find_plugin(
            gott::metadata::tags::plugin_id = which)));
  }
  
  void select_loop() { create_loop("gott::events::select_loop"); }
  void epoll_loop() { create_loop("gott::events::epoll_loop"); }
  void kqueue_loop() { create_loop("gott::events::kqueue_loop"); }

  void test_read1_select_loop() {
#ifdef HAS_UNISTD_H
    select_loop();
    meta_reader(1, 2, boost::assign::list_of(2), 3);
#endif
  }

  void test_read1_epoll_loop() {
#ifdef BUILD_EPOLL
    epoll_loop();
    meta_reader(1, 2, boost::assign::list_of(2), 3);
#endif
  }

  void test_read1_kqueue_loop() {
#ifdef BUILD_KQUEUE
    kqueue_loop();
    meta_reader(1, 2, boost::assign::list_of(2), 3);
#endif
  }

  void test_read2_select_loop() {
#ifdef HAS_UNISTD_H
    select_loop();
    meta_reader(-1, 2, boost::assign::list_of(2), 3);
#endif
  }

  void test_read2_epoll_loop() {
#ifdef BUILD_EPOLL
    epoll_loop();
    meta_reader(-1, 2, boost::assign::list_of(2), 3);
#endif
  }

  void test_read2_kqueue_loop() {
#ifdef BUILD_KQUEUE
    kqueue_loop();
    meta_reader(-1, 2, boost::assign::list_of(2), 3);
#endif
  }

  void test_read3_select_loop() {
#ifdef HAS_UNISTD_H
    select_loop();
    meta_reader(0, 2, boost::assign::list_of(2), -1, 3);
#endif
  }

  void test_read3_epoll_loop() {
#ifdef BUILD_EPOLL
    epoll_loop();
    meta_reader(0, 2, boost::assign::list_of(2), -1, 3);
#endif
  }

  void test_read3_kqueue_loop() {
#ifdef BUILD_KQUEUE
    kqueue_loop();
    meta_reader(0, 2, boost::assign::list_of(2), -1, 3);
#endif
  }

  void test_read4_select_loop() {
#ifdef HAS_UNISTD_H
    select_loop();
    meta_reader(0, 3, boost::assign::list_of(2)(3), 5);
#endif
  }

  void test_read4_epoll_loop() {
#ifdef BUILD_EPOLL
    epoll_loop();
    meta_reader(-1, 3, boost::assign::list_of(2)(3), 5);
#endif
  }

  void test_read4_kqueue_loop() {
#ifdef BUILD_KQUEUE
    kqueue_loop();
    meta_reader(-1, 3, boost::assign::list_of(2)(3), 5);
#endif
  }
};
