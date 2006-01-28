// (C) 2006 by Aristid Breitkreuz (aribrei@arcor.de)
#ifndef GOTT_SYSWRAP_INOTIFY_LINUX_HPP
#define GOTT_SYSWRAP_INOTIFY_LINUX_HPP

#include <gott/visibility.hpp>
#include <boost/cstdint.hpp>
extern "C" {
#include "inotify.h"
}

namespace gott {
GOTT_EXPORT int inotify_init_linux();
GOTT_EXPORT boost::uint32_t 
  inotify_add_watch_linux(int fd, const char *path, boost::uint32_t m);
GOTT_EXPORT void inotify_rm_watch_linux(int fd, boost::uint32_t wd);
}

#endif
