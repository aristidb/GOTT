// vim:ts=2:sw=2:expandtab:autoindent:filetype=cpp:
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

#define TRACE_DEBUG
#include "traceobject.hpp"

#include <cstdlib>

namespace gott {
namespace util {
namespace debug {

tracenodep traceobject::activefunc;
tracenodes traceobject::tracedfuncs;
  
traceobject::traceobject(std::string function,
                         std::string filename,
                         unsigned int line) {
  if(!activefunc) {
    tracedfuncs.push_back(tracenodep(new tracenode("TRACEROOT", 
                                               "NOWHERE", 
                                               0, 0, 0,
                                               tracenodep())));
    activefunc = tracedfuncs[tracedfuncs.size() - 1];
  }
  tracedfuncs.push_back(tracenodep(new tracenode(function,
                                             filename,
                                             line,
                                             std::clock(),
                                             activefunc->depth + 1,
                                             activefunc)));
  activefunc = tracedfuncs[tracedfuncs.size() - 1];
  activefunc->parent->children.push_back(activefunc);
}
  
traceobject::~traceobject() {
  // ASSERT(activefunc->parent)
  activefunc->duration = std::clock() - activefunc->start;
  activefunc = activefunc->parent;
}

}}}
