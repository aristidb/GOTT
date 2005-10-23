// Copyright (C) 2004-2005 by Rüdiger Sonderfeld <kingruedi@c-plusplus.de>
// Content: GOTT vector obj base class
// Authors: Rüdiger Sonderfeld
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

#ifndef GOTT_GUI_DESIGNER_VECTOR_OBJ_HPP_INCLUDED
#define GOTT_GUI_DESIGNER_VECTOR_OBJ_HPP_INCLUDED

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <agg_rendering_buffer.h>
#include "utility.hpp"

namespace gott { namespace gui { namespace designer {
  class filler;

  /// Base Class for Objects capable of rendering agg vectors
  struct GOTT_EXPORT vector_obj {
    typedef unsigned int depth_t;
    virtual ~vector_obj() =0;

    GOTT_LOCAL vector_obj() : depth(0), fill(0x0) { }
    GOTT_LOCAL vector_obj( depth_t depth_, filler const *f)
      : depth(depth_), fill(f)
    { }

    virtual void set_position(coord const& pos) = 0;
    virtual coord get_position() const = 0;

    /** Depth of the object in the rendering pipeline
     * 0 is the lowest depth
     */
    GOTT_LOCAL void set_depth(depth_t depth_) { depth=depth_; }
    GOTT_LOCAL depth_t get_depth() const { return depth; }

    GOTT_LOCAL void set_filler(filler const *f) { fill=f; }
    GOTT_LOCAL filler const *get_filler() const { return fill; }

    virtual void draw(agg::rendering_buffer &) = 0;

    enum hit_t {
      OUTSIDE,
      BORDER,
      INSIDE
    }
    virtual hit_t hit(coord const &c) const =0;
  private:
    depth_t depth;
    filler const *fill;
  };

  typedef boost::shared_ptr<vector_obj> shared_vec_obj;
  typedef boost::weak_ptr<vector_obj> weak_vec_obj;
}}}

#endif
