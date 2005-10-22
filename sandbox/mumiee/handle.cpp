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

#include "handle.hpp"

namespace gott{namespace gui{namespace designer{
handle::handle() {
}
rect const& handle::get_region() const{
  return region;
}
bool handle::enabled() const{
  return status;
}
void handle::enable(){
  status = true;
}
void handle::disable() {
  status = false;
}
void handle::set_position( coord const& new_position ){
  position = new_position;
  region.left = new_position.x - region.width/2;
  region.top = new_position.y - region.height/2;
}

}}}
