// Copyright (C) 2004-2005 by Andreas Pokorny andreas.pokorny@gmail.com
// Content: GOTT select loop
// Authors: Andreas Pokorny
//          Aristid Breitkreuz
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

#include "select_loop.hpp"
#include <gott/syswrap/select_unix.hpp>
#include <gott/syswrap/errno.hpp>
#include <errno.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/utility/in_place_factory.hpp>

namespace gott{namespace events{
select_loop::select_loop() : running(false) {
  FD_ZERO(&read_fds);
  FD_ZERO(&write_fds);
  FD_ZERO(&except_fds);
}

void *select_loop::do_feature(gott::QID const &type) {
  GOTT_EVENTS_FEATURE(type,fd_manager);
  GOTT_EVENTS_FEATURE(type,timer_manager);
  if (type == signal_manager::qid) {
    if (!sigmgr) sigmgr = boost::in_place(this);
    return static_cast<signal_manager *>(sigmgr.get_ptr());
  }
  return 0;
}

void select_loop::add_fd(int fd, unsigned mask, 
    boost::function<void (unsigned)> const &fun, bool wait) {
  if (callbacks.find(fd) != callbacks.end())
    throw fd_manager::installation_error();

  if (wait)
    wait_fds.insert(fd);

  handler h;
  h.callback = fun;
  h.mask = mask;
  callbacks.insert(std::make_pair(fd, h));

  if (mask & fd_manager::read)
    FD_SET(fd, &read_fds);
  if (mask & fd_manager::write)
    FD_SET(fd, &write_fds);
  if (mask & fd_manager::exception)
    FD_SET(fd, &except_fds);
}

void select_loop::remove_fd( int fd ) {
  callback_map::iterator it = callbacks.find( fd );
  if (it == callbacks.end())
    throw fd_manager::installation_error();
  wait_fds.erase(fd);
  callbacks.erase( it );
  FD_CLR(fd,&read_fds);
  FD_CLR(fd,&write_fds);
  FD_CLR(fd,&except_fds);
}

void select_loop::run(){
  running = true;

  using namespace boost::posix_time;
  size_t n = callbacks.rbegin()->first + 1;
  timeval next_event, *t; 

  int num_fd;
  while(running) {
    handle_pending_timers();

    if (has_timers()) {
      boost::posix_time::time_duration td = time_left();
      next_event.tv_sec = td.total_seconds();
      next_event.tv_usec = td.fractional_seconds();
      t = &next_event;
    } else
      t = 0;
    
    if (!has_wait_timers() && wait_fds.empty())
      break;
    try {
      num_fd = select_unix(n, &read_fds, &write_fds, &except_fds, t);

      for( callback_map::const_iterator it = callbacks.begin(), 
            e = callbacks.end(); 
         num_fd && it!=e;
         ++it)  {
        unsigned mask = 0;
        if( FD_ISSET( it->first, &read_fds ) ) 
          mask |= fd_manager::read;
        if( FD_ISSET( it->first, &write_fds) )
          mask |= fd_manager::write;
        if( FD_ISSET( it->first, &except_fds) ) 
          mask |= fd_manager::exception;
        if (mask) {
          --num_fd;
          it->second.callback(mask);
        }
      }
    } catch (errno_exception const &e) {
      if (e.number() != EINTR)
        throw;
    }

    for( callback_map::const_iterator it = callbacks.begin(), 
           e = callbacks.end(); 
         it!=e;
         ++it)  {
      if( it->second.mask & fd_manager::write )
        FD_SET( it->first, &write_fds );
      if( it->second.mask & fd_manager::read )
        FD_SET( it->first, &read_fds );
      if( it->second.mask & fd_manager::exception )
        FD_SET( it->first, &except_fds );
    }

    n = callbacks.rbegin()->first + 1;
  }

  running = false;
}

void select_loop::quit() {
  running = false;
}

select_loop::~select_loop() {
  for( callback_map::const_iterator it = callbacks.begin(), e = callbacks.end();
      it!=e;
      ++it)  {
    FD_CLR(it->first,&read_fds);
    FD_CLR(it->first,&write_fds);
    FD_CLR(it->first,&except_fds);
  }
}

}}
