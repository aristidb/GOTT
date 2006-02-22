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

#include <gott/thread/message_queue.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/barrier.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/if.hpp>
#include <iostream>

using gott::thread::message_queue;
using gott::thread::message_filter;

using namespace boost::lambda;
using boost::ref;
using namespace std;

typedef message_queue<int, 6> mq_t; 
  // 6 is the minimum size indeed (filter)
//typedef message_queue<int> mq_t;

typedef message_queue<int, 0, std::greater<int> > mq2_t;

boost::barrier step2(2), step3(2);

void consumer(volatile mq_t &mq, volatile mq2_t &mq2) {
  mq.wait_for_all(cout << _1 << ' ', _1 != 0);

  cout << endl;

  mq.wait_for_all(
    (
      bind(&mq_t::push, ref(mq), _1 + 1), 
      cout << _1 << '\n'
    ),
    _1 != 5
  );

  cout << endl;

  step2.wait();
  cout << "Got over barrier!" << endl;

  for (int i = 1; i < 10; ++i)
    mq.push(i);
  mq.push(0);


  for (int i = 0; i < 5; ++i)
    mq2.push(i);
  for (int i = 10; i >= 5; --i)
    mq2.push(i);

  step3.wait();
}

int main() {
  volatile mq_t mq;
  volatile mq2_t mq2;
  
  boost::thread thrd(bind(&consumer, ref(mq), ref(mq2)));
  
  for (int i = 80; i >= 0; --i)
    mq.push(i);

  mq.push(0);

  step2.wait();

  mq.filter_all(cout << _1 << ' ', message_filter(_1 == 0, _1 % 2 == 0));

  cout << "/ " << flush;

  mq.wait_for_all(cout << _1 << ' ', _1 < 9);
  cout << endl;

  step3.wait();

  cout << "Priority:" << endl;
  mq2.wait_for_all(cout << _1 << ' ', _1 < 10);
  cout << endl;

  thrd.join();
}
