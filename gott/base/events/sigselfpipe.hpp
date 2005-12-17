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

#ifndef GOTT_BASE_EVENTS_SIGSELFPIPE_HPP
#define GOTT_BASE_EVENTS_SIGSELFPIPE_HPP

#include "signal_manager.hpp"
#include "fd_manager.hpp"
#include <ntl.h>

namespace gott {
namespace events {

/**
 * Default feature implementation for signals.
 */
class GOTT_LOCAL sigselfpipe : public signal_manager {
public:
  sigselfpipe(fd_manager *fdm);
  ~sigselfpipe();
  sigc::signal1<void,int> &on_signal(int);
private:
  int selfpipe[2];
  void notify_in();
  VectorMap<int, sigc::signal1<void,int> > handlers;
  static void signal_handler(int); 
};

}}

#endif
