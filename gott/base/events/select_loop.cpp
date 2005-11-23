// Copyright (C) 2004-2005 by Andreas Pokorny andreas.pokorny@gmail.com
// Content: GOTT select loop
// Authors: Andreas Pokorny
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

#include <boost/date_time/posix_time/posix_time.hpp>
#include "select_loop.hpp"

namespace gott{namespace events{
select_loop::select_loop() {
  FD_ZERO(&read_fds);
  FD_ZERO(&write_fds);
  FD_ZERO(&except_fds);
}


void select_loop::add_timer( deadline_timer const& timer ) {
  if( ! timer.timer.is_not_a_date_time() ) {
    timed_events.push( timer );
  }
}
void select_loop::add_read_fd( int fd, boost::function<void()> fun ) {
  FD_SET(fd, &read_fds);
  callbacks[fd].on_read = fun;
}

void select_loop::add_write_fd( int fd, boost::function<void()> fun ) {
  FD_SET(fd, &write_fds);
  callbacks[fd].on_write = fun;
}


void select_loop::add_exception_fd( int fd, boost::function<void()> fun ) {
  FD_SET(fd, &except_fds);
  callbacks[fd].on_exception = fun;
}

void select_loop::add_fd( int fd, boost::function<void()> o_r, boost::function<void()> o_w, boost::function<void()> o_e ) {
  FD_SET(fd, &read_fds);
  FD_SET(fd, &write_fds);
  FD_SET(fd, &except_fds);
  callbacks[fd].on_read = o_r;
  callbacks[fd].on_write = o_w;
  callbacks[fd].on_exception = o_e;
}


void select_loop::remove_fd( int fd ) {
  callback_map::iterator it = callbacks.find( fd );
  if( it != callbacks.end() ) {
    callbacks.erase( it );
    FD_CLR(fd,&read_fds);
    FD_CLR(fd,&write_fds);
    FD_CLR(fd,&except_fds);
  }
}

timeval select_loop::handle_timed_events(){
  using namespace boost::posix_time;
  timeval ret;
  ret.tv_sec = ret.tv_usec = 0;
  if( ! timed_events.empty() ){
    ptime now( microsec_clock::local_time() );
    time_duration td = timed_events.top().timer - now;
    for( ; td.is_negative() || td.total_seconds() == 0 ; td = timed_events.top().timer - now  ) {
      deadline_timer::handler_type cp = timed_events.top().handler;
      timed_events.pop();
      add_timer( cp() );
    }
    ret.tv_sec = td.total_seconds();
    ret.tv_usec = td.fractional_seconds();
    //long    tv_sec;   seconds 
    //long    tv_usec;  microseconds 
  }
  return ret;
}

void select_loop::run(){
  using namespace boost::posix_time;
  size_t n = callbacks.rbegin()->first + 1;
  timeval next_event = handle_timed_events(), *t=0; 

  if( next_event.tv_sec ||next_event.tv_usec ) 
    t = &next_event;

  int num_fd;
  while( ( !timed_events.empty() || !callbacks.empty() )  
      && ( num_fd = select( n, &read_fds, &write_fds, &except_fds, t ) ) != -1 ) 
  {
    for( callback_map::const_iterator it = callbacks.begin(), e = callbacks.end(); num_fd && it!=e;++it)  {
      if( FD_ISSET( it->first, &read_fds ) ) {
        --num_fd;
        it->second.on_read();
      }
      if( FD_ISSET( it->first, &write_fds) ) 
        --num_fd,it->second.on_write();
      if( FD_ISSET( it->first, &except_fds) ) 
        --num_fd,it->second.on_exception();
    }
    next_event = handle_timed_events();

    if( next_event.tv_sec ||next_event.tv_usec )  {
      t = &next_event;
    }
    else {
      t = 0;
    }
    for( callback_map::const_iterator it = callbacks.begin(), e = callbacks.end(); num_fd && it!=e;++it)  {
      if( it->second.on_write )
        FD_SET( it->first, &write_fds );
      if( it->second.on_read )
        FD_SET( it->first, &read_fds );
      if( it->second.on_exception )
        FD_SET( it->first, &except_fds );
    }

    n = callbacks.rbegin()->first + 1;
  }
}

select_loop::~select_loop() {
  for( callback_map::const_iterator it = callbacks.begin(), e = callbacks.end(); it!=e;++it)  {
    FD_CLR(it->first,&read_fds);
    FD_CLR(it->first,&write_fds);
    FD_CLR(it->first,&except_fds);
  }
}

}}
