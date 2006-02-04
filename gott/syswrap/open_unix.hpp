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

#ifndef GOTT_SYSWRAP_OPEN_UNIX_HPP
#define GOTT_SYSWRAP_OPEN_UNIX_HPP

#include <gott/visibility.hpp>
#include <sys/types.h>
#include <fcntl.h>

namespace gott {

/**
 * Open a file. See open(2) for reference.
 * \param filename The name of the file to open.
 * \param flags Flags for opening the file.
 * \param mode The permissions / mode for the created file. Specify together 
 *             with O_CREAT in the flags.
 * \return A valid file descriptor.
 * \throw errno_exception
 */
GOTT_EXPORT int open_unix(char const *filename, int flags, mode_t mode = 0);

/**
 * Create a file. See creat(2) for reference.
 * \param filename The name of the file to open.
 * \param mode The permissions / mode for the created file.
 * \return A valid file descriptor.
 * \throw errno_exception
 */
inline int creat_unix(char const *filename, mode_t mode) {
  return open_unix(filename, O_CREAT | O_WRONLY | O_TRUNC, mode);
}

}

#endif
