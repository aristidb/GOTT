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
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2006
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
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

#include "message_queue_loop.hpp"
#include <boost/scoped_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>
#include <iostream>
#include <unistd.h>

void timer(gott::xany::Xany const &x, gott::events::main_loop &m) {
  if (x == gott::xany::Xany(0))
    m.quit_local();
  std::cout << x << ' ' << 
    boost::posix_time::microsec_clock::local_time() << std::endl;
}

void pulse(gott::events::main_loop &m) {
  for (int i = 10; i >= 0; --i) {
    m.feature<gott::events::inprocess_message_manager>().send(
        gott::xany::Xany(i));
    sleep(1);
  }
}

int main() {
  boost::scoped_ptr<gott::events::main_loop> m(
    new gott::events::message_queue_loop);
  m->feature<gott::events::inprocess_message_manager>().on_receive().connect(
      boost::bind(&timer, _1, boost::ref(*m)));
  boost::thread(boost::bind(&pulse, boost::ref(*m)));
  m->run();
  std::cout << "Done!" << std::endl;
}
