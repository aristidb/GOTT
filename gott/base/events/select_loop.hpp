#ifndef GOTT_BASE_EVENTS_SELECT_LOOP_HPP_INCLUDED
#define GOTT_BASE_EVENTS_SELECT_LOOP_HPP_INCLUDED

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

#include <map> 
#include <algorithm>
#include <queue>
#include <sys/select.h> 
#include <boost/function.hpp>
#include <boost/optional/optional.hpp>
#include <gott/visibility.hpp>
#include "deadline_timer.hpp"
#include "main_loop.hpp"
#include "fd_manager.hpp"
#include "timer_manager.hpp"
#include "sigselfpipe.hpp"

// TODO: wrong namespace, and wrong place for this file?
namespace gott{namespace events{

/**
 * Encapsulates a main loop that uses select to dispatch events.
 * Not copyable.
 */
class GOTT_EXPORT select_loop 
    : public main_loop, public fd_manager, public timer_manager {
  private:
    struct GOTT_LOCAL handler {
      boost::function<void()> on_read, on_write, on_exception;
    };
    fd_set read_fds, write_fds, except_fds;
    typedef std::map<int, handler > callback_map;
    typedef std::priority_queue<
          deadline_timer,std::vector<deadline_timer>,
          std::greater<deadline_timer> > 
        queue_type;
    callback_map callbacks;
    queue_type timed_events;
    volatile bool running; // thread-safe?!
    ::boost::optional<sigselfpipe> sigmgr;

    GOTT_LOCAL select_loop( select_loop const& cp );
    GOTT_LOCAL select_loop& operator=( select_loop const& cp );

    GOTT_LOCAL timeval handle_timed_events();
  public:
    select_loop();
    ~select_loop();

    void add_timer( deadline_timer const &timer );

    void add_read_fd(int fd, boost::function<void()> const &on_read);
    void add_write_fd( int fd, boost::function<void()> const &on_write);
    void add_exception_fd(int fd, boost::function<void()> const &on_exception);
    void add_fd(int fd, 
        boost::function<void()> const &on_read, 
        boost::function<void()> const &on_write, 
        boost::function<void()> const &on_exception );
    void remove_fd( int fd );

    void run();
    void quit();

private:
    GOTT_LOCAL void *do_feature(std::type_info const &);
};

}}

#endif

