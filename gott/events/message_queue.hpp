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
#include <boost/function.hpp>
#include <queue>

namespace gott {
namespace events {

/**
 * Thread-safe message queue.
 */
template<class Message, int Size = -1>
class message_queue : boost::noncopyable {
public:
  /// Default-Constructor.
  message_queue() {}

public:
  /**
   * Add a message to the end of this queue.
   * \param msg The message to add.
   */
  void push(Message const &msg) {
    boost::mutex::scoped_lock lock(monitor_lock);

    while (full_unsafe())
      not_full.wait(lock);

    push_unsafe(msg);

    not_empty.notify_one();
  }

  /**
   * Get and remove the first message of this queue.
   * \return The removed message.
   */
  Message pop() {
    boost::mutex::scoped_lock lock(monitor_lock);

    while (empty_unsafe())
      not_empty.wait(lock);

    Message result = pop_unsafe();

    if (size != -1)
      not_full.notify_one();

    return result;
  }

  /// Check whether the queue is empty.
  bool empty() const {
    boost::mutex::scoped_lock lock(monitor_lock);
    return empty_unsafe();
  }

  /// Check whether the queue is full.
  bool full() const {
    boost::mutex::scoped_lock lock(monitor_lock);
    return full_unsafe();
  }

public:

  /**
   * Wait until there are messages available and then send them all at once
   * to a supplied callback.
   * \param func The callback to send the messages to.
   */
  void wait_for_many(boost::function<void (Message const &)> const &func) {
    boost::mutex::scoped_lock lock(monitor_lock);

    while (empty_unsafe())
      not_empty.wait(lock);

    while (!empty_unsafe())
      func(pop_unsafe());

    not_full.notify_all();
  }

  /**
   * Send messages to a callback until it returns false.
   * \param func The receiving callback.
   */
  void wait_for_all(boost::function<bool (Message const &)> const &func) {
    boost::mutex::scoped_lock lock(monitor_lock);

    for (;;) {
      while (empty_unsafe())
        not_empty.wait(lock);

      if (!func(pop_unsafe()))
        break;

      not_full.notify_one();
    }
  }

private:
  void push_unsafe(Message const &msg) {
    queue.push(msg);
  }

  Message pop_unsafe() {
    Message result = queue.top();
    queue.pop();
    return result;
  }

  bool empty_unsafe() const {
    return queue.empty();
  }

  bool full_unsafe() const {
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
