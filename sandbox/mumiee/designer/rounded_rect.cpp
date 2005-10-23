// Copyright (C) 2004-2005 by Andreas Pokorny andreas.pokorny@gmail.com
// Content: GOTT simple handle class
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
#include <agg_renderer_scanline.h>
#include <agg_rasterizer_scanline_aa.h>
#include <agg_rounded_rect.h>
#include <agg_color_rgba.h>
#include <agg_scanline_p.h>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/construct.hpp>
#include <algorithm>
#include <iterator>
#include "utility.hpp"
#include "rounded_rect.hpp"
#include "simple_handle.hpp"


namespace gott{namespace gui{namespace designer{

rounded_rect::hit_t rounded_rect::hit(coord const &c) const {
  return OUTSIDE; //TODO
}

void rounded_rect::draw( agg::rendering_buffer &buf ) {

  typedef agg::pixfmt_rgba32 pixfmt_type;
  typedef agg::renderer_base<pixfmt_type> renderer_base_type;
  typedef agg::renderer_scanline_aa_solid<renderer_base_type> renderer_solid_type;

  pixfmt_type pixf(buf);
  renderer_base_type rb(pixf);
  renderer_solid_type ren(rb);

  agg::rasterizer_scanline_aa<> ras;
  agg::scanline_p8 sl;

  // Render two "control" circles

  // Creating a rounded rectangle
  agg::rounded_rect rrect(extents.left, extents.top, extents.left + extents.width, extents.top + extents.height, radius );
  rrect.normalize_radius();

  // Drawing as an outline
  ras.add_path(rrect);
  ren.color(agg::rgba(1,0,0));
  agg::render_scanlines(ras, sl, ren);

  ras.gamma(agg::gamma_none());
}

rounded_rect::handle_list rounded_rect::get_handles() {
  handle_list ret;
  std::copy( handles.begin(), handles.end(), std::back_insert_iterator<handle_list>(ret) );
  return ret;
}

rounded_rect::~rounded_rect()
{
}

void rounded_rect::set_position(coord const& pos){
  extents.left = pos.x;
  extents.top = pos.y;
}

coord rounded_rect::get_position() const{
  return coord(extents.left,extents.top);
}

rounded_rect::rounded_rect( rect const& p ) : extents(p), radius(0) {
  using namespace boost::lambda;
  typedef boost::shared_ptr<handle> sh_h;
  {
    simple_handle * s = new simple_handle( 
        (( var(extents.left) = bind(&coord::x, _1) )
         ,( var(extents.top) = bind(&coord::y, _1) ) )
        );
    s->set_position_handler( 
        bind(constructor<coord>(), var(extents.left), var(extents.top) ) 
        ,  (( var(extents.left) = bind(&coord::x, _1) )
          ,( var(extents.top) = bind(&coord::y, _1) ) )
        );
    handles.push_back( sh_h( s ) );
  }

  {
    simple_handle * s = new simple_handle(
         (( var(extents.left) = bind(&coord::x, _1) )
         ,( var(extents.height) = bind(&coord::y, _1) - var(extents.top) ))
        );
    s->set_position_handler( 
        bind(constructor<coord>(), var(extents.left), var(extents.top) + var(extents.height) ) 
        , (( var(extents.left) = bind(&coord::x, _1) )
         ,( var(extents.height) = bind(&coord::y, _1) - var(extents.top) ))
        );
    handles.push_back( sh_h( s ) );
  }

  {
    simple_handle * s = new simple_handle( 
         (( var(extents.width) = bind(&coord::x, _1)  - var(extents.left) )
         ,( var(extents.height) = bind(&coord::y, _1) - var(extents.top) ))
        );
    s->set_position_handler( 
        bind(constructor<coord>(), var(extents.left) + var(extents.width), var(extents.top) + var(extents.height)) 
        , (( var(extents.width) = bind(&coord::x, _1)  - var(extents.left) )
         ,( var(extents.height) = bind(&coord::y, _1) - var(extents.top) ))
        );
    handles.push_back( sh_h( s ) );
  }

  {
    simple_handle * s = new simple_handle( 
        (( var(extents.width) = bind(&coord::x, _1)  - var(extents.left) )
         ,( var(extents.top) = bind(&coord::y, _1) ))
        );
    s->set_position_handler( 
        bind(constructor<coord>(), var(extents.left) + var(extents.width), var(extents.top) ) 
        ,  (( var(extents.width) = bind(&coord::x, _1)  - var(extents.left) )
          ,( var(extents.top) = bind(&coord::y, _1) ))
        );
    handles.push_back( sh_h( s ) );
  }

}


}}}

