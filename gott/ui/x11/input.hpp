// vim:ts=2:sw=2:expandtab:autoindent:filetype=cpp:
#ifndef GOTT_UI_X11_INPUT_HPP_INCLUDED
#define GOTT_UI_X11_INPUT_HPP_INCLUDED

/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is a user interface abstraction library.
 *
 * The Initial Developer of the Original Code is
 * Andreas Pokorny (andreas.pokorny@gmail.com)
 * Portions created by the Initial Developer are Copyright (C) 2005
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *  Andreas Pokorny (andreas.pokorny@gmail.com)
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */


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
