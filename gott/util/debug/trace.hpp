// Debug Trace
//
// Copyright (C) 2004 Philipp Matthias Schaefer
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#ifndef GOTT_UTIL_DEBUG_TRACE_HPP
#define GOTT_UTIL_DEBUG_TRACE_HPP

#ifdef DEBUG

#define TRACE_DEBUG
#include "traceobject.hpp"
#include "tracefilter.hpp"

#include <boost/current_function.hpp>

  #ifndef __FILE__
  #define __FILE__ "__FILE__"
  #endif

  #ifndef __LINE__
  #define __LINE__ 0
  #endif

// problem: can be used only once in a namespace
// 1) with __LINE__ after t,tf only once in a line
// 2) TRACEME(x) .... x(BOOST...
  #define TRACEME ::gott::util::debug::traceobject t(BOOST_CURRENT_FUNCTION, \
                                                     __FILE__,__LINE__);
#else
  #define TRACEME
  #define FULLTRACE ""
#endif


#endif
