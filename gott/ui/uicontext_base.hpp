#ifndef GOTT_UI_UICONTEXT_BASE_HPP_INCLUDED
#define GOTT_UI_UICONTEXT_BASE_HPP_INCLUDED

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

#include <sigc++/signal.h>
#include <gott/visibility.hpp>
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

