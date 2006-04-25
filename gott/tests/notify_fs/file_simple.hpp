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
 * Aristid Breitkreuz (aribrei@arcor.de).
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

#include <gott/events/main_loop.hpp>
#include <gott/events/main_loop_factory.hpp>
#include <gott/events/loop_requirement.hpp>
#include <gott/notify_fs/notification_engine.hpp>
#include <gott/notify_fs/watch.hpp>

#include <boost/thread.hpp>
#include <boost/lambda/bind.hpp>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

static char const testfile[] = "/tmp/testfile";

class notify_fs_file_simple_test : public CxxTest::TestSuite
{
  gott::events::main_loop *loop;

public:
  void setUp() {
    gott::events::main_loop_factory fact;
    fact.try_add(gott::events::feature<gott::notify_fs::notification_engine>());
    loop = fact.get_alloc();
    ::creat(testfile, 00777);
  }

  void tearDown() {
    ::unlink(testfile);
    delete loop;
  }

  void test_acquire() {
    TS_ASSERT_THROWS_NOTHING(
        loop->feature<gott::notify_fs::notification_engine>());
  }

  void test_open() {
    meta_test(gott::notify_fs::file_open, &open_file);
  }

  void test_close() {
    meta_test(gott::notify_fs::file_close, &close_file);
  }

  //void test_access() {
  //  meta_test(gott::notify_fs::file_access, &access_file);
  //}

  void test_write() {
    meta_test(gott::notify_fs::file_modify, &write_to_file);
  }

  //void test_metadata() {
  //  meta_test(gott::notify_fs::file_attrib, &stat_file);
  //}

private:
  void meta_test(gott::notify_fs::ev_t event, void (*fun)()) {
    using namespace boost::lambda;
    if(!loop->feature<gott::notify_fs::notification_engine>().support_event
       (event))
      return;
    int encountered = 0;
    gott::notify_fs::watch w(
        loop->feature<gott::notify_fs::notification_engine>(),
        testfile,
        event);
    w.on_fire().connect(bind(
          &notify_fs_file_simple_test::on_event,
          this,
          var(encountered)));
    boost::thread thrd(bind(&helper_thread, fun));
    loop->run();
    TS_ASSERT_EQUALS(encountered, 1);
    thrd.join();
  }

  void on_event(int &no) {
    ++no;
    loop->quit_local();
  }

  static void open_file() {
    ::open(testfile, O_RDONLY);
  }

  static void close_file() {
    ::close(open(testfile, O_RDONLY));
  }

  static void access_file() {
    ::access(testfile, F_OK);
  }

  static void write_to_file() {
    int fd = ::open(testfile, O_WRONLY);
    ::write(fd, "-", 1);
    ::close(fd);
  }

  static void stat_file() {
    struct stat buf;
    ::stat(testfile, &buf);
  }

  static void helper_thread(void (*call)()) {
    sleep(1); // make sure the loop is running
    call();
  }
};
