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

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include "utility.hpp"
#include "rounded_rect.hpp"
#include "simple_handle.hpp"


namespace gott{namespace gui{namespace designer{

void rounded_rect::draw( agg::rendering_buffer &buf ) {
}

rounded_rect::handle_list rounded_rect::get_handles() {
  handle_list ret;
  return ret;
}

rounded_rect::~rounded_rect()
{
}

rounded_rect::rounded_rect( rect const& p ) : position(p), radius(0) {
  using namespace boost::lambda;
  typedef boost::shared_ptr<handle> sh_h;
  {
    simple_handle * s = new simple_handle( coord(p.left,p.top),
         ( var(position.left) = bind(&coord::x, _1) )
         ,( var(position.top) = bind(&coord::y, _1) )
        );
    handles.push_back( sh_h( s ) );
  }

  {
    simple_handle * s = new simple_handle( coord(p.left,p.top + p.height),
         ( var(position.left) = bind(&coord::x, _1) )
         ,( var(position.height) = bind(&coord::y, _1) - var(position.top) )
        );
    handles.push_back( sh_h( s ) );
  }

  {
    simple_handle * s = new simple_handle( coord(p.left + p.width,p.top + p.height),
         ( var(position.width) = bind(&coord::x, _1)  - var(position.left) )
         ,( var(position.height) = bind(&coord::y, _1) - var(position.top) )
        );
    handles.push_back( sh_h( s ) );
  }
}

}}}

