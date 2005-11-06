#ifndef GOTT_UI_WINDOWS_HPP_INCLUDED
#define GOTT_UI_WINDOWS_HPP_INCLUDED

// Copyright (C) 2004-2005 by Andreas Pokorny andreas.pokorny@gmail.com
// Content: GOTT generic delegating window class
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

/**
 * \brief A generic API unaware window class
 * The generic window, unlike other GUI-Systems this window class allows migrating 
 * a window from one context to another .. elaborate...
 */
class GOTT_EXPORT window : public window_base  {
  private:
    window_base * base;
  public:


    void set_size_hints();
    void update_region( rect const& region );
    uicontext* get_uicontext();

    ~window();
};

}}

#endif

