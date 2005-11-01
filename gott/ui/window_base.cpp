// Copyright (C) 2004-2005 by Andreas Pokorny andreas.pokorny@gmail.com
// Content: GOTT window base class
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

#include <gott/ui/window_base.hpp>
namespace gott{namespace ui{

window_base::~window_base()
{}

sigc::signal2<void, agg::rendering_buffer&, rect const&> & window_base::on_draw()
{
  return draw_;
}

sigc::signal1<void, rect const&> & window_base::on_configure()
{
  return configure_;
}

sigc::signal1<void, rect const&> & window_base::on_resize()
{
  return resize_;
}

sigc::signal1<void, rect const&> & window_base::on_move()
{
  return move_;
}

sigc::signal0<void> & window_base::on_focus_enter()
{
  return focus_enter_;
}

sigc::signal0<void> & window_base::on_close()
{
  return close_;
}

sigc::signal0<void> & window_base::on_focus_leave()
{
  return focus_leave_;
}

sigc::signal1<void,mouse_event const&> & window_base::on_mouse()
{
  return mouse_;
}

sigc::signal1<void,key_event const&> & window_base::on_key()
{
  return key_;
}



}}

