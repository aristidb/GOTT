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

#ifndef GOTT_UTIL_DEBUG_TRACEOBJECT_HPP
#define GOTT_UTIL_DEBUG_TRACEOBJECT_HPP

#ifdef TRACE_DEBUG

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

namespace gott {
namespace util {
namespace debug {

struct tracenode;

typedef boost::shared_ptr<tracenode> tracenodep;
typedef std::vector<tracenodep> tracenodes;

 
struct tracenode {
  std::string     function;
  std::string     filename;
  unsigned int    line;
  clock_t         start;
  clock_t         duration;
  unsigned int    depth;
  tracenodep      parent;
  tracenodes      children;
  unsigned int    count; // only used by tracefilter
  
  tracenode(std::string fc,
            std::string fn,
            unsigned int l,
            clock_t s,
            unsigned int d,
            tracenodep p)
    : function(fc), filename(fn), line(l), start(s),
      duration(0), depth(d), parent(p), count(0) {}
};
  
class tracefilter;
  
class traceobject {
  friend class tracefilter;
public:
  traceobject(std::string function,
              std::string filename,
              unsigned int line);
  ~traceobject();
  
private:
  static tracenodep      activefunc;
  static tracenodes tracedfuncs;
};
  
}}}

#endif

#endif
