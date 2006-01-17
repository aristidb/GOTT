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

#include "message_queue.hpp"
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <iostream>

using gott::events::message_queue;

typedef message_queue<int, 50> mq_t;

bool consume(int val) {
  if (val == 0)
    return false;
  std::cout << val << std::endl;
  return true;
}

void consumer(mq_t &mq) {
  mq.wait_for_all(&consume);
}

int main() {
  mq_t mq;
  
  boost::thread thrd(boost::bind(&consumer, boost::ref(mq)));
  
  mq.push(50);
  mq.push(5);
  mq.push(0);

  thrd.join();
}
