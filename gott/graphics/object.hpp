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
 * The Original Code is A Non-immediate Graphics Library.
 *
 * The Initial Developer of the Original Code is
 * Aristid Breitkreuz (aribrei@arcor.de).
 * Portions created by the Initial Developer are Copyright (C) 2005
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

#ifndef GOTT_GRAPHICS_OBJECT_HPP
#define GOTT_GRAPHICS_OBJECT_HPP

#include <gott/visibility.hpp>

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
   * Provide the size and position of the object.
   * \param transf The transformations to apply before calculating the extents.
   * \return The rectangular extents of the object.
   */
  virtual rect extents(transformations const &transf) const = 0;

  /**
   * Provide the size and position of a child of the object.
   * \param child The child of the object whose extents must be calculated.
   * \param transf The transformations to apply before calculation the extents.
   * \return The rectangular extents of the child.
   */
  virtual rect child_extents(
      object const *child,
      transformations const &transf) const = 0;

  /// Destructor.
  virtual ~object() = 0;
};

}}

#endif
