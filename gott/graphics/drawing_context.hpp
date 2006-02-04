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

#ifndef GOTT_GRAPHICS_DRAWING_CONTEXT_HPP
#define GOTT_GRAPHICS_DRAWING_CONTEXT_HPP

#include <gott/visibility.hpp>
#include <gott/properties/property.hpp>

namespace gott {

class string;

namespace graphics {

class path;
class pattern;
class line_management;
class font;
class transformations;
class source;
class clipping;

/**
 * Interface for the underlying immediate renderer.
 */
class GOTT_EXPORT drawing_context {
public:
  enum compositing_operator_t {
    operator_clear,

    operator_source,
    operator_over,
    operator_in,
    operator_out,
    operator_atop,

    operator_dest,
    operator_dest_over,
    operator_dest_in,
    operator_dest_out,
    operator_dest_atop,

    operator_xor,
    operator_add,
    operator_saturate
  };

  properties::property<compositing_operator_t> &compositing_operator;
  
  /**
   * Draw a filled path.
   * \param contours The path to fill.
   * \param contents What to fill the path with.
   * \param transf The transformations to apply to the path.
   */
  virtual void fill(path const &contours, pattern const &contents,
      transformations const &transf) = 0;

  /**
   * Draw a stroked path.
   * \param contours The path to stroke.
   * \param contents What to paint in the actually stroked regions.
   * \param line Properties of the stroke.
   * \param transf The transformations to apply to the path.
   */
  virtual void stroke(path const &contours, pattern const &contents,
      line_management const &line, transformations const &transf) = 0;

  /**
   * Draw text.
   * \param text The text to draw.
   * \param style How to draw the text.
   * \param transf The transformations to apply to the text.
   */
  virtual void text(string const &text, font const &style, 
      transformations const &transf) = 0;

  // virtual void glyphs(..) = 0;

  /**
   * Blit an image.
   * \param img The source image.
   * \param clip The blit's clip region.
   * \param transf The transformations to apply to the image.
   */
  virtual void blit(source const &img, clipping const &clip, 
      transformations const &transf) = 0;

  /// Destructor.
  virtual ~drawing_context() = 0;
};

}}

#endif
