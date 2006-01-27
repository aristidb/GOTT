// (C) 2006 by Aristid Breitkreuz (aribrei@arcor.de)
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

};

#endif
