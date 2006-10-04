// vim:ts=2:sw=2:expandtab:autoindent:filetype=cpp:
#ifndef GOTT_UI_WINDOW_BASE_HPP_INCLUDED
#define GOTT_UI_WINDOW_BASE_HPP_INCLUDED
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

#include <bitset>
#include <gott/visibility.hpp>
#include <gott/properties/property.hpp>
#include <gott/string/string.hpp>
#include <gott/graphics/geometry.hpp>
#include <gott/ui/input.hpp>

// rendering system!

namespace gott{namespace ui{
class uicontext_base;

/**
 * \brief window_flags contains a set of bitmask to specify properites of windows.
 * Some flags might not be representable in the underlying system, 
 * so there is no gurantee these flags have any effects on the window.
 *
 * \todo gott::ui::window_flags needs revision, gott::ui::window_flags::Open is not required, and gott::ui::window_flags::Visible is
 * handled by a different property, 
 */
struct GOTT_LOCAL window_flags
{
  enum {
    Clear = 0 ///< no flags specified 
      , Decoration  ///< Window decoration is enabled
      , Fullscreen ///< Window takes the whole screen
      , Visible = 4 ///< Window is visible
      , KeyEvents = 1<<3 ///< Window receives key events
      , MouseEvents = 1<<4 ///< Window receives mouse events
      , Menu = 1<<5 ///< Menu window 
      , Toolbar = 1<<6 ///< Window is / has? a tool bar window 
      , Utility = 1<<7 ///< Utility window
      , Dialog = 1<<8 ///< Dialog window
      , Splash = 1<<9 ///< Window displays a splash screen
      , Normal = 1<<10 ///< Normal window 
      , Dock = 1<<11  ///< Window is a small dock application
      , ToolTip = 1<<12 ///< Window displays a tool tip
      , Open = 1<<13 ///< Window is open
      , Defaults = (MouseEvents | KeyEvents | Visible | Decoration ) ///< Default window settings, receiving events and decoration

  };
};

/**
 * \brief holds the pixel format enum, called gott::ui::pixel_format::type
 */
struct GOTT_LOCAL pixel_format {
  enum type
  {
    undefined = 0,  ///< By default. No conversions are applied 
    bw,             ///< 1 bit per color B/W
    gray8,          ///< Simple 256 level grayscale
    gray16,         ///< Simple 65535 level grayscale
    rgb555,         ///< 15 bit rgb. Depends on the byte ordering!
    rgb565,         ///< 16 bit rgb. Depends on the byte ordering!
    rgbAAA,         ///< 30 bit rgb. Depends on the byte ordering!
    rgbBBA,         ///< 32 bit rgb. Depends on the byte ordering!
    bgrAAA,         ///< 30 bit bgr. Depends on the byte ordering!
    bgrABB,         ///< 32 bit bgr. Depends on the byte ordering!
    rgb24,          ///< R-G-B, one byte per color component
    bgr24,          ///< B-G-R, native win32 BMP format.
    rgba32,         ///< R-G-B-A, one byte per color component
    argb32,         ///< A-R-G-B, native MAC format
    abgr32,         ///< A-B-G-R, one byte per color component
    bgra32,         ///< B-G-R-A, native win32 BMP format
    rgb48,          ///< R-G-B, 16 bits per color component
    bgr48,          ///< B-G-R, native win32 BMP format.
    rgba64,         ///< R-G-B-A, 16 bits byte per color component
    argb64,         ///< A-R-G-B, native MAC format
    abgr64,         ///< A-B-G-R, one byte per color component
    bgra64,         ///< B-G-R-A, native win32 BMP format

    formats
  };
};



/**
 * \brief window_base describes the base for all window classes. 
 * The inteface is currently coupled to Antigrain Graphics, 
 * this should be changed in the near future. The agg::types 
 * will remain here until then, to allow writing other non 
 * renderer related code. 
 */
class GOTT_EXPORT window_base {
  private:
    uicontext_base *context; 
    //boost::signal2<void, agg::rendering_buffer&, rect const&> draw_;
    boost::signal1<void, rect const&> configure_
      , resize_
      , move_;
    boost::signal0<void> focus_enter_
       , focus_leave_
       , close_;
    boost::signal1<void,mouse_event const& > mouse_;
    boost::signal1<void,gott::ui::key_event const& > key_;

  public:

    /**
     * \name Window signals
     */
    //\{
    /**
     * \brief draw signal 
     * The rendering system is currently directly coupled to antigrain.
     * A correct slot only makes changes in the rectangular region defined by the 
     * second parameter.
     */
    //boost::signal2<void, agg::rendering_buffer&, rect const&>& on_draw();
    /// Both resize and move events will be handled by this signal
    boost::signal1<void, rect const&>& on_configure();
    /**
     * Only those events that do change the size of the window will 
     * emit this signal.
     */
    boost::signal1<void, rect const&>& on_resize();
    /**
     * Only move events will emit this signal.
     */
    boost::signal1<void, rect const&>& on_move();
    boost::signal0<void>& on_focus_enter();
    /// This event is emitted right before the window gets closed
    boost::signal0<void>& on_close();
    boost::signal0<void>& on_focus_leave();
    boost::signal1<void,mouse_event const & > & on_mouse();
    boost::signal1<void,key_event const&>& on_key();
    //\}

    typedef std::bitset<14> flags_type;

    typedef gott::properties::notifying_property<rect> rect_property_type;
    typedef gott::properties::notifying_property<gott::string> string_property_type;
    typedef gott::properties::notifying_property<bool> toggle_property_type;
    typedef gott::properties::notifying_property<flags_type> flags_property_type;
    
    /**
     * \name Window region interface 
     * \brief The region property should be used to change the position and size of a window. 
     * Changes to this property will be forwarded to the underlying window system. A 
     * successful change of the window extents will yield corresponding configure, move 
     * and resize events, and an update of the region property content.
     */
    //\{
    /**
     * \returns a refernce to the region property with write access
     */
    virtual rect_property_type& region() = 0;
    virtual rect_property_type const& region() const = 0;
    //\}


    /**
     * \name window title interface
     * \brief The title property should be used to change the title of a window. 
     * Changes to this property will be forwarded to the underlying window 
     * system. 
     */
    //\{
    /// \returns a refernce to a mutable title property
    virtual string_property_type & title() = 0; 
    virtual string_property_type const& title() const = 0; 
    //\}

    /**
     * \name window visibility property
     * \brief This property provides access to the visibility status of the window.
     * Changes to this property will be forwarded to the underlying window 
     * system. 
     * \todo discuss different visible states, like fullscreen, maximized 
     * minimized and normal visibility.
     */
    //\{
    ///
    virtual toggle_property_type& visible() = 0; 
    ///
    virtual toggle_property_type const& visible() const = 0; 
    //\}

#if 0
    /**
     * \name 
     * open().set(true) <- ?? sounds broken 
     */
    // virtual toggle_property_type& open() = 0; 
    // virtual toggle_property_type const& open() const = 0; 
#endif
    
    /**
     * \name Window type flags and other
     * This property displays the window type.
     * \sa gott::ui::window_flags for  possible bitmasks
     * \{
     */
    virtual flags_property_type& flags() = 0; 
    virtual flags_property_type const& flags() const = 0; 
    // \}

    /**
     * \brief sets all kinds of hints
     * The window system might not respec these hints. It should be possible
     * to set the resize steps, a minimum size, and maybe a fixed apsect ratio?
     */
    virtual void set_size_hints() = 0;

    /**
     * \brief commit changes in the local buffer to the window
     * In most implementations the window is double buffered by default,
     * in the others this function might not have an effect.
     */
    virtual void update_region( rect const& region ) = 0;

    /**
     * \brief get the uicontext this window belongs to. 
     */
    virtual uicontext_base* get_uicontext() = 0;


    /**
     * \name handling render updates
     * To ensure that new data gets visible as soon as possible, 
     * the user has to invalidate regions of the screen. The event loop
     * will then initiate a buffered redraw and screen update sequence, 
     * as soon as there are no more events to be processed. 
     * \todo The current invalidation system is based on accumulated rectangular regions, 
     *  this concept could be improvd towards polygonal regions. 
     * \{
     */
    /**
     * This method signalizes if there are pending updates for this
     * window. The uicontext should check that method, and emit the redraw
     * event. 
     */
    virtual bool needs_update() const = 0;
    /**
     * \brief returns the invalid area. 
     * To reduce and optimize redraws, the system should use the rect provided
     * by this method, to emit a proper draw signal.
     */
    virtual rect get_invalid_area() const = 0;
    /**
     * \brief invalidates a rectangular area on screen
     * Use this method to tell the system that a redraw should take place as soon 
     * as possible. This method will not signal a draw event!
     */
    virtual void invalidate_area( rect const& region ) = 0;
    //\}

    /**
     * \name Antigrain related render methods. 
     * These methods will be removed in the future. The final rendering system should not have any 
     * visable external dependencies. 
     * \{
     */
    /**
     * \brief copy the contents of the buffer onto screen 
     */
    //virtual void blit_buffer( coord const& destination, agg::rendering_buffer const& buffer, pixel_format::type buf_format = pixel_format::rgba32 ) = 0;

    /**
     * \brief copy the contents described by source, from buffer to screen.
     */
    //virtual void blit_rect( rect const& source, coord const& destination, agg::rendering_buffer const& buffer, pixel_format::type buf_format = pixel_format::rgba32 ) = 0;

    /**
     * \brief returns the screen buffer
     */
    /*virtual agg::rendering_buffer const& screen_buffer() const = 0;
    virtual agg::rendering_buffer & screen_buffer() = 0;*/
    //\}


    virtual ~window_base();
};

}}

#endif

