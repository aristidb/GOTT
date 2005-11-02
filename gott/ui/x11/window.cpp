// Copyright (C) 2004-2005 by Andreas Pokorny andreas.pokorny@gmail.com
// Content: GOTT x11 window implementation
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

#include <gott/ui/x11/window.hpp> 

namespace gott{namespace ui{namespace x11{

window::window( uicontext& app, rect const& position, string const& title, std::size_t flags ) 
  : handle(0), invalid_area(0,0,0,0), context(&app)
{

  ::Window root_window = RootWindow( context->get_display(), context->get_screen() );
  context->register_window( this );
}

gott::ui::window_base::rect_property_type& window::region()
{
  return region_;
}

gott::ui::window_base::rect_property_type const& window::region() const
{
  return region_;
}

gott::ui::window_base::string_property_type & window::title()
{
  return title_;
}
 
gott::ui::window_base::string_property_type const& window::title()
{
  return title_;
}


gott::ui::window_base::toggle_property_type& window::visible()
{
  return visibility_;
}
 
gott::ui::window_base::toggle_property_type const& window::visible() const
{
  return visibility_;
}
 


gott::ui::window_base::flags_property_type& window::flags()
{
  return flags_;
}
 
gott::ui::window_base::flags_property_type const& window::flags() const
{
  return flags_;
}
 


void window::set_size_hints(){
}

void window::update_region( rect const& region ){
  blit_buffer( buffer, region );
}

uicontext* window::get_uicontext(){
  return context;
}

bool window::needs_update() const{
  return invalid_area.width == 0 || invalid_area.height == 0;
}
rect window::get_invalidation_area() const{
  return invalid_area;
}
void window::invalidate_area( rect const& region ){
  invalid_area.add_region(region);
}


void window::blit_buffer( agg::rendering_buffer const& buffer, recont const& target_region ){
}

agg::rendering_buffer const& window::screen_buffer() const{
  return buffer;
}

agg::rendering_buffer & window::screen_buffer(){
  return buffer;
}

Window window::get_handle() const{
  return handle;
}


window::~window()
{
}



}}}

