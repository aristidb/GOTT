// Copyright (C) 2004-2005 by Andreas Pokorny andreas.pokorny@gmail.com
// Content: GOTT handle base class
// Authors: Andreas Pokorny
//
// This file is part of the Gott Project (http://gott.sf.net)
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
//version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#ifndef GOTT_GUI_DESIGNER_ROUNDED_RECT_HPP_INCLUDED
#define GOTT_GUI_DESIGNER_ROUNDED_RECT_HPP_INCLUDED
#include "handle.hpp"
#include "vector_obj.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace gott{namespace gui{namespace designer{

class rounded_rect : public vector_obj {
  private:
    typedef std::list<boost::shared_ptr<handle> > internal_handle_list;
   
    internal_handle_list handles;
    rect position;
    int radius;
  public:
    typedef std::list<boost::weak_ptr<handle> > handle_list;

    rounded_rect( rect const& position );
    ~rounded_rect();


    void draw( agg::rendering_buffer &buf );

    handle_list get_handles();


};

}}}

#endif

