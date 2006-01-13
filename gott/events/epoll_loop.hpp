// Copyright (C) 2006 by Aristid Breitkreuz (aribrei@arcor.de)
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

#ifndef GOTT_EVENTS_EPOLL_LOOP_HPP
#define GOTT_EVENTS_EPOLL_LOOP_HPP

#include "main_loop.hpp"
#include "fd_manager.hpp"
#include "timer_manager.hpp"
#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>

namespace gott {
namespace events {

class GOTT_EXPORT epoll_loop
: boost::noncopyable,
  public main_loop,
  public fd_manager,
  public standard_timer_manager {
public:
  epoll_loop();
  ~epoll_loop();

  void run();
  void quit();

  void add_fd(int fd, unsigned mask, boost::function<void (unsigned)> const &);
  void remove_fd(int fd);

private:
  void *do_feature(QID const &);

private:
  class impl;
  boost::scoped_ptr<impl> p;
};

}}

#endif
