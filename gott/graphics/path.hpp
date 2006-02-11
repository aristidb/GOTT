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

#ifndef GOTT_GRAPHICS_PATH_HPP
#define GOTT_GRAPHICS_PATH_HPP

#include <gott/visibility.hpp>
#include "primitive_types.hpp"

namespace gott {

class string;

namespace graphics {

class font;
class transformations;

/**
 * A graphical path.
 */
class path {
public:
  /// Empty path.
  GOTT_EXPORT path();

  /// Destructor.
  GOTT_EXPORT ~path();

  /// Copy-Constructor.
  GOTT_EXPORT path(path const &);
  
  /**
   * Append other path.
   * \param o Path to be appended.
   */
  GOTT_EXPORT path &operator +=(path const &o);

  /**
   * Adds a circular arc. Positional version. Connects the current point with 
   * start of the arc with a line. The new current point will be the end of
   * the arc.
   * \param center The coordinates of the center of the arc.
   * \param radius The radius of the arc.
   * \param angle1 The starting angle of the arc.
   * \param angle2 The stopping angle of the arc. If angle2 < angle1, angle2 
   *               will be increased by 2 * PI until angle2 >= angle1.
   */
  GOTT_EXPORT path &arc_p(point const &center, double radius, 
      double angle1, double angle2);

  /**
   * Adds a circular arc in reverse direction. Positional version.
   * \see arc_p
   * \param center The coordinates of the center of the arc.
   * \param radius The radius of the arc.
   * \param angle1 The starting angle of the arc.
   * \param angle2 The stopping angle of the arc. If angle2 > angle1, angle2 
   *               will be decreased by 2 * PI until angle2 <= angle1.
   */
  GOTT_EXPORT path &arc_negative_p(point const &center, double radius, 
      double angle1, double angle2);

  /**
   * Add a cubic Bezier spline to the path. Positional version.
   * Starts at the current point. The end point will be the new current point.
   * \param c1 The first control point.
   * \param c2 The scond control point.
   * \param e The end point.
   */
  GOTT_EXPORT path &curve_to_p(point const &c1, point const &c2, point const &e);

  /**
   * Adds a line from the old current point to the new.
   * \param p The new current point.
   */
  GOTT_EXPORT path &line_to(point const &p);

  /**
   * Change the current point. This will create a new subpath if necessary.
   * \param p The new current point.
   */
  GOTT_EXPORT path &move_to(point const &p);

  /**
   * Add the outlines of a text to the path.
   * \param txt The text to get the outline from.
   * \param style The text's font.
   */
  GOTT_EXPORT path &text(string const &txt, font const &style);

  // GOTT_EXPORT path &glyphs(...);
  
  /**
   * Transform the whole path.
   * \param transf The transformations to apply.
   */
  GOTT_EXPORT path &transform(transformations const &transf);

public: // implementation
  struct node;

private:
  node *head;
};

}}

#endif
