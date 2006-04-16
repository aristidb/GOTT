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
 * The Original Code is A Type-agnostic Storage Class.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2005-2006
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

#ifndef GOTT_XANY_OPERATIONS_HPP
#define GOTT_XANY_OPERATIONS_HPP

#include "xany.hpp"
#include "operations_base.hpp"
#include <ostream>
#include <gott/string/string.hpp>

namespace gott { namespace xany {

/**
 * Any equality-comparable Xany-compatible type has this interface in its
 * operations.
 */
struct GOTT_EXPORT equality_comparable : virtual operations_base {
  virtual bool equals(Xany const &, Xany const &) const = 0;
  virtual ~equality_comparable() = 0;
};

template<class T> struct equality_comparer : equality_comparable {
  bool equals(Xany const &a, Xany const &b) const {
    return Xany_cast<T>(a) == Xany_cast<T>(b);
  }
};

/**
 * The default operations implementation.
 */
template<class T> 
struct operations : equality_comparer<T> {};

/**
 * Checks whether two typeless objects are equal.
 */
GOTT_EXPORT bool operator==(Xany const &lhs, Xany const &rhs);

/**
 * Checks whether two typeless objects differ.
 */
GOTT_EXPORT bool operator!=(Xany const &lhs, Xany const &rhs);

/**
 * Any printable Xany-compatible type has this interface in its operations.
 */
struct GOTT_EXPORT printable : virtual operations_base {
  virtual void print(std::ostream &, Xany const &) const = 0;

#ifdef HAVE_WIDE_STDLIB
  virtual void print(std::wostream &, Xany const &) const = 0;
#endif

  virtual ~printable() = 0;
};

template<class T> struct printer : printable {
  void print(std::ostream &s, Xany const &v) const {
    s << Xany_cast<T>(v);
  }

#ifdef HAVE_WIDE_STDLIB
  void print(std::wostream &s, Xany const &v) const {
    s << Xany_cast<T>(v);
  }
#endif
};

/**
 * Print a typeless object to a stream.
 * \param s The stream to write to.
 * \param v The object to print.
 * \return A reference to s.
 */
template<class Ch, class ChT>
std::basic_ostream<Ch, ChT> &
operator<<(std::basic_ostream<Ch, ChT> &s, Xany const &v) {
  if (!v.empty()) {
    printable const &printer = 
      dynamic_cast<printable const &>(v.get_operations());
    printer.print(s, v);
  }
  return s;
}

#define GOTT_XANY_DECLARE_EQUALITY_COMPARABLE_AND_PRINTABLE(T) \
  template<> struct operations<T> : equality_comparer<T>, printer<T> {}

GOTT_XANY_DECLARE_EQUALITY_COMPARABLE_AND_PRINTABLE(long);
GOTT_XANY_DECLARE_EQUALITY_COMPARABLE_AND_PRINTABLE(double);
GOTT_XANY_DECLARE_EQUALITY_COMPARABLE_AND_PRINTABLE(string);

}}

#endif
