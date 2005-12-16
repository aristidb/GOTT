// Copyright (C) 2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: GOTT main loop
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

#ifndef GOTT_BASE_EVENTS_FD_MANAGER_HPP
#define GOTT_BASE_EVENTS_FD_MANAGER_HPP

#include <gott/visibility.hpp>
#include <boost/function.hpp>

namespace gott {
namespace events {

/**
 * Feature for main_loops able to watch file descriptors (UNIX).
 */
class GOTT_EXPORT fd_manager {
public:
  /// Constructor.
  fd_manager();
  /// Pure virtual destructor.
  virtual ~fd_manager() = 0;

  /**
   * Adds a file descriptor to watch and register a callback for incoming data 
   * events. If the file descriptor already exists only the ready-for-reading
   * callback is replaced.
   * \param fd File descriptor of the socket / device to watch.
   * \param on_read The callback called whenever data is waiting at the file.
   */
  virtual void add_read_fd(int fd, 
      boost::function<void()> const &on_read) = 0;

  /**
   * Adds a file descriptor to watch and registers a write callback.
   * If the file descriptor already exists only the write callback is replaced.
   * \param fd File descriptor of the socket / device to watch.
   * \param on_write The callback called whenever the file is ready for writing.
   */
  virtual void add_write_fd(int fd, 
      boost::function<void()> const &on_write) = 0;

  /**
   * Adds a file descriptor to watch and registers an exception callback. If the
   * file descriptor already exists only the exception callback is replaced.
   * \param fd File descriptor of the socket / device to watch.
   * \param on_exception The callback called whenever exceptions happen on that 
   *                     file.
   */
  virtual void add_exception_fd(int fd, 
      boost::function<void()> const &on_exception) = 0;

  /**
   * Adds a file descriptor to watch and installs callbacks for any event.
   * \param fd File descriptor of the socket / device to watch.
   * \param on_read The callback called whenever data is waiting at the file.
   * \param on_write The callback called whenever the file is ready for writing.
   * \param on_exception  The callback called whenever exceptions happen on that
   *                      file.
   */
  virtual void add_fd(int fd, 
      boost::function<void()> const &on_read,
      boost::function<void()> const &on_write, 
      boost::function<void()> const &on_exception) = 0;

  /**
   * Removes a file descriptor from the watch set.
   * \param fd File descriptor of the socket / device to remove.
   */
  virtual void remove_fd(int fd) = 0;
};

}}

#endif
