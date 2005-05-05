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
#include "tracefilter.hpp"

#include <sstream>

namespace gott {
namespace util {
namespace debug {

tracefilter::tracefilter(int timeoffset, TracefilterFlags flags)
  : filterflags(flags) {
  // convert from int (ms) to clock_t
  if (timeoffset > 0)
    filteroffset = (timeoffset * CLOCKS_PER_SEC)/1000;
  else
    filteroffset = -1;
}

std::wstring tracefilter::operator () (clock_t requesttime) {
  std::wstringstream output;
  
  // FILTER
  for (unsigned int i = 0, e = traceobject::tracedfuncs.size(); i < e; i++) {
    tracenodep tn = traceobject::tracedfuncs[i];
    
    // first check if the whole node might be discarded
    // timefilter
    if (filteroffset > 0) {
      clock_t rangebegin = requesttime - filteroffset;
      clock_t rangeend = requesttime;
      if(rangebegin > tn->start && rangeend < tn->start)
        continue;
    }
    
    if (filterflags & TF_GROUPBYFUNC) {
      // group
      // test if function was already counted
      bool found = false;
      for (unsigned int i = 0, e = filteredfuncs.size(); i < e; i++) {
        tracenodep ffn = filteredfuncs[i];
        if (tn->function == ffn->function
            && tn->filename == ffn->filename
            && tn->line == ffn->line) {
          // found
          ffn->duration = (tn->duration + ffn->duration) / 2;
          ffn->count++;
          found = true;
          break;
        }
      }
      if (!found) {
        filteredfuncs.push_back(tn);
        filteredfuncs[filteredfuncs.size() - 1]->count = 1;
      }
    } else {
      // will went to output as it is
      filteredfuncs.push_back(tn);
    }
  }

  // OUTPUT
  for (unsigned int i = 0, e = filteredfuncs.size(); i < e; i++) {
    tracenodep tn = filteredfuncs[i];
    if (!(filterflags & TF_GROUPBYFUNC)) {
      if (tn->depth) {
        for (unsigned int i = 0, e = tn->depth - 1; i < e; i++)
          output << "| ";
        output << "|-+ ";
      }
    }
    
    if (filterflags & TF_FUNC)
      output << tn->function.c_str() << ", ";
  
    if (filterflags & TF_FILE)
      output << tn->filename.c_str() << ", ";
  
    if (filterflags & TF_LINE)
      output << "line " <<  tn->line << ", ";
  
    if (filterflags & TF_START)
      output << "entered at "
             << double(tn->start)/double(CLOCKS_PER_SEC) << " s, ";

    if (filterflags & TF_DURATION)
      output << "lasted "
             << double(tn->duration)/double(CLOCKS_PER_SEC) << " s, ";
    
    if (filterflags & TF_COUNT)
      output << "called " << tn->count << " times, ";

    output << std::endl;
  }

  return std::wstring(output.str());
}

}}}
