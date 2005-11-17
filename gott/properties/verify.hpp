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

struct ignore_correct;
  
template<
  class Type,
  class Check, 
  class OnFailure, 
  class OnCorrect = ignore_correct
>
class verify {
public:
  verify(notifying_property<Type> &p, 
      Check c = Check(), 
      OnFailure f = OnFailure(),
      OnCorrect k = OnCorrect())
  : prop(p), check(c), failure(f), correct(k),
    change(prop.on_change().connect(sigc::bind(&verify::action, this))) {}

  OnCorrect &on_correct() { return correct; }

  OnFailure &on_failure() { return failure; }

  ~verify() {
    change.disconnect();
  }

private:
  notifying_property<Type> &prop;
  Check check;
  OnFailure failure;
  OnCorrect correct;
  sigc::connection change;

  void action() {
    typename Check::context con;
    if (!check(prop, con))
      failure(prop, con);
    else
      correct(prop);
  }
};

template<class T, class Check, class OnFailure, class PropertyType>
class owning_verify {
public:
  owning_verify(PropertyType const &p, 
      Check c = Check(), 
      OnFailure f = OnFailure())
  : prop(p), internal_verify(prop, c, f) {}

private:
  PropertyType prop;
  verify<T, Check, OnFailure> internal_verify;
};

template<class Type>
class check_range {
public:
  typedef Type context;

  check_range(Type const &s, Type const &b) : small(s), big(b) {}

  bool operator()(property<Type> &prop, context &con) {
    typename property<Type>::read_reference r = prop.read();
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
  Type small;
  Type big;
};

template<class Type>
struct enforce_value {
  void operator()(property<Type> &prop, Type const &value) {
    prop.set(value);
  }
};

struct ignore_correct {
  template<class T>
  void operator()(T const &) {}
};

}}

#endif
