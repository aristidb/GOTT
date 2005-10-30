#ifndef GOTT_UI_X11_WINDOW_HPP_INCLUDED
#define GOTT_UI_X11_WINDOW_HPP_INCLUDED

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



#include <gott/ui/window_base.hpp>

namespace gott{namespace ui{namespace x11{

class uicontext;

class GOTT_EXPORT window : public gott::ui::window_base {
  private:
    uicontext *context; ///<weak_ptr?
    gott::properties::concrete_property<rect,sigc_notification> region_;
    gott::properties::concrete_property<gott::string,sigc_notification> title_;
    gott::properties::concrete_property<bool,sigc_notification> visibility_;
    gott::properties::concrete_property<std::bitset<10>,sigc_notification> flags_;

  public:

    /**
     * Should we provide a default constructor?
     */
    window();

    /**
     * Should we provide a default constructor?
     */
    window( gott::ui::x11::uicontext& app, rect const& position, string const& title, std::size_t flags);

    rect_property_type& region();
    rect_property_type const& region() const;
    string_property_type & title(); 
    string_property_type const& title();

    toggle_property_type& visible(); 
    toggle_property_type const& visible() const; 

#if 0
    // toggle_property_type& open(); 
    // toggle_property_type const& open() const; 
#endif
    
    flags_property_type& flags(); 
    flags_property_type const& flags() const; 

    void open( uicontext& app, rect const& position, string const& title, std::size_t flags );


    void set_size_hints();

    void update_region( rect const& region );

    uicontext* get_uicontext();

    ~window();

};


}}}

#endif

