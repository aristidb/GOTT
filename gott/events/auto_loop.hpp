// Copyright (C) 2005-2006 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: GOTT main loop spawner
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