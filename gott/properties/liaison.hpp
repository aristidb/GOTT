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
 * The Original Code is A Properties Library.
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

#ifndef GOTT_UTIL_PROPERTIES_LIAISON_HPP
#define GOTT_UTIL_PROPERTIES_LIAISON_HPP

#include "property.hpp"
#include <boost/signals/connection.hpp>
#include <boost/lambda/bind.hpp>

namespace gott {
namespace properties {

template<class T>
struct no_conversion {
  T const &left_to_right(T const &left) { return left; }
  T const &right_to_left(T const &right) { return right; }
};

template<class Type, class Conversion = no_conversion<Type> >
class liaison {
public:
  liaison(notifying_property<Type> &lhs, notifying_property<Type> &rhs, 
      Conversion c = Conversion())
  : left(lhs),
    right(rhs), 
    left_to_right(left.on_change().connect(
          boost::lambda::bind(&liaison::left_changed, this))),
    right_to_left(right.on_change().connect(
          boost::lambda::bind(&liaison::right_changed, this))),
    conversion(c)
  {} 
  
  void block(bool should_block = true) {
    left_to_right.block(should_block);
    right_to_left.block(should_block);
  }

  void unblock() {
    block(false);
  }

private:
  notifying_property<Type> &left;
  notifying_property<Type> &right;
  boost::signals::scoped_connection left_to_right;
  boost::signals::scoped_connection right_to_left;
  Conversion conversion;

  void left_changed() {
    right_to_left.block();
    right.set(conversion.left_to_right(left.get()));
    right_to_left.unblock();
  }

  void right_changed() {
    left_to_right.block();
    left.set(conversion.right_to_left(right.get()));
    left_to_right.unblock();
  }
};

template<class T, class Conversion, class PropertyType>
class owning_liaison {
public:
  owning_liaison(PropertyType const &lhs, PropertyType const &rhs, Conversion c)
  : left(lhs), right(rhs), internal_liaison(left, right, c) {}

  void block(bool should_block = true) {
    internal_liaison.block(should_block);
  }

  void unblock() {
    internal_liaison.unblock();
  }

  PropertyType &get_left() { return left; }
  PropertyType const &get_left() const { return left; }

  PropertyType &get_right() { return right; }
  PropertyType const &get_right() const { return right; }

private:
  PropertyType left;
  PropertyType right;
  liaison<T, Conversion> internal_liaison;
};

}}

#endif
