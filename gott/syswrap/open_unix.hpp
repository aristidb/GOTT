// (C) 2006 by Aristid Breitkreuz (aribrei@arcor.de)
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
