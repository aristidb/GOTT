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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  021110307  USA

#ifndef GOTT_EVENTS_MESSAGE_QUEUE_HPP
#define GOTT_EVENTS_MESSAGE_QUEUE_HPP

#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/function.hpp>
#include <deque>

namespace gott {
namespace events {

/// Return type of a filter callback.
enum message_filter_result { 
  /// The message is not accepted and should be ignored.
  message_out_filter, 
  /// The message is accepted by the filter and should be removed.
  message_in_filter, 
  /// The message is not accepted and do not process further messages.
  message_quit 
};

/**
 * Thread-safe message queue.
 * \param Message The type of the messages.
 * \param Size The maximal size of the queue or 0 (default value) if unlimited.
 */
template<class Message, unsigned Size = 0>
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
    wait_notfull(lock);
    push_unsafe(msg);
    slot_filled();
  }

  /**
   * Get and remove the first message of this queue.
   * \return The removed message.
   */
  Message pop() {
    boost::mutex::scoped_lock lock(monitor_lock);
    wait_nonempty(lock);
    Message result = pop_unsafe();
    slot_free();
    return result;
  }

  /**
   * Get the first message of this queue but do not remove it.
   * \return A constant reference to the message to glance at.
   */
  Message const &peek() const {
    boost::mutex::scoped_lock lock(monitor_lock);
    wait_nonempty(lock);
    return peek_unsafe();
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
   * Send messages to a callback until it returns false.
   * \param func The receiving callback.
   */
  template<class T>
  void wait_for_all(T func) {
    boost::mutex::scoped_lock lock(monitor_lock);

    for (;;) {
      wait_nonempty(lock);

      Message m = pop_unsafe();
      slot_free();

      lock.unlock();
      if (!func(m))
        break;
      lock.lock();
    }
  }

  /**
   * Wait until there are messages available and then send them all at once
   * to a supplied callback.
   * \param func The receiving callback.
   */
  template<class T>
  void wait_for_many(T func) {
    boost::mutex::scoped_lock lock(monitor_lock);

    wait_nonempty(lock);

    while (!empty_unsafe()) {
      Message m = pop_unsafe();
      lock.unlock();
      func(m);
      lock.lock();
    }

    many_free();
  }

  /**
   * Wait until there are messages available and then send them to a supplied
   * callback until the callback returns false.
   * \param func The receiving callback.
   */
  template<class T>
  void wait_for_many_break(T func) {
    boost::mutex::scoped_lock lock(monitor_lock);

    wait_nonempty(lock);

    while (!empty_unsafe()) {
      Message m = pop_unsafe();
      lock.unlock();
      if (!func(m))
        break;
      lock.lock();
    }

    if (!lock.locked())
      lock.lock();

    many_free();
  }

public:
  /**
   * Remove the first message if a callback returns true.
   * \return Whether a message was removed.
   */
  template<class T>
  bool pop_if(T func) {
    boost::mutex::scoped_lock lock(monitor_lock);

    wait_nonempty(lock);

    Message const &m = peek_unsafe();
    lock.unlock();
    bool result = func(m);
    lock.lock();

    if (result) {
      pop_unsafe();
      slot_free();
    }
    return result;
  }

private:
  template<class T>
  bool filter_unsafe(T func) {
    bool do_again = true;

    typename std::deque<Message>::iterator it = queue.begin();
    while (it != queue.end()) {
      message_filter_result result = func(*it);

      if (result == message_quit) {
        do_again = false;
        break;
      }

      if (result == message_in_filter)
        it = queue.erase(it);
      else
        ++it;
    }

    any_change().notify_all();

    return do_again;
  }

public:
  /**
   * Wait until there are messages available and then send them to a supplied
   * callback, which returns a message_filter_result and remove only those for which the
   * callback returned message_in_filter. If the callback returned 
   * message_quit, return early.
   * The callback must not access the message queue.
   * Returns immediately if there is data available but none gets through the
   * filter and none is classified as quit-message.
   * \param func The filtering and receiving callback.
   * \return Whether no filter returned quit.
   */
  template<class T>
  bool filter(T func) {
    boost::mutex::scoped_lock lock(monitor_lock);
    wait_nonempty(lock);
    return filter_unsafe(func);
  }

  /**
   * Send all messages to a callback until it returns message_quit and remove 
   * those messages for which the callback returned message_in_filter.
   * The callback must not access the message queue.
   * \param func The filtering and receiving callback.
   */
  template<class T>
  void filter_all(T func) {
    boost::mutex::scoped_lock lock(monitor_lock);
    wait_nonempty(lock);
    while (filter_unsafe(func))
      any_change().wait(lock);
  }

  template<class T, class U, class R, R True>
  struct add_predicate {
    add_predicate(T f, U p) : func(f), pred(p) {}
    T func;
    U pred;
    R operator() (Message const &m) const {
      R result = pred(m);
      if (result != True)
        return result;
      func(m);
      return True;
    }
  };

public:
  /**
   * Send message to a callback until a predicate decides it should not.
   * \param func The receiving callback.
   * \param pred The predicate. Return false to make this function return.
   */
  template<class T, class U>
  void wait_for_all(T func, U pred) {
    wait_for_all(add_predicate<T, U, bool, true>(func, pred));
  }

  /**
   * Wait until there are messages available and then send them to a supplied
   * callback until a predicate returns false.
   * \param func The receiving callback.
   * \param pred The predicate.
   */
  template<class T, class U>
  void wait_for_many_break(T func, U pred) {
    wait_for_many_break(add_predicate<T, U, bool, true>(func, pred));
  }

  /**
   * Wait until there are messages available and then send them to a supplied
   * callback and remove them only if a predicate returns message_in_filter. 
   * Returns if the messages are exhausted or the predicate returns 
   * message_quit.
   * Neither the callback nor the predicate must access the message queue.
   * \param func The receiving callback.
   * \param pred The predicate.
   * \return Whether no filter returned quit.
   */
  template<class T, class U>
  bool filter(T func, U pred) {
    return filter(
        add_predicate<T, U, message_filter_result, message_in_filter>
          (func, pred));
  }

  /**
   * Send all messages through a predicate until the same returns message_quit 
   * and if the predicate returns message_in_filter, remove them and send them 
   * to a callback.
   * Neither the callback nor the predicate must access the message queue.
   */
  template<class T, class U>
  void filter_all(T func, U pred) {
    filter_all(
        add_predicate<T, U, message_filter_result, message_in_filter>
          (func, pred));
  }

private:
  void wait_nonempty(boost::mutex::scoped_lock &lock) const {
    while (empty_unsafe())
      not_empty().wait(lock);
  }

  void wait_notfull(boost::mutex::scoped_lock &lock) const {
    while (full_unsafe())
      not_full().wait(lock);
  }

private:
  void slot_free() {
    if (Size != 0)
      not_full().notify_one();
  }

  void many_free() {
    if (Size != 0)
      not_full().notify_all();
  }

  void slot_filled() {
    not_empty().notify_one();
  }

private:
  void push_unsafe(Message const &msg) {
    queue.push_back(msg);
    any_change().notify_all();
  }

  Message pop_unsafe() {
    Message result = queue.front();
    queue.pop_front();
    any_change().notify_all();
    return result;
  }

  Message const &peek_unsafe() const {
    return queue.front();
  }

  bool empty_unsafe() const {
    return queue.empty();
  }

  bool full_unsafe() const {
    return Size != 0 ? queue.size() >= (Size == 0 ? 1 : Size) : false;
  }

private:
  mutable boost::mutex monitor_lock;

private:
  boost::condition &not_empty() const { return cond[0]; }
  boost::condition &not_full() const { return cond[2]; }
  boost::condition &any_change() const { return cond[1]; }

  mutable boost::condition cond[Size == 0 ? 2 : 3];
  
private:
  std::deque<Message> queue;
};

template<class QuitPred, class FilterPred>
class message_filter_adapter {
public:
  message_filter_adapter(QuitPred q, FilterPred fi)
  : quit(q), filter(fi) {}

  template<class Message>
  message_filter_result operator() (Message const &m) const {
    if (quit(m))
      return message_quit;
    else if (filter(m))
      return message_in_filter;
    else
      return message_out_filter;
  }

private:
  QuitPred quit;
  FilterPred filter;
};

template<class T, class U>
message_filter_adapter<T, U> 
message_filter(T quit, U filter) {
  return message_filter_adapter<T, U>(quit, filter);
}

}}

#endif
