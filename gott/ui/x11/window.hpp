#ifndef GOTT_UI_X11_WINDOW_HPP_INCLUDED
#define GOTT_UI_X11_WINDOW_HPP_INCLUDED
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
 * Portions created by the Initial Developer are Copyright (C) 2___
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

#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <gott/ui/window_base.hpp>
#include <gott/ui/x11/uicontext.hpp>
#include <gott/properties/concrete_property.hpp>
#include <gott/properties/external_storage.hpp>
#include <gott/properties/sigc_notification.hpp>

namespace gott{namespace ui{namespace x11{
namespace detail { struct agg_buffer; }


/**
 * \brief x11 window class. 
 */
class GOTT_EXPORT window : public gott::ui::window_base {
  private:
    friend class uicontext;
    gott::properties::concrete_property<rect,gott::properties::sigc_notification,gott::properties::external_storage<rect> > region_;
    gott::properties::concrete_property<gott::string,gott::properties::sigc_notification,gott::properties::external_storage<gott::string> > title_;
    gott::properties::concrete_property<bool,gott::properties::sigc_notification,gott::properties::external_storage<bool> > visibility_;
    gott::properties::concrete_property<flags_type,gott::properties::sigc_notification,gott::properties::external_storage<flags_type> > flags_;
    Window handle;

    gott::ui::x11::detail::agg_buffer* impl;

    rect last_region, invalid_area;
    bool mapped_state;
    flags_type win_flags;
    gott::string title_string;

    uicontext * context;
    enum Protocols { DeleteWindow, Focus, Ping, ContextHelp, SyncRequest };

    Atom protocols[4];

    rect get_region() const;
    void set_region( rect const& r );
    void handle_sys_resize( rect const& r );

    void set_title( gott::string const& str );

    void map_window( bool new_state );
    void set_window_type( flags_type const& fl );

  public:

    /**
     * \brief creates and opens a window 
     * \param[in] app user interface system context of this window
     * \param[in] position the position and size of the window
     * \param[in] title a unicode string containing the title of the window
     * \param[in] flags a combination of ui::window_flags
     */
    window( uicontext& app, rect const& position, string const& title, std::size_t flags );

    rect_property_type& region();
    rect_property_type const& region() const;
    string_property_type & title(); 
    string_property_type const& title() const;

    toggle_property_type& visible(); 
    toggle_property_type const& visible() const; 

#if 0
    // toggle_property_type& open(); 
    // toggle_property_type const& open() const; 
#endif
    
    flags_property_type& flags(); 
    flags_property_type const& flags() const; 


    void set_size_hints();

    void update_region( rect const& region );

    uicontext* get_uicontext();


    bool needs_update() const;
    rect get_invalid_area() const;
    void invalidate_area( rect const& region );



    void blit_buffer( coord const& destination, agg::rendering_buffer const& buffer, pixel_format::type buf_format );
    void blit_rect( rect const& source, coord const& destination, agg::rendering_buffer const& buffer, pixel_format::type buf_format  );

    agg::rendering_buffer const& screen_buffer() const;
    agg::rendering_buffer & screen_buffer();


    /**
     * \name special x11 functions
     * \{
     */
    /**
     * \returns the Window/XID of the window
     */
    Window get_handle() const;
    /**
     * \brief Sets a property 
     */
    void change_property( Atom property, Atom type, int format, unsigned char *data, int nlements );
    //\}

    ~window();
};


}}}

#endif

