// Copyright (C) 2004-2006 by Aristid Breitkreuz (aribrei@arcor.de)
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

#ifndef GOTT_UTIL_TDL_STRUC_STRUC_H
#define GOTT_UTIL_TDL_STRUC_STRUC_H

#include <gott/visibility.hpp>
#include <gott/xany/xany.hpp>

namespace gott {
class string;

namespace tdl {
namespace structure {

/**
 * The interface for writable structures. Just includes regular flow without
 * backtracking.
 */
class writable_structure {
public:

  /**
   * Set the data of the current node.
   * \param x The value to write.
   */
  virtual void data(xany::Xany const &x) = 0;

  /**
   * Start a child of the current node.
   */
  virtual void begin() = 0;

  /**
   * End a node.
   */
  virtual void end() = 0;

  /**
   * Add a tag to the node.
   * \param t The tag to add.
   */
  virtual void add_tag(string const &t) = 0;

  GOTT_EXPORT virtual ~writable_structure() = 0;
};

/**
 * The interface for revocable (backtracking) structures.
 */
class revocable_structure : public writable_structure {
public:
  /// The time-point as used by a backtracking-aware structure writer.
  typedef std::ptrdiff_t pth;

  /**
   * Acquire a time-point. Don't forget to call get_rid_of.
   * \return The fresh time-point.
   */
  virtual pth point() = 0;
  
  /**
   * Revert to an acquired time-point.
   * \param pt The time-point to revert to.
   */
  virtual void revert(pth pt) = 0;

  /**
   * "Destroy" a time-point.
   * \param pt The time point to get rid of.
   */
  virtual void get_rid_of(pth pt) = 0;

  GOTT_EXPORT virtual ~revocable_structure() = 0;
};

/**
 * The interface for copyable structures. Just includes the copying portion.
 */
class copyable_structure {
public:
  /**
   * Copy the structure to another structure object.
   * \param ws The structure object to copy to.
   */
  virtual void copy_to(writable_structure &ws) const = 0;

  GOTT_EXPORT virtual ~copyable_structure() = 0;
};

/**
 * Directly convert a TDL document into a structure.
 * \param stream The stream to read from.
 * \param structure The structure to fill.
 */
GOTT_EXPORT 
void direct_struc(std::istream &stream, writable_structure &structure);

}}}

#endif
