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

#ifndef GOTT_NOTIFY_FS_INOTIFY_ENGINE_HPP
#define GOTT_NOTIFY_FS_INOTIFY_ENGINE_HPP

#include "../engine.hpp"
#include <gott/syswrap/scoped_unix_file.hpp>
#include <map>

namespace gott {
namespace notify_fs {

class GOTT_EXPORT inotify_engine : public engine {
public:
  inotify_engine();
  ~inotify_engine();
  watch_implementation *watch_alloc(string const &, ev_t, watch &);

//private:
public: //FIXME => private
  scoped_unix_file conn;
  void notify();
  std::map<boost::uint32_t, watch_implementation *> watches;
};

}}

#endif
