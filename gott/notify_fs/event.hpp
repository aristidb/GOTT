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

#ifndef GOTT_NOTIFY_FS_EVENT_HPP
#define GOTT_NOTIFY_FS_EVENT_HPP

#include <boost/cstdint.hpp>
#include <gott/visibility.hpp>
#include <gott/string/string.hpp>

namespace gott {
namespace notify_fs {

/**
 * Event flags.
 * Compatible to: inotify.
 */
enum ev_t {
  /// File was accessed.
  file_access = 0x1,
  /// File was modified.
  file_modify = 0x2,
  /// Metadata changed.
  file_attrib = 0x4,
  /// Writable file was closed.
  file_close_write = 0x8,
  /// Unwritable file was closed.
  file_close_nowrite = 0x10,
  /// File was closed.
  file_close = file_close_write | file_close_nowrite,
  /// File was opened.
  file_open = 0x20,
  /// File was moved from X.
  file_moved_from = 0x40,
  /// File was moved to X.
  file_moved_to = 0x80,
  /// File was moved.
  file_move = file_moved_from | file_moved_to,
  /// Subfile was created.
  subfile_create = 0x100,
  /// Subfile was deleted.
  subfile_delete = 0x200,
  /// File (self) was deleted.
  file_delete = 0x400,
  /// Backing filesystem was unmounted.
  extra_unmount = 0x2000,
  /// Event queue overflowed.
  extra_queue_overflow = 0x4000,
  /// File was ignored.
  exta_ignored = 0x8000,
  /// Event occurred against a directory.
  flag_directory = 0x40000000,
  /// Event is sent only once.
  flag_oneshot = 0x80000000,
  all_evens = file_access | file_modify | file_attrib | file_close_write 
    | file_close_nowrite | file_open | file_moved_from | file_moved_to
    | subfile_delete | subfile_create | file_delete
};

class watch;

typedef boost::uint32_t cookie_t;

struct event {
  watch &context;
  ev_t flags;
  cookie_t cookie;
  gott::string name;
};

}}

#endif
