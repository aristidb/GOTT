/*
   Copyright (C) 2005 by Rüdiger Sonderfeld <kingruedi@c-plusplus.de>
   Content: A stream for debug output
   Changes:

   This file is part of the Gott Project (http://gott.sf.net)

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef GOTT_UTIL_DEBUG_DEBUGSTREAM_HPP
#define GOTT_UTIL_DEBUG_DEBUGSTREAM_HPP

#include <iostream>
#include <gott/util/string/types.hpp>

namespace gott {
namespace debug {
  /** Debug Output Stream
   * \remarks
   *      This stream should be used by for outputting debug information.
   *      Except if there is a lot of output as the trace functions produce.
   *      Per default the stream is directed to stdcerr
   */
  extern ::std::basic_ostream<utf32_t> debugstream;
}}

#endif
