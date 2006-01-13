#ifndef GOTT_BASE_EVENTS_SELECT_LOOP_HPP_INCLUDED
#define GOTT_BASE_EVENTS_SELECT_LOOP_HPP_INCLUDED

// Copyright (C) 2004-2005 by Andreas Pokorny andreas.pokorny@gmail.com
// Copyright (C) 2005-2006 by Aristid Breitkreuz aribrei@arcor.de
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
#include <set>
#include <sys/select.h> 
#include <boost/function.hpp>
#include <boost/optional/optional.hpp>
#include <gott/visibility.hpp>
#include "deadline_timer.hpp"
#include "main_loop.hpp"
#include "fd_manager.hpp"
#include "timer_manager.hpp"
#include "sigselfpipe.hpp"

namespace gott{namespace events{

/**
 * Encapsulates a main loop that uses select to dispatch events.
 * Not copyable.
 */
class GOTT_EXPORT select_loop 
    : public main_loop, public fd_manager, public standard_timer_manager {
private:
  struct GOTT_LOCAL handler {
    boost::function<void (unsigned)> callback;
    unsigned mask;
  };
  fd_set read_fds, write_fds, except_fds;
  typedef std::map<int, handler> callback_map;
  std::set<int> wait_fds;
  callback_map callbacks;
  bool running;
  ::boost::optional<sigselfpipe> sigmgr;

  GOTT_LOCAL select_loop( select_loop const& cp );
  GOTT_LOCAL select_loop& operator=( select_loop const& cp );
public:
  select_loop();
  ~select_loop();

  void add_fd(int, unsigned, boost::function<void (unsigned)> const &, 
      bool = true);
  void remove_fd(int);

  sigc::signal1<void,int> &on_signal(int sig) {
    return feature<signal_manager>().on_signal(sig);
  }

  void run();
  void quit();

private:
  GOTT_LOCAL void *do_feature(gott::QID const &);
};

}}

#endif

