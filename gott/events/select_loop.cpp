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
 * Andreas Pokorny (andreas.pokorny@gmail.com )
 * Portions created by the Initial Developer are Copyright (C) 2005-2006
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *  Andreas Pokorny (andreas.pokorny@gmail.com)
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

#include "select_loop.hpp"
#include <gott/syswrap/select_unix.hpp>
#include <gott/syswrap/errno.hpp>
#include <errno.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/utility/in_place_factory.hpp>

namespace gott{namespace events{
select_loop::select_loop() 
    : message_mgr(this), sig_mgr(&message_mgr), running(false) {
}

void *select_loop::do_feature(gott::QID const &type) {
  GOTT_EVENTS_FEATURE(type,fd_manager);
  GOTT_EVENTS_FEATURE(type,timer_manager);
  if (type == inprocess_message_manager::qid)
    return &message_mgr;
  if (type == signal_manager::qid)
    return &sig_mgr;
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
    FD_SET(fd, &fd_sets.read_fds);
  if (mask & fd_manager::write)
    FD_SET(fd, &fd_sets.write_fds);
  if (mask & fd_manager::exception)
    FD_SET(fd, &fd_sets.except_fds);
}

void select_loop::remove_fd( int fd ) {
  callback_map::iterator it = callbacks.find( fd );
  if (it == callbacks.end())
    throw fd_manager::installation_error();
  wait_fds.erase(fd);
  callbacks.erase( it );
  FD_CLR(fd,&fd_sets.read_fds);
  FD_CLR(fd,&fd_sets.write_fds);
  FD_CLR(fd,&fd_sets.except_fds);
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
      num_fd = select_unix(
          n, &fd_sets.read_fds,
          &fd_sets.write_fds, &fd_sets.except_fds,
          t);

      for( callback_map::const_iterator it = callbacks.begin(), 
            e = callbacks.end(); 
         num_fd && it!=e;
         ++it)  {
        unsigned mask = 0;
        if( FD_ISSET( it->first, &fd_sets.read_fds ) ) 
          mask |= fd_manager::read;
        if( FD_ISSET( it->first, &fd_sets.write_fds) )
          mask |= fd_manager::write;
        if( FD_ISSET( it->first, &fd_sets.except_fds) ) 
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
        FD_SET( it->first, &fd_sets.write_fds );
      if( it->second.mask & fd_manager::read )
        FD_SET( it->first, &fd_sets.read_fds );
      if( it->second.mask & fd_manager::exception )
        FD_SET( it->first, &fd_sets.except_fds );
    }

    n = callbacks.rbegin()->first + 1;
  }

  running = false;
}

void select_loop::quit_local() {
  running = false;
}

select_loop::~select_loop() {
  for( callback_map::const_iterator it = callbacks.begin(), e = callbacks.end();
      it!=e;
      ++it)  {
    FD_CLR(it->first,&fd_sets.read_fds);
    FD_CLR(it->first,&fd_sets.write_fds);
    FD_CLR(it->first,&fd_sets.except_fds);
  }
}

}}
