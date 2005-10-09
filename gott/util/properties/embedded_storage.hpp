// Copyright (C) 2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: Properties library
// Authors: Aristid Breitkreuz
//
// This file is part of the Gott Project (http://gott.sf.net)
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

#ifndef GOTT_UTIL_PROPERTY_EMBEDDED_STORAGE_HPP
#define GOTT_UTIL_PROPERTY_EMBEDDED_STORAGE_HPP

namespace gott {
namespace properties {

template<class T>
class embedded_storage {
  T obj;
public:
  embedded_storage(T const &x = T()) : obj(x) {}
  typedef T *pointer;
  typedef T const *const_pointer;
  typedef T &reference;
  typedef T const &const_reference;

  T *get_pointer() { return &obj; }
  T const *get_pointer() const { return &obj; }
  void finish_pointer(T *) {}
  void finish_pointer(T const *) const {}
};

}}

#endif
