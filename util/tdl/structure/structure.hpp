// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL Data Structures
// Authors: Aristid Breitkreuz
//
// This File is part of the Gott Project (http://gott.sf.net)
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

#ifndef GOTT_UTIL_TDL_STRUC_STRUC_H
#define GOTT_UTIL_TDL_STRUC_STRUC_H

#include <gott/util/misc/commonheaders.hpp>

namespace gott {
namespace util {
namespace tdl {
namespace structure {

class writable_structure {
public:  // Regular flow

  virtual void data(xany::Xany const &x) = 0;
    // set the data of the current node to x

  virtual void begin() = 0;
    // start a child of the current node

  virtual void end() = 0;
    // end a node

  virtual void add_tag(std::wstring const &) = 0;
    // add tags to a node

  virtual void set_tags(std::list<std::wstring> const &) = 0;

  EXPORT virtual ~writable_structure() = 0;
};

class revocable_structure : public writable_structure {
public: // Roll-back
  typedef std::ptrdiff_t pth;
    // the time-point as used by a transaction-aware structure writer

  virtual pth point() = 0;
    // acquire a time-point
  
  virtual void revert(pth const &) = 0;
    // revert to the given time-point

  virtual void get_rid_of(pth const &) = 0;

  EXPORT virtual ~revocable_structure() = 0;
};

class copyable_structure {
public:
  virtual void copy_to(writable_structure &) const = 0;

  EXPORT virtual ~copyable_structure() = 0;
};

EXPORT void direct_struc(std::wistream &, writable_structure &);
  // converts a tdl stream directly into a simple tdl structure

}}}}

#endif
