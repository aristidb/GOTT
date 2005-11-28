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

#ifndef GOTT_UTIL_DEBUG_TRACEFILTER_HPP
#define GOTT_UTIL_DEBUG_TRACEFILTER_HPP

#ifdef TRACE_DEBUG

#include <boost/dynamic_bitset.hpp>

#include "traceobject.hpp"

namespace gott {
namespace util {
namespace debug {

enum TracefilterFlags {
  TF_NONE = 0x0,
  TF_FUNC = 0x1,
  TF_FILE = 0x2,
  TF_LINE = 0x4,
  TF_START = 0x8,
  TF_DURATION = 0x10,
  TF_COUNT = 0x20,
  TF_ALLFIELDS = 0x3F, // adjust  when more fields will be added
  TF_GROUPBYFUNC = 0x100
};
  // may build combinations of this as you like
  
class tracefilter {
public:
  tracefilter(int timeoffset = -1,
              TracefilterFlags flags = TF_ALLFIELDS);
  
  std::string operator ()(clock_t requesttime);
  
private:
  tracenodes filteredfuncs;
  clock_t filteroffset;
  TracefilterFlags filterflags;
};
  
}}}

#endif

#endif
