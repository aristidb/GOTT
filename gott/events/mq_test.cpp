// Copyright (C) 2006 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: GOTT message queue
// Authors: Aristid Breitkreuz
//
// This file is part of the Gott Project (http://gott.sf.net)
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include <gott/events/message_queue.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/barrier.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/if.hpp>
#include <iostream>

using gott::events::message_queue;
using gott::events::message_filter;

using namespace boost::lambda;
using boost::ref;
using namespace std;

typedef message_queue<int, 10> mq_t;
//typedef message_queue<int> mq_t;

boost::barrier step2(2);

void consumer(mq_t &mq) {
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
}

int main() {
  mq_t mq;
  
  boost::thread thrd(bind(&consumer, ref(mq)));
  
  for (int i = 80; i >= 0; --i)
    mq.push(i);

  mq.push(0);

  step2.wait();

  mq.filter_all(cout << _1 << ' ', message_filter(_1 == 0, _1 % 2 == 0));

  cout << "/ " << flush;

  mq.wait_for_all(cout << _1 << ' ', _1 < 9);
  cout << endl;

  thrd.join();
}
