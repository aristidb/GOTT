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

#ifndef GOTT_UTIL_PROPERTY_DYNAMIC_PROPERTY_HPP
#define GOTT_UTIL_PROPERTY_DYNAMIC_PROPERTY_HPP

#include <gott/util/misc/commonheaders.hpp>
#include <gott/util/properties/policy.hpp>

namespace gott {
namespace util {
namespace properties {

namespace detail {
  
template<class ValueType>
struct GOTT_EXPORT abstract_pa {
  virtual void set(ValueType const &) = 0;
  virtual ValueType const &get() const = 0;
  ValueType const &operator() () const { return get(); }
  virtual ValueType const &operator() (ValueType const &) = 0;
  virtual void 
    apply_change(boost::function<ValueType (ValueType const &)> const &) = 0;
  virtual void 
    apply_read(boost::function<void (ValueType const &)> const &) const = 0;
  virtual void
    apply_write(boost::function<ValueType()> const &) = 0;
  // ...
};

template<class Property>
struct concrete_pa : abstract_pa<typename Property::value_type> {
  // ...
};

}

class dynamic_property {
public:
  // ...

private:
  boost::shared_ptr<detail::abstract_pa> holder;
};

}}}

#endif
