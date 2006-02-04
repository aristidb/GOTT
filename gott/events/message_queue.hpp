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
    wait_for_space_internal(lock);
    push_unsafe(msg);
    slot_filled();
  }

  /**
   * Get and remove the first message of this queue.
   * \return The removed message.
   */
  Message pop() {
    boost::mutex::scoped_lock lock(monitor_lock);
    wait_for_data_internal(lock);
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
    wait_for_data_internal(lock);
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
   * Clear the message queue.
   */
  void drop() {
    boost::mutex::scoped_lock lock(monitor_lock);
    queue.clear();
    many_free();
  }
  
  /**
   * Send messages to a callback until it returns false.
   * \param func The receiving callback.
   */
  template<class T>
  void wait_for_all(T func) {
    boost::mutex::scoped_lock lock(monitor_lock);

    for (;;) {
      wait_for_data_internal(lock);

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

    wait_for_data_internal(lock);

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

    wait_for_data_internal(lock);

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

    wait_for_data_internal(lock);

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
        queue.erase(it);
        slot_free();
        do_again = false;
        break;
      }

      if (result == message_in_filter) {
        it = queue.erase(it);
        slot_free();
      } else
        ++it;
    }

    return do_again;
  }

public:
  /**
   * Wait until there are messages available and then send them to a supplied
   * callback, which returns a message_filter_result and remove only those for 
   * which the callback returned message_in_filter or message_quit. If the 
   * callback returned message_quit, return early.
   * The callback must not access the message queue.
   * Returns immediately if there is data available but none gets through the
   * filter and none is classified as quit-message.
   * \param func The filtering and receiving callback.
   * \return Whether no filter returned quit.
   */
  template<class T>
  bool filter(T func) {
    boost::mutex::scoped_lock lock(monitor_lock);
    wait_for_data_internal(lock);
    return filter_unsafe(func);
  }

  /**
   * Send all messages to a callback until it returns message_quit and remove 
   * those messages for which the callback returned message_in_filter or
   * message_quit.
   * The callback must not access the message queue.
   * \param func The filtering and receiving callback.
   */
  template<class T>
  void filter_all(T func) {
    boost::mutex::scoped_lock lock(monitor_lock);
    wait_for_data_internal(lock);
    while (filter_unsafe(func))
      wait_for_new_data_internal(lock);
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
   * callback and remove them only if a predicate returns message_in_filter
   * or message_quit. 
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
   * to a callback. The message for which the predicate returns message_quit 
   * is also removed. 
   * Neither the callback nor the predicate must access the message queue.
   */
  template<class T, class U>
  void filter_all(T func, U pred) {
    filter_all(
        add_predicate<T, U, message_filter_result, message_in_filter>
          (func, pred));
  }

public:
  /**
   * Waits until there is data available.
   */
  void wait_for_data() const {
    boost::mutex::scoped_lock lock(monitor_lock);
    wait_for_data_internal(lock);
  }

  /**
   * Waits until there is space available.
   */
  void wait_for_space() const {
    boost::mutex::scoped_lock lock(monitor_lock);
    wait_for_space(lock);
  }

  /**
   * Waits until new data floats in.
   */
  void wait_for_new_data() const {
    boost::mutex::scoped_lock lock(monitor_lock);
    wait_for_new_data_internal(lock);
  }

private:
  void wait_for_data_internal(boost::mutex::scoped_lock &lock) const {
    while (empty_unsafe())
      not_empty().wait(lock);
  }

  void wait_for_space_internal(boost::mutex::scoped_lock &lock) const {
    while (full_unsafe())
      not_full().wait(lock);
  }

  void wait_for_new_data_internal(boost::mutex::scoped_lock &lock) const {
    new_data().wait(lock);
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
    new_data().notify_all();
  }

  Message pop_unsafe() {
    Message result = queue.front();
    queue.pop_front();
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
  boost::condition &new_data() const { return cond[1]; }

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
