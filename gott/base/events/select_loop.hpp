#ifndef GOTT_BASE_EVENTS_SELECT_LOOP_HPP_INCLUDED
#define GOTT_BASE_EVENTS_SELECT_LOOP_HPP_INCLUDED

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

#include <map> 
#include <algorithm>
#include <queue>
#include <sys/select.h> 
#include <boost/function.hpp>
#include <gott/visibility.hpp>
#include "deadline_timer.hpp"
#include "main_loop.hpp"

// TODO: wrong namespace, and wrong place for this file?
namespace gott{namespace events{

/**
 * \brief select_loop encapsulates a main loop that uses pselect to dispatch events
 * select_loop is not copyable.
 */
class select_loop : public main_loop {
  private:
    struct handler {  // is local allowed here?
      boost::function<void()> on_read, on_write, on_exception;
    };
    fd_set read_fds, write_fds, except_fds;
    typedef std::map<int, handler > callback_map;
    typedef std::priority_queue<deadline_timer,std::vector<deadline_timer>,std::greater<deadline_timer> > queue_type;
    callback_map callbacks;
    queue_type timed_events;

    select_loop( select_loop const& cp );
    select_loop& operator=( select_loop const& cp );

    timeval handle_timed_events();
  public:
    select_loop() GOTT_EXPORT;
    ~select_loop() GOTT_EXPORT;

    /**
     * \brief add a gott::events::deadline_timer to the looop
     */
    void add_timer( deadline_timer const& timer ) GOTT_EXPORT;
    /**
     * \name callback registration and removal methods
     * There are three possible incomming events for each 
     * file descriptor. ...
     */
    //\{
    /**
     * \brief adds a file descriptor to watch and register a callback for incomming data events.
     * If the file descriptor already exists only the ready-for-reading callback is replaced.
     * \param[in] fd file descriptor of the socket to watch
     * \param[in] on_read callback called whenever data is waiting at the socket.
     */
    void add_read_fd( int fd, boost::function<void()> on_read ) GOTT_EXPORT;
    /**
     * \brief adds a file descriptor to watch and registers a write callback.
     * If the file descriptor already exists only the write callback is replaced.
     * \param[in] fd file descriptor of the socket to watch
     * \param[in] on_write called whenever the socket is ready for writing
     */
    void add_write_fd( int fd, boost::function<void()> on_write ) GOTT_EXPORT;
    /**
     * \brief adds a file descriptor to watch and registers a write callback.
     * If the file descriptor already exists only the excetion callback is replaced.
     * \param[in] fd file descriptor of the socket to watch
     * \param[in] on_exception  will be called if exceptions happen on that socket
     */
    void add_exception_fd( int fd, boost::function<void()> on_exception ) GOTT_EXPORT;
    void add_fd( int fd, boost::function<void()> on_read, boost::function<void()> on_write, boost::function<void()> on_exception ) GOTT_EXPORT;
    void remove_fd( int fd ) GOTT_EXPORT;
    //\}

    void run() GOTT_EXPORT;

private:
    void *do_feature(std::type_info const &);
};

}}

#endif

