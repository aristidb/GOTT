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

#ifndef GOTT_EVENTS_MESSAGE_QUEUE_HPP
#define GOTT_EVENTS_MESSAGE_QUEUE_HPP

#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <queue>

namespace gott {
namespace events {

/**
 * Thread-safe message queue.
 */
template<class Message, int Size = -1>
class message_queue : boost::noncopyable {
public:
  message_queue() {}

public:
  void push(Message const &m) {
    boost::mutex::scoped_lock lock(monitor_lock);

    while (full_unsafe())
      not_full.wait();

    push_unsafe(m);

    not_empty.notify_one();
  }

  Message pop() {
    boost::mutex::scoped_lock lock(monitor_lock);

    while (empty_unsafe())
      not_empty().wait();

    Message result = pop_unsafe();

    not_full.notify_one();

    return result;
  }

  bool empty() {
    boost::mutex::scoped_lock lock(monitor_lock);
    return empty_unsafe();
  }

  bool full() {
    boost::mutex::scoped_lock lock(monitor_lock);
    return full_unsafe();
  }

private:
  void push_unsafe(Message const &m) {
    queue.push(m);
  }

  Message pop_unsafe() {
    Message result = queue.top();
    queue.pop();
    return result;
  }

  bool empty_unsafe() {
    return queue.empty();
  }

  bool full_unsafe() {
    if (Size == -1)
      return false;
    return queue.size() >= Size;
  }

private:
  boost::mutex monitor_lock;
  boost::condition not_empty;
  boost::condition not_full;

private:
  std::queue<Message> queue;
};

}}

#endif
