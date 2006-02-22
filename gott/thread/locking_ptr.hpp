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
 * The Original Code is Small Compiler-Aided Multi-Threading Utilities.
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

#ifndef GOTT_THREAD_LOCKING_PTR_HPP
#define GOTT_THREAD_LOCKING_PTR_HPP

#include <boost/thread/mutex.hpp>
#include <boost/noncopyable.hpp>

namespace gott {
namespace thread {

/**
 * Wrapper for safely accessing thread-shared data. Inspired by Alexandrescu's
 * LockingPtr<>. Use volatile automatic variables of non-integrated type and
 * access them via this template and a mutex. The compiler will complain loudly
 * if you don't access the volatile variable correctly. For integrated types,
 * the compiler will simply not complain, which is potentially unsafe.
 *
 * Implements the Lock concept as specified by Boost.Threads.
 *
 * \param T The type of the object to be exposed.
 * \param Mutex The mutex type, which should be compatible to the Mutex
 *          concept as specified by Boost.Threads.
 */
template<class T, class Mutex>
class locking_ptr : boost::noncopyable {
public:
  /**
   * Acquire a lock on the mutex and make the object available. See lock() for
   * what acquiring the mutex means.
   * \param obj The object to be available through this locking_ptr.
   * \param mtx The mutex to be locked.
   */
  locking_ptr(volatile T &obj, Mutex &mtx)
  : p(const_cast<T *>(&obj)), l(mtx) {}

  /** 
   * Destructor. Unlocks the mutex as required by the Lock concept.
   */
  ~locking_ptr() {}
  
  /// Access the object.
  T &operator *() { return *p; }

  /// Access the object.
  T *operator->() { return p; }

public:
  /**
   * Returns type void*, non-zero if the associated mutex object has been locked
   * by clk, otherwise 0. Required by the Lock concept.
   */
  operator const void * const () { return l; }

  /**
   * Returns a bool, operator const void*() != 0. Required by the Lock concept.
   */
  bool locked() const { return l.locked(); }

  /**
   * Locks the associated mutex as required by the Lock concept.
   */
  void lock()  { l.lock(); }

  /**
   * Unlocks the associated mutex as required by the Lock concept.
   */
  void unlock() { l.unlock(); }

private:
  T *p;
  typename Mutex::scoped_lock l;
};

}}

#endif
