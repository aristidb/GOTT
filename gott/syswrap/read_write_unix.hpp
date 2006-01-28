// (C) 2006 by Aristid Breitkreuz (aribrei@arcor.de)
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
  return read(fd, &buf, sizeof(T) * len);
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
  return write(fd, &buf, sizeof(T) * len);
}

}

#endif
