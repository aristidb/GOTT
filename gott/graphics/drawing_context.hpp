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
