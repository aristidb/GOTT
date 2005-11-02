#ifndef GOTT_UI_UICONTEXT_BASE_HPP_INCLUDED
#define GOTT_UI_UICONTEXT_BASE_HPP_INCLUDED

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


#include <sigc++/signal.h>
#include <gott/util/visibility.hpp>
#include <gott/ui/input.hpp>
#include <gott/ui/window_base.hpp>

namespace gott{ namespace ui{

/**
 * \brief Base class for all user interface contexts. 
 * These classes will gather and dispatch system events to the 
 * responsible windows and organize the communication with the 
 * system. Every window must have an uicontext, thus they will 
 * register themselves with a matching uicontext.
*/
class GOTT_EXPORT uicontext_base {
  protected:
    gott::ui::key_state keys_;
    gott::ui::mouse_state mouse_;

  public:

    /**
     * \name window registration
     */
    //\{
    /**
     * \brief registers a window in the event dispatching system of the context.
     * Should be called by every window_base implementation right after 
     * successful construction.
     */
    virtual void register_window( window_base * ref ) = 0;
    /**
     * \brief removes a window from the event dispatching system of the context.
     * Should be called by every window_base implementation right before 
     * destruction. After that all incoming events to this window shall be 
     * droped.
     */
    virtual void remove_window( window_base *ref ) = 0;
    //\}


    /**
     * \brief terminate the context.
     * A call to this method ends this user interface session, and might also 
     * quit the current application if no other control loop is running. 
     */
    virtual void quit() = 0;

#if 0
    Removed - see wiki about event loop 

    virtual gott::ui::uicontext_base::status
       handle_pending_messages() = 0;
    virtual void handle_idle() = 0;
    virtual void run() = 0;

    virtual sigc::connection
    append_process_idle_handler(
      idle_signal::slot_type const& slot
      ) = 0;
#endif

    gott::ui::key_state const& get_key_state() const;
    gott::ui::mouse_state const& get_mouse_state() const;
    virtual ~uicontext_base() = 0;
};
}}

#endif

