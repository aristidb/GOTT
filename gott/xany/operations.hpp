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
#include <boost/mpl/bool.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/vector/vector0.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/empty.hpp>
#include <boost/mpl/inherit.hpp>
#include <boost/mpl/inherit_linearly.hpp>

namespace gott { namespace xany {

struct equality_comparable_tag {};
struct printable_tag {};

template<class EqualityComparable, class Printable>
struct op_traits_ctor {
  typedef 
    boost::mpl::vector0<> set0;
  typedef
    typename boost::mpl::if_<
      EqualityComparable,
      typename boost::mpl::push_back<set0, equality_comparable_tag>::type,
      set0
    >::type set1;
  typedef
    typename boost::mpl::if_<
      Printable,
      typename boost::mpl::push_back<set1, printable_tag>::type,
      set1
    >::type set2;
  typedef set2 type;
};

template<class T>
struct op_traits {
  typedef
    typename op_traits_ctor<
      typename T::equality_comparable,
      typename T::printable
    >::type type;
};

template<>
struct op_traits<void> {
  typedef op_traits_ctor<boost::mpl::false_, boost::mpl::false_>::type type;
};

template<>
struct op_traits<long> {
  typedef op_traits_ctor<boost::mpl::true_, boost::mpl::true_>::type type;
};

template<>
struct op_traits<double> {
  typedef op_traits_ctor<boost::mpl::true_, boost::mpl::true_>::type type;
};

template<class T, class Tag>
struct get_op;

template<class T> 
struct operations :
  boost::mpl::inherit2<
    typename boost::mpl::inherit_linearly<
      typename op_traits<T>::type,
      boost::mpl::inherit<boost::mpl::_1, get_op<T, boost::mpl::_2> >
    >::type,
    typename boost::mpl::if_<
      typename boost::mpl::empty<typename op_traits<T>::type>::type,
      operations_base,
      boost::mpl::empty_base
    >::type
  >::type
{};

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

template<class T>
struct get_op<T, equality_comparable_tag> : equality_comparer<T> {};

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

template<class T>
struct get_op<T, printable_tag> : printer<T> {};

/**
 * Print a typeless object to a stream.
 * \param s The stream to write to.
 * \param v The object to print.
 * \return A reference to s.
 */
GOTT_EXPORT std::ostream &operator<<(std::ostream &s, Xany const &v);

}}

#endif
