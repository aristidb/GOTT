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

#ifndef GOTT_TDL_STRUCTURE_REVOCABLE_ADAPTER_HPP
#define GOTT_TDL_STRUCTURE_REVOCABLE_ADAPTER_HPP

#include "structure.hpp"
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

namespace gott {
namespace tdl {
namespace structure {

/**
 * Adapter to add revoking capabilities to a simple writable_structure.
 * If the revoking capabilities are not used, a reference to an object of
 * this type behaves the same as its output.
 */
class revocable_adapter : public revocable_structure, boost::noncopyable {
public:
  /**
   * Constructor.
   * \param out The writable_structure to write to.
   */
  GOTT_EXPORT revocable_adapter(writable_structure &out);

  /// Destructor.
  GOTT_EXPORT ~revocable_adapter();

private:
  void begin();
  void end();
  void add_tag(string const &);
  void data(xany::Xany const &);

  pth point();
  void revert(pth);
  void get_rid_of(pth);

private:
  class impl;
  boost::scoped_ptr<impl> p;
};

}}}

#endif
