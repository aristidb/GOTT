
#ifndef GOTT_GUI_X11_APPLICATION_HPP_INLCUDED
#define GOTT_GUI_X11_APPLICATION_HPP_INLCUDED

#include <string>
#include <set>
#include <list>
#include "window.hpp"

namespace gott{ namespace gui{ namespace x11{
  
class application
{
  public:
    enum status { Continue, End };
    typedef boost::signal<void ()> idle_signal;

  private:
    Display * display;
    int screen;
    Atom protocols_atom;
    std::list<gott::gui::x11::window*> windows;
    window* focus_window;
    Cursor blank_cursor;
    idle_signal process_idle;
    mouse_state mouse_info;
    key_state key_info;
    // actions?
    void init_cursor();
    void process_event( gott::gui::x11::window* win, XEvent& event );
    gott::gui::x11::window* find_window( ::Window handle );
  public:
    application( char const* connection = 0 );

    void register_window( window * ref );
    void remove_window( window *ref );

    status handle_pending_messages();
    void handle_idle();
    void run();
    void quit();

    boost::signals::connection append_process_idle_handler( idle_signal::slot_type const& slot );

    key_state const& get_key_state() const;
    mouse_state const& get_mouse_state() const;

    // implementation dependent methods:
    Display* get_display();
    int get_screen() const;
    Atom get_atom( char const * atom_name );
    
};

}}}

#endif

