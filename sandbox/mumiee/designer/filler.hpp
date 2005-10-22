// Copyright (C) 2004-2005 by Rüdiger Sonderfeld <kingruedi@c-plusplus.de>
// Content: GOTT filler base class
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

#ifndef GOTT_GUI_DESIGNER_FILLER_OBJ_HPP_INCLUDED
#define GOTT_GUI_DESIGNER_FILLER_HPP_INCLUDED
namespace gott { namespace gui { namespace designer {
  struct GOTT_EXPORT filler {
    virtual ~filler() =0;

    virtual void fill(agg::rendering_buffer &) const =0;
  };
}}}
#endif
