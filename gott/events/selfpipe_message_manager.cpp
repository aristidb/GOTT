// vim:ts=2:sw=2:expandtab:autoindent:filetype=cpp:
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
 * The Original Code is An Event Handling Class Library.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2006
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *  Aristid Breitkreuz (aribrei@arcor.de)
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

#include "selfpipe_message_manager.hpp"
#include <gott/syswrap/pipe_unix.hpp>
#include <gott/syswrap/read_write_unix.hpp>
#include <boost/bind.hpp>

using gott::events::selfpipe_message_manager;
using gott::xany::Xany;

class selfpipe_message_manager::impl {
public:
  impl(fd_manager *fdm) : fdm(fdm) {}
  fd_manager *fdm;
  int selfpipe[2];
  sigc::signal1<void, Xany const &> on_receive_;
};

selfpipe_message_manager::selfpipe_message_manager(fd_manager *fdm)
: p(new impl(fdm)) {
  pipe_unix(p->selfpipe);
  fdm->add_fd(p->selfpipe[0], fd_manager::read, 
      boost::bind(&selfpipe_message_manager::notify_in, this), false);
}

selfpipe_message_manager::~selfpipe_message_manager() {
  p->fdm->remove_fd(p->selfpipe[0]);
  close(p->selfpipe[0]);
  close(p->selfpipe[1]);
}

sigc::signal1<void, Xany const &> &selfpipe_message_manager::on_receive() {
  return p->on_receive_;
}

void selfpipe_message_manager::notify_in() {
  void *data[1];
  read_unix(p->selfpipe[0], data);
  Xany value;
  value.recreate(data[0]);
  p->on_receive_.emit(value);
}

void selfpipe_message_manager::send(Xany const &value) throw() {
  Xany value_copy(value);
  void *data[1] = { value_copy.release() };
  write_unix(p->selfpipe[1], data);
}

