// vim:ts=2:sw=2:expandtab:autoindent:filetype=cpp:
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Thunk Continuations Class.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2005
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *  Aristid Breitkreuz (aribrei@arcor.de)
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

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

  range_t<Out *> consume_alloc() {
    std::size_t len = size();
    Out *buf = new Out[len];
    for (std::size_t i = 0; i < len; ++i)
      buf[i] = call();
    return range(buf, len);
  }

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
    return std::auto_ptr<thunk_t<Out> >(\
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
