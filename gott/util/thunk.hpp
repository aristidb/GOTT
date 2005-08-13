// Copyright (C) 2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: Generic Input/Output Helpers
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

#ifndef GOTT_UTIL_THUNK_HPP
#define GOTT_UTIL_THUNK_HPP

#include <gott/util/visibility.hpp>
#include <cstddef>

namespace gott {

template<class Out>
struct thunk_t {
  virtual ~thunk_t() {}
  virtual bool done() const = 0;
  virtual Out call() = 0;
  virtual std::size_t size() const = 0;
};

template<class Out, class In, class Context>
struct concrete_thunk_t;

template<class Out, class Context, class In>
thunk_t<Out> *thunk(In const &data, Context con = Context()) {
  return new concrete_thunk_t<Out, In, Context>(data, con);
}

struct integer_to_digits {};
struct integer_to_string {};

template<class Out, class Int>
struct concrete_thunk_t<Out, Int, integer_to_digits> : thunk_t<Out> {
  concrete_thunk_t(Int const &v, integer_to_digits) : data(v), div(1) {
    while (div <= data)
      div *= 10;
  }
  
  Int data;
  Int div;

  bool done() const { 
    return div == 1; 
  }

  Out call() {
    div /= 10;
    return (data / div) % 10;
  }

  std::size_t size() const {
    Int x = div;
    std::size_t sz = Int();
    while (div > Int()) {
      x /= 10;
      ++sz;
    }
    return sz;
  }
};

template<class CharType, class Int>
struct concrete_thunk_t<CharType, Int, integer_to_string> : thunk_t<CharType> {
  concrete_thunk_t(Int const &v, integer_to_string) 
  : helper(v, integer_to_digits()) {}
  
  concrete_thunk_t<CharType, Int, integer_to_digits> helper;

  bool done() const { 
    return helper.done(); 
  }

  CharType call() {
    return CharType('0' + helper.call());
  }

  std::size_t size() const {
    return helper.size();
  }
};

}

#endif
