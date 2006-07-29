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

#ifndef GOTT_SYSWRAP_SYSCONF_POSIX_HPP
#define GOTT_SYSWRAP_SYSCONF_POSIX_HPP

#include <gott/visibility.hpp>
#include <unistd.h>
#include <limits.h>

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
