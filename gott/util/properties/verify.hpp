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

#ifndef GOTT_UTIL_PROPERTIES_VERIFY_HPP
#define GOTT_UTIL_PROPERTIES_VERIFY_HPP

#include "property.hpp"
#include <sigc++/connection.h>
#include <sigc++/bind.h>

namespace gott {
namespace properties {

template<class T, class Check, class OnFailure>
class verify {
public:
  verify(property<T> &p, Check c = Check(), OnFailure f = OnFailure())
  : prop(p), check(c), on_failure(f), 
    change(prop.on_change().connect(sigc::bind(&verify::action, this))) {}

  ~verify() {
    change.disconnect();
  }

private:
  property<T> &prop;
  Check check;
  OnFailure on_failure;
  sigc::connection change;

  void action() {
    typename Check::context con;
    if (!check(prop, con))
      on_failure(prop, con);
  }
};

template<class T>
class check_range {
public:
  typedef T context;

  check_range(T const &s, T const &b) : small(s), big(b) {}

  bool operator()(property<T> &prop, context &con) {
    typename property<T>::read_reference r = prop.read();
    if (*r < small) {
      con = small;
      return false;
    }
    if (*r > big) {
      con = big;
      return false;
    }
    return true;
  }

private:
  T small;
  T big;
};

template<class T>
struct enforce_value {
  void operator()(property<T> &prop, T const &value) {
    prop.set(value);
  }
};

}}

#endif
