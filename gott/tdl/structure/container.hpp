// Copyright (C) 2006 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: TDL Data Structures
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

#ifndef GOTT_TDL_STRUCTURE_CONTAINER_HPP
#define GOTT_TDL_STRUCTURE_CONTAINER_HPP

#include "structure.hpp"
#include <boost/scoped_ptr.hpp>

namespace gott {
namespace tdl {
namespace structure {

/**
 * Generic container.
 */
class container : public writable_structure, public copyable_structure {
public:
  /// Constructor.
  GOTT_EXPORT container();

  /// Copy-Constructor.
  GOTT_EXPORT container(container const &);

  /// Destructor.
  GOTT_EXPORT ~container();

  /// Assignment-Operator.
  GOTT_EXPORT void operator=(container const &);

  /// Clear the container.
  GOTT_EXPORT void clear();

  /// Compare to another container.
  GOTT_EXPORT bool operator==(container const &) const;

  /// Compare to another container.
  inline bool operator!=(container const &o) const {
    return !operator==(o);
  }

  /// Copy contents to another structure.
  GOTT_EXPORT void copy_to(writable_structure &w) const;

private:
  void begin();
  void end();
  void add_tag(string const &);
  void data(xany::Xany const &);

private:
  class impl;
  boost::scoped_ptr<impl> p;
};

}}}

#endif
