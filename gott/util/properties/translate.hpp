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

#ifndef GOTT_UTIL_PROPERTIES_TRANSLATE_HPP
#define GOTT_UTIL_PROPERTIES_TRANSLATE_HPP

#include <utility>

namespace gott {
namespace properties {

template<class T> class property;

template<class New, class Old>
struct direct_translation {
private:
  typedef std::pair<New *, void *> ap;
  typedef std::pair<New const *, void *> cap;
public:
  template<class T>
  ap box(T x) {
    return ap(new New(*x), new T(x));
  }

  template<class T>
  cap const_box(T x) const {
    return cap(new New(*x), new T(x));
  }

  template<class T>
  void unbox(ap x) {
    T *p = static_cast<T *>(x.second);
    **p = Old(*x.first);
    delete x.first;
    delete p;
  }
  
  template<class T>
  void const_unbox(cap x) const {
    delete x.first;
    delete static_cast<T *>(x.second);
  }
};

}}

#endif