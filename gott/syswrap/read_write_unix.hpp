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

#ifndef GOTT_SYSWRAP_READ_WRITE_UNIX_HPP
#define GOTT_SYSWRAP_READ_WRITE_UNIX_HPP

#include <cstddef>
#include <gott/visibility.hpp>

namespace gott {

/**
 * Read bytes from a stream. See read(2) for reference.
 * \param fd The file descriptor of the stream to read from.
 * \param buf The buffer to read to.
 * \param len The length of the buffer.
 * \throw errno_exception
 */
GOTT_EXPORT std::size_t read_unix(int fd, void *buf, std::size_t len);

/**
 * Read bytes from a stream. See read(2) for reference.
 * \param fd The file descriptor of the stream to read from.
 * \param buf The buffer to read to.
 * \throw errno_exception
 */
template<class T, std::size_t len>
std::size_t read_unix(int fd, T (&buf)[len]) {
  return read_unix(fd, &buf, sizeof(T) * len);
}

/**
 * Write bytes to a stream. See write(2) for reference.
 * \param fd The file descriptor of the stream to write to.
 * \param buf The buffer to write from.
 * \param len The length of the buffer.
 * \throw errno_exception
 */
GOTT_EXPORT std::size_t write_unix(int fd, void const *buf, std::size_t len);

/**
 * Write bytes to a stream. See write(2) for reference.
 * \param fd The file descriptor of the stream to write to.
 * \param buf The buffer to write from.
 * \throw errno_exception
 */
template<class T, std::size_t len>
std::size_t write_unix(int fd, T const (&buf)[len]) {
  return write_unix(fd, &buf, sizeof(T) * len);
}

}

#endif
