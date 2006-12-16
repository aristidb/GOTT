// Copyright (C) 2004-2005 by Rüdiger Sonderfeld <kingruedi@c-plusplus.de>
// Content: GOTT frame class
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

#include "frame.hpp"

namespace gott{namespace gui{namespace designer{
  void frame::add(vector_obj *o) {
    boost::scoped_ptr obj(o); //Exception safety if resize throws
    depth_t depth=obj->depth();
    if(depth >= frame_.size()) {
      frame_.resize(depth);
    }
    obj.reset();
    frame_[depth].push_back(o);
  }

  void frame::draw(agg::rendering_buffer &buf) {
    for(iterator i=frame_.begin();
	i != frame_.end();
	++i)
      for(vector_iterator j=i.begin();
	  j!=i.end();
	  ++j)
	j->draw(buf);
  }
}}}
