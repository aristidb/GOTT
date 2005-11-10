// Copyright (C) 2005 by Aristid Breitkreuz (aribrei@arcor.de)
// Content: Non-immediate graphics library
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

#ifndef GOTT_GRAPHICS_OBJECT_HPP
#define GOTT_GRAPHICS_OBJECT_HPP

#include <gott/util/visibility.hpp>

namespace gott {
namespace graphics {

class drawing_context;
class transformations;
class invalidation;
class rect;

/**
 * Interface for graphics objects. 
 */
class GOTT_EXPORT object {
public:
  /**
   * Draw a clobbered region.
   * \param out The context to draw to.
   * \param transf The transformations to apply.
   * \param invalid The clobbered region.
   * \return Newly clobbered regions. Meaningless if there is no object above.
   */
  virtual invalidation draw(
      drawing_context &out, 
      transformations const &transf,
      invalidation const &invalid) const = 0;

  /**
   * Return the regions that have to be repainted <i>before</i> the object can 
   * be drawn. Every transparent (but visible, this means alpha != 0) bit of the
   * object has to be included. Other bits may be included.
   * \param transf The transformations to apply.
   * \param invalid The interesting region.
   * \return Where to repaint the background.
   */
  virtual invalidation transparent_parts(
      transformations const &transf,
      invalidation const &invalid) const = 0;

  /**
   * Return the regions that do <i>not</i> have to be painted behind the object.
   * Omitting bits is not lethal. The result must not include non-opaque bits.
   * \param transf The transformations to apply.
   * \param invalid The interesting region.
   * \return Where not to paint any background.
   */
  virtual invalidation opaque_parts(
      transformations const &transf,
      invalidation const &invalid) const = 0;

  /**
   * Provide the clobbered region when a child changed.
   * \param child The child that changed.
   */
  virtual invalidation child_changed(object const *child) const = 0;

  /**
   * Provide the size of the object.
   * \param transf The transformations to apply before calculating the size.
   * \return The rectangular extents of the object.
   */
  virtual rect extents(transformations const &transf) const = 0;

  /// Destructor.
  virtual ~object() = 0;
};

}}

#endif
