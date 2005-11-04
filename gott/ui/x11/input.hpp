#ifndef GOTT_UI_X11_INPUT_HPP_INCLUDED
#define GOTT_UI_X11_INPUT_HPP_INCLUDED

// Copyright (C) 2004-2005 by Andreas Pokorny andreas.pokorny@gmail.com
// Content: GOTT User interface context base class
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

#include <X11/Xdefs.h>
#include <X11/X.h>
#include <X11/keysym.h>

#include <gott/ui/input.hpp>

namespace gott{namespace ui{namespace x11{namespace detail{

/**
 * \brief Internal key translation structure. 
 * Not intended for public use. 
 * Translates X11 Kesymbols into our generic key system.
 */
struct GOTT_LOCAL key_table
{
  private:
    unsigned char t[2][256];
    key_table();
    key_table(key_table const&);
    key_table& operator=(key_table const&);
  public:
    key_code translate_key( KeySym const& sym ) const;
    static key_table const & get_instance();
};


}}}}
#endif
