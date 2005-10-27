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

#ifndef GOTT_UTIL_PROPERTIES_LIAISON_HPP
#define GOTT_UTIL_PROPERTIES_LIAISON_HPP

#include "property.hpp"
#include <sigc++/connection.h>
#include <sigc++/bind.h>

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
  liaison(property<Type> &lhs, property<Type> &rhs, Conversion c = Conversion())
  : left(lhs),
    right(rhs), 
    left_to_right(left.on_change().connect(sigc::bind(&liaison::left_changed, this))),
    right_to_left(right.on_change().connect(sigc::bind(&liaison::right_changed, this))),
    conversion(c)
  {}

  ~liaison() {
    left_to_right.disconnect();
    right_to_left.disconnect();
  }

  void block(bool should_block = true) {
    left_to_right.block(should_block);
    right_to_left.block(should_block);
  }

  void unblock() {
    block(false);
  }

private:
  property<Type> &left;
  property<Type> &right;
  sigc::connection left_to_right;
  sigc::connection right_to_left;
  Conversion conversion;

  void left_changed() {
    right_to_left.block();
    right.set(conversion.left_to_right(left.get()));
    right_to_left.unblock();
  }

  void right_changed() {
    left_to_right.block();
    left.set(conversion.right_to_left(right.get()));
    right_to_left.unblock();
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
