// Copyright (C) 2004-2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL Schema engine
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

#ifndef GOTT_UTIL_TDL_SCHEMA_BY_NAME_HPP
#define GOTT_UTIL_TDL_SCHEMA_BY_NAME_HPP

#include <boost/scoped_ptr.hpp>
#include <gott/visibility.hpp>
#include <gott/tdl/schema/rule.hpp>

namespace gott {
class string;

namespace tdl {
namespace schema {

/**
 * Manages all named (native) schema types.
 */
class by_name_t {
  by_name_t();
  ~by_name_t();
  friend by_name_t &by_name();

public:
  /**
   * Add a type to the database.
   * \param name The name to find the type with.
   * \param type The type.
   */
  GOTT_EXPORT void add(string const &name, abstract_rule const &type);

  template<class T>
  void add(string const &name) {
    item_constructor type_con = &construct_item<T>;
    item_check accept_empty = &T::accept_empty;
    add(name, abstract_rule(type_con, accept_empty));
  }

  /**
   * Get a type from the database.
   * \param name The type's name.
   * \param att The attributes to supply to the type.
   * \param children The children to supply to the type.
   * \return A rule_t for the wanted type.
   */
  GOTT_EXPORT 
  rule_t get(string const &name, rule_attr_t const &att, 
             Vector<rule_t> pick_ &children) const;

  class impl;
  boost::scoped_ptr<impl> p;
};

GOTT_EXPORT by_name_t &by_name();

}}}

#endif
