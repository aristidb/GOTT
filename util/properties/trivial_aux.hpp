// Copyright (C) 2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: Properties library
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

#ifndef GOTT_UTIL_PROPERTY_TRIVIAL_AUX_HPP
#define GOTT_UTIL_PROPERTY_TRIVIAL_AUX_HPP

namespace gott {
namespace util {
namespace properties {

struct no_lock {
  typedef no_lock read_lock, write_lock, read_write_lock;
};

template<class ValueType, class Storage>
struct direct_access {
  bool set(Storage &s, ValueType const &v) const { 
    *s.get_pointer() = v;
    return true;
  }

  ValueType const &get(Storage const &s) const {
    return *s.get_pointer();
  }

  typedef typename Storage::pointer pointer;
  typedef typename Storage::const_pointer const_pointer;

  pointer build(Storage &s) const {
    return s.get_pointer();
  }

  const_pointer build(Storage const &s) const {
    return s.get_pointer();
  }

  bool done_with(pointer) const {
    return true;
  }

  bool done_with(const_pointer) const {
    return true;
  }
};

struct no_notification {
  void notify(void *) const {}
};

}}}

#endif
