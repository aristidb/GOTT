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

simple_handle::simple_handle( pos_update_signal::slot_type const& update_handler )  {
  pos_update.connect( update_handler );
}
void simple_handle::set_position_handler( boost::function<coord ()> getter, boost::function<void (coord const&)> setter ) {
  get_coord = getter;
  set_coord = setter;
  coord c = get_coord();
  rect r = get_region();
  r.left = c.x - r.width/2;
  r.top = c.y - r.height/2;
  set_region( r );
}

simple_handle::simple_handle( pos_update_signal::slot_type const& update_handler, pos_update_signal::slot_type const& end_drag )  {
  pos_update.connect( update_handler );
  final_pos.connect( end_drag );
}

boost::signals::connection 
simple_handle::add_position_handler( pos_update_signal::slot_type const& slot ) 
{
  return pos_update.connect( slot );
}

boost::signals::connection 
simple_handle::add_final_position_handler( pos_update_signal::slot_type const& slot ) 
{
  return final_pos.connect( slot );
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
    prim_renderer.line_color(agg::rgba(0,0,0,0.5));
    prim_renderer.rectangle( r.left, r.top, r.left + r.width, r.top + r.height );
  }
}

bool simple_handle::begin_drag( coord position, size_t button_index ) {
  if( enabled() && button_index == 1 ) {
    set_position( position );
    pos_update( position );
    
    // handle movement
    return true;
  }
  return false;
}
void simple_handle::drag( coord new_position) {
  set_position( new_position );
  pos_update( new_position );
}
bool simple_handle::end_drag( coord new_position) {
  set_position( new_position );
  pos_update( new_position );
  final_pos( new_position );
  return true;
}

void simple_handle::set_position( coord const& pos ) {
  set_coord(pos);
}
coord simple_handle::get_position() const{
  return get_coord();
}
}}}

