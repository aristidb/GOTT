// Copyright (C) 2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: GOTT main loop spawner
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

#ifndef GOTT_BASE_EVENTS_AUTO_LOOP_HPP
#define GOTT_BASE_EVENTS_AUTO_LOOP_HPP

#include <typeinfo>
#include <gott/visibility.hpp>
#include <sigc++/signal.h>

namespace gott {
namespace events {
class main_loop;

/**
 * Responsible for automatically creating any needed main_loops.
 */
class auto_loop {
public:
  auto_loop() GOTT_EXPORT;
  ~auto_loop() GOTT_EXPORT;

  /**
   * Create and run the requested main_loops but do not use this thread.
   */
  void spawn_noblock() GOTT_EXPORT;

  /**
   * Create and run the requested main_loops using the current thread.
   */
  void spawn_block() GOTT_EXPORT;

  GOTT_EXPORT
  sigc::signal1<void, main_loop &> &require_feature(std::type_info const &f);

  template<class T>
  sigc::signal1<void, main_loop &> &require_feature() {
    return require_feature(typeid(T));
  }
};

}}

#endif
