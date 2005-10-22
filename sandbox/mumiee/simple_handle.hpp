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

#ifndef GOTT_GUI_DESIGNER_SIMPLE_HANDLE_HPP_INCLUDED
#define GOTT_GUI_DESIGNER_SIMPLE_HANDLE_HPP_INCLUDED
#include <boost/signals.hpp>
#include "handle.hpp"

namespace gott{namespace gui{namespace designer{


/**
 * Provide a simple rectangular handle without 
 * constraints.
 */
struct simple_handle : public handle {

  public:
    simple_handle();

    /**
     * \brief handle draws itself in here: 
     * TODO redesign! 
     */
    virtual void draw( agg::rendering_buffer & );

    /**
     * people should reimplement that to use a certain button
     * @returns true when the drag start should be succesful
     */
    virtual bool begin_drag( coord position, size_t button_index );
    /**
     * people should reimplement that to implement constraints
     * @returns true when the drag start should be succesful
     */
    virtual void continue_drag( coord new_position);
    /**
     * people should reimplement that to implement constraints
     * @returns true when the drag start should be succesful
     */
    virtual void end_drag( coord new_position);


};
}}}

#endif

