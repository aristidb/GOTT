#ifndef GOTT_UI_WINDOW_BASE_HPP_INCLUDED
#define GOTT_UI_WINDOW_BASE_HPP_INCLUDED

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

#include <bitset>
#include <gott/util/visibility.hpp>
#include <sigc++/signal.h>
#include <gott/util/properties/property.hpp>
#include <gott/util/string/string.hpp>
#include <gott/util/geometry.hpp>
#include <gott/ui/input.hpp>

// rendering system!
#include <agg_rendering_buffer.h>

namespace gott{namespace ui{
class uicontext_base;

/**
 * \brief These flags are used to define the type and look of the window
 * Some flags might not be representable in the underlying system, 
 * so there is no gurantee that these flags have any effects on 
 * the window.
 *
 * \todo these flags need revision, Open is not required, and Visible is
 * handled by a different property, 
 */
struct GOTT_LOCAL window_flags
{
  enum {
    Clear = 0
      , Decoration 
      , Fullscreen
      , Visible = 4
      , KeyEvents = 1<<3
      , MouseEvents = 1<<4
      , Menu = 1<<5
      , Toolbar = 1<<6 ///< Window is / has? a Toolbar window 
      , Utility = 1<<7
      , Dialog = 1<<8
      , Splash = 1<<9 ///< Window displays a Splashscreen
      , Normal = 1<<10
      , Dock = 1<<11  ///< The window is a small dock application
      , ToolTip = 1<<12 ///< The Window displays a Tooltip
      , Open = 1<<13
      , Defaults = (MouseEvents | KeyEvents | Visible | Decoration )

  };
};

/**
 * \brief holds the pixel format enum, \see pixel_format::type
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
    sigc::signal2<void, agg::rendering_buffer&, rect const&> draw_;
    sigc::signal1<void, rect const&> configure_
      , resize_
      , move_;
    sigc::signal0<void> focus_enter_
       , focus_leave_
       , close_;
    sigc::signal1<void,mouse_event const& > mouse_;
    sigc::signal1<void,gott::ui::key_event const& > key_;

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
    sigc::signal2<void, agg::rendering_buffer&, rect const&>& on_draw();
    /// Both resize and move events will be handled by this signal
    sigc::signal1<void, rect const&>& on_configure();
    /**
     * Only those events that do change the size of the window will 
     * emit this signal.
     */
    sigc::signal1<void, rect const&>& on_resize();
    /**
     * Only move events will emit this signal.
     */
    sigc::signal1<void, rect const&>& on_move();
    sigc::signal0<void>& on_focus_enter();
    /// This event is emitted right before the window gets closed
    sigc::signal0<void>& on_close();
    sigc::signal0<void>& on_focus_leave();
    sigc::signal1<void,mouse_event const & > & on_mouse();
    sigc::signal1<void,key_event const&>& on_key();
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
     * This property displays the window type, \see window_flags for 
     * possible bitmasks
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
    virtual void blit_buffer( coord const& destination, agg::rendering_buffer const& buffer, pixel_format::type buf_format = pixel_format::rgba32 ) = 0;

    /**
     * \brief copy the contents described by source, from buffer to screen.
     */
    virtual void blit_rect( rect const& source, coord const& destination, agg::rendering_buffer const& buffer, pixel_format::type buf_format = pixel_format::rgba32 ) = 0;

    /**
     * \brief returns the screen buffer
     */
    virtual agg::rendering_buffer const& screen_buffer() const = 0;
    virtual agg::rendering_buffer & screen_buffer() = 0;
    //\}


    virtual ~window_base();
};

}}

#endif

