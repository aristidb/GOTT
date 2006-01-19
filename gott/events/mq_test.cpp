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
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <iostream>

using gott::events::message_queue;
using namespace boost::lambda;
using boost::ref;

typedef message_queue<int, 4> mq_t;

void consumer(mq_t &mq) {
  mq.wait_for_all(std::cout << _1 << ' ', _1 != 0);
  std::cout << std::endl;
  mq.wait_for_all(
    (
      bind(&mq_t::push, ref(mq), _1 + 1), 
      std::cout << _1 << '\n'
    ),
    _1 != 5
  );
}

int main() {
  mq_t mq;
  
  boost::thread thrd(bind(&consumer, ref(mq)));
  
  for (int i = 80; i >= 0; --i)
    mq.push(i);

  mq.push(0);

  thrd.join();
}
