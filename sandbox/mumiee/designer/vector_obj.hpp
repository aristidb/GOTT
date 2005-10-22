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
#include "utility.hpp"

namespace gott { namespace gui { namespace designer {
  class filler;

  struct GOTT_EXPORT vector_obj {
    virtual ~vector_obj() =0;

    vector_obj() : depth(0), fill(0x0) { }
    vector_obj(coord const &pos, depth_t depth_, filler const *f)
      : position(pos), depth(depth_), fill(f)
    { }

    void set_position(coord const& pos) { position=pos; }
    coord const &get_position() const { return position; }

    typedef unsigned int depth_t;
    void set_depth(depth_t depth_) { depth=depth_; }
    depth_t get_depth() const { return depth; }

    void set_filler(filler const *f) { fill=f; }
    filler const *get_filler() const { return fill; }
  private:
    coord position;
    depth_t depth;
    filler const *fill;
  };
}}}

#endif
