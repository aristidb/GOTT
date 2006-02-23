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
#include <algorithm>

namespace gott {
namespace thread {

/// Return type of a filter callback.
enum message_filter_result { 
  /// The message is not accepted and should be ignored.
  message_out_filter, 
  /// The message is accepted by the filter and should be removed.
  message_in_filter, 
  /// The message is not accepted and do not process further messages.
  message_quit 
};

struct no_priority {
  template<class T>
  bool operator()(T,T) { return false; } // never called
};

template<class T>
struct care_priority {
  enum { value = 1 };
};

template<>
struct care_priority<no_priority> {
  enum { value = 0 };
};

/**
 * Thread-safe message queue.
 * \param Message The type of the messages.
 * \param Size The maximal size of the queue or 0 (default value) if unlimited.
 * \param PriorityCompare The type of the priority comparer.
 *                        Should return x1.priority < x2.priority.
 *                        Pass no_priority if you want the queue to behave plain
 *                        FIFO (no_priority is the default).
 */
template<class Message, unsigned Size = 0, class PriorityCompare = no_priority>
class message_queue : boost::noncopyable {
public:
  /// Constructor.
  message_queue(PriorityCompare p = PriorityCompare()) : pcmp(p) {}

public:
  /**
   * Add a message to the end of this queue.
   * \param msg The message to add.
   */
  void push(Message const &msg) {
    boost::mutex::scoped_lock lock(monitor_lock);
    wait_for_space_u(lock);
    push_u(msg);
    slot_filled_u();
  }

  /**
   * Get and remove the first message of this queue.
   * \return The removed message.
   */
  Message pop() {
    boost::mutex::scoped_lock lock(monitor_lock);
    wait_for_data_u(lock);
    Message result = pop_u();
    slot_free_u();
    return result;
  }

  /**
   * Get the first message of this queue but do not remove it.
   * \return A constant reference to the message to glance at.
   */
  Message const &peek() const {
    boost::mutex::scoped_lock lock(monitor_lock);
    wait_for_data_u(lock);
    return peek_u();
  }

  /// Check whether the queue is empty.
  bool empty() const {
    boost::mutex::scoped_lock lock(monitor_lock);
    return empty_u();
  }

  /// Check whether the queue is full.
  bool full() const {
    boost::mutex::scoped_lock lock(monitor_lock);
    return full_u();
  }

public:
  /**
   * Clear the message queue.
   */
  void drop() {
    boost::mutex::scoped_lock lock(monitor_lock);
    queue.clear();
    many_free_u();
  }
  
  /**
   * Send messages to a callback until it returns false.
   * \param func The receiving callback.
   */
  template<class T>
  void wait_for_all(T func) {
    boost::mutex::scoped_lock lock(monitor_lock);

    for (;;) {
      wait_for_data_u(lock);

      Message m = pop_u();
      slot_free_u();

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

    wait_for_data_u(lock);

    while (!empty()) {
      Message m = pop_u();
      lock.unlock();
      func(m);
      lock.lock();
    }

    many_free_u();
  }

  /**
   * Wait until there are messages available and then send them to a supplied
   * callback until the callback returns false.
   * \param func The receiving callback.
   */
  template<class T>
  void wait_for_many_break(T func) {
    boost::mutex::scoped_lock lock(monitor_lock);

    wait_for_data_u(lock);

    while (!empty()) {
      Message m = pop_u();
      lock.unlock();
      if (!func(m))
        break;
      lock.lock();
    }

    if (!lock.locked())
      lock.lock();

    many_free_u();
  }

public:
  /**
   * Remove the first message if a callback returns true.
   * \return Whether a message was removed.
   */
  template<class T>
  bool pop_if(T func) {
    boost::mutex::scoped_lock lock(monitor_lock);

    wait_for_data_u(lock);

    Message const &m = peek_u();
    lock.unlock();
    bool result = func(m);
    lock.lock();

    if (result) {
      pop_u();
      slot_free_u();
    }
    return result;
  }

private:
  template<class T>
  bool filter_u(T func, bool strict_priority) {
    bool do_again = true;

    if (care_priority<PriorityCompare>::value && strict_priority)
      std::sort(queue.rbegin(), queue.rend(), pcmp);

    typename std::deque<Message>::iterator it = queue.begin();
    while (it != queue.end()) {
      message_filter_result result = func(*it);

      if (result == message_quit) {
        queue.erase(it);
        slot_free_u();
        do_again = false;
        break;
      }

      if (result == message_in_filter) {
        it = queue.erase(it);
        slot_free_u();
      } else
        ++it;
    }

    if (care_priority<PriorityCompare>::value)
      std::make_heap(queue.begin(), queue.end(), pcmp);

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
   * \param strict_priority Whether all elements should be passed in correct
   *             priority order. This might be slow. This parameter has no 
   *             effect if PriorityCompare is no_priority.
   * \return Whether no filter returned quit.
   */
  template<class T>
  bool filter(T func, bool strict_priority = false) {
    boost::mutex::scoped_lock lock(monitor_lock);
    wait_for_data_u(lock);
    return filter_u(func, strict_priority);
  }

  /**
   * Send all messages to a callback until it returns message_quit and remove 
   * those messages for which the callback returned message_in_filter or
   * message_quit.
   * The callback must not access the message queue.
   * \param func The filtering and receiving callback.
   * \param strict_priority Whether all elements should be passed in correct
   *             priority order. This might be slow. This parameter has no 
   *             effect if PriorityCompare is no_priority.
   */
  template<class T>
  void filter_all(T func, bool strict_priority = false) {
    boost::mutex::scoped_lock lock(monitor_lock);
    wait_for_data_u(lock);
    while (filter_u(func, strict_priority))
      wait_for_new_data_u(lock);
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
   * \param strict_priority Whether all elements should be passed in correct
   *             priority order. This might be slow. This parameter has no 
   *             effect if PriorityCompare is no_priority.
   * \return Whether no filter returned quit.
   */
  template<class T, class U>
  bool filter(T func, U pred, bool strict_priority = false) {
    return filter(
        add_predicate<T, U, message_filter_result, message_in_filter>
          (func, pred),
        strict_priority);
  }

  /**
   * Send all messages through a predicate until the same returns message_quit
   * and if the predicate returns message_in_filter, remove them and send them 
   * to a callback. The message for which the predicate returns message_quit 
   * is also removed. 
   * Neither the callback nor the predicate must access the message queue.
   * \param func The receiving callback.
   * \param pred The predicate.
   * \param strict_priority Whether all elements should be passed in correct
   *             priority order. This might be slow. This parameter has no 
   *             effect if PriorityCompare is no_priority.
   */
  template<class T, class U>
  void filter_all(T func, U pred, bool strict_priority = false) {
    filter_all(
        add_predicate<T, U, message_filter_result, message_in_filter>
          (func, pred),
        strict_priority);
  }

public:
  /**
   * Waits until there is data available.
   */
  void wait_for_data() const {
    boost::mutex::scoped_lock lock(monitor_lock);
    wait_for_data_u(lock);
  }

  /**
   * Waits until there is space available.
   */
  void wait_for_space() const {
    boost::mutex::scoped_lock lock(monitor_lock);
    wait_for_space_u(lock);
  }

  /**
   * Waits until new data floats in.
   */
  void wait_for_new_data() const {
    boost::mutex::scoped_lock lock(monitor_lock);
    wait_for_new_data_u(lock);
  }

private:
  void wait_for_data_u(boost::mutex::scoped_lock &lock) const {
    while (empty_u())
      not_empty_u().wait(lock);
  }

  void wait_for_space_u(boost::mutex::scoped_lock &lock) const {
    while (full_u())
      not_full_u().wait(lock);
  }

  void wait_for_new_data_u(boost::mutex::scoped_lock &lock) const {
    new_data_u().wait(lock);
  }

private:
  void slot_free_u() {
    if (Size != 0)
      not_full_u().notify_one();
  }

  void many_free_u() {
    if (Size != 0)
      not_full_u().notify_all();
  }

  void slot_filled_u() {
    not_empty_u().notify_one();
  }

private:
  void push_u(Message const &msg) {
    queue.push_back(msg);
    if (care_priority<PriorityCompare>::value)
      std::push_heap(queue.begin(), queue.end(), pcmp);
    new_data_u().notify_all();
  }

  Message pop_u() {
    Message result = queue.front();
    if (!care_priority<PriorityCompare>::value) {
      queue.pop_front();
    } else {
      std::pop_heap(queue.begin(), queue.end(), pcmp);
      queue.pop_back();
    }
    return result;
  }

  Message const &peek_u() const {
    return queue.front();
  }

  bool empty_u() const {
    return queue.empty();
  }

  bool full_u() const {
    return Size != 0 ? queue.size() >= (Size == 0 ? 1 : Size) : false;
  }

private:
  mutable boost::mutex monitor_lock;

private:
  boost::condition &not_empty_u() const { return cond[0]; }
  boost::condition &not_full_u() const { return cond[2]; }
  boost::condition &new_data_u() const { return cond[1]; }

  mutable boost::condition cond[Size == 0 ? 2 : 3];
  
private:
  std::deque<Message> queue;
  PriorityCompare const pcmp;
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
