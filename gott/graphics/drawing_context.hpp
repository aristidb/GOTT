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

#include <gott/util/visibility.hpp>

namespace gott {
namespace graphics {

class path;
class pattern;
class line_management;
class fill_rule;

/**
 * Interface for the underlying immediate renderer.
 */
class GOTT_EXPORT drawing_context {
public:
  virtual void fill(path const &contours, pattern const &contents) = 0;
  virtual void stroke(path const &contours, line_management const &line) = 0;

  virtual ~drawing_context() = 0;
};

}}

#endif
