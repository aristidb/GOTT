// Copyright (C) 2004-2005 by Andreas Pokorny andreas.pokorny@gmail.com
// Content: GOTT handle base class
// Authors: Andreas Pokorny
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

#include <agg_pixfmt_rgba.h>
#include <agg_renderer_base.h>
#include <agg_renderer_primitives.h>
#include <agg_color_rgba.h>
#include "simple_handle.hpp"

namespace gott{namespace gui{namespace designer{

simple_handle::simple_handle() {
}

void simple_handle::draw( agg::rendering_buffer & buffer ) {
  if( enabled() ) {
    typedef agg::pixfmt_rgba32 pixfmt_type;
    typedef agg::renderer_base<pixfmt_type> renderer_base_type;
    pixfmt_type pixf(buffer);
    renderer_base_type rbase(pixf);


    agg::renderer_primitives<renderer_base_type>  prim_renderer(rbase);

    rect const& r = get_region();
    prim_renderer.fill_color(agg::rgba(0,0,0,0.5));
    prim_renderer.rectangle( r.left, r.top, r.left + r.width, r.top + r.height );
  }
}

bool simple_handle::begin_drag( coord position, size_t button_index ) {
  if( enabled() && button_index == 1 ) {
    // set new posistion 
    // handle movement
    return true;
  }
  return false;
}
void simple_handle::continue_drag( coord new_position) {
  // set new posistion 
  // handle movement
}
void simple_handle::end_drag( coord new_position) {
  // set new posistion 
  // handle movement
  // handle end drag
}

}}}

