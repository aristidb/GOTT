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
#include <gott/string/qid.hpp>
#include <boost/function.hpp>

namespace gott {
namespace events {

/**
 * Feature for main_loops able to watch file descriptors (*NIX) or sockets 
 * (*NIX + Win32).
 */
class GOTT_EXPORT fd_manager {
public:
  /// Constructor.
  fd_manager();
  /// Pure virtual destructor.
  virtual ~fd_manager() = 0;

  static QID const qid;

  /// Event on a file descriptor.
  enum event {
    /// A read has occurred.
    read = 1,
    /// A write has occurred.
    write = 2,
    /// Something other happened.
    exception = 4,
    /// Anything happened.
    any = read | write | exception
  };

  class GOTT_EXPORT installation_error {};

  /**
   * Add a file descriptor to watch.
   * \param fd The file descriptor of the soon watched object.
   * \param mask Any combination of read, write and exception.
   *           Determines which events are caught for this descriptor.
   * \param cb Callback called whenever an event happens on this descriptor
   *           and it is in the @p mask. Its parameter will be a combination
   *           of the events that occurred.
   * \throw installation_error If the descriptor is already registered or
   *           something else happens.
   */
  virtual void add_fd(int fd, unsigned mask, 
      boost::function<void (unsigned)> const &cb) = 0;

#if 0
  /**
   * Change an already watched file descriptor to watch.
   * \param fd The file descriptor of the watched object.
   * \param mask Any combination of read, write and exception.
   *           Determines which events are caught for this descriptor.
   * \param cb Callback called whenever an event happens on this descriptor
   *           and it is in the @p mask.
   * \throw installation_error If the descriptor is not yet registered or
   *           something else happens.
   */ 
  virtual void change_fd(int fd, unsigned mask,
      boost::function<void (event)> const &cb) = 0;
#endif

  /**
   * Remove a file descriptor from the set of watched objects.
   * \param fd The file descriptor of the watched objects.
   * \throw installation_error If the registered was not registered.
   */
  virtual void remove_fd(int fd) = 0;
};

}}

#endif
