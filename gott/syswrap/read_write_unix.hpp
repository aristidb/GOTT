// (C) 2006 by Aristid Breitkreuz (aribrei@arcor.de)
#ifndef GOTT_SYSWRAP_READ_WRITE_UNIX_HPP
#define GOTT_SYSWRAP_READ_WRITE_UNIX_HPP

#include <cstddef>
#include <gott/visibility.hpp>

namespace gott {

GOTT_EXPORT std::size_t read_unix(int fd, void *buf, std::size_t len);

template<class T, std::size_t len>
std::size_t read_unix(int fd, T (&buf)[len]) {
  return read(fd, &buf, sizeof(T) * len);
}

GOTT_EXPORT std::size_t write_unix(int fd, void const *buf, std::size_t len);

template<class T, std::size_t len>
std::size_t write_unix(int fd, T const (&buf)[len]) {
  return write(fd, &buf, sizeof(T) * len);
}

}

#endif
