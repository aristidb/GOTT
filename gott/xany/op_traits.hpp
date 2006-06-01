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

#ifndef GOTT_XANY_OP_TRAITS_HPP
#define GOTT_XANY_OP_TRAITS_HPP

#include <boost/mpl/bool.hpp>
#include <boost/type_traits/is_fundamental.hpp>

namespace gott { namespace xany {

template<
  class T, 
  bool fundamental =
    boost::is_fundamental<T>::value || boost::is_pointer<T>::value
>
struct default_op_traits {
  typedef typename T::equality_comparable equality_comparable;
  typedef typename T::printable printable;
};

template<class T>
struct default_op_traits<T, true> {
  typedef boost::mpl::true_ equality_comparable;
  typedef boost::mpl::true_ printable;
};

template<class T>
struct op_traits : default_op_traits<T> {
};

template<>
struct op_traits<void> {
  typedef boost::mpl::false_ equality_comparable;
  typedef boost::mpl::false_ printable;
};

template<class T>
struct is_noop {
  typedef boost::mpl::bool_<
    !op_traits<T>::equality_comparable::value &&
    !op_traits<T>::printable::value
  > type;
};

}}

#endif
