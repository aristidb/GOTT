// Copyright (C) 2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: Filesystem notification library
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

#ifndef GOTT_NOTIFY_FS_WATCH_HPP
#define GOTT_NOTIFY_FS_WATCH_HPP

#include "event.hpp"
#include <gott/visibility.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <sigc++/signal.h>
#include <stdexcept>

namespace gott {
namespace notify_fs {

class watch_implementation;

class watch : boost::noncopyable {
public:
  GOTT_EXPORT watch(gott::string const &path, ev_t mask);
  ~watch() {}
  sigc::signal1<void, event const &> on_fire() const {
    return fire;
  }

private:
  sigc::signal1<void, event const &> fire;
  boost::scoped_ptr<watch_implementation> p;
};

class GOTT_EXPORT watch_installation_failure : public std::runtime_error {
public:
  watch_installation_failure(gott::string const &file);
  ~watch_installation_failure() throw();
};

class GOTT_EXPORT watch_implementation {
public:
  virtual ~watch_implementation() = 0;
};

}}

#endif
