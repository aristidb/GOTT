// (C) 2006 by Aristid Breitkreuz (aribrei@arcor.de)
#ifndef GOTT_SYSWRAP_SYSCONF_POSIX_HPP
#define GOTT_SYSWRAP_SYSCONF_POSIX_HPP

#include <gott/visibility.hpp>
#include <unistd.h>

namespace gott {
/**
 * System configuration. Access constants from the system. See sysconf(3) and
 * <unistd.h>(P).
 */
namespace sys_conf {

typedef unsigned long len_t;

/**
 * A minimum for the maximum length of the arguments to the exec() family of 
 * functions as guaranteed by POSIX.
 */
const len_t posix_arg_max = _POSIX_ARG_MAX;

/**
 * A minimum for the maximum number of simultaneous processes per user id as
 * guaranteed by POSIX.
 */
const len_t posix_child_max = _POSIX_CHILD_MAX;

/**
 * A minimum for the maximum length of a hostname, not including the final NUL,
 * as guaranteed by POSIX.
 */
const len_t posix_host_name_max = _POSIX_HOST_NAME_MAX;

const len_t posix_login_name_max = _POSIX_LOGIN_NAME_MAX;

const len_t posix_open_max = _POSIX_OPEN_MAX;

const len_t posix2_re_dup_max = _POSIX2_RE_DUP_MAX;

const len_t posix_stream_max = _POSIX_STREAM_MAX;

const len_t posix_symloop_max = _POSIX_SYMLOOP_MAX;

const len_t posix_tty_name_max = _POSIX_TTY_NAME_MAX;

const len_t posix_timezone_name_max = _POSIX_TZNAME_MAX;

const len_t posix_version_ct = _POSIX_VERSION;

/**
 * The maximum length of the arguments to the exec() family of the functions.
 * Must not be less than posix_arg_max.
 */
inline len_t arg_max() {
  return sysconf(_SC_ARG_MAX);
}

/**
 * The maximum number of simultaneous processes per user id. Must not be less 
 * than posix_child_max.
 */
inline len_t child_max() { return sysconf(_SC_CHILD_MAX); }

inline len_t host_name_max() { return sysconf(_SC_HOST_NAME_MAX); }

inline len_t login_name_max() { return sysconf(_SC_LOGIN_NAME_MAX);
}

inline len_t open_max() { return sysconf(_SC_PAGESIZE); }

inline len_t pagesize() { return sysconf(_SC_PAGESIZE); }

inline len_t re_dup_max() { return sysconf(_SC_RE_DUP_MAX); }

inline len_t stream_max() { return sysconf(_SC_STREAM_MAX); }

inline len_t symloop_max() { return sysconf(_SC_SYMLOOP_MAX); }

inline len_t tty_name_max() { return sysconf(_SC_TTY_NAME_MAX); }

inline len_t timezone_name_max() { return sysconf(_SC_TZNAME_MAX); }

inline long posix_version() { return sysconf(_SC_VERSION); }

}}

#endif
