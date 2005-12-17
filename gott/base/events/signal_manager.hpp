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

#ifndef GOTT_BASE_EVENTS_SIGNAL_MANAGER_HPP
#define GOTT_BASE_EVENTS_SIGNAL_MANAGER_HPP

#include <gott/visibility.hpp>
#include <sigc++/signal.h>

namespace gott {
namespace events {

/**
 * Feature for main_loops able to deal with operating system signals.
 */
class GOTT_EXPORT signal_manager {
public:
  /// Constructor.
  signal_manager();
  /// Pure virtual destructor.
  virtual ~signal_manager() = 0;

  /**
   * In-program signal for signal notifications. Each signal should be 
   * associated to a single main_loop or signal_manager - or a 
   * std::runtime_error will be thrown.
   */
  virtual sigc::signal1<void, int> &on_signal(int sig) = 0;

protected:
  static void register_signal(int sig, signal_manager *handler);
  static void unregister_all(signal_manager *handler);
  static signal_manager *find(int sig);
};

}}

#endif
