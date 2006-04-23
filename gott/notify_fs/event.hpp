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
 * The Original Code is A Filesystem Notification Library.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2005
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *  Aristid Breitkreuz (aribrei@arcor.de)
 *  Rüdiger Sonderfeld <kingruedi@c-plusplus.de>
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
  /// Access to the file was revoked
  file_access_revoke = 0x20000000, //in some way similar to extra_unmount!
  /// The link count on the file changed.
  file_link = 0x80000,
  /// The file referenced by the descriptor was extended.
  file_extend = 0x40000,
  all_events = file_access | file_modify | file_attrib | file_close_write 
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
