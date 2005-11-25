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

#include <gott/visibility.hpp>
#include <gott/range.hpp>
#include <cstddef>
#include <memory>

namespace gott {

template<class Out>
struct thunk_t {
  virtual ~thunk_t() {}
  virtual bool done() const = 0;
  virtual Out call() = 0;
  virtual std::size_t size() const = 0;

  range_t<Out *> consume() {
    std::size_t len = size();
    Out *buf = new Out[len];
    for (std::size_t i = 0; i < len; ++i)
      buf[i] = call();
    return range(buf, len);
  }

  operator range_t<Out *>() { return consume(); }

  template<class T>
  range_t<T> send(range_t<T> const &out) {
    T it = out.begin();
    while (it != out.end() && !done())
      *it++ = call();
    return range(out.begin(), it);
  }
};

template<class Out, class In, class Context>
struct concrete_thunk_t;

#define GOTT_REDEFINE_THUNK_CONTEXT(tag, context) \
  template<class Out, class In> \
  std::auto_ptr<thunk_t<Out> > tag(In data, context con = context()) { \
    return std::auto_ptr<thunk_t<Out> >( \
        new concrete_thunk_t<Out, In, context>(data, con)); \
  }

#define GOTT_DEFINE_THUNK_CONTEXT(tag) \
  struct tag ## _tag {}; \
  GOTT_REDEFINE_THUNK_CONTEXT(tag, tag ## _tag)

GOTT_DEFINE_THUNK_CONTEXT(integer_to_digits)
GOTT_DEFINE_THUNK_CONTEXT(integer_to_string)
GOTT_DEFINE_THUNK_CONTEXT(thunk_cast)

template<class Out, class Int>
struct concrete_thunk_t<Out, Int, integer_to_digits_tag> : thunk_t<Out> {
  concrete_thunk_t(Int const &v, integer_to_digits_tag) : data(v), div(1) {
    while (div <= data)
      div *= 10;
  }
  
  Int data;
  Int div;

  bool done() const { 
    return div == 1; 
  }

  Out call() {
    if (div > 1)
      div /= 10;
    return (data / div) % 10;
  }

  std::size_t size() const {
    Int x = 1;
    std::size_t sz = 0;
    do {
      ++sz;
      x *= 10;
    } while (x <= data);
    return sz;
  }
};

template<class CharType, class Int>
struct concrete_thunk_t<CharType, Int, integer_to_string_tag> 
: thunk_t<CharType> {
  concrete_thunk_t(Int const &v, integer_to_string_tag) 
  : helper(v, integer_to_digits_tag()) {}
  
  concrete_thunk_t<CharType, Int, integer_to_digits_tag> helper;

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

template<class New, class Delegate>
struct concrete_thunk_t<New, Delegate, thunk_cast_tag> : thunk_t<New> {
  concrete_thunk_t(Delegate d, thunk_cast_tag)
  : delegate(d) {}

  bool done() const {
    return delegate->done();
  }

  New call() {
    return New(delegate->call());
  }

  std::size_t size() const {
    return delegate->size();
  }

  Delegate delegate;
};

}

#endif
