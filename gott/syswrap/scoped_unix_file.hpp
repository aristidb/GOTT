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
 * The Original Code is Low-level System Wrapping Library.
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

#ifndef GOTT_SYSWRAP_SCOPED_UNIX_FILE_HPP
#define GOTT_SYSWRAP_SCOPED_UNIX_FILE_HPP

#include <gott/visibility.hpp>
#include <boost/noncopyable.hpp>

namespace gott {

/**
 * This class is a low-level wrapper for dealing with file descriptors.
 */
class scoped_unix_file : boost::noncopyable {
public:
  /**
   * Construct a closed scoped_unix_file.
   */
  scoped_unix_file() : fd(-1) {}

  /**
   * Construct an opened scoped_unix_file from an opened file descriptor.
   */
  scoped_unix_file(int fd_) : fd(fd_) {}

  /**
   * Destructor. Automatically closes the saved file descriptor if the object
   * is opened.
   * Ignores any error while closing. This might lead to unexpected behaviour.
   * You might want to use close() therefore.
   */
  GOTT_EXPORT ~scoped_unix_file();

  /**
   * Change the saved file descriptor and close the old if necessary. Does not
   * ignore errors in close().
   * \param fd_ The new file descriptor.
   * \throw errno_exception
   */
  void reset(int fd_) {
    close();
    fd = fd_;
  }

  /**
   * Get the file descriptor and stop it from being closed.
   * \return The old file descriptor.
   */
  int release() {
    int result = fd;
    fd = -1;
    return result;
  }

  /**
   * Close the saved file descriptor.
   * \throw errno_exception
   */
  GOTT_EXPORT void close();

  /**
   * Get the saved file descriptor.
   * \return The file descriptor. -1 if none.
   */
  int access() const { return fd; }

  /**
   * Check if there is a saved file descriptor.
   * \return Whether there is a saved file descriptor.
   */
  bool is_open() const { return fd != -1; }

private:
  int fd;
};

}

#endif
