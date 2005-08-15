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
#include <gott/util/visibility.hpp>

namespace gott {
class string;

namespace tdl {
namespace schema {

/**
 * Manages all named (native) schema types.
 */
class name_manager_t {
  name_manager_t();
  ~name_manager_t();
  friend name_manager_t &name_manager();

public:
  /**
   * Add a type to the database.
   * \param name The name to find the type with.
   * \param type The type's id (see get_factory()).
   */
  GOTT_EXPORT void add(string const &name, unsigned type);

  /**
   * Get a type from the database.
   * \param name The type's name.
   * \return The type's id  (see get_factory()).
   */
  GOTT_EXPORT unsigned get(string const &name) const;

  class IMPL;
  boost::scoped_ptr<IMPL> p;
};

name_manager_t &name_manager();

}}}

#endif
