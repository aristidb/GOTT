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

#ifndef GOTT_GUI_DESIGNER_FRAME_HPP_INCLUDED
#define GOTT_GUI_DESIGNER_FRAME_HPP_INCLUDED

#include <vector>
#include <boost/scoped_ptr.hpp>
#include <boost/ptr_container/ptr_list.hpp>

namespace gott{namespace gui {namespace designer {
  /** Holds a frame
   *
   * It is internaly represented by a vector with an entry for each vector
   * layer (depth) that holds a list of pointers to heap managed vector_objs.
   */
  struct frame {
    typedef unsigned int depth_t;
    typedef boost::ptr_list<vector_obj*> vector_list;
    typedef std::vector<vector_list> depth_list;

    typedef depth_list::iterator iterator;
    iterator begin() { return frame_.begin(); }
    iterator end() { return frame_.end(); }

    typedef vector_list::iterator vector_iterator;
    vector_iterator begin(depth_t d) { return frame_[d].begin(); }
    vector_iterator end(depth_t d) { return frame_[d].end(); }

    typedef depth_list::const_iterator const_iterator;
    const_iterator begin() const { return frame_.begin(); }
    const_iterator end() const { return frame_.end(); }

    typedef vector_list::const_iterator vector_const_iterator;
    vector_const_iterator begin(depth_t d) const { return frame_[d].begin(); }
    vector_const_iterator end(depth_t d) const { return frame_[d].end(); }

    void add(vector_obj *o);
    void erase(iterator i) { frame_.erase(i); }
    void vector_erase(depth_t d, vector_iterator i) { frame_[d].erase(i); }

    void draw(agg::rendering_buffer &buf);
  private:
    depth_list frame_;
  };
}}}
#endif
