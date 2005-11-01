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

struct window_flags
{
  enum {
    Clear = 0
      , Decoration 
      , Fullscreen
      , Visible = 4
      , KeyEvents = 1<<3
      , MouseEvents = 1<<4
      , Menu = 1<<5
      , Toolbar = 1<<6
      , Utility = 1<<7
      , Dialog = 1<<8
      , Splash = 1<<9
      , Normal = 1<<10
      , Dock = 1<<11
      , ToolTip = 1<<12
      , Open = 1<<13
      , Defaults = (MouseEvents | KeyEvents | Visible | Decoration )

  };
};


/**
 * window_base describes the base for all window classes, event 
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
    sigc::singal1<void,MoUsE_event const& > mouse_;
    sigc::signal1<void,key_event const& > key_;

  public:

    window_base( uicontext_base * context );

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
    sigc::singal1<void,MoUsE_event const & > & on_mouse();
    sigc::signal1<void,key_event const&>& on_key();
    //\}


    typedef gott::properties::property<rect> rect_property_type;
    typedef gott::properties::property<gott::string> string_property_type;
    typedef gott::properties::property<bool> toggle_property_type;
    typedef gott::properties::property<std::bitset<10> > flags_property_type;
    
    /**
     * \name Window region interface 
     * \brief The region property should be used to change the position and size of a window. 
     * Changes to this property will be forwarded to the underlying window system. A 
     * successful change of the window extents will yield corresponding configure, move 
     * and resize events, and an update of the region property content.
     */
    //\{
    /// \returns a refernce to the region property with write access
    virtual rect_property_type& region() = 0;
    /// \returns a reference region property with read only access
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
    /// \returns a const refernce to a mutable title property
    virtual string_property_type const& title() const = 0; 
    //\}

    /**
     * \name window visibility property
     * \brief This property provides access to the visibility status of the window.
     * Changes to this property will be forwarded to the underlying window 
     * system. 
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
     * Currently not implemented:
     */
    virtual flags_property_type& flags() = 0; 
    virtual flags_property_type const& flags() const = 0; 

    /**
     * \brief opens a new window using the uicontext
     * \param[in]
     * \param[in]
     * \param[in] flags is a combination of window flags
     */
    virtual void open( uicontext_base& app, rect const& position, string const& title, std::size_t flags ) = 0;


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

    virtual ~window_base();
};

}}

#endif

