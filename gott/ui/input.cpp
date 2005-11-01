// Copyright (C) 2004-2005 by Andreas Pokorny andreas.pokorny@gmail.com
// Content: GOTT input structures
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


#include <gott/ui/input.hpp>
#include <cstring>

namespace gott{namespace ui{

key_event::key_event( key_code c, event_type t)  
    : type(t), code(c)
{}

key_state::key_state()
  : keyboard( std::size_t(gott::ui::NoKey), static_cast<unsigned char>(0) )
{}

mouse_state::mouse_state()
{
  std::memset( buttons, 0, sizeof(buttons) );
}


}}

