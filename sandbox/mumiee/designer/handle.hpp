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


#ifndef GOTT_GUI_DESIGNER_HANDLE_HPP_INCLUDED
#define GOTT_GUI_DESIGNER_HANDLE_HPP_INCLUDED

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <agg_rendering_buffer.h>
#include "utility.hpp"
#include "vector_obj.hpp"

namespace gott{namespace gui{namespace designer{
class handle : public vector_obj{
  private:
    rect region;
    bool status;
  public:
    handle( vector_obj::damage_type const& d );
    rect const& get_region() const;
    void set_region( rect const& reg );
    bool enabled() const;
    void enable();
    void disable();

    /**
     * \brief handle draws itself in here: 
     * TODO redesign! 
     */
    virtual void draw( agg::rendering_buffer & ) = 0;

    /**
     * people should reimplement that to use a certain button
     * @returns true when the drag start was succesful
     */
    virtual bool begin_drag( coord position, size_t button_index )= 0;
    /**
     * people should reimplement that to implement constraints
     */
    virtual void drag( coord new_position) = 0;
    /**
     * people should reimplement that to implement constraints
     * @returns true when the drag end was succesful
     */
    virtual bool end_drag( coord new_position) = 0;

};

typedef boost::shared_ptr<handle> shared_handle;
typedef boost::weak_ptr<handle> weak_ptr;


}}}

#endif 

