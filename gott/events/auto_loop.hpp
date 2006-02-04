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
 * Portions created by the Initial Developer are Copyright (C) 2005-2006
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
 * ***** END LICENSE BLOCK ***** *//

#ifndef GOTT_BASE_EVENTS_AUTO_LOOP_HPP
#define GOTT_BASE_EVENTS_AUTO_LOOP_HPP

#include <gott/visibility.hpp>
#include <sigc++/signal.h>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <cstddef>

namespace gott {
class QID;

namespace events {
class main_loop;
class loop_requirement;

/**
 * Responsible for automatically creating any needed main_loops.
 */
class auto_loop : boost::noncopyable {
public:
  auto_loop() GOTT_EXPORT;
  ~auto_loop() GOTT_EXPORT;

  /**
   * Create and run the requested main_loops but do not use this thread.
   */
  void spawn_noblock() GOTT_EXPORT;

  /**
   * Create and run the requested main_loops using the current thread.
   */
  void spawn_block() GOTT_EXPORT;

  /**
   * Specify the maximal number of main_loops created.
   * \param n The maximum or 0 if there shall be no maximum.
   */
  void maximal_loop_count(unsigned n);

  /**
   * Calculate the number of threads that would be spawned if spawn_noblock
   * was to be called now.
   * \return The number of threads.
   */
  std::size_t thread_count() const;

  typedef std::size_t thread_descriptor;

  /**
   * Add a requirement and if necessary a main_loop.
   * \return A descriptor of the thread that will be spawned for this 
   *         requirement.
   */
  GOTT_EXPORT
  thread_descriptor add(loop_requirement const &);

  /**
   * Get the signal emitted before a main_loop is run.
   * \param td The descriptor of the thread where the main_loop will be run.
   * \return A reference to the signal.
   */
  GOTT_EXPORT
  sigc::signal1<void, main_loop &> &before_run(thread_descriptor const &td);
  
  /**
   * Join all main_loop threads. Useful in combination with spawn_noblock.
   */
  GOTT_EXPORT
  void join_all();

  /**
   * Quit all main_loops.
   */
  GOTT_EXPORT
  void quit_all();

public: // internal
  bool try_feature(QID const &, loop_requirement const *);

private:
  class impl;
  boost::scoped_ptr<impl> p;
};

}}

#endif
